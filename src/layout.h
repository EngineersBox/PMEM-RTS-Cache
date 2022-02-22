#pragma once

#ifndef PMEM_MMAP_IO_LAYOUT_H
#define PMEM_MMAP_IO_LAYOUT_H

#define LAYOUT_NAME "intro_2"
#define MAX_BUF_LEN 10

struct my_root {
    char buf[MAX_BUF_LEN];
};

#endif //PMEM_MMAP_IO_LAYOUT_H
