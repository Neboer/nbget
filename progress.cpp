#include "progress.h"
#include<iostream>
#include <unistd.h>
#include <iomanip>

static const char *humanSize(curl_off_t bytes) {
    char const *suffix[] = {"B", "KB", "MB", "GB", "TB"};
    char length = sizeof(suffix) / sizeof(suffix[0]);

    int i = 0;
    double dblBytes = bytes;

    if (bytes > 1024) {
        for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
            dblBytes = bytes / 1024.0;
    }

    static char output[200];
    sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
    return output;
}

void show_progress(curl_off_t file_length) {
    curl_off_t total_download = 0;
    curl_off_t last_download = 0;
    while (total_download < file_length) {
        total_download = 0;
        pthread_mutex_lock(&tpListLock);
        int active = 0;
        for (Task* task:globalTaskList) {
            total_download += task->download;
            if(task->status == STATUS_RUNNING){
                active++;
            }
        }
        pthread_mutex_unlock(&tpListLock);
        cerr << "workers: " << active << "current speed: " << humanSize(total_download - last_download) << "/s "
             << setprecision(4) << (float) total_download / (float) file_length * 100 << "% " << total_download << " " << globalTaskList.back()->end  <<  "\r";
        last_download = total_download;
        sleep(1);
    }
}

//void show_progress_(curl_off_t file_length){
//    while (true){
//        cerr << "\r";
//        pthread_mutex_lock(&tpListLock);
//        for(Task* task:globalTaskList){
//            cerr << task->start << "-" << task->end << " ";
//        }
//        pthread_mutex_unlock(&tpListLock);
//        sleep(1);
//    }
//}

void start_progress_thread(curl_off_t file_length) {
    new thread(show_progress, file_length);
}