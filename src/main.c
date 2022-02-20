#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "structure/cache.h"

int main(int argc, char *argv[]) {
    cache c;
    void* entryAlloc = allocateEntries(&c, 5);

    if (entryAlloc == MAP_FAILED || entryAlloc == NULL) {
        printf("Mapping Failed\n");
        return 1;
    }

    uint32_t index = putEntry(&c, (cache_entry){ .ttl = 100, .value = 42 });
    if (index == -1) {
        printf("Unable to put entry into cache\n");
        free(c.entries);
        return 1;
    }

    cache_entry* returned = getEntry(&c, index);
    if (returned == NULL) {
        printf("Could not get entry at index %d from cache\n", index);
        free(c.entries);
        return 1;
    }

    printf("Value: %d, TTL: %d\n", returned->value, returned->ttl);

    if (freeEntries(&c) != 0) {
        printf("UnMapping Failed\n");
        return 1;
    }

    return 0;
}