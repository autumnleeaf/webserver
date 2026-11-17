#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H
#include <stdint.h>

#include "hashtable.h"

#define MAX_SIZE_METHOD 8
#define MAX_SIZE_TARGET 8000
#define MAX_SIZE_VERSION 8

typedef enum {
    ORIGIN_FORM,
    ABSOLUTE_FORM,
    AUTHORITY_FORM,
    ASTERISK_FORM
} request_target_type;

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

typedef struct {
    uint8_t* method;
    request_target_type target_type;
    uint8_t* target;
    uint8_t* version;
    hashtable* fields;
    uint8_t* body;
    size_t body_length;
} request;

request* create_request();

void free_request(request* req);

size_t parse_message_body(request* req, const uint8_t* message, size_t index);

static size_t get_encodings(uint8_t* data, transfer_encoding* encodings);

static size_t decode_body(request* req, const uint8_t* message, const transfer_encoding* encodings, size_t length);

static size_t chunk_body(request* req, const uint8_t* message);

static void read_chunk_size(size_t* chunk_size);

bool parse_start_line(const request* req, const uint8_t* message, size_t* index);

bool parse_fields(const request* req, const uint8_t* message, size_t* index);

#endif //WEBSERVER_REQUEST_H
