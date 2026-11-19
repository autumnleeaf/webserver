#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

int listener(const char* host, const char* port);

static void handle_request(const char* message);

#endif //WEBSERVER_LISTENER_H
