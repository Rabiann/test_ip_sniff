#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SOCKET_NAME "/tmp/sniffer.sock"
#define ER_ALRYR -2
#define ER_FSTAR -3
#define ER_NOTRG -4
#define ER_FSTOP -5
#define ER_UNKNC -100

struct ipc_request {
    char cmd;
    char data[15];
};

enum Command {
    Start     = 0,
    Stop      = 1,
    SelectIf  = 2,  
};

typedef int ipc_response;

int run_server();
void stop_server(void);

ipc_response handle_command(struct ipc_request req);