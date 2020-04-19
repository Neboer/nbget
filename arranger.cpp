#include "arranger.h"

queue<Task> taskMessageQueue;


vector<Task> generate_initial_tasks(curl_off_t size, vector<string> proxy_list) {
    vector<Task> task_list;
    for (int i = 0; i < proxy_list.size(); ++i) {
        task_list.insert(task_list.end(), Task(proxy_list.at(i), size * i, size * (i + 1)));
    }
    return task_list;
}