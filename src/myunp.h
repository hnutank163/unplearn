#ifndef _MY_UNP_H
#define _MY_UNP_H

#include <sys/types.h>
#include <sys/socket.h>  //socket definitions
#include <sys/un.h>        //unix domian socket

#include <sys/time.h>
#include <time.h>

#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>  //inet_xxx functions
#include <errno.h>
#include <fcntl.h>          //set nonblocking .etc
#include <netdb.h>

#include <sys/select.h>

#include <signal.h>
#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/uio.h>

#include <strings.h>
#include <sys/ioctl.h>
#include <pthread.h>

//const value defines
#define MAXLINE 4096
#define BUFFSIZE 8192

#define MAX(a, b) (a) > (b) ? (a) : (b)

//function when error happend;
void    err_dump(const char *, ...);
void    err_msg(const char *, ... );
void    err_quit(const char *, ... );
void    err_ret(const char *, ... );
void    err_sys(const char *, ... );

//basic socket fucntions
ssize_t readn(int fd, void *vptr, size_t len );
ssize_t writen(int fd, const void *, size_t len );
ssize_t readline(int fd, void *vptr, size_t len);

int Accept(int fd, struct sockaddr * addr, socklen_t *len);
void Bind(int fd, struct sockaddr *, socklen_t len);
void Connect(int fd, struct sockaddr * addr, socklen_t len);
void Listen(int, int);

int Select(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tva);

typedef void Sigfunc(int);
Sigfunc *Signal(int signo, Sigfunc *func);

#endif
