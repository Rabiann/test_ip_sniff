#include "setup.h"


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

int setup(char buf_if[16]) {
    pcap_t* hdl;
    struct ip_addr_store store = create_ip_addr_store(buf_if);

    hdl = pcap_create(buf_if, errbuf);
    
}