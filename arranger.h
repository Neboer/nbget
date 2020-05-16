#pragma once

#include "task.h"
#include <queue>
#include <list>

using namespace std;
extern pthread_mutex_t tpListLock;
extern pthread_mutex_t messageLock;

struct download_status {
    queue<Task *> taskMessageQueue{};
    list<Task *> globalTaskList{};
    list<proxy> globalProxyList{};
// pending proxy server是空闲的代理服务器列表。
    queue<proxy> pending_proxy_servers{};
};

void initialize_proxy_list(char **proxies_str_list, int count, download_status *status);

void initialize_task_list(const string &filename, const string &download_address, curl_off_t file_size, download_status *status);

void message_loop(const string &filename, const string &download_address, curl_off_t file_length, download_status *status);

