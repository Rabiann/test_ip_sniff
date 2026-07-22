#include "sniffer.h"

void print_packet_info(u_char *user,
    const struct pcap_pkthdr *h,
    const u_char *bytes)
{
    struct user_params* params = (struct user_params*)user;
    int padding = params->padding;

    uint32_t addr = (uint32_t)(bytes[padding + 3]) |
        (uint32_t)(bytes[padding+2] << 8) |
        (uint32_t)(bytes[padding+1] << 16) |
        (uint32_t)(bytes[padding] << 24);

    printf("%u.%u.%u.%u\n", (unsigned char)((addr & 0xff000000) >> 24),
        (unsigned char)((addr & 0x00ff0000) >> 16),
        (unsigned char)((addr & 0x0000ff00) >> 8),
        (unsigned char)(addr & 0x000000ff));
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

int run_sniffer(pcap_t* handle, char* errbuf) {    
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
        .padding = padding
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