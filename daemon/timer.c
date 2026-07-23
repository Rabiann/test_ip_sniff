#include <unistd.h>
#include <string.h>

#include "timer.h"

bool dump_flag = false;
bool timer_running = true;

pthread_mutex_t dump_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dump_cond = PTHREAD_COND_INITIALIZER;

void* timer_loop(void* arg) {
    struct ip_addr_store* store = (struct ip_addr_store*)arg;

    while (true) {
        sleep(DUMP_INTERVAL_SEC);

        pthread_mutex_lock(&dump_mutex);

        if (!timer_running) {
            pthread_mutex_unlock(&dump_mutex);
            break;
        }

        dump_flag = true;
        
        while (dump_flag && timer_running) {
            pthread_cond_wait(&dump_cond, &dump_mutex);
        }

        if (!timer_running) {
            pthread_mutex_unlock(&dump_mutex);
            break;
        }
        
        save_snapshot(store->if_name);
        pthread_mutex_unlock(&dump_mutex);
    }

    return NULL;
}

int start_timer(struct ip_addr_store* store) {
    if (pthread_create(&t_timer, NULL, timer_loop, (void*)store)) {
        return 1;
    }
    return 0;
}

int stop_timer(void) {
    pthread_mutex_lock(&dump_mutex);
    timer_running = false;
    pthread_cond_broadcast(&dump_cond);
    pthread_mutex_unlock(&dump_mutex);

    if (pthread_join(t_timer, NULL)) {
        return 1;
    }
    return 0;
}