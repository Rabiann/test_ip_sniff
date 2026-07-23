#pragma once

#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdbool.h>

#include "stat_counter.h"

#define EN_HDR_LEN    14
#define IPv4_SRC_ADDR_PADD 12

typedef unsigned char u_char;

void print_packet_info(u_char *user, 
    const struct pcap_pkthdr *h, 
    const u_char *bytes);

struct user_params {
    int padding;
    struct ip_addr_store* store;
};

void show_all_interfaces(char* errbuf);
int run_sniffer(pcap_t*, struct ip_addr_store*, char*);
int get_default_interface(char if_buf[15], char* errbuf);
bool is_if_exist(char if_buf[15], char* errbuf);