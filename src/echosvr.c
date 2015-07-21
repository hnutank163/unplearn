#include "myunp.h"

#define SEVR_PORT 8888

void str_echo(int fd);
void sig_child(int signo);

int
main(int argc, char ** argv)
{
    int listenfd, connfd;
    char buf[MAXLINE];
    struct sockaddr_in svraddr, cliaddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(SEVR_PORT);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    Listen(listenfd, 17);
    Signal(SIGCHLD, sig_child);
    for(;;)
    {
        pid_t pid;
        socklen_t clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("accept connection from %s %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(cliaddr)), ntohs(cliaddr.sin_port));
        if( (pid = fork()) == 0 ) //in child process
        {
            printf("enter child process\n");
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

     printf("enter str_echo\n");
     for(;;)
     {
          n = readline(fd, buf, MAXLINE);
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

void sig_child(int signo)
{
     pid_t pid;
     int stat;
     if( (pid = waitpid(-1, &stat, WNOHANG) ) > 0)
         printf("child precess %d terminated\n", pid);
}
