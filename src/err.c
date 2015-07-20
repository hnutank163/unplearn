#include <errno.h>
#include <stdarg.h>
#include "myunp.h"
#include <syslog.h>

static void err_doit(int errflaf, int level, const char *, va_list);

void
err_ret(const char *fmt, ... )
{
    va_list     ap;
     va_start(ap, fmt );
     err_doit(1, LOG_INFO, fmt, ap );
     va_end(ap);
    return;
}

void err_sys(const char *fmt, ... )
{
     va_list ap;
     va_start(ap, fmt);
     err_doit(1, LOG_ERR, fmt, ap);
     va_end(ap);
     exit(1);
}

void err_dump(const char * fmt, ...)
{
     va_list ap;

     va_start(ap, fmt);
     err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    abort();
    exit(1);
}

void err_msg(const char *fmt, ...)
{
     va_list ap;

     va_start(ap, fmt);
     err_doit(1, LOG_INFO, fmt, ap);
     va_end(ap);
     return;
}

void err_quit(const char * fmt, ...)
{
     va_list ap;

     va_start(ap, fmt);
     err_doit(0, LOG_ERR, fmt, ap);
     va_end(ap);
     exit(1);
}

static void
err_doit(int errflag, int level, const char *fmt, va_list ap)
{
    int     err_save, n;
    char    buf[MAXLINE];

    err_save = errno;
    vsnprintf(buf, MAXLINE, fmt, ap);
    n = strlen(buf);
    if(errflag)
        snprintf(buf+n, MAXLINE - n, ": %s", strerror(err_save));

    strcat(buf, "\n");

    fflush(stdout);
    fputs(buf, stderr);
    fflush(stderr);
}
