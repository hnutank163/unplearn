#include "myunp.h"

#define SEVR_PORT 8888

void str_echo(int fd);

int
main(int argc, char ** argv)
{
    int listenfd, connfd;
    struct sockaddr_in svraddr, cliaddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(SEVR_PORT);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    Listen(listenfd, 17);

    for(;;)
    {
        pid_t pid;
        socklen_t clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        if( (pid = fork()) == 0 ) //in child process
        {
             close(listenfd);
             str_echo(connfd);
             exit(0); //exit child process
        }
        close(connfd);
    }
}

void str_echo(int fd)
{
     ssize_t n;
     char buf[MAXLINE];

     for(;;)
     {
          n = readn(fd, buf, MAXLINE);
          if( n > 0 )
              writen(fd, buf, n);
          else if( n ==0 )
              break; //EOF
          else if(n < 0 && errno == EINTR)
          {
              continue;
          }
          else
            err_sys("str_echo: read error");
     }

}
