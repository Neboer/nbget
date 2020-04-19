//extern "C" {

#include "task.h"

#include <utility>

Task::Task(proxy proxy_info, curl_off_t start, curl_off_t end) {
    this->use_proxy = proxy_info;
    this->start = start;
    this->end = end;
    download = 0;
    status = STATUS_CREATE;
    speed = 0;
}

void Task::execute(const string &fileName, const string &download_address) {
    this->taskThread = thread(part_download, download_address, fileName, this);
}

void Task::wait() {
    this->taskThread.join();
}