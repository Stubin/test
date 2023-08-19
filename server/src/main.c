#define _GNU_SOURCE
#include "factory.h"
#include "cmd.h"
#include "md5.h"
#include "list.h"
char *argv1;
char InitDir[1024]={0};
MYSQL *conn;
struct sockaddr_in client;
socklen_t clientLen;
node_t *fdsNode ;
int threadNum;
int j;
int curIndex=1;
int soltIndex=0;
list lists[30];
int map[1024]={-1};
int mapUserId[1024]={0};
int exitPipe[2];
time_t past,now;
void sigFunc(int no)
{
    printf("sig no %d cmoing \n",no);
    write(exitPipe[1],"s",1);
}
int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,2);
    argv1 = argv[1];
    //初始化factory
    threadNum = atoi(confGet(argv[1],"threadNum"));
    int capacity = atoi(confGet(argv[1],"capacity"));
    factory_t pool;
    factoryInit(&pool,threadNum,capacity);
    //启动线程池
    factoryStart(&pool);
    //初始化tcp
    int sockFd;
    conn =db_get_database_connection(conn,"localhost","root","mrjguet","test");
    char ip[16]={0};
    strcpy(ip,confGet(argv[1],"ip")); 
    char port[16]={0}; 
    strcpy(port,confGet(argv[1],"port")); 
    tcpInit(ip,port,&sockFd);
    signal(SIGINT,sigFunc);
    pipe(exitPipe);
    //父进程接受任务交给子进程
    int newFd;
    pQuet pq=&pool.que;
    pNode_t pNew;
    //保存初始目录
    char shareDir[1024]={0};
    strcpy(shareDir,confGet(argv1,"shareDir"));
    realpath(shareDir,InitDir);
    //初始化MYSQL
    conn =db_get_database_connection(conn,"localhost","root","mrjguet","test");
    //epoll初始化
    int epfd = epoll_create(1);
    struct epoll_event *evs = (struct epoll_event*)calloc(threadNum+1,sizeof(struct epoll_event));
    int readyNum;
    epollAdd(epfd,sockFd);
    epollAdd(epfd,exitPipe[0]);
    //每个线程自己的路径
    char curDir[1024]={0};
    int chret = chdir(InitDir);
    if(chret == -1)
    {
        perror("chidr");
    }
    strcpy(curDir,getcwd(NULL,0));
    //开始code=0
    //记录newFd
    int *fds =(int*)calloc(threadNum+1,sizeof(int));
    //记录登录的用户信息
    fdsNode =(node_t*)calloc(threadNum+1,sizeof(node_t));
    int count=0;//记录当前监听的个数
    train train;
    int registerFlag;
    char filed1[100]={0};
    char filed2[100]={0};
    char token[37]={0};
    char token_temp[1024]={0};
    MYSQL_RES *res;
    MYSQL_ROW mysqlRow;
    long row,col;
    char sql[300]={0};
    char cmd[300]={0};
    short r;
    //校验密码
    char *salt;
    char username[20]={0};
    char passwd[512]={0};
    int success;
    char buf[1024]={0}; 
    int datelen=0;       
    int type;      
    short  t;
    char logsql[1024]={0};
    char *dest[100]={0};
    int splitNum;
    int dirCode=0;
    char user_id[10]={0};
    long expire_time;
    //超时断开
    //环形队列
    //环形链表中每个槽对应的链表
    list lists[30];
    for(int i=0;i<30;i++)
    {
        LinkListInit(&lists[i]);
    }
    int map[1024]={-1};
    int mapUserId[1024]={0};
    past=now=time(NULL);
    while(1)
    {   
        readyNum =  epoll_wait(epfd,evs,threadNum+2,1000);
        if(readyNum ==-1)
        {

            continue;
        }
        for(int i = 0;i<readyNum;i++)
        {
            if(evs[i].data.fd == exitPipe[0])
            {
                bzero(sql,sizeof(sql));
                sprintf(sql,"%s","delete from token");
                mysql_query(conn,sql);
                exit(0);
            }
            //监听新的连接
            if(evs[i].data.fd == sockFd)
            {
                bzero(&client,sizeof(client));
                newFd = accept(sockFd,(struct sockaddr*)&client,&clientLen);
                ERROR_CHECK(newFd,-1,"accept");
                //接受客户端发来的token
                bzero(token,sizeof(token));
                recycleRecv(newFd,&type,4);
                recycleRecv(newFd,&datelen,4);
                recycleRecv(newFd,token,datelen);
                //如果用户存在有效token，则将任务交给子线程
                bzero(sql,sizeof(sql));
                sprintf(sql,"%s%s%s","select user_id,expire_time from token where user_token = '",token,"'");
                printf("sql=%s\n",sql);
                r= mysql_query(conn,sql);
                if(r)
                {
                    printf("Error making query:%s\n",mysql_error(conn));
                }else
                {
                    res=mysql_use_result(conn);
                    mysqlRow = mysql_fetch_row(res);
                    row = mysql_num_rows(res);
                    /* bzero(user_id,sizeof(user_id)); */
                    /* printf("row[0]=%s\n",mysqlRow[0]); */
                    /* strcpy(user_id,mysqlRow[0]); */
                    if(row == 0)
                    {
                        //不存在则监控该fd，并进行登录操作
                        epollAdd(epfd,newFd);
                        fds[count]=newFd;//记录新连接的newFd
                        count++;
                    }else
                    {
                        expire_time = atoi(mysqlRow[1]);
                        time_t now;
                        time(&now);
                        mysql_free_result(res);
                        if(now-expire_time <180)
                        {
                            //更新过期时间
                            bzero(sql,sizeof(sql));
                            sprintf(sql,"%s%ld%s%d","update token set expire_time =",now," where user_id =",atoi(mysqlRow[0]));
                            t = mysql_query(conn,sql);
                            if(t)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));  
                            }else
                            {
                                printf("t = %d,update tokenid = %s success\n",t,user_id);
                            }

                            //当命令为puts和gets时将任务放入队列
                            pNew =(node_t*)calloc(1,sizeof(node_t));
                            pNew->newFd = newFd;
                            pNew->dirCode = fdsNode[count].dirCode;
                            strcpy(pNew->username,username);
                            pthread_mutex_lock(&pq->mutex);
                            factory_que_set(pq,pNew);
                            pthread_mutex_unlock(&pq->mutex);
                            pthread_cond_signal(&pool.cond);

                        }
                        else
                        {
                            //token过期，从数据库中删除过期的toen
                            bzero(sql,sizeof(sql));
                            sprintf(sql,"%s%d","delete from token where user_id=",atoi(mysqlRow[0]));
                            t = mysql_query(conn,sql);
                            if(t)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));  
                            }else
                            {
                                printf("t = %d,delete tokenid = %s success\n",t,mysqlRow[0]);
                            }
                        }
                    }
                }
            }else
            {
                for(j=0;j<threadNum;j++)
                {
                    if(evs[i].data.fd==fds[j])
                    {   
                        if(fdsNode[j].loginFlag == 0)
                        {
                            //登录
                            //得到用户名用来获取颜值
                            bzero(username,sizeof(username));
                            recycleRecv(fds[j],&type,4);
                            recycleRecv(fds[j],&datelen,4);
                            recycleRecv(fds[j],&username,datelen);
                            //从数据库查询颜值并给客户端发送颜值
                            bzero(sql,sizeof(sql));
                            sprintf(sql,"%s%s%s","select salt,password,id from user where username = '",username,"'");
                            /* printf("sql=%s\n",sql); */
                            r= mysql_query(conn,sql);
                            if(r)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));
                            }else
                            {
                                res=mysql_use_result(conn);
                                mysqlRow = mysql_fetch_row(res);
                                row = mysql_num_rows(res);
                                // row==0说明不存在该用户，则创建颜值并插入数据库
                                if(row == 0)
                                {
                                    //发送是否注册的标志
                                    train.type =0;
                                    registerFlag=0;
                                    memcpy(train.buf,&registerFlag,sizeof(registerFlag));
                                    train.datalen =sizeof(registerFlag);
                                    send(fds[j],&train,train.datalen+8,0);
                                    //接受用户注册的密码,在加密后并插入数据库
                                    char cryptPasswd[512]={0};
                                    bzero(passwd,sizeof(passwd));
                                    recycleRecv(fds[j],&type,4);
                                    recycleRecv(fds[j],&datelen,4);
                                    recycleRecv(fds[j],&passwd,datelen);
                                    bzero(sql,sizeof(sql));
                                    salt = GenerateStr();
                                    strcpy(cryptPasswd,crypt(passwd,salt));
                                    sprintf(sql,"%s%s%s%s%s%s%s%s%s%s","insert into user (salt,username,password) values('",salt,"',","'",username,"',","'",cryptPasswd,"'",")");
                                    t=mysql_query(conn,sql);
                                    if(t==0)
                                    {
                                        printf("insert %s success\n",username);
                                    }
                                    printf("sql=%s\n",sql);
                                    //用来暂存颜值和密码
                                    bzero(filed1,sizeof(filed1));
                                    bzero(filed2,sizeof(filed2));
                                    strcpy(filed1,salt);
                                    strcpy(filed2,cryptPasswd);
                                    /* printf("filed1=%s\n",filed1); */
                                    /* printf("filed2=%s\n",filed2); */
                                }
                                else
                                {

                                    /* train.type =0; */
                                    /* registerFlag=1; */
                                    /* memcpy(train.buf,&registerFlag,sizeof(registerFlag)); */
                                    /* train.datalen =sizeof(registerFlag); */
                                    /* send(fds[j],&train,train.datalen+8,0); */
                                    //用来暂存颜值和密码
                                    bzero(filed1,sizeof(filed1));
                                    bzero(filed2,sizeof(filed2));
                                    strcpy(filed1,mysqlRow[0]);
                                    strcpy(filed2,mysqlRow[1]);
                                    strcpy(user_id,mysqlRow[2]);
                                    /* printf("filed1=%s\n",filed1); */
                                    /* printf("filed2=%s\n",filed2); */

                                }
                                mysql_free_result(res);
                                /* col = mysql_num_fields(res); */
                            }
                            /* printf("row=%ld,col=%ld",row,col); */
                            //发那个颜值
                            train.type =0;
                            strcpy(train.buf,filed1);
                            /* printf("mysqlRow[0]=%s,[1]=%s\n",mysqlRow[0],mysqlRow[1]); */
                            train.datalen = strlen(filed1);
                            send(fds[j],&train,train.datalen+8,0);
                            //得到密码 
                            bzero(passwd,sizeof(passwd));
                            recycleRecv(fds[j],&type,4);
                            recycleRecv(fds[j],&datelen,4);
                            recycleRecv(fds[j],&passwd,datelen);
                            /* printf("passwd=%s\n",passwd); */
                            //记录请求信息
                            bzero(logsql,sizeof(logsql));
                            sprintf(logsql,"%s%s%s%s%s%s","insert into Rlog (ip,username) values('",inet_ntoa(client.sin_addr),"',","'",username,"')");
                            /* printf("logsql=%s\n",logsql); */
                            t=mysql_query(conn,logsql);
                            if(t)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));  
                            }else
                            {
                                printf("t = %d,insert %s success\n",t,username);
                            }
                            //与数据库查询出来的密码进行比较
                            if(strcmp(passwd,filed2)!= 0)
                            {
                                printf("密码不对,断开连接\n");
                                //告诉客户端连接失败
                                success=0;
                                send(fds[j],&success,sizeof(int),0);
                                close(fds[j]); 
                                continue;
                            }
                            //告诉客户端连接成功
                            success=1;
                            send(fds[j],&success,sizeof(int),0);
                            fdsNode[j].loginFlag =1;
                            fdsNode[j].dirCode=0;
                            fdsNode[j].newFd=fds[j];
                            mapUserId[fds[j]]=atoi(user_id);
                            printf("mapUserId[%d]=%d\n",fds[j],atoi(user_id));
                            printf("x=%d",mapUserId[fds[j]]);
                            strcpy(fdsNode[j].username,username);
                            //生成token，返回给客户端
                            time_t now;
                            time(&now);
                            strcat(token_temp,user_id);
                            strcat(token_temp,ctime(&now));
                            Compute_string_md5((unsigned char *)token_temp, strlen(token_temp), token);
                            printf("token=%s\n",token);
                            //将token插入数据库，同时发给客户端
                            train.type=0;
                            train.datalen=strlen(token);
                            strcpy(train.buf,token);
                            send(fds[j],&train,train.datalen+8,0);

                            bzero(sql,sizeof(sql));
                            sprintf(sql,"%s%d%s%s%s%ld%s","insert into token (user_id,user_token,expire_time) values(",atoi(user_id),",'",token,"',",now+180,")");
                            /* printf("sql=%s\n",sql); */
                            t = mysql_query(conn,sql);
                            if(t)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));  
                            }else
                            {
                                printf("t = %d,insert %s success\n",t,token);
                            }

                        }
                        else
                        {

                            /*----------------------------------------*/
                            //当命令为瞬时命令，由主线程处理
                            //1.接受命令
                            bzero(buf,sizeof(buf));
                            bzero(logsql,sizeof(logsql));

                            int ret = recycleRecv(fds[j],&type,4);
                            recycleRecv(fds[j],&datelen,4);
                            recycleRecv(fds[j],buf,datelen);
                            printf("buf=%s\n",buf);
                            bzero(cmd,sizeof(cmd));
                            strcpy(cmd,buf);
                            //将操作记录下入日志
                            sprintf(logsql,"%s%s%s%s%s%s%s%s%s","insert into Olog (username,dir,cmd) values ('",username,"',","'",curDir,"',","'",buf,"')");
                            /* printf("logsql=%s\n",logsql); */
                            t=mysql_query(conn,logsql);
                            if(t)
                            {
                                printf("Error making query:%s\n",mysql_error(conn));  
                            }else
                            {
                                printf("t = %d,insert %s success\n",t,username);
                            }

                            if(-1== ret)
                            {
                                close(fds[j]);
                                fdsNode[j].loginFlag=0;
                                //将token从数据库删除
                                bzero(sql,sizeof(sql));
                                sprintf(sql,"%s%d","delete from token where user_id =",atoi(user_id));
                                t = mysql_query(conn,sql);
                                if(t==0)
                                {
                                    printf("delete user_id = %s token success\n",user_id);
                                }
                                printf("close %d\n",fds[j]);
                                break;
                            }
                            //2.解析命令
                            bzero(dest,sizeof(dest));
                            split(buf," ",dest,&splitNum);
                            for(int j=0;j<splitNum;j++)
                            {
                                printf("%s\n",dest[j]);
                            }
                            if(dest[0]==NULL)
                            {
                                break;
                            }
                            //3.返回结果给客户端
                            if(strcmp(dest[0],"ls")==0)
                            {
                                cmd_ls(fds[j],fdsNode[j].username,fdsNode[j].dirCode);
                            }else if(strcmp(dest[0],"cd")==0)
                            {
                                //相当于只输入cd
                                if(splitNum == 1)
                                {
                                    break;
                                }
                                printf("dirCode = %d\n",fdsNode[j].dirCode);
                                cmd_cd(dest[1],&fdsNode[j].dirCode,fdsNode[j].username,fds[j]);
                                printf("dirCode1 = %d\n",fdsNode[j].dirCode);
                            }else if(strcmp(dest[0],"pwd")==0)
                            {
                                cmd_pwd(fdsNode[j].dirCode,fds[j],fdsNode[j].username);
                            }else if(strcmp(dest[0],"remove")==0)
                            {
                                if(splitNum == 1)
                                {
                                    break;
                                }
                                cmd_remove(dest[1],fds[j],fdsNode[j].dirCode,fdsNode[j].username);
                            }else if(strcmp(dest[0],"mkdir")==0)
                            {
                                if(splitNum == 1)
                                {
                                    break;
                                }
                                cmd_mkdir(dest[1],fdsNode[j].dirCode,fdsNode[j].username);
                            }
                            else
                            {

                            }
                        }
                        //代表有操作
                        //从map结构中查出newFd在哪个队列

                        //从该队列中删除该newFd，并加入到当前的Index的队列,更新map的索引值
                        soltIndex =curIndex-1;
                        if(map[fds[j]]!=-1)
                        {
                            LinkListRemove(&lists[map[fds[j]]],fds[j]);
                        }
                            LinkListPushFront(&lists[soltIndex], fds[j]);

                        printf("soltIndex=%d,fds[j=%d\n",soltIndex,fds[j]);
                        map[fds[j]]=soltIndex;
                        printf("fds[j]= %d mapfds[j]为=%d\n",newFd,map[fds[j]]);
                        past=time(NULL);
                        break;
                    }

                }
            }

        }
        //充当定时器
        now = time(NULL);
        if(now-past>=1)
        {
            curIndex +=1;
            if(curIndex==29)
            {
                curIndex=1;
            }
        }
        //超时处理
        if(readyNum == 0)
        {
            now = time(NULL);
            /* printf("time_wait\n"); */
            //判断连上的客户端是否有操作
            if(now-past>=1)
            {
                //当链表为空，说明没有超时的，不为空，说明这个链表都超时了
                list temp=lists[curIndex];
                //当前index对应的连接都断开,并删除节点
                while(temp!=NULL)
                {
                    close(temp->data);
                    printf("连接 fd = %d 超时断开\n",temp->data);
                    for(int k=0;k<threadNum;k++)
                    {
                        if(temp->data == fdsNode[k].newFd)
                        {
                            fdsNode[k].loginFlag=0;
                            break;
                        }
                    }
                    printf("temp->data = %d,mapUserId[temp>data]=%d",temp->data,mapUserId[temp->data]);
                    bzero(sql,sizeof(sql));
                    sprintf(sql,"%s%d","delete from token where user_id =",mapUserId[temp->data-1]);
                    t = mysql_query(conn,sql);
                    if(t==0)
                    {
                        printf("delete user_id = %d token success\n",mapUserId[temp->data]);
                    }
                    LinkListRemove(&lists[map[temp->data]],temp->data);
                    /* map[temp->data]=-1; */
                    //将token从数据库删除
                    //找到newFd对应的user_id
                    temp=temp->next;
                }
                past =now;
            }


        }
    }
    return 0;
}
