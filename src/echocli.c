#include "myunp.h"

#define SERV_PORT 8888
void str_cli(FILE *fp, int sockfd);

int
main(int argc, char ** argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if( argc != 2 )
        err_quit("usage <ipaddr>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    str_cli(stdin, sockfd);
    exit(0);
}


void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    while( fgets(sendline, MAXLINE, fp ) != NULL )
    {
        int ret = writen(sockfd, sendline, strlen(sendline));
        if( readline(sockfd, recvline, MAXLINE) == 0 )
            err_quit("str_cli: server terminated");

        fputs(recvline, stdout);
    }
}
