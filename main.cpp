#include <iostream>
#include "task.h"
#include "arranger.h"
#include "progress.h"
#include "http.h"
#include <vector>
#include <string>

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
            string(argv[1]),
            string(argv[argc - 1])

    };
    return argument;
}

void create_file(const char *filename, curl_off_t size) {
    FILE *file = fopen(filename, "wb");
    if (file == nullptr) {
        fputs("create file error!", stderr);
        exit(-1);
    }
    fseeko(file, (__off_t) (size - 1), SEEK_SET);
    fwrite("\0", 1, 1, file);
    fclose(file);
}

int main(int argc, char **argv) {
    argument args = parser(argc, argv);
    initialize_proxy_list(args.proxy_list, args.proxy_count);
    curl_off_t file_length = get_file_size(args.download_address.c_str());
    create_file(args.file_name.c_str(),file_length);
    initialize_task_list(args.file_name, args.download_address, file_length);
    start_progress_thread(file_length);
    message_loop(args.file_name, args.download_address, file_length);
    for (Task* &task:globalTaskList) {
        (*task).wait();
    }
}
