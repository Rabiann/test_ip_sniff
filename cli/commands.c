#include "commands.h"
#include "interfaces.h"
#include <stdlib.h>
#include <errno.h>
#include <search.h>

static char* Start_cmd  = "start";
static char* Stop_cmd   = "stop";
static char* Show_cmd1  = "show";
static char* Show_cmd2  = "count";
static char* Select_cmd1 = "select";
static char* Select_cmd2 = "iface";
static char* Show_stat  = "stat";
static char* Show_help  = "--help";

static int ip_str_cmp(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

static void free_ip_node(void *nodep) {
    free(*(char **)nodep);
}

int parse_command(struct ipc_request* req, int argc, char* argv[]) {
    if (argc < 2)
        show_help();

    if (argc == 2 && strcmp(argv[1], Start_cmd) == 0) { 
        req->cmd = Start;
        return 1;
    }

    if (argc == 2 && strcmp(argv[1], Stop_cmd) == 0) {
        req->cmd = Stop;
        return 1;
    }

    if (argc == 4 && strcmp(argv[1], Show_cmd1)  == 0 &&
        strcmp(argv[3], Show_cmd2) == 0) {
        show_ip_count(argv[2]);
        return 1;
    }

    if (argc == 4 && strcmp(argv[1], Select_cmd1)  == 0 &&
        strcmp(argv[2], Select_cmd2) == 0) {

        req->cmd = SelectIf;
        strncpy(req->data, argv[2], sizeof(req->data) - 1);
        req->data[sizeof(req->data) - 1] = 0;
        return 1;
    }

    if (argc < 4 && strcmp(argv[1], Show_stat) == 0) {
        if (argc == 3) {
            show_address_stats(argv[2]);   
        } else {
            show_all_stats();
        }

        return 1;
    } 

    if (argc == 2 && strcmp(argv[1], Show_help) == 0) {
        show_help();
    } 

    show_help();
    return 0;
}

void show_ip_count(char ip_addr[17]) {
    char errbuf[PCAP_ERRBUF_SIZE];
    char** interfaces = get_all_interfaces(errbuf);
    if (interfaces == NULL) {
        fprintf(stderr, "Error getting interfaces: %s\n", errbuf);
        return;
    }

    long unsigned int total_count = 0;

    for (int i = 0; interfaces[i] != NULL; i++) {
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "%ssf.%s.csv", D_DATA_PATH, interfaces[i]);

        FILE* fp = fopen(file_path, "r");
        if (fp == NULL) {
            if (errno == ENOENT) {
                continue; 
            }
            perror("fopen");
            continue;
        }

        char line[256];
        if (fgets(line, sizeof(line), fp) == NULL) {
            fclose(fp);
            continue;
        }

        char read_ip[17];
        long unsigned int count;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%16[^,],%lu", read_ip, &count) == 2) {
                if (strcmp(read_ip, ip_addr) == 0) {
                    total_count += count;
                }
            }
        }
        fclose(fp);
    }

    for (int i = 0; interfaces[i] != NULL; i++) {
        free(interfaces[i]);
    }
    free(interfaces);

    printf("Total packets for %s: %lu\n", ip_addr, total_count);
}

void show_all_stats() {
    char errbuf[PCAP_ERRBUF_SIZE];
    char** interfaces = get_all_interfaces(errbuf);
    if (interfaces == NULL) {
        fprintf(stderr, "Error getting interfaces: %s\n", errbuf);
        return;
    }

    void *root = NULL;
    long unsigned int total_packets = 0;
    long unsigned int unique_ips = 0;

    for (int i = 0; interfaces[i] != NULL; i++) {
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "%ssf.%s.csv", D_DATA_PATH, interfaces[i]);

        FILE* fp = fopen(file_path, "r");
        if (fp == NULL) {
            if (errno == ENOENT) continue;
            perror("fopen");
            continue;
        }

        char line[256];
        if (fgets(line, sizeof(line), fp) == NULL) {
            fclose(fp);
            continue;
        }

        char ip_buf[17];
        long unsigned int count;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "%16[^,],%lu", ip_buf, &count) == 2) {
                total_packets += count;

                char *ip_key = strdup(ip_buf);
                if (ip_key == NULL) {
                    perror("strdup");
                    continue;
                }

                void *node = tsearch(&ip_key, &root, ip_str_cmp);
                if (node != NULL) {
                    if (*(char**)node == ip_key) {
                        unique_ips++;
                    } else {
                        free(ip_key);
                    }
                } else {
                    perror("tsearch");
                    free(ip_key);
                }
            }
        }
        fclose(fp);
    }

    printf("Total unique IP addresses: %lu\n", unique_ips);
    printf("Total packets received: %lu\n", total_packets);

    for (int i = 0; interfaces[i] != NULL; i++) {
        free(interfaces[i]);
    }
    free(interfaces);

    if (root != NULL) {
        tdestroy(root, free_ip_node);
    }
}

void show_address_stats(char* iface) {
    void *root = NULL;
    long unsigned int total_packets = 0;
    long unsigned int unique_ips = 0;

    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%ssf.%s.csv", D_DATA_PATH, iface);

    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            fprintf(stderr, "No statistics found for interface '%s'\n", iface);
            return;
        }
        perror("fopen");
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return;
    }

    char ip_buf[17];
    long unsigned int count;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%16[^,],%lu", ip_buf, &count) == 2) {
            total_packets += count;
            
            char *ip_key = strdup(ip_buf);
            if (ip_key == NULL) {
                perror("strdup");
                continue;
            }

            void *node = tsearch(&ip_key, &root, ip_str_cmp);
            if (node != NULL) {
                if (*(char**)node == ip_key) {
                    unique_ips++;
                } else {
                    free(ip_key);
                }
            } else {
                perror("tsearch");
                free(ip_key);
            }
        }
    }
    fclose(fp);

    printf("Statistics for interface '%s':\n", iface);
    printf("  Total unique IP addresses: %lu\n", unique_ips);
    printf("  Total packets received: %lu\n", total_packets);

    if (root != NULL) {
        tdestroy(root, free_ip_node);
    }
}

void show_help() {
    printf("Snifferd - IP sniffer service\n\n"
           "Usage:\n"
           "  start                  - Start sniffing on the default interface\n"
           "  stop                   - Stop sniffing\n"
  		   "  show [ip] count        - Print the number of packets received from an IP address\n"
           "  select iface [iface]   - Select an interface for sniffing\n"
           "  stat [iface]           - Show statistics for a particular interface, or for all if omitted\n"
           "  --help                 - Show this help information\n");
}