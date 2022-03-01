#pragma once

#ifndef RTS_CACHE_QUEUE_H
#define RTS_CACHE_QUEUE_H

#include <stdint.h>

typedef struct Node {
    struct Node* prev;
    struct Node* next;
    int32_t value;
} Node;

typedef struct Queue {
    unsigned count;
    Node* front;
    Node* back;
} Queue;

typedef struct HashKey {
    int capacity;
    Node* array;
} HashKey;

void dequeue(Queue* queue);
void enqueue(Queue* queue, HashKey* key, int32_t value);
void get(Queue* queue, HashKey* key);

#endif //RTS_CACHE_QUEUE_H
