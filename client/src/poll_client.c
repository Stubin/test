#define _GNU_SOURCE
#include "head.h"
#include "utils.h"
typedef struct
{
    char arg[20];
    char ip[20];
    char port[10];
    char token[37];
    char command[100];
}data_t,*pData_t;
int recycleRecv(int,void*,int,char*);
int recvfile(char *arg,int socketFd);
void *threadFunc(void *arg)
{
    pData_t p=(pData_t)arg;
    int socketFd = socket(AF_INET,SOCK_STREAM,0);
    if(socketFd ==-1)
    {
        perror("socket");
        return NULL;
    }
    struct sockaddr_in addr;
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(p->port));
    addr.sin_addr.s_addr = inet_addr(p->ip);
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int ret = connect(socketFd,(struct sockaddr*)&addr,addrlen);
    if(ret==-1)
    {
        perror("connect");
        return NULL;
    }
    //发送token给服务器，说明我已经登录过了
    train train;
    train.type=0; 
    train.datalen=strlen(p->token);
    strcpy(train.buf,p->token);
    send(socketFd,&train,train.datalen+8,0);
    //发送命令
    train.datalen=strlen(p->command);
    strcpy(train.buf,p->command);
    send(socketFd,&train,train.datalen+8,0);
    tranfile(p->arg,socketFd);
    pthread_exit(NULL);
}
void *threadFunc1(void *arg)
{
    pData_t p=(pData_t)arg;
    int socketFd = socket(AF_INET,SOCK_STREAM,0);
    if(socketFd ==-1)
    {
        perror("socket");
        return NULL;
    }
    struct sockaddr_in addr;
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(p->port));
    addr.sin_addr.s_addr = inet_addr(p->ip);
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int ret = connect(socketFd,(struct sockaddr*)&addr,addrlen);
    if(ret==-1)
    {
        perror("connect");
        return NULL;
    }
    //发送token给服务器，说明我已经登录过了
    train train;
    train.type=0; 
    train.datalen=strlen(p->token);
    strcpy(train.buf,p->token);
    send(socketFd,&train,train.datalen+8,0);
    //发送命令
    train.datalen=strlen(p->command);
    strcpy(train.buf,p->command);
    send(socketFd,&train,train.datalen+8,0);
    recvfile(p->arg,socketFd);
    pthread_exit(NULL);
}
int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,4);
    int socketFd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in addr;
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t addrlen = sizeof(struct sockaddr_in);
    train train;
    char buf[65535]={0};
    int datelen=0;
    int type;
    char salt[512]={0};
    char cryptPasswd[512]={0};
    char command[100];
    //获取用户名
    train.type=0;
    train.datalen = strlen(argv[3]);
    strcpy(train.buf,argv[3]);
    //获取密码
    char *passwd;
    passwd=getpass("请输入密码:");
    int ret = connect(socketFd,(struct sockaddr*)&addr,addrlen);
    ERROR_CHECK(ret,-1,"connect");
    //先发token(这里只是为了保持协议一致)
    char token[37]={0};
    train.type=0; 
    train.datalen=strlen(token);
    strcpy(train.buf,token);
    send(socketFd,&train,train.datalen+8,0);
    //发用户名
    train.datalen = strlen(argv[3]);
    strcpy(train.buf,argv[3]);
    send(socketFd,&train,train.datalen+4+4,0);
    //接受颜值
    recycleRecv(socketFd,&type,4,NULL);
    recycleRecv(socketFd,&datelen,4,NULL);
    recycleRecv(socketFd,&salt,datelen,NULL);
    //得到加密后的密码
    /* printf("passwd=%s\n",passwd);; */
    /* printf("salt=%s\n",salt);; */
    strcpy(cryptPasswd,crypt(passwd,salt));
    //发送密码给服务器验证
    train.datalen = strlen(cryptPasswd);
    strcpy(train.buf,cryptPasswd);
    send(socketFd,&train,train.datalen+4+4,0);
    printf("cryptPasswd=%s\n",cryptPasswd);
    //如果服务器返回0，代表用户没有注册,1代表已经注册了
    /* int registerFlag =0; */
    /* //接受用户名是否存在的标志 */
    /* recycleRecv(socketFd,&type,4,NULL); */
    /* recycleRecv(socketFd,&datelen,4,NULL); */
    /* recycleRecv(socketFd,&registerFlag,datelen,NULL); */
    /* char *registerPasswd; */
    /* if(registerFlag == 0) */
    /* { */
    /*    registerPasswd = getpass("你还没有注册，请输入你要注册的密码"); */   
    /*    train.datalen = strlen(registerPasswd); */
    /*    strcpy(train.buf,registerPasswd); */
    /*    send(socketFd,&train,train.datalen+4+4,0); */
    /*    printf("registerPasswd = %s\n",registerPasswd); */

    /* } */

    //接受服务器返回是否成功
    int flag;
    recycleRecv(socketFd,&flag,sizeof(int),NULL);
    if(flag == 0)
    {
        printf("密码错误\n");
        exit(0);
    }
    //接受token
    recycleRecv(socketFd,&type,4,0);
    recycleRecv(socketFd,&datelen,4,0);
    recycleRecv(socketFd,token,datelen,0);
    system("clear");
    printf("connect success\n");
    /* printf("token=%s\n",token); */
    int epfd = epoll_create(1);
    epollAdd(epfd,0);
    epollAdd(epfd,socketFd);
    struct epoll_event evs[2];
    int readyNum;
    int fd;
    char *dest[100];
    int splitNum;
    char temp[1024]={0};
    struct stat statbuf;
    while(1)
    {
        readyNum = epoll_wait(epfd,evs,2,-1);
        for(int i = 0;i<readyNum;i++)
        {
            if(evs[i].data.fd == 0)
            {
                bzero(buf,sizeof(buf));
                bzero(dest,sizeof(dest));
                bzero(temp,sizeof(temp));
                read(0,buf,sizeof(buf));
                //复制到temp。不复制的话发送的命令会错乱
                strcpy(temp,buf);
                train.type=0;
                strcpy(train.buf,temp);
                train.datalen =strlen(temp)-1;
                send(socketFd,&train,train.datalen+8,0);
                //区分是发文件还是命令
                bzero(command,sizeof(command));
                strncpy(command,temp,strlen(temp)-1);

                split(temp," ",dest,&splitNum);
                if(strcmp(dest[0],"puts")==0)
                {
                    pthread_t pthid;
                    data_t pdata;
                    bzero(&pdata,sizeof(data_t));
                    strcpy(pdata.arg,dest[1]);
                    strcpy(pdata.ip,argv[1]);
                    strcpy(pdata.port,argv[2]);
                    strcpy(pdata.token,token);
                    strcpy(pdata.command,command);
                    pthread_create(&pthid,NULL,threadFunc,&pdata);
                }else if(strcmp(dest[0],"gets")==0)
                {
                    pthread_t pthid;
                    data_t pdata;
                    bzero(&pdata,sizeof(data_t));
                    strcpy(pdata.arg,dest[1]);
                    strcpy(pdata.ip,argv[1]);
                    strcpy(pdata.port,argv[2]);
                    strcpy(pdata.token,token);
                    strcpy(pdata.command,command);
                    pthread_create(&pthid,NULL,threadFunc1,&pdata);

                }
                printf("send comman %s",buf);

            }else if(evs[i].data.fd == socketFd)
            {
                bzero(buf,sizeof(buf));
                //接受数据类型
                recycleRecv(socketFd,&type,4,NULL);
                if(type==0)
                {
                    //接受数据长度
                    recycleRecv(socketFd,&datelen,sizeof(int),NULL);
                    //接受数据内容
                    recycleRecv(socketFd,buf,datelen,NULL);
                    printf("%s",buf);
                }

            }

        }
    }
    close(socketFd);
    return 0;
}

