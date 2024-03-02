#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* return_int(void* arg) {
	return (void*)42;
}

void* return_str(void* arg) {
	return (void*)"Hello World!";
}

int main() {
	pthread_t tid;
	int err;
	printf("%d\n", getpid());
	pthread_t thread[5];
	printf("MAIN [%d %d %d]\n", getpid(), getppid(), gettid());
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
	err = pthread_create(&thread[0], &thread_attr, return_int, NULL);
	err = pthread_create(&thread[1], &thread_attr, return_str, NULL);
	
	void* status;
	
	pthread_join(thread[0], &status);
	printf("return value = %d\n", (int)status);
	pthread_join(thread[1], &status);
	printf("return value = %s\n", (char*)status);
	
	pthread_attr_destroy(&thread_attr);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	pthread_exit(NULL);
	return 0;
}
