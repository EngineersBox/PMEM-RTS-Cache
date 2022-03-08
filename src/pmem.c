#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <libpmemobj.h>

#include "cache/cache.h"

#ifndef _WIN32
#define CREATE_MODE_RW (S_IWUSR | S_IRUSR)
#include <unistd.h>
static inline int file_exists(const char* path) {
	return access(path, F_OK);
}
#else
#define CREATE_MODE_RW (S_IWRITE | S_IREAD)
static inline int file_exists(const char* path) {
    return _access(path, 0);
}
#endif

POBJ_LAYOUT_BEGIN(cache_pobj);
POBJ_LAYOUT_ROOT(cache_pobj, struct CacheRoot);
POBJ_LAYOUT_END(cache_pobj);

typedef struct CacheRoot {
    TOID(struct Cache) cache;
} CacheRoot;

static PMEMobjpool *pop;
static TOID(struct CacheRoot) root;
static TOID(struct Cache) cache;

int create_or_open_pool(const char* path) {
    char* errorMsg;
    if (file_exists(path) != 0) {
        pop = pmemobj_create(
            path,
            POBJ_LAYOUT_NAME(cache_pobj),
            PMEMOBJ_MIN_POOL,
            CREATE_MODE_RW
        );
        if (pop == NULL) {
            errorMsg = "failed to create pool: %s\n"; goto error;
        }
        root = POBJ_ROOT(pop, struct CacheRoot);
    } else {
        pop = pmemobj_open(path, POBJ_LAYOUT_NAME(cache_pobj));
        if (pop == NULL) {
            errorMsg = "failed to open pool: %s\n"; goto error;
        }
        root = POBJ_ROOT(pop, struct CacheRoot);
        cache = D_RO(root)->cache;
    }
    if (TOID_IS_NULL(root)) {
        errorMsg = "could not create root: %s\n"; goto error;
    }
    return 0;
error:
    fprintf(
        stderr,
        errorMsg,
        pmemobj_errormsg()
    );
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <file-name>\n", argv[0]);
        return 1;
    }
    const char* path = argv[1];

    int err;
    if ((err = create_or_open_pool(path)) != 0) {
        return err;
    }

    if (TOID_IS_NULL(cache)) {
        err = cache_new(pop, &D_RW(root)->cache, 5);
        if (err != 0) {
            pmemobj_close(pop);
            perror("cache_new");
            return 1;
        }
        cache = D_RO(root)->cache;
    }

    uint64_t key = 42;
    int value = 1234;
    err = cache_set(pop, cache, key, value);
    if (err != 0) {
        fprintf(
            stderr,
            "Failed to set key %llu with value %d\n",
            key,
            value
        );
        return 1;
    }

    int retrievedValue;
    err = cache_get(pop, cache, key, &retrievedValue);
    if (err != 0) {
        fprintf(
            stderr,
            "Failed to get key %llu\n",
            key
        );
        return 1;
    }
    printf(
        "Retrieved value for key %llu: %d\n",
        key,
        retrievedValue
    );

    return 0;
}