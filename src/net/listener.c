#include "listener.h"
#include "definitions.h"
#include "util.h"
#include "http_parser.h"

#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int listener(const char* host, const char* port) {
    struct addrinfo* serv_info = get_serve_info(host, port);

    int sockfd = get_bound_sockfd(serv_info);

    wait_for_requests(sockfd);

    return 0;
}

static struct addrinfo* get_serve_info(const char* host, const char* port) {
    struct addrinfo hints = {0}, *serv_info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int success = getaddrinfo(host, port, &hints, &serv_info);
    if (success == -1) {
        fprintf(stderr, "Get Address Info: %s\n", gai_strerror(success));
        exit(1);
    }

    return serv_info;
}

static int get_bound_sockfd(struct addrinfo* serv_info) {
    int sockfd = 0, reuse_addr = 1;
    struct addrinfo *p_info;
    for (p_info = serv_info; p_info != NULL; p_info = p_info->ai_next) {
        if ((sockfd = socket(p_info->ai_family, p_info->ai_socktype, p_info->ai_protocol)) == -1) {
            fprintf(stderr, "Socket Error: %s\n", strerror(errno));
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1) {
            close(sockfd);
            fprintf(stderr, "Setsockopt Error: %s\n", strerror(errno));
            exit(1);
        }

        if (bind(sockfd, p_info->ai_addr, p_info->ai_addrlen) == -1) {
            close(sockfd);
            fprintf(stderr, "Bind Error: %s\n", strerror(errno));
            continue;
        }

        break;
    }

    freeaddrinfo(serv_info);

    if (p_info == NULL) {
        fprintf(stderr, "Failed to bind to port.\n");
        exit(1);
    }

    return sockfd;
}

static void wait_for_requests(const int sockfd) {
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int new_fd = 0;

    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "Listen Error: %s\n", strerror(errno));
        exit(1);
    }

    while (true) {
        addr_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &addr_size);
        if (new_fd == -1) {
            fprintf(stderr, "Accept Error: %s\n", strerror(errno));
            continue;
        }

        handle_request(new_fd);
        close(new_fd);
    }
}

static void handle_request(const int new_fd) {
    char message[BUFFER];
    const ssize_t bytes_read = recv(new_fd, &message, sizeof(message), 0);
    if (bytes_read == -1) {
        fprintf(stderr, "Receive Error: %s\n", strerror(errno));
        exit(1);
    }
    request* req = parse_http_request(message);

    printf("method: %s\n", (char*)req->method);
    printf("target: %s\n", (char*)req->target);
    printf("version: %s\n", (char*)req->version);

    ht_iterator iterator = ht_iterator_init(req->fields);
    while (ht_iterator_next(&iterator)) {
        printf("key: %s, value: %s\n", iterator.key, (char *)iterator.value);
    }

    printf("\n");

    if (req->body_length > 0) {
        printf("body length: %d\n", (int)req->body_length);
        printf("body: %s\n", (char*)req->body);
    }

    free_request(req);
}