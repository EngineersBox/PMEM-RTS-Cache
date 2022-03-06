#include <stdio.h>

#include "cache/cache.h"
#include "queue/queue.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <file-name>\n", argv[0]);
        return 1;
    }

    return 0;
}