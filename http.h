#pragma once

#include <curl/curl.h>
#include <string>
#include <cstdio>
#include <cstring>
#include "task.h"
#include "arranger.h"

#define LOWEST_SPEED_BPS 100
using namespace std;

void
part_download(const string &download_address, const string &file_name, Task *task, queue<Task *> *taskMessageQueue);

curl_off_t get_file_size(const string &download_address, const string &proxy);
