#include "unp.h"
#include <stdio.h>

int
main(int argc, char ** argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct sockaddr_in svraddr;

    if( argc !=2  )
    {
        err_sys("socket error");
    }

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 )
        err_sys("socket error");
    svraddr.sin_family = AF_INET;
    svraddr.sin_port =htons(13);

    if( inet_pton(AF_INET, argv[1], &svraddr.sin_addr) <=0 )
        err_quit("inet_ptop error for %s", argv[1]);

    if( connect(sockfd, (struct sockaddr *) &svraddr, sizeof(svraddr)) < 0 )
        err_sys("connect error");

    while( (n = read(sockfd, recvline, MAXLINE)) > 0 )
    {
         recvline[n] = 0;
         if( fputs(recvline, stdout) == EOF )
             err_sys("fputs error");
    }
    if(n < 0)
        err_sys("read error");

    exit(0);
}
