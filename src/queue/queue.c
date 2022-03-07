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
        sizeof(struct Queue) + (sizeof(TOID(struct QueueEntry)) * size),
        queue_constructor,
        &size
    );
}

static size_t queue_size(const struct Queue* queue) {
    return queue->back - queue->front;
}

static int queue_enqueue(PMEMobjpool* pop, struct Queue* queue, int64_t data) {
    if (queue == NULL || queue_size(queue) == queue->capacity) {
        return -1;
    }
    size_t pos = queue->back % queue->capacity;
    int ret = 0;
    TX_BEGIN(pop) {
        TX_ADD_DIRECT(&queue->back);
        queue->back++;
        TOID(struct QueueEntry) entry = TX_NEW(struct QueueEntry);
            D_RW(entry)->value = data;
        TX_ADD_DIRECT(&queue->entries[pos]);
        queue->entries[pos] = entry;
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    return ret;
}

static int queue_dequeue(PMEMobjpool* pop, struct Queue* queue) {
    if (queue == NULL || queue_size(queue) == 0) {
        return -1;
    }
    size_t pos = queue->front % queue->capacity;
    int ret = 0;
    TX_BEGIN(pop) {
        TX_ADD_DIRECT(&queue->front);
        queue->front++;
        TX_FREE(queue->entries[pos]);
    } TX_ONABORT {
        ret = -1;
    } TX_END;
    return ret;
}