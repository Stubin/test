#include "head.h"
#define FILESIZE 10485760
int tranfile(char* filename,int newFd,char *realName)
{
    //先打开文件
    printf("md5= %s\n",filename);
    printf("realFilename = %s\n",realName);
    int fd = open(filename,O_RDWR);
    ERROR_CHECK(fd,-1,"open");
    //定义小火车，datelen代表要发送的数据长度,buf代表要发送的数据
    train train;
    train.type=1;
    //先发文件名
    train.datalen = strlen(realName);
    strcpy(train.buf,realName);
    send(newFd,&train,train.datalen+8,0);
    //接受文件偏移
    off_t offset;
    recycleRecv(newFd,&train.type,4);
    recycleRecv(newFd,&train.datalen,4);
    recycleRecv(newFd,&offset,train.datalen);
    lseek(fd,offset,SEEK_SET);
    //发文件大小
    struct stat buf;
    fstat(fd,&buf);
    train.datalen = sizeof(buf.st_size);
    memcpy(train.buf,&buf.st_size,train.datalen);
    send(newFd,&train,train.datalen+8,0);

    long total=0;
    if(buf.st_size<FILESIZE)
    {
        //循环发送文件内容
        while((train.datalen = read(fd,train.buf,sizeof(train.buf))))
        {
            int ret = send(newFd,&train,train.datalen+8,0);
            if(ret == -1)
            {
                break;
            }
            total+=ret;
        }
        printf("total = %ld\n",total);
        //发送文件结束标志
        send(newFd,&train,train.datalen+8,0);

    }else
    {
        //获取已经下载的字节数
        recycleRecv(newFd,&train.type,4);
        recycleRecv(newFd,&train.datalen,4);
        recycleRecv(newFd,&offset,train.datalen);
        printf("offset = %ld\n",offset);
        //mmap发文件
        char *pMap = (char*)mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        ERROR_CHECK(pMap,(char*)-1,"mmap");
        total = send(newFd,pMap+offset,buf.st_size-offset,0);
        munmap(pMap,buf.st_size);
    }
    close(fd);
    return 0;
}

