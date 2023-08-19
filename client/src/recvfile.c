#include "head.h"
#include "utils.h"

int recycleRecv(int,void*,int,char*);
int recvfile(char *arg,int socketFd)
{
    struct stat statbuf;
    char buf[65535]={0};
    int datelen=0;
    int type;
    train train;
    recycleRecv(socketFd,&type,4,NULL);
    //接受文件名
    recycleRecv(socketFd,&datelen,4,NULL);
    recycleRecv(socketFd,buf,datelen,NULL);
    int fd,ret;
    fd = open(buf,O_RDWR|O_CREAT,0666);
    char filename[1024]={0};
    strcpy(filename,buf);
    ERROR_CHECK(fd,-1,"open");
    //获取文件大小用于断点下载
    ret = fstat(fd,&statbuf);
    ERROR_CHECK(ret,-1,"fstat");
    train.type=1;
    if(statbuf.st_size != 0)
    {
        //发送偏移给服务器
        train.datalen = sizeof(statbuf.st_size);
        memcpy(train.buf,&statbuf.st_size,sizeof(statbuf.st_size));
        send(socketFd,&train,train.datalen+4+4,0);
        lseek(fd,0,SEEK_END);
    }else
    {
        train.datalen = sizeof(statbuf.st_size);
        memcpy(train.buf,&statbuf.st_size,sizeof(statbuf.st_size));
        send(socketFd,&train,train.datalen+4+4,0);
    }
    //接受文件大小
    off_t filesize;    
    recycleRecv(socketFd,&type,4,NULL);
    recycleRecv(socketFd,&datelen,4,NULL);
    recycleRecv(socketFd,&filesize,datelen,NULL);
    printf("filesize = %ld\n",filesize);
    //发送小文件
    if(filesize<FILESIZE)
    {
        while(1)
        {
            recycleRecv(socketFd,&type,4,NULL);
            recycleRecv(socketFd,&datelen,4,NULL);
            if(datelen == 0)
            {
                break;
            }
            recycleRecv(socketFd,&buf,datelen,NULL);
            /* printf("buf = %s\n",buf); */
            write(fd,buf,datelen);
        }

    }
    else
    {
        //发送大文件
        ftruncate(fd,filesize);
        //读取已经下载的字节数
        long haveDown;
        char downFile[1024]={0};
        strcpy(downFile,filename);
        strcat(downFile,".download");
        int fd1 = open(downFile,O_RDWR);
        //文件能打开读取，没有就为0
        if(fd1 == -1)
        {
            haveDown = 0;
        }else
        {
            read(fd1,&haveDown,sizeof(long));

        }
        //发送给服务器已经下载的字节数
        printf("haveDown = %ld\n",haveDown);
        train.type=1;
        train.datalen = sizeof(haveDown);
        memcpy(train.buf,&haveDown,sizeof(haveDown));
        send(socketFd,&train,train.datalen+4+4,0);
        //发送数据
        char *pMap = (char*)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        ERROR_CHECK(pMap,(char*)-1,"mmap"); 
        recycleRecv(socketFd,pMap+haveDown,filesize-haveDown,filename);
        munmap(pMap,filesize);
    }
    close(fd);
    return 0;
}

