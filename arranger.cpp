#include "arranger.h"

#define TEST_DOWNLOAD_BYTES 0xFFFFFF // 16M is enough
#define EXPECT_DOWNLOAD_TIME_SEC 30

queue<int> taskMessageQueue;
list<Task> globalTaskList;
list<proxy> globalProxyList;

void initialize_proxy_list(char **proxies_str_list, int count) {
    for (int i = 0; i < count; i++) {
        proxy proxy_to_insert = {string(proxies_str_list[i]), i};
        globalProxyList.insert(globalProxyList.end(), proxy_to_insert);
    }
}

void initialize_task_list(string filename, string download_address) {
    for (const proxy &single_proxy:globalProxyList) {
        globalTaskList.insert(globalTaskList.end(), Task(single_proxy, globalProxyList.size() * TEST_DOWNLOAD_BYTES,
                                                         (globalProxyList.size() + 1) * TEST_DOWNLOAD_BYTES));

    }
    for (Task &task:globalTaskList) {
        task.execute(filename, download_address);
    }
}

struct proxy_and_speed {
    proxy proxy_server;
    curl_off_t last_download_speed;
};

void cover_or_trim_task(proxy_and_speed proxy_and_speed_info, list<Task>::iterator task, string filename,
                        string download_address) {
    if (proxy_and_speed_info.last_download_speed * EXPECT_DOWNLOAD_TIME_SEC + task->start > task->end) {
        // 重新分配这个task并立即开始下载
        *task = Task(proxy_and_speed_info.proxy_server, task->start, task->end);
        task->execute(filename, download_address);
    } else {
        // 尝试将这一块失败的下载“切开”
        Task task1 = Task(proxy_and_speed_info.proxy_server, task->start, task->start +
                                                                          proxy_and_speed_info.last_download_speed *
                                                                          EXPECT_DOWNLOAD_TIME_SEC);
        Task task2 = Task(
                task->start + proxy_and_speed_info.last_download_speed * EXPECT_DOWNLOAD_TIME_SEC,
                task->end, STATUS_ERROR);
        task1.execute(filename, download_address);
        task2.execute(filename, download_address);
        globalTaskList.insert(task, task1);
        globalTaskList.insert(task, task2);
        globalTaskList.remove(*task);
    }
}


// the main message loop for listening to progress complete event.
void message_loop(string filename, string download_address, curl_off_t file_length) {
    int task_id;
    // pending proxy server是空闲的代理服务器列表。
    queue<proxy_and_speed> pending_proxy_servers;
    while (true) {
        if (!taskMessageQueue.empty()) {
            task_id = taskMessageQueue.front();
            taskMessageQueue.pop();
            for (auto task = globalTaskList.begin(); task != globalTaskList.end(); task++) {
                if (task->task_id == task_id) {
                    if (task->status == STATUS_ERROR) {
                        globalProxyList.remove(task->use_proxy);
                        if (!pending_proxy_servers.empty()) {
                            proxy_and_speed useful_proxy = pending_proxy_servers.front();
                            cover_or_trim_task(useful_proxy, task, filename, download_address);
                            goto loop_end;
                        }
                    } else {
                        // 不是错误，就是成功了。下载成功，就重新查询列表中是否有失败的下载，如果有则要“切开”分配下载任务。
                        for (auto new_task = globalTaskList.begin(); new_task != globalTaskList.end(); new_task++) {
                            if (new_task->status == STATUS_ERROR) {
                                proxy_and_speed current_proxy = {task->use_proxy, task->speed};
                                cover_or_trim_task(current_proxy, new_task, filename, download_address);
                                goto loop_end;
                            }
                        }
                        // 遍历了所有任务，也找不到下载错误的任务，就分配新的任务让这个空出来的服务器下载。
                        curl_off_t new_block_length = task->speed * EXPECT_DOWNLOAD_TIME_SEC;
                        if (globalTaskList.end()->end < file_length) {
                            Task task_to_append = Task(task->use_proxy, globalTaskList.end()->end, 0);
                            // 全局下载任务还没有分配到尽头，需要继续往下分配。
                            if (globalTaskList.end()->end + new_block_length < file_length) {
                                task_to_append.end = globalTaskList.end()->end + new_block_length;
                            } else {
                                // 整个下载在这个任务分配完毕后已经走到了尽头。
                                task_to_append.end = file_length;
                            }
                            globalTaskList.insert(globalTaskList.end(), task_to_append);
                            task_to_append.execute(filename, download_address);
                            goto loop_end;
                        } else {
                            // 找不到新块，就把代理服务器和下载速度放在待命区域，准备下载。
                            proxy_and_speed free_proxy_server = {task->use_proxy, task->speed};
                            pending_proxy_servers.push(free_proxy_server);
                            if (pending_proxy_servers.size() == globalProxyList.size()) {
                                // 所有的服务器都已经空出来了，可以完成下载了。
                                return;
                            }
                            goto loop_end;
                        }
                    }
                }
            }
        }
        loop_end:;
    }
}
