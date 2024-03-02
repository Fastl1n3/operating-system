#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define SIZE 100

void reverse_name(char* old_name, char* new_name) {
	int len = strlen(old_name);
	for (int i = len - 1; i >= 0; i--) {
		new_name[len - i - 1] = old_name[i];
	}
}

void copy_bytes(int fdr, int fdw) {
	char buf;
	int offset = -1;
	int ptr = lseek(fdr, -1, SEEK_END);
	while (ptr != -1) {
		read(fdr, &buf, 1);
		write(fdw, &buf, 1);
		ptr = lseek(fdr, -2, SEEK_CUR);
	}
}

int main(int argc, char** argv) {	
	char name_dir[SIZE];
	if (argc < 2) {
		perror("No dir");
		return 0;
	}
	strcpy(name_dir, argv[1]);
	DIR* dr = opendir(name_dir);
	if (dr == NULL) {
		perror(name_dir);
		return 0;
	}
	
	char* name_newdir = (char*)calloc(SIZE, sizeof(char));
	reverse_name(name_dir, name_newdir);
	strcat(name_newdir, "/");
	strcat(name_dir, "/");
	mkdir(name_newdir, 0777);
	
	struct dirent* actual_file;
	int fdr, fdw;
	while (actual_file = readdir(dr)) {
		if (actual_file->d_type != DT_REG) {
			continue;
		}
		char* name_newfile = (char*)calloc(SIZE, sizeof(char));
		char* name_allpath = (char*)calloc(SIZE, sizeof(char));
		
		strcat(name_allpath, name_dir);
		strcat(name_allpath, actual_file->d_name);
		fdr = open(name_allpath, O_RDONLY);
		memset(name_allpath, 0, strlen(name_allpath));
		if (fdr == -1) {
			perror(name_allpath);
		}

		reverse_name(actual_file->d_name, name_newfile);
		strcat(name_allpath, name_newdir);
		strcat(name_allpath, name_newfile);
		fdw = open(name_allpath, O_CREAT | O_WRONLY);
		if (fdw == -1) {
			perror(name_allpath);
		}
		copy_bytes(fdr, fdw);
		free(name_allpath);
		free(name_newfile);
		close(fdr);
		close(fdw);
	}
	return 0;
}