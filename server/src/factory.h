#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "que.h"
typedef  struct
{
    que_t que;
    pthread_cond_t cond;
    pthread_t *pthid;
    int threadNum;
    int startFlag;
}factory_t,*pFactory_t;
int factoryInit(pFactory_t,int,int);
int factoryStart(pFactory_t);
#endif
