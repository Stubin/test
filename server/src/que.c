#include "que.h"
void factory_que_init(pQuet que,int capacity)
{
    que->capacity = capacity;
    que->size = 0;
    que->head=que->tail = NULL;
    pthread_mutex_init(&que->mutex,NULL);
}
//从尾部插入
void factory_que_set(pQuet pq,pNode_t pnew)
{
    if(pq->size == 0)
    {
        pq->head =pnew;
        pq->tail =pnew;
    }
    else
    {
        pq->tail->next = pnew;
        pq->tail = pnew;
    }
    pq->size++;
}
int factory_que_get(pQuet pq,pNode_t *p)
{
    if(pq->size == 0)
    {
        return -1;
    }
    *p = pq->head;
    pq->head = pq->head->next;
    if(NULL == pq->head)
    {
        pq->tail =NULL;
    }
    pq->size--;
    return 0;
}
