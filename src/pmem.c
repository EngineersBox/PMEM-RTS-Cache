#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <libpmemobj.h>
#include <stdbool.h>

#include "cache/cache.h"

POBJ_LAYOUT_BEGIN(cache_pobj);
POBJ_LAYOUT_ROOT(cache_pobj, struct CacheRoot);
POBJ_LAYOUT_END(cache_pobj);

typedef struct CacheRoot {
    TOID(struct Cache) cache;
} CacheRoot;

static PMEMobjpool *pop;
static TOID(struct CacheRoot) root;
static TOID(struct Cache) cache;

bool file_exists (char *filename) {
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <file-name>\n", argv[0]);
        return 1;
    }
    char* path = argv[1];
    PMEMobjpool *pop;

    if (file_exists(path) != 0) {
        pop = pmemobj_create(
            path,
            POBJ_LAYOUT_NAME(cache_pobj),
            PMEMOBJ_MIN_POOL,
            0666
        );

        if (pop == NULL) {
            fprintf(
                stderr,
                "failed to create pool: %s\n",
                pmemobj_errormsg()
            );
            return 1;
        }

        root = POBJ_ROOT(pop, struct CacheRoot);
    } else {
        pop = pmemobj_open(path, POBJ_LAYOUT_NAME(cache_pobj));
        if (pop == NULL) {
            fprintf(
                stderr,
                "failed to open pool: %s\n",
                pmemobj_errormsg()
            );
            return 1;
        }
        root = POBJ_ROOT(pop, struct CacheRoot);
        cache = D_RO(root)->cache;
    }

    if (TOID_IS_NULL(cache)) {
        int err = cache_new(pop, &D_RW(root)->cache, 5);
        if (err != 0) {
            pmemobj_close(pop);
            perror("cache_new");
            return 1;
        }
        cache = D_RW(root)->cache;
    }

    return 0;
}