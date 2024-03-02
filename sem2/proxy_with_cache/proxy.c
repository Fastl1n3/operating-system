#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "cache.h"

#define BUFFER_SIZE 2000000
#define LIST_SIZE 10

int num_threads = 0;
cache_t cache_list;

void sig_handler(int signum) {
	//printf("Message from sig_handler\n");
	signal(signum, SIG_IGN);
	pthread_exit(NULL);
}

void clear_resourses(int client_socket, char* buffer, char* new_msg, char* dst_hostname, char* url) {
    close(client_socket);
    free(buffer);
    free(new_msg);
    free(dst_hostname);
    free(url);
    num_threads--;
}

void getUrl(char* msg_buf, char* url) {

    char* url_tmp = strstr(msg_buf, "http");

    int i = 0;
    while (url_tmp[i] != ' ') {
        url[i] = url_tmp[i];
        i++;
    }
    url[i] = '\0';
    printf("url: %s\n", url);
}

int parse_msg(char *msg_buf, int *rqst_length, char *new_msg, char *serv_hostname, char* url) {
    printf("parse_msg...\n");
    getUrl(msg_buf, url);
    int i = 0;
    char method[10] = {0};
    while(msg_buf[i] != ' ') {
        method[i] = msg_buf[i];
        i++;
    }
    strcpy(new_msg, method);
    strcat(new_msg, " ");

    int host_len = 0;
    i += 8;
    while (msg_buf[i] != '/') {
        serv_hostname[host_len] = msg_buf[i];
        host_len++;
        i++;
    }
    
    strcat(new_msg, msg_buf + i);
    char* version = strstr(new_msg, "HTTP/1");
    version[7] = '0';
    serv_hostname[host_len] = '\0';
    printf("serv_hostname: %s\n", serv_hostname);
    *rqst_length = strlen(new_msg);
    return 0;
}


void *proxy_thread(void * arg) {
    signal(SIGPIPE, sig_handler);
    int dst_socket, rqst_length, size;
    
    struct sockaddr_in dst_addr;
    struct hostent *dst_host;
    char *buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    char *new_msg = (char*)malloc(sizeof(char) * BUFFER_SIZE);  
    char *dst_hostname = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    char* url = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    int client_socket = *((int*) arg);
    
    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
    
    rqst_length = read(client_socket, buffer, BUFFER_SIZE);
    if(rqst_length < 0) {
        printf("Error read from client\n");
        write(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n", 28);
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }
    
    new_msg[0] = '\0';
    dst_hostname[0] = '\0'; 
    size = parse_msg(buffer, &rqst_length, new_msg, dst_hostname, url);
    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
 
    //=========================поиск из кеша
    node_t* cur_r_node = search_cache(&cache_list, url);

    if (cur_r_node != NULL) {
        printf("URL: %s has found in cache\n", url);
        printf("=============================================\n");
        ssize_t readed = 0;
        while (1) {
            
            memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
            ssize_t len = get_response_cache(cur_r_node, buffer, &readed, BUFFER_SIZE);
            if (len == -1) {
                printf("reading cache has end\n");
                break;
            }
            write(client_socket, buffer, len);
        }
        printf("exit\n");
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }

    if(size < 0) {
        write(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n", 28);
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }
    
    dst_host = gethostbyname(dst_hostname);
    if (dst_host == NULL) {
        printf("Cannot find dst_host\n");
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }
    
    memset((char*) &dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(80);
    bcopy((char*) dst_host->h_addr, (char*) &dst_addr.sin_addr.s_addr, dst_host->h_length);
    
    dst_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(dst_socket < 0) {
        printf("Error dst_socket\n");
        pthread_exit(NULL);
    }
   
    if(connect(dst_socket, (struct sockaddr*) &dst_addr, sizeof(dst_addr)) < 0) {
        printf("Error connect to dst_host\n");
        close(dst_socket);
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }

    size = write(dst_socket, new_msg, rqst_length);
    printf("new msg:\n%s\n", new_msg);
    
    if(size < 0) {
        printf("Error write to dst_host\n");
        close(dst_socket);
        clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
        pthread_exit(NULL);
    }
    
    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
    ssize_t readed = read(dst_socket, buffer, BUFFER_SIZE);
    node_t* cur_node = NULL;
    while (readed > 0) { 
        cur_node = add_cache(&cache_list, url, buffer, readed);
        ssize_t writed = write(client_socket, buffer, readed);
        if (writed < 0) {
            printf("$$$$ %d\n", cur_node->is_all);
            set_all(cur_node, 1);
            printf("ERROR write to client socket\n");
            close(dst_socket);
            clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
            pthread_exit(NULL);
        }
        
        memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
        readed = read(dst_socket, buffer, BUFFER_SIZE);
       
        if (readed < 0) {
            printf("$$$$ %d\n", cur_node->is_all);
            set_all(cur_node, 1);
            printf("ERROR read from dst_socket\n");
            close(dst_socket);
            clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
            pthread_exit(NULL);
        } 
    }
    set_all(cur_node, 1);

    printf("============================================= thread is finished\n");
    close(dst_socket);
    clear_resourses(client_socket, buffer, new_msg, dst_hostname, url);
    pthread_exit(NULL);
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Port is not defined\n");
        exit(1);
    }
    
    init_cache(&cache_list, LIST_SIZE);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("Failed to create server socket.\n");
        return 0;
    }
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("Failed to bind server socket.\n");
        return 0;
    }
    if (listen(serverSocket, 10) == -1) {
        printf("Failed to listen on server socket.\n");
        return 0;
    }
    printf("PORT : %d\n", atoi(argv[1]));
    
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        pthread_t thread;
        
        int client_socket = accept(serverSocket, (struct sockaddr*) &client_addr, &client_len);
        printf("Accepted\n");

        int err = pthread_create(&thread, NULL, proxy_thread, &client_socket);
        if(err) {
            printf("Error pthread create\n");
            close(client_socket);
        }
    }

    close(serverSocket);
    return 0;
}