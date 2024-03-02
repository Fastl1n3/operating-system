#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int glob = 12;
int main(int argc, char* argv[]) {
	int loc = 5;
	printf("global: %p, %d\n", &glob, glob);
	printf("local: %p, %d\n", &loc, loc);
	printf("pid %d\n\n", getpid());
	int status;
	pid_t pid = fork();
	switch (pid) {
	case -1:
		perror("fork");
		exit(1);

	case 0: //child
		printf("Child pid: %d, parent pid: %d\n", getpid(), getppid());
		printf("Child global: %p, %d\n", &glob, glob);
		printf("Child local: %p, %d\n", &loc, loc);
		glob = 25;
		loc = 7;
		printf("Child new global: %p %d\n", &glob, glob);
		printf("Child new local: %p %d\n", &loc, loc);
		//sleep(10);
		exit(5);
	default: //parent
		
		sleep(30);
		wait(&status);
		printf("Parent global: %p, %d\n", &glob, glob);
		printf("Parent local: %p, %d\n", &loc, loc);
		if (WIFEXITED(status)) {
			printf("Status: %d\n", WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			printf("Signal: %d\n", WTERMSIG(status));
		}
		else if (WIFSTOPPED(status)) {
			printf("Signal: %d\n", WSTOPSIG(status));
		}
	}
	return 0;
}