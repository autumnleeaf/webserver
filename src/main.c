#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"
#include "request_header.h"

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

    uint8_t message[BUFFER];
    const ssize_t bytes_read = recv(new_fd, &message, sizeof(message), 0);
    if (bytes_read == -1) return errno;

    size_t index = 0;
    request_header* req_header = create_req_header();
    if (req_header == NULL) exit(1);

    parse_start_line(req_header, message, &index);
    parse_fields(req_header, message, &index);

    printf("method: %s\n", (char*)req_header->method);
    printf("target: %s\n", (char*)req_header->target);
    printf("version: %s\n", (char*)req_header->version);

    ht_iterator iterator = ht_iterator_init(req_header->fields);
    while (ht_iterator_next(&iterator)) {
        printf("key: %s, value: %s\n", iterator.key, (char *)iterator.value);
    }

    free_req_header(req_header);
    return 0;
}
