#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <search.h>
#include "sniffer.h"

#define INIT_DARRAY_CAP 32

struct ip_addr_darray {
    uint32_t len;
    uint32_t cap;
    struct ip_stats* ptr;
};

struct ip_addr_store create_ip_addr_store(char if_name[16]);
int inc(struct ip_addr_store* store, uint32_t ip_addr);

struct ip_addr_darray create_ip_addr_darray();
int push(struct ip_addr_darray* vec, struct ip_stats stats);

struct ip_addr_store {
    char if_name[16];
    void* root;
    struct ip_addr_darray vec;
};

int compare_fn(void*, void*);