 #include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#define SIZE 8192 * 2 



int count = 0;
void on_stack() {
	char arr[SIZE];
	count += SIZE;
	printf("%d\n", count);
	usleep(50000);
	on_stack();
}

void on_heap() {
	long long buf = SIZE;
	while(1) {
		malloc(buf);
		count += buf;
		printf("%d\n", count);
		usleep(50000);
	}
}

void on_heap_with_reg() {
	long long buf = SIZE * SIZE;
	int i = 0;
	while (1) {
		malloc(buf);
		count += buf;
		printf("%d\n", count);
		i++;
		if (i > 50) {
			mmap(NULL, 4096 * 10, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		}
		usleep(500000);
	}
}

void read_and_write_from_reg() {
	char* p = 0x20000000;
	mmap(p, 4096 * 10, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	sleep(5);
	p[4096 * 5] = 123;
	printf("%d\n", p[4096 * 5]);
	munmap(p + 4096 * 3, 4096 * 3);
	sleep(10);
	//p[100] = 156;
//	printf("%d\n", p[100]);
}

void handler(int signum) {
	printf("Memory error\n");
	//signal(signum, SIG_DFL);
	exit(3);
}

int main(int argc, char* argv[]) {
	printf("pid: %d\n", getpid());
	signal(SIGSEGV, handler);
	sleep(10);
	//on_stack();
	//on_heap();
	//on_heap_with_reg();
	read_and_write_from_reg();
	
	return 0;
}