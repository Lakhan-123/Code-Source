#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>

char *PATH = "tpf_unix_socketC.server";
#define MAX 20
void func(int connfd);

int main() {
    int rc, len, sockfd;
    struct sockaddr_un cliAddr;
    char buffer[MAX];

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket() failed..\n");
        exit(EXIT_FAILURE);
    }

    memset(&cliAddr, 0, sizeof(struct sockaddr_un));

    cliAddr.sun_family = AF_UNIX;
    memcpy(cliAddr.sun_path, PATH, strlen(PATH));
    //cliAddr.sun_path = PATH;
    len = sizeof(cliAddr);
   
    printf("waiting on connect()..\n");
    rc = connect(sockfd, (const struct sockaddr*)&cliAddr, len);
    if(rc == -1) {
        perror("connect() failed..\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("To server: ");
    int n=0;
    while((buffer[n++] = getchar()) != '\n'); 
    write(sockfd, buffer, MAX);
    memset(buffer, 0, MAX);
    read(sockfd, buffer, sizeof(buffer));
    printf("from server: %s", buffer);
    //func(sockfd);
    close(sockfd);

    return 0;
}

void func(int sockfd)
{
    char buff[MAX];
    int n, rc;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        rc = read(sockfd, buff, sizeof(buff));
        printf("From client: %d\n", rc);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}