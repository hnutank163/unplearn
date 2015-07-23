#include "myunp.h"

#define SEVR_PORT 8888

void str_echo(int fd);
void sig_child(int signo);

#define MAXCLIENT 512

int
main(int argc, char ** argv)
{
    int listenfd, connfd, i, maxindex;
    int n, nready;
    int client[MAXCLIENT];
    fd_set rset;
    int maxfds;
    char buf[MAXLINE];
    struct sockaddr_in svraddr, cliaddr;
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(SEVR_PORT);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenfd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    Listen(listenfd, 17);
    Signal(SIGCHLD, sig_child);
    FD_ZERO(&rset);
    maxfds = listenfd + 1;
    socklen_t clilen = sizeof(cliaddr);

    for(i=0; i<MAXCLIENT; i++)
        client[i] = -1;
    maxindex = 0;
    for(;;)
    {
        FD_SET(listenfd, &rset);
        nready = Select(maxfds, &rset, NULL, NULL, NULL);
        if( FD_ISSET(listenfd, &rset) )
        {
            connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            printf("Accept Connection from %s %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(cliaddr)), ntohs(cliaddr.sin_port));
            maxfds = MAX(maxfds, connfd) + 1;
            for(i=0; i<MAXCLIENT; i++)
            {
                if( client[i] < 0 )
                {
                    client[i] = connfd;
                    break;
                }
            }
            if( i == MAXCLIENT )
                err_quit("too many clients");
            FD_SET(connfd, &rset);
            maxfds = MAX(maxfds, connfd) + 1;
            maxindex = MAX(maxindex, i);   //record max index of client valid
        }

        for(i=0; i<maxindex; i++)
        {
            int sockfd;
            if( (sockfd = client[i]) < 0 )    //find valid socket fd
                continue;

            printf("sockfd = %d\n", sockfd);
            if( FD_ISSET(sockfd, &rset) )
            {
                printf("client FD_ISSET\n");
                if( (n = read(sockfd, buf, MAXLINE)) == 0 ) // read EOF
                {
                     close(sockfd);
                     FD_CLR(sockfd, &rset);
                     client[i] = -1;
                }
                else
                    writen(sockfd, buf, n);
            }
            if( -- nready == 0 )
                break;  //process all readable socket fd
        }
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
