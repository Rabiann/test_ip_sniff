#include "interfaces.h"

char** get_all_interfaces(char* errbuf) {
    pcap_if_t* start;
    pcap_if_t* elem;
    pcap_findalldevs(&start, errbuf);

    int devscount = 0;

    elem = start;
    while (elem != NULL) {
        ++devscount;
        elem = elem->next;
    }

    char** interf = (char**)malloc((devscount+1) * sizeof(char*));
    int i = 0;
    elem = start;
    while (elem != NULL) {
        interf[i] = (char*)malloc(16 * sizeof(char));
        strcpy(interf[i], elem->name);
        ++i;
        elem = elem->next;
    }

    interf[i] = NULL; // making last ptr to point to NULL to find the end
    pcap_freealldevs(start);
    return interf;
}
