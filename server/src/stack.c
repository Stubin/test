#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
 
//创建栈，此时栈中没有任何元素
Stack* CreateStack()
{
    Stack *stack = (Stack*)malloc(sizeof(Stack));
    if(NULL != stack)
    {
       stack->next = NULL;
       return stack;
    }
    printf("out of place.\n");
    return NULL;
}
 
//清空栈
void StackEmpty(Stack* stack)
{
    while(!IsEmpty(stack))
    {
        PopStack(stack);
    }
    printf("now stack is empty. \n");
}
 
//撤销栈
void DestoryStack(Stack* stack)
{
    free(stack);
    printf("now stack is destoryed. \n");
    exit(0);
}
 
int IsEmpty(Stack* stack)
{
    return (stack->next == NULL);
}
 
//入栈，成功返回1，失败返回0， 把元素 data 存入 栈 stack 中
int PushStack(Stack* stack, DataType data)
{
    Stack* newst = (Stack*)calloc(1,sizeof(Stack));
    if(NULL != newst)
    {
        strcpy(newst->data, data);
        newst->next = stack->next;  //s->next = NULL;
        stack->next = newst;
        return 1;
    }
    printf("out of place PushStack.\n");
    return 0;
}
 
/*
    出栈，成功返回1，失败返回0，出栈不取出元素值，只是删除栈顶元素。
    如出栈要实现，取出元素值，并释放空间，可结合取栈顶元素函数做修改，这里不再给出。
 */
 
int PopStack(Stack* stack)
{
    Stack* tmpst;
    if(!IsEmpty(stack))
    {
        tmpst = stack->next;
        stack->next = tmpst->next;
        free(tmpst);
        return 1;
    }
    return 0;
}
 
//å栈顶元素，仅取出栈顶元素的值，取出之后，该元素，任然存在栈中。成功返回元素值，失败输出提示信息，并返回 -1
char* GetTopElement(Stack* stack)
{
    if(!IsEmpty(stack))
    {
        return stack->next->data;
    }
    printf("stack is empty GetTopElement.\n");
    return NULL;
}

