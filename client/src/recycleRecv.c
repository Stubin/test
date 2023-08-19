#include "head.h"
//循环接受
int recycleRecv(int fds,void *buf,int len,char *filename)
{
    int fd;
    char temp[1024]={0};
    if(filename!=NULL)
    {
        strcpy(temp,filename);
        strcat(temp,".download");
        fd = open(temp,O_CREAT|O_RDWR,0666);
    }
    char *p= (char*)buf;
    int total=0,ret;
    while(total<len)
    {
        ret = recv(fds,p+total,len-total,0);
        total += ret; 
        /* ftruncate(fd,0); */
        lseek(fd,0,SEEK_SET);
        //保存下载了多少用于断点续传
        write(fd,&total,sizeof(total));
        if(len>=FILESIZE)
        {
            printf("%5.2f%%\r",(float)total/len*100);
            fflush(stdout);

        }
    }
    if(filename!=NULL)
    {
        close(fd);
        remove(temp);
    }
    return 0;
}

