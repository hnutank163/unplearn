#ifndef _EVENT_H_
#define _EVENT_H_
#include <sys/epoll.h>

class event
{
public:
    event();
    ~event();

private:
    int fd;
    int epfd;
    void (*read_cb) (int fd, short what, void *arg);
    void (*write_cb) (int fd, short what, void *arg);
};

#endif

