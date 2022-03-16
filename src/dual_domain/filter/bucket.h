#pragma once

#ifndef RTS_CACHE_BUCKET_H
#define RTS_CACHE_BUCKET_H

#include <inttypes.h>

#define BUCKET_SIZE 4

typedef uint8_t Fingerprint;
typedef struct Bucket {
    Fingerprint fp[BUCKET_SIZE];
} Bucket;

int bucket_new(Bucket* bucket);
int bucket_free(Bucket* bucket);
void bucket_reset(Bucket* bucket);

int bucket_insert(Bucket* bucket, Fingerprint fingerprint);
int bucket_delete(Bucket* bucket, Fingerprint fingerprint);
int bucket_fp_index(Bucket* bucket, Fingerprint fingerprint);

#endif //RTS_CACHE_BUCKET_H
