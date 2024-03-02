#include <dlfcn.h>

int main() {
	void* lib; 
	void (*print_hello)(void);
	
	lib = dlopen("./libdlopen.so", RTLD_LAZY);
	print_hello = dlsym(lib, "hello_from_dynamic_lib");
	print_hello();
	dlclose(lib);
	return 0;
}