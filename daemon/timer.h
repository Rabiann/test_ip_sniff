#pragma once

#include <pthread.h>
#include <stdbool.h>

#include "sniffer.h"

#define DUMP_INTERVAL_SEC 5

extern bool dump_flag;
extern bool timer_running;

extern pthread_mutex_t dump_mutex;
extern pthread_cond_t dump_cond;


void* timer_loop(void* arg);
int start_timer(struct ip_addr_store* store);
int stop_timer(void);