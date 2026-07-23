#pragma once

#include <pcap.h>
#include <string.h>
#include <stdlib.h>

#define D_DATA_PATH "/var/lib/snifferd/"
char** get_all_interfaces(char* errbuf);
