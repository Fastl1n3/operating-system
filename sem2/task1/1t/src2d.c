#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* mythread(void* arg) {
	printf("thread %d\n", gettid());
	pthread_detach(pthread_self());
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
	printf("%d\n", getpid());
	pthread_t thread[5];
	printf("MAIN [%d %d %d]\n", getpid(), getppid(), gettid());
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	while (1) {
		err = pthread_create(&thread[0], NULL, mythread, NULL);
		if (err) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
	}
	

	pthread_attr_destroy(&thread_attr);
	
	pthread_exit(NULL);
	return 0;
}
