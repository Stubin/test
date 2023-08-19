#include <func.h>

int tcpInit(char *ip,char *port,int *fd)
{
    int socketFd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    *fd=socketFd;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int val=1;
    setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(int));
    int ret = bind(socketFd,(struct sockaddr*)&addr,addrlen);
    ERROR_CHECK(ret,-1,"bind");
    ret = listen(socketFd,10);
    ERROR_CHECK(ret,-1,"listen");
    return 0;
}

