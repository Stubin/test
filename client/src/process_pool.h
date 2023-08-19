#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>
#include<stdlib.h>
#include<pwd.h>
#include<time.h>
#include<grp.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <sys/sem.h>
#include<sys/msg.h>
#include<signal.h>
#include<pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#define ARGS_CHECK(argc,val) {if(argc != val)  {printf("args error\n");return -1; } }
#define ERROR_CHECK(ret,val,info) {if(ret==val){perror(info);return -1; }}
#define THREAD_CHECK(ret,str){if(str!=0) printf("%s : %s",str,strerror(ret))}
#define FILENAME "file"
typedef struct
{
    pid_t pid;//子进程id
    int pipe;//子进程一端的管道
    short busy;//0代表空闲，1代表忙碌
}proceess_data_t;

typedef struct
{
    int datalen;
    char buf[1000];
}train;
int make_child(proceess_data_t*,int);

void child_handle(int,proceess_data_t*);
int recv_fd(int,int*);
int send_fd(int,int);
int tcpInit(char*,char*,int*);
int epollAdd(int ,int);
int tranfile(int);
