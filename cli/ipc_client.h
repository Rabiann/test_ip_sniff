#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../common.h"

ipc_response run_client(struct ipc_request req);

int start_d();