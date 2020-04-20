cmake_minimum_required(VERSION 3.16)
project(nbpet)

set(CMAKE_CXX_STANDARD 14)

add_executable(nbpet main.cpp task.cpp http.cpp arranger.cpp progress.cpp)