#include "request.h"
#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

request* create_request() {
    request* req = malloc(sizeof(request));
    if (req == NULL) return nullptr;

    req->method = calloc(MAX_SIZE_METHOD, sizeof(uint8_t));
    if (req->method == NULL) {
        free(req);
        printf("Method failed to allocate.\n");
        return nullptr;
    }

    req->target = calloc(MAX_SIZE_TARGET, sizeof(uint8_t));
    if (req->target == NULL) {
        free(req->method);
        free(req);
        printf("Target failed to allocate.\n");
        return nullptr;
    }

    req->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (req->version == NULL) {
        free(req->method);
        free(req->target);
        free(req);
        printf("Version failed to allocate.\n");
        return nullptr;
    }

    req->fields = create_hashtable();
    if (req->fields == NULL) {
        free(req->method);
        free(req->target);
        free(req->version);
        free(req);
        printf("Hash table failed to allocate.\n");
        return nullptr;
    }

    req->body = nullptr;

    return req;
}

void free_request(request* req) {
    free(req->method);
    free(req->target);
    free(req->version);
    free_hashtable(req->fields);
    if (req->body != NULL) free(req->body);
    free(req);
}