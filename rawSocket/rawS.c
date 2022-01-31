#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include "headerStruct.h"

#define PKT_LEN 8102
#define PORT 8080
#define handle_error(Emsg) do { perror(Emsg); exit(EXIT_FAILURE); } while(0);

unsigned short checksum(unsigned short *buffer, int n) {
    unsigned long sum;

    for(sum = 0; n > 0; n--) {
        sum += *buffer++;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

int main() {
    int sockfd, rc;
    char buffer[PKT_LEN], msg[20];
    struct sockaddr_in sin, cli;
    struct ipHeader *ip = (struct ipHeader*)buffer;
    struct udpHeader *udp = (struct udpHeader*) (buffer + sizeof(struct ipHeader));

    int one = 1;
    const int *val = &one;

    memset(buffer, 0, sizeof(PKT_LEN));
    memset(msg, 0, sizeof(msg));
    memset(&sin, 0, sizeof(sin));
    memset(&cli, 0, sizeof(cli));

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) handle_error("socket() failed..\n");
    printf("socket() success..\n");

    sin.sin_family = AF_INET;//type
    sin.sin_addr.s_addr = INADDR_ANY;//IP address
    sin.sin_port = htons(PORT);//port number

    ip->ip_ihl = 5;
    ip->ip_ver = 4;
    ip->ip_tos = 16;
    ip->ip_length = sizeof(struct ipHeader) + sizeof(struct udpHeader);
    ip->ip_ident = htons(54321);
    ip->ip_ttl = 64;
    ip->ip_protocol = 17;
    ip->ip_sIP = inet_addr("127.0.0.1");
    ip->ip_dIP = inet_addr("127.0.0.1");
    ip->ip_checksum = 0;
    
    udp->udp_sPort = htons(PORT);
    udp->udp_dPort = htons(PORT);
    udp->udp_length = sizeof(struct udpHeader);
    udp->udp_checksum = 0;

    int n = sizeof(struct udpHeader) + sizeof(struct ipHeader);

    rc = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one));
    if(rc < 0) handle_error("setsockopt() failed..\n");
    printf("setsockopt() success..\n");

    rc = bind(sockfd, (const struct sockaddr*)&sin, sizeof(sin));
    if(rc == -1) handle_error("bind() failed..\n");
    printf("bind() success..\n");

    int len = sizeof(cli);

    rc = recvfrom(sockfd, msg, strlen(msg), MSG_WAITALL, (struct sockaddr*)&cli, &len);
    if(rc == -1) handle_error("recvfrom() failed..\n");
    msg[rc] = '\0';
    printf("%d bytes of msg from client: %s\n", rc, msg);
    
    rc = sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr*)&cli, len);
    printf("sendto(): %d\n", rc);

    close(sockfd);
    return 0;
}