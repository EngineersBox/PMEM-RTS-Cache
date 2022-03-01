#pragma once

#ifndef RTS_CACHE_QUEUE_H
#define RTS_CACHE_QUEUE_H

#include <stdint.h>

#include "../hash_ss/common_sampling.h"

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

// Declared in src/hash_ss/sampling.cu
extern int superSample(color* data,
                       unsigned int s_width, unsigned int s_height,
                       unsigned int t_width, unsigned int t_height);

#endif //RTS_CACHE_QUEUE_H
