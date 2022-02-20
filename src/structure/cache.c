#include "cache.h"
#include <stdio.h>
#include <sys/mman.h>

cache_entry* getEntry(cache* ptr, uint32_t index) {
    if (ptr == NULL || ptr->count <= index) {
        return NULL;
    }
    return &ptr->entries[index];
}

uint32_t putEntry(cache* ptr, cache_entry entry) {
    if (ptr == NULL
        || ptr->count >= ptr->allocatedSize
        || ptr->entries == NULL
        || ptr->entries == MAP_FAILED) {
        return -1;
    }
    ptr->entries[ptr->count++] = entry;
    return ptr->count - 1;
}

void* allocateEntries(cache* ptr, size_t size) {
    if (ptr == NULL || size > MAX_CACHE_ENTRIES) {
        return NULL;
    }
    ptr->allocatedSize = size;
    ptr->count = 0;
    return ptr->entries = mmap(
        NULL,
        size * sizeof(cache_entry),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        0,
        0
    );
}

int8_t freeEntries(cache* ptr) {
    if (ptr == NULL) {
        return -1;
    }
    return munmap(ptr->entries, ptr->allocatedSize * sizeof(cache_entry));
}