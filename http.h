#pragma once
#include <curl/curl.h>
#include <string>
#include <cstdio>
#include <cstring>
#include "task.h"
#include "arranger.h"

#define LOWEST_SPEED_BPS 10
using namespace std;

void part_download(const string &download_address, const string &file_name, Task *task);

curl_off_t get_file_size(const char *download_address);
