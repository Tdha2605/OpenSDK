#ifndef _POLL_H_
#define _POLL_H_


#define POLLIN  1
#define POLLPRI 2
#define POLLOUT 4
#define POLLERR 8
#define POLLHUP 16

#define SYS_POLLIN  POLLIN
#define SYS_POLLPRI POLLPRI
#define SYS_POLLOUT POLLOUT
#define SYS_POLLERR POLLERR
#define SYS_POLLHUP POLLHUP

typedef unsigned int nfds_t ;


struct pollfd {
    int fd;
    short events;
    short revents;
};

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif









