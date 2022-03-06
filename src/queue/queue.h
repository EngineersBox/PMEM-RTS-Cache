#pragma once

#ifndef RTS_CACHE_QUEUE_H
#define RTS_CACHE_QUEUE_H

#include <stdint.h>
#include <libpmemobj.h>

POBJ_LAYOUT_BEGIN(queue);
POBJ_LAYOUT_ROOT(queue, struct QueueRoot);
POBJ_LAYOUT_TOID(queue, struct QueueEntry);
POBJ_LAYOUT_TOID(queue, struct Queue);
POBJ_LAYOUT_END(queue);

struct QueueEntry {
    int64_t value;
};

struct Queue {
    size_t front;
    size_t back;
    size_t capacity;
    TOID(struct QueueEntry) entries[];
};

struct QueueRoot {
    TOID(struct Queue) queue;
};

static int queue_constructor(PMEMobjpool* pop, void* ptr, void* arg);
static int queue_new(PMEMobjpool* pop, TOID(struct Queue)* q, size_t size);
static int queue_free(PMEMobjpool* pop, TOID(struct Queue)* q);

static size_t queue_size(const struct Queue* queue);

static int queue_enqueue(PMEMobjpool* pop, struct Queue* queue, int64_t data);
static int queue_dequeue(PMEMobjpool* pop, struct Queue* queue);

#endif //RTS_CACHE_QUEUE_H
