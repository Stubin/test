#include "head.h"
#include "stack.h"
MYSQL *conn;
int cmd_pwd(int dirCode,int newFd,char *username);
int cmd_ls(int newFD,char *username,int dirCode)
{
    //查询名字为username的文件信息
    train train;
    train.type=0;
    char sql[300]={0};
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(sql,"%s%d%s%s%s","select type,filename,filesize from VFS where precode =",dirCode," and owner ='",username,"'");
    puts(sql);
    unsigned int t = mysql_query(conn,sql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if(row == NULL)
        {
            printf("Didn't find data\n");
            return -1;
        }else
        {
            do{
                sprintf(train.buf,"%-5s%-15s%s\n",row[0],row[1],row[2]);
                printf("buf =%s",train.buf);
                train.type = 0;
                train.datalen = strlen(train.buf);
                send(newFD,&train,sizeof(int)+4+train.datalen,0);

            }while((row = mysql_fetch_row(res))!=NULL);
        }
        mysql_free_result(res); 
    }

    return 0;
}


int cmd_cd(char* dir,int *dirCode,char *username,int newFd)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sql[200]={0};
    if(strcmp(dir,".")==0)
    {
        return 0;
    }else if(strcmp(dir,"..")==0)
    {
        sprintf(sql,"%s%d","select precode from VFS where code =",*dirCode);

    }else
    {
        //查询当前目录下是否有该目录，有则修改Dircode，无则提示错误
        sprintf(sql,"%s%d%s%s%s%s%s","select code from VFS where precode =",*dirCode," AND type ='d' AND filename = '",dir,"' AND owner ='",username,"'");
    }
    printf("sql = %s\n",sql);
    //查询出来的code值不等于dirCode说明存在该文件夹
    int t =mysql_query(conn,sql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if(row == NULL)
        {
            printf("Didn't find data\n");
        }else
        {
            *dirCode = atoi(row[0]);
            printf("dirCode = %d\n",*dirCode);
        }
        mysql_free_result(res); 

    }
    cmd_pwd(*dirCode,newFd,username);
    return 0;
}
int cmd_pwd(int dirCode,int newFd,char *username)
{
    Stack * stack= CreateStack();
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sql[200]={0};
    char *stackRes;
    train train;
    int precode;
    bzero(&train,sizeof(train));
    train.type = 0;
    //每个用户自己的目录
    strcat(train.buf,"/home/");
    strcat(train.buf,username);
    strcat(train.buf,"/");
    //查询当前目录
    precode = dirCode;
    while(precode!=0)
    {
        bzero(sql,sizeof(sql));
        sprintf(sql,"%s%d","select precode,filename from VFS where code =",precode);
        int t =mysql_query(conn,sql);
        printf("sql = %s\n",sql);
        if(t)
        {
            printf("Error making query:%s\n",mysql_error(conn));
        }else
        {
            res=mysql_store_result(conn);
            row = mysql_fetch_row(res);
            if(row == NULL)
            {
                printf("Didn't find data\n");
            }else
            {
                PushStack(stack,row[1]);
                printf("row[1]=%s\n",row[1]);
                precode =atoi(row[0]);
                printf("precode=%d\n",precode);
            }
            mysql_free_result(res); 
        }
    }
    //拼接字符串用于发送
    while((stackRes=GetTopElement(stack))!=NULL)
    {
        strcat(train.buf,stackRes);
        PopStack(stack);
        strcat(train.buf,"/");
    }
    printf("train.buf=%s\n",train.buf);
    strcat(train.buf,"\n");
    train.datalen = strlen(train.buf);
    send(newFd,&train,sizeof(int)+4+train.datalen,0); 


    return 0;
}
int cmd_remove(char *filename,char newFd,int dirCode,char *username)
{
    char sql[300]={0};
    MYSQL_RES *res;
    MYSQL_ROW row;
    unsigned int r;
    char md5[128]={0};
    int count;
    //先查询出文件的引用计数和磁盘存储的md5值
    sprintf(sql,"%s%d%s%s%s%s%s%s","select md5sum,count from VFS where  precode =",dirCode ," AND filename ='",filename,"'"," AND owner = '",username,"'");
    int t = mysql_query(conn,sql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if(row == NULL)
        {
            printf("Didn't find data\n");
        }else
        {
            count =atoi(row[1]);
            strcpy(md5,row[0]);
            /* printf("md5=%s\n",md5); */
            mysql_free_result(res); 
            //大于1就不是真正的删除，而是减少引用计数，同时更新其他拥有者对该文件的引用计数
            bzero(sql,sizeof(sql));
            count--;
            sprintf(sql,"%s%d%s%s%s","update VFS set count =",count," where md5sum ='",md5,"'");  
            /* printf("sql = %s\n",sql); */
            t = mysql_query(conn,sql);
            if(t==0)
            {
                printf("file %s  count--\n",filename);
            }

        }

    }
    bzero(sql,sizeof(sql));
    sprintf(sql,"%s%s%s%s%s%d","delete from VFS where owner ='",username,"' AND filename = '",filename,"' AND precode =",dirCode);
    t = mysql_query(conn,sql);
    if(t==0)
    {
        printf("count = %d\n",count);
        if(count==0)
        {
            t =remove(md5);
            ERROR_CHECK(t,-1,"remove");
            printf("disk file %s remove\n",filename);
        }
    }
    /* printf("sql = %s\n",sql); */
    return 0;

}
int cmd_gets(char *filename,int newFd,char *username,int dirCode)
{
    conn =db_get_database_connection(conn,"localhost","root","mrjguet","test");
    printf("cmd_gets newFd = %d\n",newFd);
    printf("filename = %s\n",filename);
    printf("username = %s\n",username);
    printf("dirCode = %d\n",dirCode);
    MYSQL_RES *res;
    MYSQL_ROW row;
    char sql[300]={0};
    char type;
    char md5[300]={0};
    sprintf(sql,"%s%s%s%s%s%d","select md5sum,type from VFS where owner ='",username,"' AND filename = '",filename,"' AND precode = ",dirCode);
    printf("sql = %s\n",sql);
    int t = mysql_query(conn,sql);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        if(row == NULL)
        {
            printf("Didn't find data\n");
        }else
        {
            if(strcmp(row[1],"d")==0)
            {
                printf("can not send dir\n");
                mysql_free_result(res); 
            }else
            {
                strcpy(md5,row[0]);
                mysql_free_result(res); 
                tranfile(md5,newFd,filename); 
            }
        }
    }
    return 0;
}
int cmd_puts(char *filename,int newFd,char *username,int dirCode)
{
    conn =db_get_database_connection(conn,"localhost","root","mrjguet","test");
    printf("cmd_puts newFd = %d\n",newFd);
    train train;
    int type,fd;
    int datelen=0;
    char buf[1024]={0};
    char md5[128]={0};
    int t;
    int count =1;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char needSendFlag;
    //接受md5
    recycleRecv(newFd,&type,4);
    recycleRecv(newFd,&datelen,4);
    recycleRecv(newFd,md5,datelen);

    if(strlen(md5)==0)
    {
        return -1;
    }
    fd = open(md5,O_RDWR|O_CREAT,0666);                    
    ERROR_CHECK(fd,-1,"open");
    //接受文件大小
    off_t filesize;        
    recycleRecv(newFd,&type,4);
    recycleRecv(newFd,&datelen,4);
    recycleRecv(newFd,&filesize,datelen);
    printf("filesize = %ld\n",filesize);
    char sql[300]={0};
    //先查询该文件已经存在的个数
    sprintf(sql,"%s%s%s","select count(*) from VFS where md5sum ='",md5,"'");
    t = mysql_query(conn,sql);
    printf("sql=%s\n",sql); 
    if(t==0)
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        count = atoi(row[0])+1;
        mysql_free_result(res);
        if(count>1)
        {
            //告知客户端不用发送了
            needSendFlag =0;
            train.type =0;
            train.datalen =1;
            strcpy(train.buf,&needSendFlag);
            send(newFd,&train,train.datalen+8,0);
            printf("妙传成功\n");
            goto next;
        }
    }
    //告知客户端继续发送
    needSendFlag =1;
    train.type =0;
    train.datalen =1;
    strcpy(train.buf,&needSendFlag);
    send(newFd,&train,train.datalen+8,0);
    while(1)
    {   
        recycleRecv(newFd,&type,4);
        recycleRecv(newFd,&datelen,4);
        if(datelen == 0)
        {   
            break;
        }   
        recycleRecv(newFd,&buf,datelen);
        write(fd,buf,datelen);
    }   
