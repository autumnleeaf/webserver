#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H
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

request* create_request();

void free_request(request* req);

#endif //WEBSERVER_REQUEST_H
