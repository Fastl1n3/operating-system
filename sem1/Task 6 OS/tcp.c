#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8888

int main() {
	int srv_sock;
	int clt_sock;
	int pid;
	srv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_sock == -1) {
		printf("Create socket error %s\n", strerror(errno));
		return -1;
	}
	struct sockaddr_in srv_sockaddr; //хранит порт и ip отдельно
	memset(&srv_sockaddr, 0, sizeof(srv_sockaddr));
	srv_sockaddr.sin_family = AF_INET;
	srv_sockaddr.sin_port = htons(PORT);
	srv_sockaddr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(srv_sock, (struct sockaddr*)&srv_sockaddr, sizeof(srv_sockaddr)) == -1) {
		printf("Bind socket error: %s\n", strerror(errno));
		close(srv_sock);
		return -1;
	}
	
	if (listen(srv_sock, 8) == -1) {
		printf("Listen error: %s\n", strerror(errno));
		close(srv_sock);
		return -1;
	}
	struct sockaddr_in clt_sockaddr;
	socklen_t len;
	while (1) {
		memset(&clt_sockaddr, 0, sizeof(clt_sockaddr));
		clt_sock = accept(srv_sock, (struct sockaddr*)&clt_sockaddr, &len);
		if (clt_sock == -1) {
			printf("Accept error: %s\n", strerror(errno));
			close(srv_sock);
			return -1;
		}
		else {
			printf("Accept client: %s:%d\n", inet_ntoa(clt_sockaddr.sin_addr), clt_sockaddr.sin_port);
		}	
		pid = fork();
		if (pid == 0) {
			break;
		}
		close(clt_sock);
	}
	close(srv_sock);
	while (1) {
		int ret;
		char buf[128];
		ret = read(clt_sock, buf, sizeof(buf));
		ret = write(clt_sock, buf, ret);
		if (ret == 0 || ret == -1) {
			printf("Disconnected %s:%d\n", inet_ntoa(clt_sockaddr.sin_addr), clt_sockaddr.sin_port);
			close(clt_sock);
			exit(0);
		}
	}
	
	return 0;
}