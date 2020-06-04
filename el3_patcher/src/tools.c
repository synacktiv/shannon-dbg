#include "tools.h"

int pagemap_get_entry(PagemapEntry *entry, int pagemap_fd, uintptr_t vaddr)
{
	size_t nread;
	ssize_t ret;
	uint64_t data;

	nread = 0;
	while (nread < sizeof(data))
	{
		ret = pread(pagemap_fd, &data, sizeof(data),
					(vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
		nread += ret;
		if (ret <= 0)
		{
			return 1;
		}
	}
	entry->pfn = data & (((uint64_t)1 << 54) - 1);
	entry->soft_dirty = (data >> 54) & 1;
	entry->file_page = (data >> 61) & 1;
	entry->swapped = (data >> 62) & 1;
	entry->present = (data >> 63) & 1;
	return 0;
}

int virt_to_phys_user(uintptr_t *paddr, uintptr_t vaddr)
{
	char pagemap_file[BUFSIZ];
	int pagemap_fd;

	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/self/pagemap");
	pagemap_fd = open(pagemap_file, O_RDONLY);
	if (pagemap_fd < 0)
	{
		return 1;
	}
	PagemapEntry entry;
	if (pagemap_get_entry(&entry, pagemap_fd, vaddr))
	{
		return 1;
	}
	close(pagemap_fd);
	*paddr = (entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE));
	return 0;
}


uint8_t *mmap_shared(uint32_t shareLen, uintptr_t *phys_addr)
{
	while (1)
	{
		uint8_t *share = mmap(NULL,
							  PAGE_ALIGN(shareLen),
							  PROT_READ | PROT_WRITE,
							  MAP_ANONYMOUS | MAP_PRIVATE,
							  0,
							  0);
		memset(share, 0x41, shareLen);

		uintptr_t share_phys;
		if (virt_to_phys_user(&share_phys, (uintptr_t)share))
		{
			return NULL;
		}
		if (share_phys < 0xFFFFFFFF)
		{
			*phys_addr = share_phys;
			return share;
		}
	}
}
