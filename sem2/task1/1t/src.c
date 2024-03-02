#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
int global = 2;
void* mythread(void* arg) {
	int local = 5;
	static int static_local = 1;
	const int clocal = 12;
	printf("mythread t_self: %ld, pid: %d, ppid: %d, tid: %d, lcl: %p, stat lcl: %p, cnst lcl: %p, glbl: %p\n", pthread_self(), getpid(), getppid(), gettid(),
																&local, &static_local, &clocal, &global);
	printf("GLOBAL: %d\n", ++global);
	printf("LOCAL: %d\n", ++local);
	//sleep(10);
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
	printf("%d\n", getpid());
	pthread_t threads[5];
	//sleep(10);
	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	err = pthread_create(&threads[0], NULL, mythread, NULL);
	err = pthread_create(&threads[1], NULL, mythread, NULL);
	err = pthread_create(&threads[2], NULL, mythread, NULL);
	err = pthread_create(&threads[3], NULL, mythread, NULL);
	err = pthread_create(&threads[4], NULL, mythread, NULL);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}
	/*for (int i = 0; i < 5; i++) {
		printf("%ld ", threads[i]);
	}*/
	//printf("\n");
	//sleep(10);
	pthread_exit(NULL);
	return 0;
}
