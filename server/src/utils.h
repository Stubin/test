#ifndef  __UTILS_H__
#define __UTILS_H__
#define STR_LEN 10//定义随机输出的字符串长度。 
#include "head.h"
void split(char *src,const char *separator,char **dest,int *num);
char* confGet(char *conf,char *key);
void error_quit(char *msg);
void get_salt(char *salt,char *passwd);
char *GenerateStr();
#endif
