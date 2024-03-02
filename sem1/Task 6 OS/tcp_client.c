#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int serv_sock;
    struct sockaddr_in servaddr;
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(serv_sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    while(1) {
        char buf[128];
        scanf("%s", buf);
        write(serv_sock, buf, strlen(buf));
        memset(buf, 0, sizeof(buf));
        read(serv_sock, buf, sizeof(buf));
        printf("%s\n", buf);
        if (strcmp(buf, "q") == 0) {
            printf("Exit\n");
            close(serv_sock);
            break;
        }
    }
    return 0;
}