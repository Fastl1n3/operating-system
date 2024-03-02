#include <stdio.h>
#include <unistd.h> // для write и syscall
#include <sys/syscall.h>// для определений SYS_xxx

//void print_hello() {
//	printf("Hello World1\n");
//}

//void write_hello() {
//	write(1, "Hello World2\n", 13);
//}

ssize_t my_write(int field, const void* buf, size_t count) {
	return syscall(SYS_write, field, buf, count);
}

int main() {
	//print_hello();
	//write_hello();
	my_write(1, "Hello world3\n", 13);
	//printf("%d w, %d e\n", SYS_write, SYS_exit);
	return 0;
}
