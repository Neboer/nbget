//extern "C" {

#include "task.h"


int global_task_id = 0;

Task::Task(proxy proxy_info, curl_off_t start, curl_off_t end) {
    this->task_id = global_task_id;
    global_task_id++;
    this->use_proxy = proxy_info;
    this->start = start;
    this->end = end;
    download = 0;
    status = STATUS_CREATE;
    speed = 0;
}
// 这是个假的“Task”，只起到占位的作用。
Task::Task(curl_off_t start, curl_off_t end, short status){
    this->task_id = global_task_id;
    global_task_id++;
    this->use_proxy = proxy();
    this->start = start;
    this->end = end;
    download = 0;
    this->status = status;
    speed = 0;
}


void Task::execute(const string &fileName, const string &download_address) {
    this->taskThread = new thread(part_download, download_address, fileName, this);
}

void Task::wait() {
    this->taskThread->join();
}