#pragma once

#include <stdint.h>

#define D_DATA_PATH "/var/lib/snifferd/"
#define SOCKET_NAME "/tmp/sniffer.sock"

struct ip_stats {
    uint32_t src_addr;
    uint32_t count;
};

enum Command {
    Start     = 0,
    Stop      = 1,
    SelectIf  = 2,  
};

struct ipc_request {
    char cmd;
    char data[16];
};

typedef int ipc_response;
