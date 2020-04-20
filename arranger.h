#ifndef NBPET_ARRANGER_H
#define NBPET_ARRANGER_H

#include "task.h"
#include <queue>
#include <list>

using namespace std;
extern queue<int> taskMessageQueue;
extern list<Task> globalTaskList;
extern list<proxy> globalProxyList;
extern pthread_mutex_t tpListLock;

void initialize_proxy_list(char **proxies_str_list, int count);

void initialize_task_list(string filename, string download_address);

void message_loop(const string& filename, const string& download_address, curl_off_t file_length);

#endif //NBPET_ARRANGER_H
