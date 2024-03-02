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
	srv_sock = socket(AF_INET, SOCK_DGRAM, 0);
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
	struct sockaddr_in clt_sockaddr;
	socklen_t len = sizeof(struct sockaddr);
	while (1) {
		char buf[128];
		int n = recvfrom(srv_sock, buf, sizeof(buf), 0, (struct sockaddr*)&clt_sockaddr, &len);
		printf("Recieved dgram from %s:%d; len = %d\n", inet_ntoa(clt_sockaddr.sin_addr), clt_sockaddr.sin_port, n);
		sendto(srv_sock, buf, n, 0, (struct sockaddr*)&clt_sockaddr, len);
	}

	return 0;
}