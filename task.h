#ifndef NBPET_TASK_H
#define NBPET_TASK_H

#include <curl/curl.h>
#include <thread>

#include "string"
#include "http.h"

#define STATUS_CREATE 0
#define STATUS_RUNNING 1
#define STATUS_COMPLETE 2
#define STATUS_ERROR 3


using namespace std;

extern int global_task_id;

struct proxy {
    string address;
    int index;
};

class Task {
public:
    int task_id;
    proxy use_proxy;
    curl_off_t start;
    curl_off_t end;
    curl_off_t download;
    short status;
    unique_ptr<thread> taskThread;
    curl_off_t speed;

    Task(proxy proxy_info, curl_off_t start, curl_off_t end);

    Task(curl_off_t start, curl_off_t end, short status);

    void execute(const string &fileName, const string &download_address);

    void wait();

};


#endif //NBPET_TASK_H
