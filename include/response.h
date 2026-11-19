#ifndef WEBSERVER_RESPONSE_H
#define WEBSERVER_RESPONSE_H
#include <stdint.h>

typedef struct {
    uint8_t* version;
    uint16_t status;
    uint8_t* reason;
} response;

response* create_response();

void free_response(response* response);

#endif //WEBSERVER_RESPONSE_H
