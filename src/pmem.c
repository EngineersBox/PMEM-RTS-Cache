#include <stdio.h>
#include <time.h>

#include "cache/cache.h"
#include "queue/queue.h"

int64_t getMillis(void) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return (now.tv_sec * 1000) + ((int64_t) now.tv_nsec) / 1000000;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <file-name>\n", argv[0]);
        return 1;
    }

    Cache cache = {};
    int8_t err = allocateEntries(&cache, argv[1]);
    if (err != 0) {
        printf("Could not allocate entries\n");
        return 1;
    }

    CacheEntry entry = {
        .value = 42,
        .timestamp = getMillis(),
    };
    err = putEntry(&cache, &entry);
    if (err != 0) {
        printf("Could not insert entry\n");
        return 1;
    }
    printf("Cache size: %d\n", cache.lastIdx);

    const CacheEntry storedEntry = {};
    err = getEntry(&cache, cache.lastIdx, &storedEntry);
    if (err != 0) {
        printf("Could not get entry\n");
        return 1;
    }
    printf("Timestamp: %lld Value: %d\n", storedEntry.timestamp, storedEntry.value);

    freeEntries(&cache);

    return 0;
}