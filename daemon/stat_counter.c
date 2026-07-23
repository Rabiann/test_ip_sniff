#include "stat_counter.h"
#include <string.h>

struct ip_stats** stats_arr;
uint32_t stats_arr_idx;

struct ip_addr_store create_ip_addr_store(char if_name[16]) {
    struct ip_addr_store store;
    
    strncpy(store.if_name, if_name, 16);
    store.root = NULL;
    store.tree_size = 0;

    return store;
}

int inc(struct ip_addr_store* store, uint32_t ip_addr) {
    struct ip_stats dummy = {
        .count = 0,
        .src_addr = ip_addr
    };

    struct ip_stats* p_dummy = (struct ip_stats*)malloc(sizeof(struct ip_stats));
    if(p_dummy == NULL) return 1;
    *p_dummy = dummy;

    struct ip_stats** res = tsearch(p_dummy, &store->root, compare_fn);
    if(res == NULL) {
        free(p_dummy);
        return 1;
    }

    if ((*res)->count > 0) {
        free(p_dummy);
    } else {
        if(*res == p_dummy) {
            store->tree_size++;
        }
    }

    (*res)->count++;
    return 0;
}

int compare_fn(const void* p1, const void* p2) {
    struct ip_stats* p_stats1 = *(struct ip_stats* const*)p1;
    struct ip_stats* p_stats2 = *(struct ip_stats* const*)p2;

    uint32_t ip_addr1 = p_stats1->src_addr;
    uint32_t ip_addr2 = p_stats2->src_addr;

    if (ip_addr1 > ip_addr2) {
        return 1;
    } else if (ip_addr1 < ip_addr2) {
        return -1;
    }

    return 0;
}

void action_fn(const void* p_node, VISIT v, int l) {
    if (v == postorder || v == leaf) {
        struct ip_stats* stat = *(struct ip_stats**)p_node;
        stats_arr[stats_arr_idx] = stat;
        ++stats_arr_idx;
    }
}

void action_free(const void* p_node, VISIT v, int l) {
    if (v == postorder || v == leaf) {
        struct ip_stats* p_stat = *(struct ip_stats**)p_node;
        free(p_stat);
    }
}

int make_snapshot(const struct ip_addr_store* store) {
    stats_arr = (struct ip_stats**)malloc(store->tree_size * sizeof(struct ip_stats*));
    if (stats_arr == NULL) return 1;
    stats_arr_idx = 0;

    twalk(store->root, action_fn);
    return 0;
}


int save_snapshot(const char* if_name) {
    if (create_folder(D_DATA_PATH)) {
        free(stats_arr);
        return 1;
    }

    FILE* fptr;

    char if_path[40] = {0};
    strcat(if_path, D_DATA_PATH);
    strcat(if_path, "sf.");
    strcat(if_path, if_name);
    strcat(if_path, ".csv");

    fptr = fopen(if_path, "w");
    if (fptr == NULL) {
        fprintf(stderr, "failed to open file %s\n", if_path);
        free(stats_arr);
        return 1;
    }

    if (fputs("Address,Count\n", fptr)) {
        fprintf(stderr, "failed to write to file %s\n", if_path);
        free(stats_arr);
        return 1;
    }

    for (int i = 0; i < stats_arr_idx; i++) {
        uint32_t addr = stats_arr[i]->src_addr;
        uint32_t cnt  = stats_arr[i]->count;

        fprintf(fptr, "%u.%u.%u.%u,%u\n", 
            (unsigned char)((addr & 0xff000000) >> 24),
            (unsigned char)((addr & 0x00ff0000) >> 16),
            (unsigned char)((addr & 0x0000ff00) >> 8),
            (unsigned char)(addr & 0x000000ff),
            cnt    
        );
    }

    fclose(fptr);
    free(stats_arr);
    return 0;
}

int read_store(struct ip_addr_store* store) {
    if (create_folder(D_DATA_PATH)) {
        return 1;
    }

    FILE* fptr;

    char if_path[40] = {0};
    strcat(if_path, D_DATA_PATH);
    strcat(if_path, "sf.");
    strcat(if_path, store->if_name);
    strcat(if_path, ".csv");

    fptr = fopen(if_path, "r");
    if (fptr == NULL) {
        fprintf(stderr, "failed to open file %s\n", if_path);
        return 1;
    }

    char line_buf[MAX_LINE_LEN];

    if (fgets(line_buf, MAX_LINE_LEN, fptr) == NULL) {
        return 0;
    }

    unsigned char oct1;
    unsigned char oct2;
    unsigned char oct3;
    unsigned char oct4;
    uint32_t cnt;

    while (fgets(line_buf, MAX_LINE_LEN, fptr)) {
        if (sscanf(line_buf, "%u.%u.%u.%u,%u\n", &oct1, &oct2, &oct3, &oct4, &cnt) != 5) {
            fprintf(stderr, "read_store error: cant read line");
            return 1;
        }

        uint32_t ip_addr = (uint32_t)oct1 << 24 | (uint32_t)oct2 << 16 | (uint32_t)oct3 << 8 | (uint32_t)oct4;
        
        struct ip_stats dummy = { .count = cnt, .src_addr = ip_addr };
        struct ip_stats* p_dummy = (struct ip_stats*)malloc(sizeof(struct ip_stats));
        if (p_dummy == NULL) return 1;
        *p_dummy = dummy;

        struct ip_stats** res = tsearch(p_dummy, &store->root, compare_fn);

        if (res == NULL) {
            fprintf(stderr, "read_store error: failed to insert stat");
            return 1;
        }
        if(*res == p_dummy) {
            store->tree_size++;
        } else {
            free(p_dummy);
            (*res)->count += cnt;
        }
    }

    fclose(fptr);
    return 0;
}

int create_folder(const char* path) {
    if (mkdir(path, 0755) == -1) {
        if (errno != EEXIST) {
            perror("[stat_counter] create folder");
            return 1;
        }
    }
    return 0;
}

void free_node(void *nodep) {
    free(*(void**)nodep);
}


int free_store(struct ip_addr_store* store) {
    if(store->root != NULL) {
        tdestroy(store->root, free_node);
    }
    store->root = NULL;
    store->tree_size = 0;
    return 0;
}
