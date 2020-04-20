cmake_minimum_required(VERSION 3.16)
project(nbpet)

set(CMAKE_CXX_STANDARD 17)
include_directories(libcurl)
find_library(curl libcurl)
find_library(pthread pthread)
link_libraries(curl pthread)
add_executable(nbpet main.cpp task.cpp http.cpp arranger.cpp progress.cpp)