#include "utils.h"
extern char *ip;
extern char *port;
void split(char *src,const char *separator,char **dest,int *num) {
	/*
	 *      src 源字符串的首地址(buf的地址) 
	 *              separator 指定的分割字符
	 *                      dest 接收子字符串的数组
	 *                              num 分割后子字符串的个数
	 *                                  */
	char *pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止 
		return;
	if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止 
		return;
	pNext = (char *)strtok(src,separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
	while(pNext != NULL) {
		*dest++ = pNext;
		++count;
		pNext = (char *)strtok(NULL,separator);  //必须使用(char *)进行强制类型转换

	}  
	*num = count;

}
char* confGet(char *conf,char *key)
{
	FILE *file =fopen(conf,"r+");
	if(NULL == file)
	{
		fclose(file);
		perror("fopen");
	}
	char temp[100]={0};
	char *dest[100]={0};
	int splitNum;
	while(fgets(temp,sizeof(temp),file))
	{
		split(temp,"=",dest,&splitNum);
		if(strcmp(key,dest[0])==0)
		{
			//将换行去掉
			dest[1][strlen(dest[1])-1]='\0';
			fclose(file);
			return  dest[1];
		}
		bzero(temp,sizeof(temp));
		bzero(dest,sizeof(dest));
	}
	fclose(file);
	return NULL;
}
void help(void) 
{ 
	printf("用户密码验证程序\n第一个参数为用户名!\n"); 
	exit(-1); 
}
void error_quit(char *msg) 
{ 
	perror(msg); 
	exit(-2); 
}
void get_salt(char *salt,char *passwd) 
{ 
	int i,j; 
	//取出salt,i记录密码字符下标,j记录$出现次数 
	for(i=0,j=0;passwd[i] && j != 3;++i) 
	{ 

		if(passwd[i] == '$') {
			++j; 
		}

	}
	strncpy(salt,passwd,i-1);
}



char *GenerateStr() 

{ 

    char *str = (char*)calloc(STR_LEN+1,sizeof(char));
	int i,flag; 

	srand(time(NULL));//通过时间函数设置随机数种子，使得每次运行结果随机。 

	for(i = 0; i < STR_LEN; i ++) 

	{ 

		flag = rand()%3; 

		switch(flag) 

		{

		case 0: 

			str[i] = rand()%26 + 'a'; 

			break; 

		case 1: 

			str[i] = rand()%26 + 'A'; 

			break; 

		case 2: 

			str[i] = rand()%10 + '0'; 

			break; 

		} 

	}

	printf("%s\n", str);//输出生成的随机数。 

	return str; 

}

