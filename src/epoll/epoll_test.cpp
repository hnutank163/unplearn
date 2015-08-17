#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

using namespace std;
const int MAXEVENTS=128;
const char *port="9999";
int create_and_bind(const char *port);
int make_socket_nonblocking(int fd);

int
main()
{
    struct epoll_event event, events[MAXEVENTS];
    int fd, epfd;
    int ret;

    fd = create_and_bind(port);
    if( fd==-1 )
        return 1;

    ret = make_socket_nonblocking(fd);
    if(ret==-1)
        return 1;

    epfd = epoll_create1(EPOLL_CLOEXEC);
    if( -1 == epfd )
    {
        perror("epoll_create");
        return 1;
    }

    event.data.fd = fd;
    event.events = EPOLLIN|EPOLLET;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    if( ret==-1 )
    {
         perror("epoll_ctl");
         return 1;
    }

    memset(events, 0, sizeof(events));
    while(true)
    {
        int n, i;
        n = epoll_wait(epfd, events, MAXEVENTS, -1);

        for(i=0; i<n; i++)
        {
            uint32_t ev = events[i].events;
            int sock = events[i].data.fd;
            if( ev & EPOLLIN ) //read
            {
                if( sock == fd )
                {//new connection come
                    while(true)
                    {
                        struct sockaddr_in addr;
                        socklen_t socklen = sizeof(addr);
                         int connfd = accept(fd, (struct sockaddr *)&addr, &socklen);
                         if( connfd == -1 )
                         {
                              if(errno == EAGAIN || errno == EINTR)
                              {
                                  usleep(1000);
                                  continue;
                              }
                              break;
                         }


                    }
                }
            }
        }
    }
}

int create_and_bind(const char * port)
{
    struct addrinfo hints;
    struct addrinfo *result,  *rp;
    int ret;
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(NULL, port, &hints, &result);
    if( ret != 0 )
    {
         fprintf(stderr, "getaddrinfo :%s\n", gai_strerror(ret));
         return -1;
    }

    for( rp=result; rp != NULL; rp = result->ai_next )
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if( fd < 0 )
            continue;

        ret = bind(fd, rp->ai_addr, rp->ai_addrlen);
        if( ret == 0 )
            break;
        close(fd);
    }

    if( rp == NULL ) //mean bind failed
    {
         perror("bind");
         return -1;
    }

    freeaddrinfo(result);
    return fd;
}


int make_socket_nonblocking(int fd)
{
     int flag, ret;

     flag = fcntl(fd, F_GETFL, 0);
     if( flag == -1 )
     {
         perror("fcntl");
         return -1;
     }

     ret = fcntl(fd, F_SETFL, flag|O_NONBLOCK);
     if( ret == -1 )
     {
          perror("fcntl");
          return -1;
     }

     return 0;
}















