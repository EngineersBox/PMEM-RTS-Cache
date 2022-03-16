#include "scalable_cuckoo_filter.h"

#include "bucket.h"

#include <stdarg.h>
#include <stdlib.h>

uint32_t scf_default_scale_factor_func(uint32_t currentSize) {
    return currentSize * BUCKET_SIZE * 2;
}

int scf_configure(ScalableCuckooFilter* scf) {
    if (scf == NULL) {
        return -1;
    }
    if (scf->loadFactor == 0) {
        scf->loadFactor = CF_S_DEFAULT_LOAD_FACTOR;
    }
    if (scf->scaleFactor == NULL) {
        scf->scaleFactor = scf_default_scale_factor_func;
    }
    if (scf->filters == NULL) {
        scf->filters = (CuckooFilter**) calloc(1, sizeof(CuckooFilter*));
        if (cf_new(scf->filters[0], CF_S_DEFAULT_CAPACITY) == -1) {
            return -1;
        }
        scf->capacity = CF_S_DEFAULT_CAPACITY;
        scf->filterCount = 1;
    }
    return 0;
}

int scf_new(ScalableCuckooFilter* scf, size_t vargs_len, ...) {
    scf = (ScalableCuckooFilter*) malloc(sizeof(ScalableCuckooFilter));
    if (scf == NULL) {
        return -1;
    }
    va_list vargs_ptr;
    va_start(vargs_ptr, vargs_len);
    for (int i = 0; i < vargs_len; i++) {
        Option* option = va_arg(vargs_ptr, Option*);
        (*option)(scf);
    }
    va_end(vargs_ptr);
    return scf_configure(scf);
}

int scf_free(ScalableCuckooFilter* scf) {
    if (scf == NULL) {
        return 0;
    }
    if (scf->filters == NULL) {
        return -1;
    }
    for (int i = 0; i < scf->filterCount; i++) {
        if (cf_free(scf->filters[i]) == -1) {
            return -1;
        }
    }
    free(scf->filters);
    free(scf);
    return 0;
}

int scf_reset(ScalableCuckooFilter* scf) {
    for (int i = 0; i < scf->filterCount; i++) {
        if (cf_reset(scf->filters[i]) == -1) {
            return -1;
        }
    }
    return 0;
}

int scf_is_init(ScalableCuckooFilter* scf) {
    return scf != NULL && scf->filters;
}

int scf_lookup(ScalableCuckooFilter* scf, const char* key, size_t len) {
    if (!scf_is_init(scf)) {
        return -1;
    }
    for (int i = 0; i < scf->filterCount; i++) {
        if (cf_lookup(scf->filters[i], key, len)) {
            return 1;
        }
    }
    return 0;
}

int scf_insert(ScalableCuckooFilter* scf, const char* key, size_t len) {
    int needScale = 0;
    CuckooFilter* lastFilter = scf->filters[scf->filterCount - 1];
    if (((float)lastFilter->count / (float)(lastFilter->bucketPower)) > scf->loadFactor) {
        needScale = 1;
    } else {
        needScale = !cf_insert(lastFilter, key, len);
    }
    if (!needScale) {
        return 1;
    }
    CuckooFilter* newFilter;
    if (!cf_new(newFilter, scf->scaleFactor(lastFilter->bucketPower))) {
        return -1;
    }
    scf->filters[scf->filterCount] = newFilter;
    scf->filterCount++;
    return cf_insert(newFilter, key, len);
}

int scf_insert_unique(ScalableCuckooFilter* scf, const char* key, size_t len) {
    if (scf_lookup(scf, key, len)) {
        return 0;
    }
    return scf_insert(scf, key, len);
}

int scf_delete(ScalableCuckooFilter* scf, const char* key, size_t len) {
    if (!scf_is_init(scf)) {
        return -1;
    }
    for (int i = 0; i < scf->filterCount; i++) {
        if (cf_delete(scf->filters[i], key, len)) {
            return -1;
        }
    }
    return 0;
}