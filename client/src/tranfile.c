#include "head.h"
#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
int Compute_file_md5(const char *file_path, char *value);
int recycleRecv(int fds,void *buf,int len,char *filename);
int tranfile(char* filename,int newFd)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    char temp[100]={0};
    char res[100]={0};
    int type;
    int datalen;
    char data[1024]={0};
    strncpy(temp,filename,strlen(filename)-1);
    //定义小火车，datelen代表要发送的数据长度,buf代表要发送的数据
    train train;
    train.type=1;
    //先发文件名,不发送真正文件名，发送文件的md5码
 	Compute_file_md5(temp ,res);
    train.datalen = strlen(res);
    strcpy(train.buf,res);
    send(newFd,&train,train.datalen+8,0);
    //先打开文件
    int fd = open(temp,O_RDWR);
    ERROR_CHECK(fd,-1,"open");
    /* //发文件大小 */
    struct stat buf;
    fstat(fd,&buf);
    train.datalen = sizeof(buf.st_size);
    memcpy(train.buf,&buf.st_size,train.datalen);
    send(newFd,&train,train.datalen+8,0);
    long total=0;
    char needSendFlag;
    recycleRecv(newFd,&type,4,NULL);
    recycleRecv(newFd,&datalen,4,NULL);
    recycleRecv(newFd,&needSendFlag,1,NULL);
    if(needSendFlag==0)
    {
        goto next;
    }

    //循环发送文件内容
    while((train.datalen = read(fd,train.buf,sizeof(train.buf))))
    {
        int ret = send(newFd,&train,train.datalen+8,0);
        if(ret == -1)
        {
            return -1;
        }
        total+=ret;
    }
    printf("total = %ld\n",total);
    //发送文件结束标志
    send(newFd,&train,train.datalen+8,0);
next:
    close(fd);
    return 0;
}

int Compute_file_md5(const char *file_path, char *md5_str)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;
 
	fd = open(file_path, O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}
 
	// init md5
	MD5Init(&md5);
 
	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);
		if (-1 == ret)
		{
			perror("read");
			return -1;
		}
 
		MD5Update(&md5, data, ret);
 
		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}
 
	close(fd);
 
	MD5Final(&md5, md5_value);
 
	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; // add end
 
	return 0;
}
