#pragma once

#include "sniffer.h"

extern char errbuf[PCAP_ERRBUF_SIZE];

int setup_default_if();
int setup_if(char[15]);
int setup(char[15]); 
static void* start_sniffer();