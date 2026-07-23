#pragma once

#include "ipc_client.h"
#include <stdio.h>

enum Command {
    Start     = 0,
    Stop      = 1,
    SelectIf  = 2,  
};

int parse_command(struct ipc_request* req, int argc, char* argv[]);
void show_help();

void show_address_stats(char* iface);
void show_all_stats();
void show_ip_count(char ip_addr[17]);