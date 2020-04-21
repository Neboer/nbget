#pragma once

#include <curl/curl.h>
#include <thread>

#include "string"

#define STATUS_CREATE 0
#define STATUS_RUNNING 1
#define STATUS_COMPLETE 2
#define STATUS_ERROR 3


using namespace std;

extern int global_task_id;

struct proxy {
    string address;
    int index;

    bool operator==(const proxy &rhs) const;

};

struct Task {
    int task_id;
    proxy proxy_server;
    curl_off_t start;
    curl_off_t end;
    curl_off_t download;
    short status;
    thread taskThread;
    curl_off_t speed;

    Task(proxy proxy_info, curl_off_t start, curl_off_t end);

    Task(curl_off_t start, curl_off_t end, short status);

    // a task object cannot be compared with or copy.
    Task(Task &task) = delete;

    void execute(const string fileName, const string download_address);

    void wait();

    bool operator==(const Task &rhs) = delete;
};
