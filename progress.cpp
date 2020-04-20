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
        pthread_mutex_lock(&tpListLock);
        int proxy_count = globalProxyList.size();
        for (Task &task:globalTaskList) {
            total_download += task.download;
        }
        pthread_mutex_unlock(&tpListLock);
        cerr << "workers: " << proxy_count << "current speed: " << humanSize(total_download - last_download) << "/s "
             << setprecision(1) << (float) total_download / (float) file_length << "%\r";
        last_download = total_download;
        sleep(1);
    }
}

void start_progress_thread(curl_off_t file_length) {
    new thread(show_progress, file_length);
}