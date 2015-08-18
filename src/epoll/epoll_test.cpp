#include <stdio.h>
#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
    ifstream in("index.html", ios::in);
    struct epoll_event event, events[MAXEVENTS];
    int fd, epfd;
    int ret;
    string sbuf;
    fd = create_and_bind(port);
    if( fd==-1 )
        return 1;

    if( 0 != listen(fd, 10) )
    {
        perror("listen");
        return 1;
    }

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

    if( in.is_open() )
    {
        istreambuf_iterator<char> beg(in), end;
        sbuf += string(beg, end);
        sbuf = string("HTTP/1.1 200 OK")+string("\r\n\r\n")+sbuf;
        cout<<sbuf;
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
            char addrbuf[128];
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
                              if(errno == EAGAIN )
                                break;
                              perror("accept");
                              break;
                         }
                        cout<<"recv connection from "<<inet_ntop(AF_INET, &addr.sin_addr,addrbuf, 16)<<" "<<ntohs(addr.sin_port)<<endl;
                        make_socket_nonblocking(connfd);
                        event.data.fd = connfd;
                        event.events = EPOLLET|EPOLLIN;
                        if( -1 == epoll_ctl(epfd,EPOLL_CTL_ADD, connfd, &event))
                            perror("epoll_ctl add connfd");
                    }
                }
                else
                {
                     int err = 0;
                     char buf[256];
                     int n=0, nread;
                     while(true)
                     {
                         nread = recv(sock, buf+n, 256, 0);
                         if(nread > 0)
                             n += nread;
                         else if(nread == -1)
                         {
                            if( errno != EAGAIN )
                                perror("read error");
                            break;
                         }
                         else if(nread == 0)
                         {
                              err = 1;
                              cout<<"client terminated\n";
                              close(sock);
                              epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
                              break;
                         }
                     }
                     if( !err  )
                     {
                        buf[n] = 0;
                        cout<<"recv "<<buf<<endl;
                        send(sock, sbuf.c_str(), sbuf.size(), 0);
                        close(sock);
             /*           event.events = ev | EPOLLOUT | EPOLLIN;
                        event.data.fd = sock;
                        if( -1 == epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &event) )
                            perror("epoll_ctl_mod");*/
                     }
                }
            }
            else if( ev & EPOLLOUT )
            {
                int nwrite = 0, size=sbuf.size(), n = sbuf.size();
                while( n>0 )
                {
                    nwrite = send(sock, sbuf.c_str()+size-n, n, 0);
                    if( nwrite<n )
                    {
                        if( nwrite == -1 && errno != EAGAIN )
                        {
                            perror("write");
                            break;
                        }
                    }
                    n -= nwrite;
                }
                close(sock);
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
    inet_pton(AF_INET, "192.168.110.132", &hints.ai_addr);
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
        else
        {
             int ops = 1;
             if( -1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&ops, sizeof(int)) )
                 perror("setsockopt");
             printf("setsockopt success\n");
        }

        ret = bind(fd, rp->ai_addr, rp->ai_addrlen);
        if( ret == 0 )
            break;
        else
        {
            close(fd);
            perror("bind");
            return -1;
        }
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















