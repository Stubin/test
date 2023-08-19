#include "factory.h"
#include "list.h"
#include"cmd.h"
extern char* argv1;
extern char InitDir[1024];
//初始化MYSQL
extern MYSQL *conn;
extern struct sockaddr_in client;
extern  socklen_t clientLen;
extern node_t *fdsNode ;
extern int threadNum;
extern int j;
extern int curIndex;
extern int soltIndex;
extern list lists[30];
extern int map[1024];
extern int mapUserId[1024];
time_t past,now;
void* threadFunc(void *arg)
{
    pFactory_t p =(pFactory_t)(arg);
    pQuet pq =&p->que;
    int ret;
    pNode_t delNode;
    char *dest[100]={0};
    int splitNum;
    int k;
    while(1)
    {
        pthread_mutex_lock(&pq->mutex);
        if(pq->size == 0)
        {
            pthread_cond_wait(&p->cond,&pq->mutex);
        }
        //去队列取连接任务
        ret = factory_que_get(pq,&delNode);
        pthread_mutex_unlock(&pq->mutex);
        if(ret != -1)
        {
            short  t;
            char buf[1024]={0}; 
            int datelen=0;       
            int type;      
            int runRes;
            char logsql[1024]={0};
            /*-------------------------------*/
            //epoll初始化
            int epfd = epoll_create(1);
            epollAdd(epfd,delNode->newFd);
            struct epoll_event evs[1];
            int readyNum;
            //每个线程自己的路径
            char curDir[1024]={0};
            int chret = chdir(InitDir);
            if(chret == -1)
            {
                perror("chidr");
            }
            strcpy(curDir,getcwd(NULL,0));
            /* int dirCode; */
            /* bzero(sql,sizeof(sql)); */
            /* sprintf(sql,"select ") */
            while(1)
            {

                readyNum =  epoll_wait(epfd,evs,1,-1);
                for(int i=0;i<readyNum;i++)
                {
                    if(evs[i].data.fd == delNode->newFd&&evs[i].events==EPOLLIN)
                    {
                        if(evs[i].events == EPOLLRDHUP)
                        {
                            close(delNode->newFd);
                            puts("goto close");
                            
                        }
                        //1.获取命令
                        bzero(buf,sizeof(buf));
                        bzero(logsql,sizeof(logsql));

                        recycleRecv(delNode->newFd,&type,4);
                        recycleRecv(delNode->newFd,&datelen,4);
                        recycleRecv(delNode->newFd,buf,datelen);
                        printf("buf1=%s\n",buf);
                        if(datelen==0)
                        {
                            close(delNode->newFd);
                            puts("goto close");
                        }
                        //将操作记录下入日志
                        sprintf(logsql,"%s%s%s%s%s%s%s%s%s","insert into Olog (username,dir,cmd) values ('",delNode->username,"',","'",curDir,"',","'",buf,"')");
                        /* printf("logsql=%s\n",logsql); */
                        t=mysql_query(conn,logsql);
                        if(t)
                        {
                            printf("Error making query:%s\n",mysql_error(conn));  
                            break;
                        }else
                        {
                            printf("t = %d,insert %s success\n",t,delNode->username);
                        }

                    }
                    //2.解析命令
                    bzero(dest,sizeof(dest));
                    split(buf," ",dest,&splitNum);
                    /* for(int j=0;j<splitNum;j++) */
                    /* { */
                    /*     printf("%s\n",dest[j]); */
                    /* } */
                    if(dest[0]==NULL)
                    {
                        break;
                    }
                    //3.返回结果给客户端
                    if(strcmp(dest[0],"gets")==0)
                    {
                        if(splitNum == 1)
                        {
                            break;
                        }
                        /* cmd_gets(dest[1],delNode->newFd,fdsNode[k].username,fdsNode[k].dirCode); */
                        cmd_gets(dest[1],delNode->newFd,fdsNode[j].username,fdsNode[j].dirCode);

                    }else if(strcmp(dest[0],"puts")==0)
                    {
                        if(splitNum == 1)
                        {
                            break;
                        }
                        /* cmd_puts(dest[1],delNode->newFd,fdsNode[k].username,fdsNode[k].dirCode); */
                        cmd_puts(dest[1],delNode->newFd,fdsNode[j].username,fdsNode[j].dirCode);

                    }
                        soltIndex =curIndex-1;
                        if(map[delNode->newFd]!=-1)
                        {
                            LinkListRemove(&lists[map[delNode->newFd]],delNode->newFd);
                        }
                        if(lists[soltIndex]!=NULL)
                        {
                        LinkListPushBack(&lists[soltIndex], delNode->newFd);

                        }
                        printf("soltIndex=%d,fds[j=%d\n",soltIndex,delNode->newFd);
                        map[delNode->newFd]=soltIndex;
                        printf("fds[j]= %d mapfds[j]为=%d\n",delNode->newFd,map[delNode->newFd]);
                        past=time(NULL);
                }
            }
            //释放节点
        }
close:
        free(delNode);
        delNode = NULL;
    }
}
int factoryInit(pFactory_t p,int threadNum,int que_capacity)
{
    factory_que_init(&p->que,que_capacity);
    pthread_cond_init(&p->cond,NULL);
    p->pthid =(pthread_t*)calloc(threadNum,sizeof(pthread_t));
    p->threadNum = threadNum;
    p->startFlag = 0;
    return 0;
}

int factoryStart(pFactory_t p)
{
    if(!p->startFlag)
    {
        //循环创建子线程
        for(int i= 0;i<p->threadNum;i++)
        {
            pthread_create(p->pthid+i,NULL,threadFunc,p);
        }

    }
    p->startFlag =1;
    return 0; 
}

