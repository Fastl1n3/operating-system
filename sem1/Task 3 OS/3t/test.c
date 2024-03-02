#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#define N 256

typedef struct {
	uint64_t pfn : 55;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
	unsigned int exclus : 1;
} Pagemap;

int pagemap_get_entry(Pagemap* entry, int pagemap_fd, uint64_t vaddr) {
	int nread = 0;
	int ret;
	uint64_t data;
	ret = pread(pagemap_fd, 
				&data, 
				sizeof(data), 
				(vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
	nread += ret;
	entry->pfn = data & (((uint64_t)1 << 54) - 1);
	entry->soft_dirty = (data >> 54) & 1;
	entry->exclus = (data >> 56) & 1;
	entry->file_page = (data >> 61) & 1;
	entry->swapped = (data >> 62) & 1;
	entry->present = (data >> 63) & 1;
	return 0;
}

int main(int argc, char** argv) {
	char buffer[N];
	char maps_file[N];
	char pagemap_file[N];
	FILE* maps_fd;
	int pagemap_fd;
	if (argc < 2) {
		printf("Usage: %s pid\n", argv[0]);
		return 0;
	}
	snprintf(maps_file, sizeof(maps_file), "/proc/%s/maps", argv[1]);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%s/pagemap", argv[1]);
	maps_fd = fopen(maps_file, "r");
	if (maps_fd < 0) {
		perror("open maps");
		return 0;
	}
	pagemap_fd = open(pagemap_file, O_RDONLY);
	if (pagemap_fd < 0) {
		perror("open pagemap");
		return 0;
	}
	
	while (fgets(buffer, N, maps_fd)) {
		//printf("%s\n", buffer);
		uint64_t low = 0, high = 0;
		const char* lib_name = 0;
		int i = 0;
		while (buffer[i] != '-') {
			char c = buffer[i++];
			low *= 16;
			if (c >= '0' && c <= '9') {
				low += c - '0';
			}
			else if (c >= 'a' && c <= 'f') {
				low += c - 'a' + 10;
			}
			else {
				break;
			}
		}
		i++;
		while (buffer[i] != ' ') {
			char c = buffer[i++];
			high *= 16;
			if (c >= '0' && c <= '9') {
				high += c - '0';
			}
			else if (c >= 'a' && c <= 'f') {
				high += c - 'a' + 10;
			}
			else {
				break;
			}
		}
		for (int field = 0; field < 4; field++) {
			i++;
			while (buffer[i] != ' ') {
				i++;
			}
		}
		while (buffer[i] == ' ') {
			i++;
		}
		lib_name = buffer + i;
				
		Pagemap entry;
		for (uint64_t addr = low; addr < high; addr += sysconf(_SC_PAGE_SIZE)) {
			if (!pagemap_get_entry(&entry, pagemap_fd, addr)) {
				printf("addr: %lx; pfn: %lx; soft-dirty: %u; exclus: %u; file/shared: %u; swapped %u; present: %u; library: %s\n",
					(uint64_t)addr,
					(uint64_t)entry.pfn,
					entry.soft_dirty,
					entry.exclus,
					entry.file_page,
					entry.swapped,
					entry.present,
					lib_name
				);
			}
		}
	}
	fclose(maps_fd);
	close(pagemap_fd);
	return 0;
}