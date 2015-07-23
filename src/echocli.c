#include "myunp.h"

#define SERV_PORT 8888
void str_cli(FILE *fp, int sockfd);
void str_cli_select(FILE *fp, int sockfd);

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
    str_cli_select(stdin, sockfd);
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


void str_cli_select(FILE *fp, int sockfd)
{
     int maxfds, stdinsign;
     fd_set rset;
     char buf[MAXLINE];
     int n;

     stdinsign = 0;

     FD_ZERO(&rset);

     for(;;)
     {
          if(stdinsign == 0)
              FD_SET(fileno(fp), &rset);
          FD_SET(sockfd, &rset);
          maxfds = MAX(fileno(fp), sockfd) + 1;
          Select(maxfds, &rset, NULL, NULL, NULL);
          if( FD_ISSET(sockfd, &rset) )
          {
               if( (n=read(sockfd, buf, MAXLINE)) == 0 )
               {
                   if( stdinsign )
                   {
                        printf("normal terminated\n");
                        return ;
                   }
                   err_quit("str_cli: server terminated");

               }
               write(fileno(stdout), buf, n);
          }

          if( FD_ISSET(fileno(fp), &rset) )
          {
               if( ( n = read(fileno(fp), buf, MAXLINE) ) == 0 )
               {
                   stdinsign = 1;
                   shutdown(sockfd, SHUT_WR);
                   FD_CLR(fileno(fp), &rset);
                   continue;
               }
               writen(sockfd, buf, n);
          }
     }
}
