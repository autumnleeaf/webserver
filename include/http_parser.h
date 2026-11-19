#ifndef WEBSERVER_HTTP_PARSER_H
#define WEBSERVER_HTTP_PARSER_H
#include <stddef.h>
#include <stdint.h>

#include "hashtable.h"
#include "request.h"

typedef enum {
    CHUNKED,
    COMPRESS,
    DEFLATE,
    GZIP,
    IDENTITY,
    TRAILERS,
    X_COMPRESS,
    X_GZIP
} transfer_encoding;

request* parse_http_request(const char* raw_data);

static int parse_start_line(uint8_t* method, uint8_t* target, uint8_t* version, const char* raw_data);

static int parse_fields(hashtable* fields, const char* raw_data);

static size_t parse_message_body(uint8_t* body, const hashtable* fields, const char* raw_data);

static size_t get_encodings(uint8_t* data, transfer_encoding* encodings);

static size_t decode_body(uint8_t* body, const transfer_encoding* encodings, size_t length, const char* raw_data);

static size_t chunk_body(uint8_t* body, const char* raw_data);

static void read_chunk_size(size_t* chunk_size);

#endif
