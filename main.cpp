#include <iostream>
#include "task.h"
#include "arranger.h"
#include "progress.h"
#include "http.h"

struct argument {
    char **proxy_list;
    int proxy_count;
    string download_address;
    string file_name;
};

argument parser(int argc, char **argv) {
    argument argument = {
            argv + 2,
            argc - 3,
            string(argv[1], strlen(argv[1])),
            string(argv[argc - 1], strlen(argv[argc - 1]))

    };
    return argument;
}

void create_file(const char *filename, curl_off_t size) {
    FILE *file = fopen(filename, "wb");
    if (file == nullptr) {
        fputs("create file error!", stderr);
        exit(-1);
    }
    fseeko(file, size - 1, SEEK_SET);
    fwrite("\0", 1, 1, file);
    fclose(file);
}

int main(int argc, char **argv) {
    curl_global_init(CURL_GLOBAL_ALL);
    download_status Status{};// Status是全局的，包含当前下载的所有状态
    argument args = parser(argc, argv);
    initialize_proxy_list(args.proxy_list, args.proxy_count, &Status);
    curl_off_t file_length = get_file_size(args.download_address, Status.globalProxyList.front().address);
    create_file(args.file_name.c_str(), file_length);
    initialize_task_list(args.file_name, args.download_address, file_length, &Status);
    start_progress_thread(file_length, &Status);
    message_loop(args.file_name, args.download_address, file_length, &Status);
    for (Task *&task:Status.globalTaskList) {
        (*task).wait();
    }
    curl_global_cleanup();
}
