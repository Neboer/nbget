#include <iostream>
#include "task.h"
#include "arranger.h"
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
}

int main(int argc, char **argv) {
    argument args = parser(argc, argv);
    initialize_proxy_list(args.proxy_list, args.proxy_count);
    initialize_task_list(args.file_name, args.download_address);
    
}
