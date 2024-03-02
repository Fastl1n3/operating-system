#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define SIZE 100

void make_dir(char* name) {
	mode_t mode = 0777;
	if (mkdir(name, mode) == -1) {
		perror("Make dir");
		exit(0);
	}
}

void print_dir_content(char* name) {
	DIR* dr = opendir(name);
	if (dr == NULL) {
		perror(name);
		exit(0);
	}
	struct dirent* actual_file;
	while (actual_file = readdir(dr)) {
		printf("%s (type: %d)\n", actual_file->d_name, actual_file->d_type);
	}
}

void remove_file(char* name) {
	if (remove(name) == -1) {
		perror("Remove file");
		exit(0);
	}
}

void remove_dir(char* name) {
	DIR* dr = opendir(name);
	if (dr == NULL) {
		perror(name);
		exit(0);
	}
	strcat(name, "/");
	char* name_allpath = (char*)calloc(SIZE, sizeof(char));
	struct dirent* actual_file;
	while (actual_file = readdir(dr)) {
		strcat(name_allpath, name);
		strcat(name_allpath, actual_file->d_name);
		//printf("%s %d\n", name_allpath, actual_file->d_type);
		if (actual_file->d_type != DT_DIR) {
			remove_file(name_allpath);
		}
		else if (strcmp(actual_file->d_name, ".") != 0 && strcmp(actual_file->d_name, "..") != 0) {
			remove_dir(name_allpath);
		}
		memset(name_allpath, 0, strlen(name_allpath));
	}
	remove(name);
	free(name_allpath);
}

void print_file_content(char* name) {
	int fdr = open(name, O_RDONLY);
	if (fdr == -1) {
		perror("Print file content");
		exit(0);
	}
	char c;
	while (read(fdr, &c, 1)) {
		printf("%c", c);
	}
}

void create_file(char* name) {
	int a;
	if (a = open(name, O_CREAT, 0777) == -1) {
		perror("Create file");
		exit(0);
	}
	close(a);
}

void create_slink(char* name, char* namelink) {
	if (symlink(name, namelink) == -1) {
		perror("sym link");
		exit(0);
	}
}

void read_slink(char* name) {
	char buf[SIZE] = { 0 };
	if (readlink(name, buf, SIZE) == -1) {
		perror("read slink");
		exit(0);
	}
	printf("%s\n", buf);
}

void create_hlink(char* name, char* namelink) {
	if (link(name, namelink) == -1) {
		perror("sym link");
		exit(0);
	}
}

void check_per(char* name) {
	struct stat stat_file;
	if (stat(name, &stat_file) == -1) {
		perror("check_per");
		exit(0);
	}
	printf("Num of hlinks: %ld, permissions: %04o\n", stat_file.st_nlink, stat_file.st_mode);
}

void change_mode(char* name, char* mode) {
	int perm = 0;
	int mul = 1;
	for (int i = 0; i < strlen(mode); i++) {
		perm += (mode[i] - '0') * mul;
		mul *= 8;
	}
	if (chmod(name, perm) == -1) {
		perror("Change mode");
		exit(0);
	}
}

int main(int argc, char** argv) {
	if (strcmp(argv[0], "./mkdir") == 0) {
		make_dir(argv[1]);
	}
	if (strcmp(argv[0], "./print_dir") == 0) {
		print_dir_content(argv[1]);
	}
	if (strcmp(argv[0], "./rmdir") == 0) {
		remove_dir(argv[1]);
	}
	if (strcmp(argv[0], "./touch") == 0) {
		create_file(argv[1]);
	}
	if (strcmp(argv[0], "./print_file") == 0) {
		print_file_content(argv[1]);
	}
	if (strcmp(argv[0], "./rmfile") == 0) {
		remove_file(argv[1]);
	}
	if (strcmp(argv[0], "./create_slink") == 0) {
		create_slink(argv[1], argv[2]);
	}
	if (strcmp(argv[0], "./print_slink") == 0) {
		read_slink(argv[1]);
	}
	if (strcmp(argv[0], "./rm_slink") == 0) {
		remove_file(argv[1]);
	}
	if (strcmp(argv[0], "./create_hlink") == 0) {
		create_hlink(argv[1], argv[2]);
	}
	if (strcmp(argv[0], "./check_per") == 0) {
		check_per(argv[1]);
	}
	if (strcmp(argv[0], "./chmod") == 0) {
		change_mode(argv[1], argv[2]);
	}
	//make_dir(argv[1]);
	//print_dir_content(name_dir);
	//remove_dir(name_dir);
	//strcat(name_dir, "/qwe");
	//remove(name_dir);
	//print_file_content(name_dir);
	//create_file(name_dir);
	//create_slink(name_dir, argv[2]);
	//remove_file(name_dir);
	//read_slink(name_dir);
	//create_hlink(name_dir, argv[2]);
	//check_per(name_dir);
	//change_mode(argv[1], argv[2]);
	return 0;
}