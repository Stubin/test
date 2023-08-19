#include <43func.h>
//循环接受
int recycleRecv(int fds,void *buf,int len)
{
    char *p= (char*)buf;
    int total=0,ret;
    while(total<len)
    {
        ret = recv(fds,p+total,len-total,0);
        if(ret == 0)
        {
            return -1;
        }
        total += ret; 

    }
    return 0;
}

