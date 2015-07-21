#include "myunp.h"

ssize_t readn(int fd, void *vptr, size_t len)
{
    int nleft, nread;
    char *ptr;

    nleft = len;
    ptr = (char *) vptr;
    for(;;)
    {
        nread = read(fd, ptr, nleft);
        if( nread == 0)
            break;    //EOF
        else if(nread <0)
        {
            if(errno == EINTR)
                nread = 0 ; //continue read
            else
                return -1;
        }

        nleft -= nread;
        ptr += nread;
    }

    return (len - nleft);
}

ssize_t writen(int fd, const void *vptr, size_t len)
{
    int nleft, nwrite;
    const char *ptr;

    ptr = vptr;
    nleft = len;

    while( nleft > 0 )
    {
        nwrite = write(fd, ptr, nleft);
        if( nwrite <= 0 )
        {
            if( nwrite < 0 && errno == EINTR  )
                nwrite = 0;
            else
                return -1;
        }

        nleft -= nwrite;
        ptr += nwrite;
    }

    return len ;
}


static int read_cnt = 0;
static char read_buf[MAXLINE];
static char * read_ptr;
static ssize_t my_read(int fd, char *ptr)
{
    if(read_cnt <= 0)
    {
         for(;;)
         {
             read_cnt = read(fd, read_buf, sizeof(read_buf));
             if( read_cnt <0)
             {
                 if( errno == EINTR )
                     continue;
                 return -1;
             }
             else if(read_cnt == 0)
                 return 0;
            read_ptr = read_buf;
            break;
         }
    }
    read_cnt -- ;
    *ptr = *read_ptr ++ ;
    return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    size_t n;
    ssize_t rc;
    char ch, *ptr;

    ptr = vptr;
    for(n=1; n<maxlen; n++) //last byte is terminate
    {
         rc  = my_read(fd, &ch);
         if( rc == 1 )
         {//read one char
            *ptr++ = ch;
            if(ch == '\n')
                break;
         }
         else if( rc == 0 )
         {
              *ptr = 0; //terminate
              return n-1;
         }
         else
             return -1;
    }

    *ptr = 0;
    return n;
}


int  Accept(int fd, struct sockaddr * addr, socklen_t *len )
{
    int n;
    for(;;)
    {
        n = accept(fd, addr, len);
        if( n <0  )
        {
            if( errno == EPROTO || errno == ECONNABORTED )
                continue;
            else
                err_sys("accept error");
        }
        return n;
    }
}

void Bind(int fd, struct sockaddr * addr, socklen_t len )
{
     if( 0 != bind(fd, addr, len ))
         err_sys("bind error");
}

void Connect(int fd, struct sockaddr *addr, socklen_t len)
{
     if( connect(fd, addr, len) < 0 )
         err_sys("connect error");
}

void Listen(int fd, int backlog)
{
     if( listen(fd, backlog) < 0 )
         err_sys("listen error");
}

Sigfunc *Signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    if(signo == SIGALRM)
    {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    }
    else
    {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }

    if(sigaction(signo, &act, &oact) < 0 )
        return SIG_ERR;

    return oact.sa_handler;
}
