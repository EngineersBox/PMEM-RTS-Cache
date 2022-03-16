#include "bucket.h"

#include <stdlib.h>
#include <string.h>

int bucket_new(Bucket* bucket) {
    bucket = (Bucket*) malloc(sizeof(Bucket));
    return bucket == NULL ? -1 : 0;
}

int bucket_free(Bucket* bucket) {
    if (bucket == NULL) {
        return -1;
    }
    free(bucket->fp);
    return 0;
}

void bucket_reset(Bucket* bucket) {
    memset(bucket->fp, 0, BUCKET_SIZE);
}

int bucket_insert(Bucket* bucket, Fingerprint fingerprint) {
    for (int i = 0; i < BUCKET_SIZE; i++) {
        if (bucket->fp[i] == 0) {
            bucket->fp[i] = fingerprint;
            return 0;
        }
    }
    return -1;
}

int bucket_delete(Bucket* bucket, Fingerprint fingerprint) {
    for (int i = 0; i < BUCKET_SIZE; i++) {
        if (bucket->fp[i] == fingerprint) {
            bucket->fp[i] = 0;
            return 0;
        }
    }
    return -1;
}

int bucket_fp_index(Bucket* bucket, Fingerprint fingerprint) {
    for (int i = 0; i < BUCKET_SIZE; i++) {
        if (bucket->fp[i] == fingerprint) {
            return i;
        }
    }
    return -1;
}
