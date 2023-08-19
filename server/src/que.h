#ifndef __QUE_H__
#define __QUE_H__
#include "head.h"
typedef struct node
{
    int user_id;
    int loginFlag;
    int newFd;
    int dirCode;
    char username[32];
    char comman[32];//记录命令
    struct node *next;
}node_t,*pNode_t;
typedef struct
{
    pNode_t head,tail;
    int capacity;
    int size;
    pthread_mutex_t mutex;
}que_t,*pQuet;
void factory_que_init(pQuet pq, int capacity);
void factory_que_set(pQuet pq, pNode_t pnew);
int factory_que_get(pQuet pq, pNode_t* p);
void factory_que_destroy(pQuet pq);
int factory_que_full(pQuet pq);
int factory_que_empty(pQuet pq);
#endif