next:
    //如果该用户已经存在了该文件，则不用进行插入操作
    bzero(sql,sizeof(sql));
    sprintf(sql,"%s%s%s%s%s","select count(*) from VFS where owner ='",username,"' and md5sum = '",md5,"'");
    mysql_query(conn,sql);
    /* printf("sql=%s\n",sql); */ 
    if(t==0)
    {
        res=mysql_store_result(conn);
        row = mysql_fetch_row(res);
        //r==0代表，本用户没有上传过该文件，等于1代表上传过
        int r = atoi(row[0]);
        printf("r == %d\n",r);
        mysql_free_result(res);
        //如果自己已经上传过了，就不用插入数据了
        if(r==1)
        {
            goto close;
        }
    }
    //插入数据库
    bzero(sql,sizeof(sql));
    sprintf(sql,"%s%d%s%s%s%s%s%s%s%s%ld%s%d%s","insert into VFS (precode,filename,owner,md5sum,filesize,type,count) values(",dirCode,",'",filename,"','",username,"',","'",md5,"',",filesize,",'f',",count,")");
    printf("sql=%s\n",sql);
    mysql_query(conn,sql);
    if(t==0)
    {
        printf("puts file %s success\n",filename);
    }else
    {
        printf("%s\n",mysql_error(conn));
    }
    //更新其他相同md5的文件的引用
    bzero(sql,sizeof(sql));
    sprintf(sql,"%s%d%s%s%s","update VFS set count =",count," where md5sum = '",md5,"'" );
    t = mysql_query(conn,sql);
    printf("sql=%s\n",sql); 
close:
    close(fd);
    return 0;

}
int cmd_mkdir(char* dirName,int curDirCode,char* username)
{
    //插入数据库
    char sql[300]={0};
    sprintf(sql,"%s%d%s%s%s%s%s%s%s%s%d%s%d%s","insert into VFS (precode,filename,owner,md5sum,filesize,type,count) values(",curDirCode,",'",dirName,"','",username,"',","'","0","',",4096,",'d',",1,")");
    printf("sql=%s\n",sql);
    int t = mysql_query(conn,sql);
    if(t==0)
    {
        printf("mkdir  %s success\n",dirName);
    }else
    {
        printf("%s\n",mysql_error(conn));
    }
    return 0;
}


