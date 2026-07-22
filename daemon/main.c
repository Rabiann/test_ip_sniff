#include "sniffer.h"

char errbuf[PCAP_ERRBUF_SIZE];

int main() {
    pcap_init(PCAP_CHAR_ENC_UTF_8, errbuf);
    pcap_t* hdl;
    char* dev = "wlp0s20f3";

    hdl = pcap_create(dev, errbuf);

    run_sniffer(hdl, errbuf);
    return 0;
}