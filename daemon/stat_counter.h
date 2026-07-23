#pragma once
#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <search.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "common.h"

#define INIT_DARRAY_CAP 32
#define D_DATA_PATH "/var/lib/snifferd/"
#define MAX_LINE_LEN 32

struct ip_addr_store create_ip_addr_store(char if_name[16]);
int inc(struct ip_addr_store* store, uint32_t ip_addr);

struct ip_addr_store {
    char if_name[16];
    uint32_t tree_size;
    void* root;
};

extern struct ip_stats** stats_arr;
extern uint32_t stats_arr_idx;

int compare_fn(const void*, const void*);
void action_fn(const void*, VISIT, int);
void action_free(const void*, VISIT, int);
void free_node(void *nodep);

int free_store(struct ip_addr_store* store);

int create_folder(const char* path);

int make_snapshot(const struct ip_addr_store* store);
int save_snapshot(const char* if_name);
int read_store(struct ip_addr_store* store);