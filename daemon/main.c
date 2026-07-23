#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "setup.h"
#include "ipc_server.h"

volatile sig_atomic_t running = 1;

void shutdown_handler(int signum) {
    (void)signum;
    running = 0;
}

int main() {
    pthread_t ipc_thread;
    
    signal(SIGINT, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    if (setup_default_if()) {
        fprintf(stderr, "Failed to setup default interface on first run.\n");
        return 1;
    }

    if (pthread_create(&ipc_thread, NULL, run_server, NULL)) {
        fprintf(stderr, "Failed to create IPC server thread.\n");
        stop_sniffer();
        return 1;
    }

    // Keep the main thread alive until a shutdown signal is received
    while (running) {
        sleep(1);
    }

    // Stop sniffer and IPC server
    if (stop_sniffer()) {
        fprintf(stderr, "Failed to stop sniffer gracefully.\n");
    }

    stop_server();
    if (pthread_join(ipc_thread, NULL)) {
        fprintf(stderr, "Failed to join IPC server thread.\n");
    }

    return 0;
}