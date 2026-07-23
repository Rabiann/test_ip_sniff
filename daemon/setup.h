#pragma once

#include <pthread.h>
#include <stdbool.h>
#include "sniffer.h"

extern char errbuf[PCAP_ERRBUF_SIZE];

int setup_default_if();
int setup_if(char[16]);
int setup_curr_if();
int setup(char[16]);
int stop_sniffer(void);
bool is_sniffer_running(void);