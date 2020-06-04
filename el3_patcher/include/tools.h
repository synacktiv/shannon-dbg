#ifndef TOOLS_H
#define TOOLS_H

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sched.h>
#include <sys/mman.h>
#include "dbg.h"

#define PAGE_SIZE_EXYNOS 0x00001000
#define PAGE_MASK_EXYNOS (~(PAGE_SIZE_EXYNOS - 1))
#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE_EXYNOS - 1) & PAGE_MASK_EXYNOS)
#define PAGE_ADDR(addr) (addr & PAGE_MASK_EXYNOS)

typedef struct
{
	uint64_t pfn : 54;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
} PagemapEntry;

uint8_t *mmap_shared(uint32_t shareLen, uintptr_t *phys_addr);

#endif