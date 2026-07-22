#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_NAME "/tmp/sniffer.sock"

struct ipc_request {
    char cmd;
    char data[15];
};

typedef int ipc_response;

int run_server();

ipc_response handle_command(struct ipc_request req);