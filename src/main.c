#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/hashtable.h"

#define BACKLOG 10
#define BUFFER 2048
#define PORT "3490"

int main() {
    struct sockaddr_storage other_addr;
    socklen_t addr_size;
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int success = getaddrinfo("localhost", PORT, &hints, &res);
    if (success == -1) return errno;

    const int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    success = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (success == -1) return errno;

    success = listen(sockfd, BACKLOG);
    if (success == -1) return errno;

    addr_size = sizeof other_addr;
    const int new_fd = accept(sockfd, (struct sockaddr*)&other_addr, &addr_size);

    char message[BUFFER];
    const ssize_t bytes_read = recv(new_fd, &message, sizeof(message), 0);
    if (bytes_read == -1) return errno;
    // Skip initial line until CRLF
    size_t index = 0;
    while (message[index] != '\r' && message[index + 1] != '\n') index++;
    index += 2;

    // Store headers in a hash table
    hash_table* header_fields = create_hash_table();
    if (header_fields == NULL) return 1;

    while (message[index] != '\r' && message[index + 1] != '\n') {
        // Search for the colon to separate the key and value
        size_t start_index = index;
        while (message[index] != ':') index++;
        char *key = calloc(index - start_index, sizeof(char));
        strncpy(key, message + start_index, index - start_index);
        index += 2;
        start_index = index;
        while (message[index] != '\r' && message[index + 1] != '\n') index++;
        char *value = calloc(index - start_index, sizeof(char));
        strncpy(value, message + start_index, index - start_index);
        hash_table_set(header_fields, key, value);
        index += 2;
    }

    free_hash_table(header_fields);
    return 0;
}
