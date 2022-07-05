#include "unpipc.h"
#include <stdarg.h>
#include <errno.h>        /* for definition of errno */
#include <stdarg.h>        /* ANSI C header file */
#include <sys/syslog.h>


//MARK:- read
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    int        n;
    ssize_t rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
//        if ( (rc = _my_read(fd, &c)) == 1) {
        if ( (rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;//每次用_my_read函数读取一个char c，然后存入ptr，也就是vptr里面，然后指针右移，以便存入下一个char c
            if (c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            printf("读取到了文件末尾EOF \n");
            return(n - 1);    /* EOF, n - 1 bytes were read */
        } else{
            printf("readline error \n");
            return(-1);    /* error */
        }
    }

    *ptr = 0;
    return(n);
}

//MARK:- ERROR
int        daemon_proc;        /* set nonzero by daemon_init() */

static void    err_doit(int, int, const char *, va_list);

/* Nonfatal error related to system call
 * Print message and return */

void
err_ret(const char *fmt, ...)
{
    va_list        ap;

    va_start(ap, fmt);
    err_doit(1, LOG_INFO, fmt, ap);
    va_end(ap);
    return;
}

/* Fatal error related to system call
 * Print message and terminate */

void
err_sys(const char *fmt, ...)
{
    va_list        ap;

    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

/* Fatal error related to system call
 * Print message, dump core, and terminate */

void
err_dump(const char *fmt, ...)
{
    va_list        ap;

    va_start(ap, fmt);
    err_doit(1, LOG_ERR, fmt, ap);
    va_end(ap);
    abort();        /* dump core and terminate */
}

/* Nonfatal error unrelated to system call
 * Print message and return */

void
err_msg(const char *fmt, ...)
{
    va_list        ap;

    va_start(ap, fmt);
    err_doit(0, LOG_INFO, fmt, ap);
    va_end(ap);
    return;
}

/* Fatal error unrelated to system call
 * Print message and terminate */

void
err_quit(const char *fmt, ...)
{
    va_list        ap;

    va_start(ap, fmt);
    err_doit(0, LOG_ERR, fmt, ap);
    va_end(ap);
    exit(1);
}

/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */

static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
    int        errno_save;
    long        n;
    char    buf[MAXLINE + 1];

    errno_save = errno;        /* value caller might want printed */
#ifdef    HAVE_VSNPRINTF
    vsnprintf(buf, MAXLINE, fmt, ap);    /* safe */
#else
    vsprintf(buf, fmt, ap);                    /* not safe */
#endif
    n = strlen(buf);
    if (errnoflag)
        snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
    strcat(buf, "\n");

    if (daemon_proc) {
        syslog(level, "%s", buf);
    } else {
        fflush(stdout);        /* in case stdout and stderr are the same */
        fputs(buf, stderr);
        fflush(stderr);
    }
    return;
}

void sig_chld(int signo)
{
    pid_t    pid;
    int        stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
    }
    return;
}

//MARK:- IPC name
char *
px_ipc_name(const char *name)
{
    char    *dir, *dst, *slash;

    if ( (dst = malloc(PATH_MAX)) == NULL)
        return(NULL);

        /* 4can override default directory with environment variable */
    if ( (dir = getenv("PX_IPC_NAME")) == NULL) {
#ifdef    POSIX_IPC_PREFIX
        dir = POSIX_IPC_PREFIX;        /* from "config.h" */
#else
        dir = "/tmp/";                /* default */
#endif
    }
        /* 4dir must end in a slash */
    slash = (dir[strlen(dir) - 1] == '/') ? "" : "/";
    snprintf(dst, PATH_MAX, "%s%s%s", dir, slash, name);

    return(dst);            /* caller can free() this pointer */
}
/* end px_ipc_name */

char *
Px_ipc_name(const char *name)
{
    char    *ptr;

    if ( (ptr = px_ipc_name(name)) == NULL)
        err_sys("px_ipc_name error for %s", name);
    return(ptr);
}


//MARK:- Others
int
sleep_us(unsigned int nusecs)
{
    struct timeval    tval;

    if (nusecs == 0)
        return(0);

    for ( ; ; ) {
        tval.tv_sec = nusecs / 1000000;
        tval.tv_usec = nusecs % 1000000;
        if (select(0, NULL, NULL, NULL, &tval) == 0)
            return(0);        /* all OK */
        /*
         * Note than on an interrupted system call there's not
         * much we can do, since the timeval{} isn't updated with the time
         * remaining.  We could obtain the clock time before the call, and
         * then obtain the clock time here, subtracting them to determine
         * how long select() blocked before it was interrupted, but that
         * seems like too much work :-)
         */
        if (errno != EINTR)
            return(-1);
        /* else go around again */
    }
}

void
Sleep_us(unsigned int nusecs)
{
    if (sleep_us(nusecs) == -1)
        err_sys("sleep_us error");
}