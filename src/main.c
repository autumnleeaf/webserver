#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

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
    if (bytes_read >= 1) {
        size_t bytes_sent = send(new_fd, message, bytes_read, 0);
        if (bytes_sent == -1) return errno;
        size_t total_bytes_sent = bytes_sent;
        while (total_bytes_sent < bytes_read) {
            char new_message[BUFFER];
            strncpy(new_message, message + total_bytes_sent, bytes_read - total_bytes_sent);
            bytes_sent = send(new_fd, &new_message, bytes_read - bytes_sent, 0);
            if (bytes_sent == -1) return errno;
            total_bytes_sent += bytes_sent;
        }
    }

    return 0;
}
