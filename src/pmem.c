#include <stdio.h>

#include "cache/cache.h"

long long getNanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <file-name>\n", argv[0]);
        return 1;
    }

    Cache cache;
    int8_t err = allocateEntries(&cache, 3, argv[1]);
    if (err != 0) {
        printf("Could not allocate entries");
        return 1;
    }

    CacheEntry entry = {
        .timestamp = getNanos(),
        .value = 483525
    };
    err = putEntry(&cache, &entry);
    if (err != 0) {
        printf("Could not allocate entries");
        return 1;
    }

    CacheEntry storedEntry = {};
    err = getEntry(&cache, cache.lastIdx, &entry);
    if (err != 0) {
        printf("Could not allocate entries");
        return 1;
    }
    printf("Timestamp: %ldl Value: %d", storedEntry.timestamp, storedEntry.value);

    freeEntries(&cache);

    return 0;
}