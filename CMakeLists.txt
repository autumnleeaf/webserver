cmake_minimum_required(VERSION 4.3)
project(webserver C)

set(CMAKE_C_STANDARD 23)

include_directories(headers)

add_executable(webserver src/main.c
        src/hashtable.c
        headers/hashtable.h
        src/request_header.c
        headers/request_header.h)
