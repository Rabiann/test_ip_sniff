#include "setup.h"
#include "timer.h"
#include <stdio.h>
#include <signal.h> 
#include <string.h>

char errbuf[PCAP_ERRBUF_SIZE];
static pcap_t* hdl;
static pthread_t sniffer_thread;
static char curr_if[16];
static struct ip_addr_store store;
static volatile sig_atomic_t sniffer_is_running = 0;

struct ip_addr_store* get_store(void) {
    return &store;
}

static void* start_sniffer(void* arg) {
    (void)arg;

    run_sniffer(hdl, &store, errbuf);
    return NULL;
}

int setup_default_if() {
    char buf_if[16];

    if (get_default_interface(buf_if, errbuf)) {
        fprintf(stderr, "setup_default_if error: no interfaces found\n");
        return 1;
    }
    
    return setup(buf_if);
}

int setup_if(char buf_if[16]) {
    if (!is_if_exist(buf_if, errbuf)) {
        return 1;
    }

    return setup(buf_if);
}

int setup_curr_if() {
    if (curr_if[0] == '\0') {
        return 1;
    }

    return setup(curr_if);
}

int setup(char buf_if[16]) {
    if (sniffer_is_running) {
        fprintf(stderr, "Sniffer is already running. Please stop it first.\n");
        return 1;
    }

    strcpy(curr_if, buf_if); // saving current interface to run it later
    store = create_ip_addr_store(buf_if);
    if (read_store(&store)) {
        return 1;
    }

    hdl = pcap_create(buf_if, errbuf);
    if (hdl == NULL) {
        fprintf(stderr, "pcap_create error: %s\n", errbuf);
        return 1;
    }

    if (pthread_create(&sniffer_thread, NULL, start_sniffer, NULL)) {
        fprintf(stderr, "pthread_create error\n");
        pcap_close(hdl);
        return 1;
    }

    if (start_timer(&store)) {
        fprintf(stderr, "start_timer error\n");
        pcap_breakloop(hdl);
        pthread_join(sniffer_thread, NULL);
        pcap_close(hdl);
        return 1;
    }
    
    sniffer_is_running = 1;
    return 0;
}

int stop_sniffer(void) {
    if (!sniffer_is_running) {
        return 0;
    }

    pcap_breakloop(hdl);
    
    if (pthread_join(sniffer_thread, NULL)) {
        fprintf(stderr, "pthread_join error\n");
    }
    
    pcap_close(hdl);

    if (stop_timer()) {
        fprintf(stderr, "stop_timer error\n");
    }

    make_snapshot(&store);
    save_snapshot(store.if_name);

    free_store(&store);
    sniffer_is_running = 0;

    return 0;
}

bool is_sniffer_running(void) {
    return sniffer_is_running;
}