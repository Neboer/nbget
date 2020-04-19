#include "http.h"

int progress_callback(Task *task, curl_off_t dltotal, curl_off_t now_download_size, curl_off_t ultotal,
                      curl_off_t ulnow) {
    task->download = now_download_size;
    return 0;
}

void part_download(const string &download_address, const string &file_name, Task *task) {
    // rb+ explanation: open in this mode, the file won't get destroyed. But it requires a long enough file in advance.
    FILE *file = fopen(file_name.c_str(), "rb+");
    // fseeko can take larger param as position up to 64 bytes integer.
    fseeko(file, task->start, SEEK_SET);
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, download_address.c_str());
    char range_string[60]; // maybe 60 is just enough...
    sprintf(range_string, "%ld-%ld", task->start, task->end);
    curl_easy_setopt(curl, CURLOPT_RANGE, range_string);
    curl_easy_setopt(curl, CURLOPT_PROXY, task->use_proxy.address.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    // warning: the download will abort when meet low speed problem.
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, LOWEST_SPEED_BPS);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, task);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    // bytes per second
    int result = curl_easy_perform(curl);
    fclose(file);
    if (result != CURLE_OK) {
        task->status = STATUS_ERROR;
        fprintf(stderr, "\rerror:%s\n", curl_easy_strerror((CURLcode) result));
        task->speed = -1;// this is not enough. Error handling will happens on the top of these code.
    } else {
        task->status = STATUS_COMPLETE;
        curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD_T, &(task->speed));
    }
    taskMessageQueue.push(*task);
    curl_easy_cleanup(curl);
}


void copy_and_low(const char *source, char *dest, size_t length) {
    for (int offset = 0; offset < length; offset++) {
        dest[offset] = (char) tolower(source[offset]);
    }
}


static size_t header_callback(char *buffer, size_t size, size_t nitems, curl_off_t *userdata) {
    char head_data[nitems + 1];
    copy_and_low(buffer, head_data, nitems);
    head_data[nitems - 2] = 0;
    char *head_occur_position = strstr(head_data, "content-length: ");
    if (head_occur_position) {
        *userdata = strtoull(head_occur_position + sizeof("content-length: ") - 1, nullptr, 0);
    }
    return nitems * size;
}

curl_off_t get_file_size(char *download_address) {
    CURL *curl = curl_easy_init();
    curl_off_t total_file_length;
    curl_easy_setopt(curl, CURLOPT_URL, download_address);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &total_file_length);
    curl_easy_perform(curl);
    fputs("body request\n", stderr);
    return total_file_length;
}
