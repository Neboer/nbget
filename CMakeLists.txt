cmake_minimum_required(VERSION 3.16)
project(nbpet)

set(CMAKE_CXX_STANDARD 14)

add_executable(nbpet main.cpp task.cpp task.h http.cpp http.h arranger.cpp arranger.h)