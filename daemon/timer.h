#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "stat_counter.h"

#define DUMP_INTERVAL_SEC 5

atomic_bool dump_flag = false;
static atomic_bool timer_running = true;

static pthread_t t_timer;

static void* timer_loop(void* arg);
int start_timer(void);
int stop_timer(void);