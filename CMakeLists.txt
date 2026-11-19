cmake_minimum_required(VERSION 4.3)
project(webserver C)

set(CMAKE_C_STANDARD 23)

include_directories(include)

add_executable(webserver src/main.c
        src/env/hashtable.c
        include/hashtable.h
        src/net/request.c
        include/request.h
        src/http/http_parser.c
        include/http_parser.h
        include/definitions.h)
