#include "queue.h"

#include <libpmemobj.h>

static int queue_constructor(PMEMobjpool* pop, void* ptr, void* arg) {
    struct Queue *q = ptr;
    size_t *capacity = arg;
    q->front = 0;
    q->back = 0;
    q->capacity = *capacity;

    pmemobj_persist(pop, q, sizeof(*q));
    return 0;
}

static int queue_new(PMEMobjpool* pop, TOID(struct Queue)* q, size_t size) {
    return POBJ_ALLOC(
        pop,
        q,
        struct Queue,
        sizeof(struct Queue) + sizeof(TOID(struct QueueEntry)) * size,
        queue_constructor,
        &size
    );
}