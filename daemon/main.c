#include <pcap.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "sniffer.h"
#include "stat_counter.h"
#include "timer.h"

// Global handle for pcap session
pcap_t* hdl;

// Struct to pass arguments to the sniffer thread
struct sniffer_thread_params {
    struct ip_addr_store* store;
    char* errbuf;
};

// Signal handler for graceful shutdown
void shutdown_handler(int signum) {
    printf("\nCaught signal %d, shutting down...\n", signum);
    if (hdl) {
        pcap_breakloop(hdl);
    }
}

// Thread function to run the sniffer
void* sniffer_thread_func(void* args) {
    struct sniffer_thread_params* params = (struct sniffer_thread_params*)args;
    run_sniffer(hdl, params->store, params->errbuf);
    return NULL;
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    char* dev = "wlp0s20f3";

    // Initialize pcap
    pcap_init(PCAP_CHAR_ENC_UTF_8, errbuf);
    hdl = pcap_create(dev, errbuf);
    if (hdl == NULL) {
        fprintf(stderr, "pcap_create() failed: %s\n", errbuf);
        return 1;
    }

    // Setup signal handlers for graceful shutdown
    signal(SIGINT, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    // Create the statistics store
    struct ip_addr_store store = create_ip_addr_store(dev);

    // Start the timer thread
    if (start_timer(&store) != 0) {
        fprintf(stderr, "Failed to start timer thread.\n");
        pcap_close(hdl);
        return 1;
    }

    // Create and start the sniffer thread
    pthread_t sniffer_thread;
    struct sniffer_thread_params sniffer_params = { .store = &store, .errbuf = errbuf };
    if (pthread_create(&sniffer_thread, NULL, sniffer_thread_func, &sniffer_params) != 0) {
        fprintf(stderr, "Failed to start sniffer thread.\n");
        stop_timer();
        pcap_close(hdl);
        return 1;
    }

    printf("Sniffer started on %s. Press Ctrl+C to stop.\n", dev);

    // Wait for the sniffer thread to finish (it will be broken by the signal handler)
    pthread_join(sniffer_thread, NULL);

    printf("Sniffer thread stopped. Finalizing...\n");

    // Stop the timer thread gracefully
    printf("Stopping timer thread...\n");
    stop_timer();

    // Now that all threads are stopped, perform one final save to prevent data loss.
    printf("Performing final save...\n");
    make_snapshot(&store);
    save_snapshot(store.if_name);

    printf("Cleanup complete. Exiting.\n");

    // Final cleanup
    pcap_close(hdl);
    pthread_mutex_destroy(&dump_mutex);
    pthread_cond_destroy(&dump_cond);
    free_store(&store);


    return 0;
}