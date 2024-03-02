#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //��� fork � execvp
#include <sys/ptrace.h>
#include <sys/user.h> //��� ���������
#include <sys/wait.h>
int main ( int argc, char * argv[] ) {
	int status; 
	struct user_regs_struct regs;
	int counter = 0;
	int in_call =0;
	pid_t pid = fork();
	switch(pid) {
		case -1: 
			perror("fork");
			exit(1);

		case 0: //child
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			if (argc == 1) {
				printf("No args\n");
				exit(1);
			}
			if (execvp(argv[1], argv + 1) == -1) { // �������� ���������� �����. ��������� ������� � ���������� ������ ��������
				perror("execvp");
				exit(1);
			}

		default: //parent
			wait(&status); // ���� ������ �� ���� ��� ���. ������� ����������� �� execvp		
			while(!WIFEXITED(status)) {
				ptrace(PTRACE_GETREGS, pid, NULL, &regs);
				if(!in_call) {
					printf("SystemCall %lld\n",regs.orig_rax);
					in_call = 1;
					counter++;
				}
				else {
					in_call = 0;
				}
				ptrace(PTRACE_SYSCALL, pid, NULL, NULL); //��������� ������� � ������. ��������� �����. ��������
				wait(&status); // ���� ������ �� ���� ��� ����. ������ ������� ��� ���������� 
			}
			if (regs.orig_rax == 231) {
				printf("SystemCall %lld\n", regs.orig_rax);
				counter++;
			}
	}
	printf("Number of system calls = %d\n", counter);
	return 0; 
}
