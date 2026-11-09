#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

#define BUFFER_SIZE 1024
#define PORT "3490"

int main() {
    struct addrinfo hints = {0}, *res;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int success = getaddrinfo("localhost", PORT, &hints, &res);
    if (success == -1) return errno;

    const int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    success = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (success == -1) return errno;

    char buffer[BUFFER_SIZE];
    const ssize_t bytes_read = recv(sockfd, &buffer, sizeof(buffer), 0);
    if (bytes_read == -1) return errno;
    if (bytes_read >= 1) {
        printf("Received %d bytes:\n", (int)bytes_read);
        printf("%s\n", buffer);
    }
    return 0;
}