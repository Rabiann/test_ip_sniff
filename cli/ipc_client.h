#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SOCKET_NAME           "/tmp/sniffer.sock"
#define ERROR_D_NOT_STARTED   111

struct ipc_request {
    char cmd;
    char data[15];
};

typedef int ipc_response;

ipc_response run_client(struct ipc_request req);

int start_d();