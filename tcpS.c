#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#define MAX 80
#define PORT 8080
#define MIN 60
#define SA struct sockaddr
#define TRUE 1
#define FALSE 0

void func(int connfd);

int main() {
	int sockfd, len, cnt=0, on=1;
	struct sockaddr_in servaddr, cli;
	char buffer[80];
	fd_set rdfs;
	struct timeval tv;
	int retval, ready, new_fd, end_server = FALSE;
	int close_conn, foo;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else	printf("Socket successfully created..\n");

	// if((ioctl(sockfd, FIONBIO, (char *)&on)) == -1) {
	// 	perror("ioctl() failed..\n");
	// 	close(sockfd);
	// 	exit(EXIT_FAILURE);
	// }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else	printf("Socket successfully binded..\n");

	if((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else	printf("Server listening..\n");

	FD_ZERO(&rdfs);
	int maxS = sockfd;
	FD_SET(maxS, &rdfs);

	tv.tv_sec = 2*MIN;
	tv.tv_usec = 0;

	printf("\nsockfd: %d\n", sockfd);

	printf("select() is going on..\n");
	retval = select(maxS+1, &rdfs, NULL, NULL, &tv);
	printf("select() return %d\n", retval);
	if (retval == -1) {
		perror("----------->select() error\n");
		exit(EXIT_FAILURE);
	}
	else if (retval == 0){
		printf("----------->timed out..\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i<=maxS; i++) {
		if(FD_ISSET(i, &rdfs)){
				printf("i: %d\n", i);
				new_fd = accept(sockfd, NULL, NULL);
				printf("accepted %d socket\n", sockfd);
				if(new_fd == -1) {
					if(errno != EWOULDBLOCK) {
						printf("accept() error..\n");
						exit(EXIT_FAILURE);
					}
				}
				FD_SET(new_fd, &rdfs);
				if(new_fd > maxS)	maxS = new_fd;

				foo = read(new_fd, buffer, MAX);
				printf("read return: %d\n", foo);
				if(foo == -1) {
					perror("read() failed..\n");
					exit(EXIT_FAILURE);
				}
				printf("%d bytes recived as %s\n", foo, buffer);
				len = foo;
				foo = write(new_fd, buffer, len);
				if(foo == -1) {
					perror("send() failed..\n");
					exit(EXIT_FAILURE);
				}
		}
	}

	return 0;
}

//chatting function
void func(int connfd)
{
	char buff[MAX];
	int n;
	for (;;)
	{
		bzero(buff, MAX);
		read(connfd, buff, sizeof(buff));
		printf("From client: %sTo client : ", buff);

		bzero(buff, MAX);
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(connfd, buff, sizeof(buff));

		if (strncmp("exit", buff, 4) == 0)
		{
			printf("Server Exit...\n");
			break;
		}
	}
}