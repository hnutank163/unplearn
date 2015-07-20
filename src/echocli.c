#include "myunp.h"

#define SERV_PORT 8888
void str_cli(int connfd);

int
main(int argc, char ** argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if( argc != 2 )
        err_quit("usage <ipadr>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr = inet
}
