#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int global_not_init;
int global_init = 10;
const int global_const = 5;

void f1() {
	int local;
	static int static_not_init;
	static int static_init = 3;
	printf("global not init: %p (%d)\n", &global_not_init, global_not_init);
	printf("global init:     %p (%d)\n", &global_init, global_init);
	printf("global const:    %p (%d)\n\n", &global_const, global_const);
	printf("local:           %p (%d)\n\n", &local, local);
	printf("static not init: %p (%d)\n", &static_not_init, static_not_init);
	printf("static init:     %p (%d)\n\n", &static_init, static_init);
}

int* return_addr() {
	int a = 5;
	return &a;
}

void work_with_heap() {
	char* str = (char*)malloc(sizeof(char) * 100);
//	sleep(5);
	strcpy(str, "Hello World!");
	printf("Before: %s\n", str);
	//sleep(5);
	free(str);;
	printf("After: %s\n", str);
	
	char* str2 = (char*)malloc(sizeof(char) * 50);
	strcpy(str2, "Hello World!");
	char* p = str2 + 4;
	printf("Second from mid %s\n", p);
	free(p);
	printf("Last %s\n", str2);
}

void env_var() {
	printf("ENV VAR before: %s\n", getenv("TEST"));
	setenv("TEST", "WORLD", 1);
	printf("ENV VAR after: %s\n", getenv("TEST"));
}
int main() {
	printf("Pid: %d\n", getpid());
	//f1();
	//printf("%d\n", return_addr());
	//sleep(10);
	//work_with_heap();
	env_var();
	//sleep(30);
	return 0;
}
