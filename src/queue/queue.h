#pragma once

#ifndef RTS_CACHE_QUEUE_H
#define RTS_CACHE_QUEUE_H

#include <stdint.h>
#include <libpmemobj.h>

#ifndef QUEUE_TX_TYPE_OFFSET
#define QUEUE_TX_TYPE_OFFSET 1004
#endif

typedef struct Node {
    struct Node* prev;
    struct Node* next;
    int32_t value;
} Node;

TOID_DECLARE(struct Node, QUEUE_TX_TYPE_OFFSET + 1)

typedef struct Queue {
    unsigned count;
    Node* front;
    Node* back;
} Queue;

TOID_DECLARE(struct Queue, QUEUE_TX_TYPE_OFFSET);

typedef struct HashKey {
    int capacity;
    Node* array;
} HashKey;

void dequeue(Queue* queue);
void enqueue(Queue* queue, HashKey* key, int32_t value);
void get(Queue* queue, HashKey* key);

#endif //RTS_CACHE_QUEUE_H
