#include "myunp.h"
#include <time.h>

int
main(int argc, char ** argv)
{
    int listenfd, connfd;
    int port = 13;
    struct sockaddr_in svraddr, cliaddr;
    char sendline[MAXLINE];

    if( argc == 2 )
        port = atoi(argv[1]);

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0 )
        err_sys("create socket error");

    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(port);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( 0 != bind(listenfd, (struct sockaddr *)&svraddr, sizeof(svraddr )) )
        err_sys("bind error");

    if( 0 != listen(listenfd, 5) )
        err_sys("listen error");

    for(;;)
    {
        time_t ticks ;
        socklen_t len = sizeof(cliaddr );
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len );
        printf("recv connection from %s %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, sendline, sizeof(sendline)),
                ntohs(cliaddr.sin_port));
        ticks = time(NULL);
        snprintf(sendline, sizeof(sendline ), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendline, strlen(sendline));
        close(connfd);
    }
}

