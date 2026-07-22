#include "stat_counter.h"

struct ip_addr_store create_ip_addr_store(char if_name[16]) {
    struct ip_addr_store store;
    
    strncpy(store.if_name, if_name, 16);
    store.root = malloc(sizeof(void*));
    store.vec = create_ip_addr_darray();

    return store;
}

int inc(struct ip_addr_store* store, uint32_t ip_addr) {
    tsearch(ip_addr, store->root, NULL);
    
    return 0;
}

int compare_fn(uint32_t* p_ip_addr1, uint32_t* p_ip_addr2) {
    uint32_t ip_addr1 = *p_ip_addr1;
    uint32_t ip_addr2 = *p_ip_addr2;

    return (int)(ip_addr1 - ip_addr2);
}

struct ip_addr_darray create_ip_addr_darray() {
    struct ip_addr_darray darray;
    darray.cap = INIT_DARRAY_CAP;
    darray.len = 0;
    struct ip_stats* ptr = (struct ip_stats*)malloc(darray.cap * sizeof(struct ip_stats));
    darray.ptr = ptr;
    return darray;
}

int push(struct ip_addr_darray* vec, struct ip_stats stats) {
    if (vec->cap < vec->len+1) {
        uint32_t new_cap = (uint32_t)(vec->cap * 3 / 2);

        struct ip_stats* temp = (struct ip_stats*)realloc(vec->ptr, sizeof(new_cap * sizeof(struct ip_stats)));

        if (temp == NULL) {
            return 1;
        } 

        vec->cap = new_cap;
        vec->ptr = temp;
    }

    vec->ptr[++(vec->len)] = stats;
    return 0;
}


