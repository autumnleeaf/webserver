#include "util.h"
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
        fprintf(stderr, "Method failed to allocate.\n");
        return nullptr;
    }

    req->target = calloc(MAX_SIZE_TARGET, sizeof(uint8_t));
    if (req->target == NULL) {
        free(req->method);
        free(req);
        fprintf(stderr, "Target failed to allocate.\n");
        return nullptr;
    }

    req->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (req->version == NULL) {
        free(req->method);
        free(req->target);
        free(req);
        fprintf(stderr,"Version failed to allocate.\n");
        return nullptr;
    }

    req->fields = create_hashtable();
    if (req->fields == NULL) {
        free(req->method);
        free(req->target);
        free(req->version);
        free(req);
        fprintf(stderr, "Hash table failed to allocate.\n");
        return nullptr;
    }

    req->body = nullptr;

    return req;
}

response* create_response() {
    response* res = malloc(sizeof(response));
    if (res == NULL) return nullptr;

    res->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (res->version == NULL) {
        free(res);
        fprintf(stderr, "Version failed to allocate.\n");
        return nullptr;
    }

    res->reason = calloc(MAX_SIZE_REASON, sizeof(uint8_t));
    if (res->reason == NULL) {
        free(res->reason);
        free(res);
        fprintf(stderr, "Reason failed to allocate.\n");
        return nullptr;
    }

    return res;
}

void free_request(request* req) {
    free(req->method);
    free(req->target);
    free(req->version);
    free_hashtable(req->fields);
    if (req->body != NULL) free(req->body);
    free(req);
}

void free_response(response* res) {
    free(res->version);
    free(res->reason);
    free(res);
}