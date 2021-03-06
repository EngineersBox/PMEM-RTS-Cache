// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2015-2019, Intel Corporation */

#ifndef RTS_CACHE_HASHMAP_TX_H
#define RTS_CACHE_HASHMAP_TX_H

#include <stddef.h>
#include <stdint.h>
#include <hashmap.h>
#include <libpmemobj.h>

#ifndef HASHMAP_TX_TYPE_OFFSET
#define HASHMAP_TX_TYPE_OFFSET 1004
#endif

struct hashmap_tx; TOID_DECLARE(struct hashmap_tx, HASHMAP_TX_TYPE_OFFSET + 0);
/* layout definition */
TOID_DECLARE(struct buckets, HASHMAP_TX_TYPE_OFFSET + 1);
TOID_DECLARE(struct entry, HASHMAP_TX_TYPE_OFFSET + 2);

struct entry {
    uint64_t key;
    PMEMoid value;
    TOID(struct entry) next;
};

struct buckets {
    size_t nbuckets;
    TOID(struct entry) bucket[];
};

struct hashmap_tx {
    uint32_t seed;
    uint32_t hash_fun_a;
    uint32_t hash_fun_b;
    uint64_t hash_fun_p;
    uint64_t count;
    TOID(struct buckets) buckets;
};

int hm_tx_check(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap);
int hm_tx_create(PMEMobjpool *pop, TOID(struct hashmap_tx) *map, void *arg);
int hm_tx_init(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap);
int hm_tx_insert(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, uint64_t key, PMEMoid value);
PMEMoid hm_tx_remove(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, uint64_t key);
PMEMoid hm_tx_get(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, uint64_t key);
int hm_tx_lookup(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, uint64_t key);
int hm_tx_foreach(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, int (*cb)(uint64_t key, PMEMoid value, void *arg), void *arg);
size_t hm_tx_count(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap);
int hm_tx_cmd(PMEMobjpool *pop, TOID(struct hashmap_tx) hashmap, unsigned cmd, uint64_t arg);

#endif //RTS_CACHE_HASHMAP_TX_H
