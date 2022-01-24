#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>

#define SOCK_PATH  "tpf_unix_socketC.server"
#define MAX 256
void func(int connfd);

int main() {
    int sockfd, len, rc, cli_sock;
    struct sockaddr_un srvrAddr, cliAddr;
    char buffer[MAX];
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket() failed..\n");
        exit(EXIT_FAILURE);
    }
    memset(&srvrAddr, 0, sizeof(srvrAddr));
    memset(&cliAddr, 0, sizeof(cliAddr));
    memset(buffer, 0, sizeof(buffer));
    srvrAddr.sun_family = AF_UNIX;
    strcpy(srvrAddr.sun_path, SOCK_PATH);
    len = sizeof(srvrAddr);
    if((bind(sockfd, (const struct sockaddr*)&srvrAddr, len)) == -1) {
        perror("bind failed..\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    rc = listen(sockfd, 10);
    if(rc == -1) {
        perror("listen() failed..\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    len = sizeof(cliAddr);
    cli_sock = accept(sockfd, (struct sockaddr*)&cliAddr, &len);
    if(rc == -1) {
        perror("accept() failed..\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }else {
        printf("accept()..\n");
    }

    rc = read(cli_sock, buffer, sizeof(buffer));
    printf("from client: %s", buffer);
    write(cli_sock, buffer, MAX);
    //func(cli_sock);

    return 0;
}

void func(int connfd)
{
    char buff[MAX];
    int n;
    int rc;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        rc = read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %d\t To client : ", rc);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');
   
        // and send that buffer to client
        write(connfd, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}