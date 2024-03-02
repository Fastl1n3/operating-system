#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	printf("pid: %d\n", getpid());
	sleep(1);
	int err = execl("./exec", "exec", "!!!", NULL);
	printf("Hello World\n");
	return 0;
}