#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	printf("pid %d\n\n", getpid());
	int status;
	pid_t pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
		exit(1);

	case 0: //child
		printf("From child; his pid: %d\n", getpid());
		sleep(20);
		printf("Finish child\n");
		break;
	default: //parent
		printf("From parent, his pid: %d\n", getpid());
		sleep(10);
		printf("Finish parent\n");
		exit(0);
	}
	return 0;
}