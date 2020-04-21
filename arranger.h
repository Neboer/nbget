#pragma once
#include "task.h"
#include <queue>
#include <list>

using namespace std;
extern queue<Task*> taskMessageQueue;
extern list<Task*> globalTaskList;
extern list<proxy> globalProxyList;
extern queue<proxy> pending_proxy_servers;// pending proxy server是空闲的代理服务器列表。
extern pthread_mutex_t tpListLock;
extern pthread_mutex_t messageLock;

void initialize_proxy_list(char **proxies_str_list, int count);

void initialize_task_list(string filename, string download_address, int file_size);

void message_loop(const string filename, const string download_address, curl_off_t file_length);

