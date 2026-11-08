#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>

#define BACKLOG 10
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

    return 0;
}
