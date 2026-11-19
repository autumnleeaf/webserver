#include "response.h"
#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>

response* create_response() {
    response* res = malloc(sizeof(response));
    if (res == NULL) return nullptr;

    res->version = calloc(MAX_SIZE_VERSION, sizeof(uint8_t));
    if (res->version == NULL) {
        free(res);
        printf("Version failed to allocate.\n");
        return nullptr;
    }

    res->reason = calloc(MAX_SIZE_REASON, sizeof(uint8_t));
    if (res->reason == NULL) {
        free(res->reason);
        free(res);
        printf("Reason failed to allocate.\n");
        return nullptr;
    }

    return res;
}

void free_response(response* res) {
    free(res->version);
    free(res->reason);
    free(res);
}