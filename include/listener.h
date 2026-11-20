#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

int listener(const char* host, const char* port);

static struct addrinfo* get_serve_info(const char* host, const char* port);

static int get_bound_sockfd(struct addrinfo* serv_info);

static void wait_for_requests(int sockfd);

static void handle_request(const char* message);

#endif //WEBSERVER_LISTENER_H
