#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
char* start_heap = 0x20000000;
char* heap_ptr = 0x20000000;
void* my_malloc(int size) {
	void *p
	return p;
}
int main(int argc, char* argv[]) {
	printf("pid: %d\n", getpid());
	mmap(start_heap, 4096 * 1024 * 100, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	return 0;
}