#include "sniffer.h"

void print_packet_info(u_char *user,
    const struct pcap_pkthdr *h,
    const u_char *bytes)
{
    struct user_params* params = (struct user_params*)user;
    int padding = params->padding;
    struct ip_addr_store* store = params->store;

    uint32_t addr = (uint32_t)(bytes[padding + 3]) |
        (uint32_t)(bytes[padding+2] << 8) |
        (uint32_t)(bytes[padding+1] << 16) |
        (uint32_t)(bytes[padding] << 24);

    inc(store, addr);

    // check flag
    // build flat list
    // signal
}


void show_all_interfaces(char* errbuf) {
    pcap_if_t* start;
    pcap_if_t* elem;
    pcap_findalldevs(&start, errbuf);

    elem = start;
    while (elem != NULL) {
        printf("%s\n", elem->name);
        elem = elem->next;
    }
    pcap_freealldevs(start);
}

int run_sniffer(pcap_t* handle, struct ip_addr_store* store, char* errbuf) {    
    if (pcap_activate(handle)) {
        pcap_perror(handle, "pcap_activate error: ");
        return 1;
    }

    if (pcap_setdirection(handle, PCAP_D_IN)) {
        pcap_perror(handle, "pcap_setdirection error: ");
        return 1;
    }
    
    int padding;
    int format = pcap_datalink(handle);

    switch (format) {
        case DLT_EN10MB:
            padding = EN_HDR_LEN + IPv4_SRC_ADDR_PADD;
            break;
        default:
            fprintf(stderr, "pcap_datalink error: unsupported protocol\n");
            return 1;
    }

    struct user_params params = {
        .padding = padding,
        .store = store
    };

    if (pcap_loop(
        handle,
        0,
        print_packet_info,
        (u_char*)&params
    )) {
        pcap_perror(handle, "pcap_loop error: ");
        pcap_close(handle);
        return 1;
    }

    return 0;
}