#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "../common.h"

#define ER_ALRYR -2
#define ER_FSTAR -3
#define ER_NOTRG -4
#define ER_FSTOP -5
#define ER_UNKNC -100

void* run_server(void* arg);
void stop_server(void);

ipc_response handle_command(struct ipc_request req);