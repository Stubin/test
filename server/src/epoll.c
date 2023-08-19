#include <func.h>

int epollAdd(int epfd,int fd)
{
    struct epoll_event event;
    event.data.fd = fd ;
    event.events = EPOLLIN| EPOLLRDHUP;
    int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
    ERROR_CHECK(ret,-1,"epoll_ctl_add");
    return 0;
}

