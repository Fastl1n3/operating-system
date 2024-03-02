#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	uint64_t pfn : 55;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
} PagemapEntry;

int pagemap_get_entry(PagemapEntry* entry, int pagemap_fd, uint64_t vaddr) {
	int nread = 0;
	int ret;
	uint64_t data;
	while (nread < sizeof(data)) {
		ret = pread(pagemap_fd, &data, sizeof(data), (vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
		nread += ret;
		if (ret <= 0) {
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

int main(int argc, char** argv) {
	char buffer[BUFSIZ];
	char maps_file[BUFSIZ];
	char pagemap_file[BUFSIZ];
	int maps_fd;
	int pagemap_fd;
	if (argc < 2) {
		printf("Usage: %s pid\n", argv[0]);
		return 0;
	}
	snprintf(maps_file, sizeof(maps_file), "/proc/%s/maps", argv[1]);
	snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%s/pagemap", argv[1]);
	maps_fd = open(maps_file, O_RDONLY);
	if (maps_fd < 0) {
		perror("open maps");
		return 0;
	}
	pagemap_fd = open(pagemap_file, O_RDONLY);
	if (pagemap_fd < 0) {
		perror("open pagemap");
		return 0;
	}
	ssize_t length = 1;
	printf("addr pfn soft-dirty file/shared swapped present library\n");
	while (length > 0) {
		length = read(maps_fd, buffer , sizeof(buffer));
		//printf("%s\n**\n", buffer);
		
		for (int i = 0; i < length; i++) {
			uint64_t low = 0, high = 0;
			if (buffer[i] == '\n' && i) {
				const char* lib_name = 0;
				int x = i - 1;
				while (x && buffer[x] != '\n') {
					x--;
				}
				if (buffer[x] == '\n') {
					x++;
				}
				size_t beginning = x;
				while (buffer[x] != '-' && x < sizeof(buffer)) {
					char c = buffer[x++];
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
				while (buffer[x] != '-' && x < sizeof(buffer)) {
					x++;
				}
				if (buffer[x] == '-') {
					x++;
				}
				while (buffer[x] != ' ' && x < sizeof(buffer)) {
					char c = buffer[x++];
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
					x++;
					while (buffer[x] != ' ' && x < sizeof(buffer)) {
						x++;
					}
				}
				while (buffer[x] == ' ' && x < sizeof(buffer)) {
					x++;
				}
				int y = x;
				while (buffer[y] != '\n' && y < sizeof(buffer)) {
					y++;
				}
				buffer[y] = 0;
				lib_name = buffer + x;
				
				PagemapEntry entry;
				for (uint64_t addr = low; addr < high; addr += sysconf(_SC_PAGE_SIZE)) {
					if (!pagemap_get_entry(&entry, pagemap_fd, addr)) {
						printf("%lx %lx %u %u %u %u %s\n",
							(uint64_t)addr,
							(uint64_t)entry.pfn,
							entry.soft_dirty,
							entry.file_page,
							entry.swapped,
							entry.present,
							lib_name
						);
					}
				}
				buffer[y] = '\n';
			}
		}
	}
	close(maps_fd);
	close(pagemap_fd);
	return 0;
}