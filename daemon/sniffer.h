#pragma once

#include <stdio.h>
#include <pcap.h>
#include <string.h>

#define EN_HDR_LEN    14
#define IPv4_SRC_ADDR_PADD 12

typedef unsigned char u_char;

struct user_params {
    int padding;
};

void print_packet_info(u_char *user, 
    const struct pcap_pkthdr *h, 
    const u_char *bytes);

struct ip_stats {
    // stored in network byte order, so when printing, convert to host byte order
    uint32_t src_addr;
    uint32_t count;
};

void show_all_interfaces(char* errbuf);
int run_sniffer(pcap_t*, char*);

