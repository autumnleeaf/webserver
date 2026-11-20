#ifndef WEBSERVER_UTIL_H
#define WEBSERVER_UTIL_H
#include <stdint.h>

#include "hashtable.h"

typedef enum {
    ORIGIN_FORM,
    ABSOLUTE_FORM,
    AUTHORITY_FORM,
    ASTERISK_FORM
} request_target_type;

typedef struct {
    uint8_t* method;
    request_target_type target_type;
    uint8_t* target;
    uint8_t* version;
    hashtable* fields;
    uint8_t* body;
    size_t body_length;
} request;

typedef struct {
    uint8_t* version;
    uint16_t status;
    uint8_t* reason;
} response;

request* create_request();

response* create_response();

void free_request(request* req);

void free_response(response* response);

#endif //WEBSERVER_UTIL_H
