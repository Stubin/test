#ifndef  __STACK_H_	
#define  __STACK_H__
#include "head.h"
typedef char DataType[32];
typedef struct node1{
    DataType data;
    struct node1 * next;
}Stack;
 
Stack* CreateStack();   //创建栈
void StackEmpty(Stack* );  //清空栈
void DestoryStack(Stack*); //撤销(删除)栈
int IsEmpty(Stack*);    //判空
int PushStack(Stack*, DataType);    //入栈
int PopStack(Stack*);   //出栈
char* GetTopElement(Stack*); //取栈顶元素
 
#endif
