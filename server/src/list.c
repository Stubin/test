#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"list.h"

#define TESTHEADER printf("\n===============%s================\n", __FUNCTION__)

/*
 *
 * 初始化链表
 *
 */ 

void LinkListInit(LinkNode** phead)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    *phead = NULL;
}
//////////////////////////////////////////////////////////////////////////////
////LinkListInit的测试代码
/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
////测试代码
/////////////////////////////////////////////////////////////////////////////

void TestPrintChar(LinkNode* phead, char* msg)
{
    printf("%s\n",msg);
    if(phead == NULL)
    {
        return;
    }
    LinkNode* cur = phead;
    for(; cur != NULL; cur = cur -> next)
    {
        printf("[%c | %p] -> ", cur -> data, cur -> next);
    }
    printf("\n");
}


/*
 * 
 * 创建一个新结点
 *
 */

LinkNode* LinkNodeCreate(LinkNodeType value)
{
    LinkNode* new_node = (LinkNode*)malloc(sizeof(LinkNode));
    new_node -> data = value;
    new_node -> next = NULL;
}

//////////////////////////////////////////////////////////////////////////////
////以下是LinkNodeCreate的测试代码
/////////////////////////////////////////////////////////////////////////////

void TestCreat()
{
    LinkNode* head;
    LinkListInit(&head);
    head = LinkNodeCreate('a');
    TestPrintChar(head, "创建一个新结点:");
}

/*
 *
 * 销毁一个结点
 *
 */
void LinkNodeDestroy(LinkNode** phead)
{
    free(*phead);
    *phead = NULL;
}

/*
 *
 * 尾插一个元素
 *
 */

void LinkListPushBack(LinkNode** phead, LinkNodeType value)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        LinkNode* new_node = LinkNodeCreate(value);
        *phead = new_node;
        return;
    }
    LinkNode* cur = *phead;
    for(; cur -> next != NULL; cur = cur -> next)
    {
        ;
    }
    LinkNode* new_node = LinkNodeCreate(value);
    cur -> next = new_node;
}

//////////////////////////////////////////////////////////////////////////////
////LinkListPushBack的测试代码
//////////////////////////////////////////////////////////////////////////////


void PushBackTest()
{
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushBack(&head, 'x');
    LinkListPushBack(&head, 'y');
    LinkListPushBack(&head, 'a');
    LinkListPushBack(&head, 'q');
    TestPrintChar(head, "尾插一个元素:");
}

/*
 *
 * 尾删一个元素
 *
 */

void LinkListPopBack(LinkNode** phead)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    LinkNode* cur = *phead;
    if(cur -> next == NULL)//只有一个结点
    {
        LinkNodeDestroy(phead);
        return;
    }
    for(; cur -> next -> next != NULL; cur = cur -> next)
    {
        ;
    }
    LinkNodeDestroy(&(cur -> next));

}

////////////////////////////////////////////////////////////////////////////
////LinkListPopBack的测试代码
///////////////////////////////////////////////////////////////////////////

void PopBackTest()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    //LinkListPushBack(&head, 'x');
    //TestPrintChar(head, "尾插一个元素");
    //LinkListPopBack(&head);

    //TestPrintChar(head, "尾删只有一个元素的链表:");

    LinkListPushBack(&head, 'y');
    LinkListPushBack(&head, 'a');
    LinkListPushBack(&head, 'q');
    LinkListPushBack(&head, 'z');
    TestPrintChar(head, "尾插三个元素");

    LinkListPopBack(&head);
    TestPrintChar(head, "尾删一个三个元素");
}

/*
 *
 * 头插
 *
 */

void LinkListPushFront(LinkNode** phead, LinkNodeType value)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)//链表为空
    {
        LinkListPushBack(phead, value);
        return;
    }
    LinkNode* new_head = LinkNodeCreate(value);
    new_head -> next = *phead;
    *phead = new_head;
}

///////////////////////////////////////////////////////////////////////////////////
//////LinkListPushFront的测试代码
///////////////////////////////////////////////////////////////////////////////////

void TestPushFront()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    TestPrintChar(head, "头插四个元素");
}

/*
 *
 *头删
 *
 */

void LinkListPopFront(LinkNode** phead)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    if((*phead) -> next == NULL)//链表只有一个元素
    {
        LinkListPopBack(phead);
        return;
    }
    LinkNode* new_head = *phead;
    *phead = new_head -> next;
    LinkNodeDestroy(&new_head);
}

////////////////////////////////////////////////////////////////////////////////////////////
////LinkListPopFront的测试代码
////////////////////////////////////////////////////////////////////////////////////////////

void TestPopFront()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPopFront(&head);
    TestPrintChar(head, "头删空链表：");

    LinkListPushFront(&head, 'a');
    TestPrintChar(head, "头插一个元素：");
    LinkListPopFront(&head);
    TestPrintChar(head, "头删一个元素：");

    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    TestPrintChar(head, "头插六个个元素：");
    LinkListPopFront(&head);
    LinkListPopFront(&head);
    TestPrintChar(head, "头删两个元素：");

}

/*
 *
 * 查找 to_find 元素，并且返回其对应的地址
 * 如果没有找到，就返回空
 *
 */

LinkNode* LinkListFind(LinkNode* phead, LinkNodeType to_find)
{
    if(phead == NULL)
    {
        return NULL;//非法输入
    }
    LinkNode* cur = phead;
    for(; cur != NULL; cur = cur -> next)
    {
        if(cur -> data== to_find)
        {
            return cur;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
////LinkListFind的测试代码
//////////////////////////////////////////////////////////////////////////

void TestFind()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkNode* ret = LinkListFind(head, 'a');
    TestPrintChar(head,"头插六个元素");
    printf("&a = %p\n", ret);
}

/*
 *
 *在 pos 之前插入元素 value
 *
 */

void LinkListInsert(LinkNode** phead, LinkNode* pos, LinkNodeType value)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL || (*phead) -> next == NULL)//只有一个元素，或者没有元素
    {
        LinkListPushFront(phead, value);
        return;
    }
    LinkNode* cur = *phead;
    LinkNode* new_node = LinkNodeCreate(value);
    while(1)
    {
        if(cur -> next == pos)
        {
            new_node -> next = pos;
            cur -> next = new_node;
            return;
        }
        cur = cur -> next;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
////LinkNodeInsert的测试代码
/////////////////////////////////////////////////////////////////////////////////////////

void TestInsert()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkListInsert(&head, head -> next, '1');
    TestPrintChar(head, "往e前面插入元素‘1’：");
}

/*
 *
 * 在 pos 之后插入一个元素
 *
 */

void LinkListInsertAfter(LinkNode** phead, LinkNode* pos, LinkNodeType value)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)//链表为空
    {
        LinkListPushBack(phead, value);
        return;
    }

    LinkNode* new_node = LinkNodeCreate(value);
    new_node -> next = pos -> next;
    pos -> next = new_node;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////LinkListInsertAfer的测试代码
/////////////////////////////////////////////////////////////////////////////////////////////////

void TestInsertAfter()
{
    TESTHEADER;
    LinkNode* head; 
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkListInsertAfter(&head, head -> next -> next, '1');
    TestPrintChar(head, "往d后面插入元素‘1’：");
}

/*
 *
 * 删除指定位置 pos 处的元素
 *
 */

void LinkListEraser(LinkNode** phead, LinkNode* pos)
{
    if(phead == NULL || pos == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    if((*phead) -> next == NULL)
    {
        LinkNodeDestroy(phead);
        return;
    }
    LinkNode* cur = *phead;
    while(cur -> next != pos)
    {
        cur = cur -> next;
    }
    LinkNode* to_eraser = cur -> next;
    LinkNodeDestroy(&pos);
    cur -> next = to_eraser -> next;
}

/////////////////////////////////////////////////////////////////////////////////////
////LinkListEraser的测试代码
//////////////////////////////////////////////////////////////////////////////////////

void TestEraser()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    TestPrintChar(head, "头插5个元素：");
    LinkListEraser(&head, head -> next);
    TestPrintChar(head, "删除e:");
}

/*
 *
 * 删除指定位置 pos 处的元素
 *
 */

void LinkListEraser2(LinkNode** phead, LinkNode* pos)
{
    pos -> data = (pos -> next) -> data;
    LinkNode* to_eraser = pos -> next;
    pos -> next = to_eraser -> next;
    LinkNodeDestroy(&(to_eraser));
}

///////////////////////////////////////////////////////////////////////////////
////LinkListEraser2的测试代码
////////////////////////////////////////////////////////////////////////////////

void TestEraser2()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    TestPrintChar(head, "头插5个元素：");
    LinkListEraser2(&head, head -> next);
    TestPrintChar(head, "删除e:");
}

/*
 *
 * 不用遍历链表在 pos 前插入一个元素
 *
 */

void LinkNodeInsertBefor(LinkNode** phead, LinkNode* pos, LinkNodeType value)
{
    if(phead == NULL || pos == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)//链表为空
    {
        LinkListPushFront(phead, value);
        return;
    }
    LinkNode* new_node = LinkNodeCreate(value);
    new_node -> data = pos -> data;
    pos -> data = value;
    new_node -> next = pos -> next;
    pos -> next = new_node;
}

/////////////////////////////////////////////////////////////////////////////////////
////LinkNodeInsertBefor的测试代码
/////////////////////////////////////////////////////////////////////////////////////

void TestInsertBefor()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkListInsert(&head, head -> next, '1');
    TestPrintChar(head, "往e前面插入元素‘1’：");
}

/*
 *
 * 删除指定值的元素
 *
 */

void LinkListRemove(LinkNode** phead, LinkNodeType to_delete)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    LinkNode* cur;
    cur = *phead;
    while(cur != NULL)
    {
        if(cur -> data == to_delete)
        {
            break;
        }
        cur = cur -> next;
    }
    if(cur == NULL)
    {
        return;
    }
    LinkListEraser(phead, cur);
    return;
}

/////////////////////////////////////////////////////////////////////////////
/////LinkListRemove的测试代码
/////////////////////////////////////////////////////////////////////////////

void TestRemove()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    TestPrintChar(head, "插入5个元素：");
    LinkListRemove(&head, 'e');
    TestPrintChar(head, "删除‘e’：");
}

/*
 *
 * 删除指定值得所有元素
 *
 */

void LinkListRemoveAll(LinkNode** phead, LinkNodeType value)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    LinkNode* cur = *phead;
    while(cur != NULL)
    {
        if(cur -> data == value)
        {
            LinkListRemove(phead, value);
        }
        cur = cur -> next;
    }
}

///////////////////////////////////////////////////////////////////////////////
////LinkListRemove的测试代码
///////////////////////////////////////////////////////////////////////////////

void TestRemoveAll()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    TestPrintChar(head, "插入9个元素");
    LinkListRemoveAll(&head, 'a');
    TestPrintChar(head, "删除链表中的'a':");

}
    
/*
 *
 * 求链表元素个数
 *
 */

size_t LinkNodeSize(LinkNode* phead)
{
    if(phead == NULL)
    {
        return 0;
    }
    int count = 0;
    LinkNode* cur = phead;
    for(; cur != NULL; cur = cur -> next)
    {
        count ++;
    }
    return count;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////LinkNodeSize的测试代码
/////////////////////////////////////////////////////////////////////////////////////////

void TestSize()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    int count = 0;
    count = LinkNodeSize(head);
    printf("expected count = %d, actuall count = %d\n", 9, count);
}


/*
 *
 * 链表判空
 *
 */

int LinkListIsEmpty(LinkNode* phead)
{
    if(phead == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////
/////LinKListIs的测试代码///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void TestEmpty()
{
    LinkNode* head;
    LinkListInit(&head);
    int ret = LinkListIsEmpty(head);
    printf("expected ret = 1, actuall ret = %d\n", ret);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    ret = LinkListIsEmpty(head);
    printf("expected ret = 0, actuall ret = %d\n", ret);

}
/*
 *
 * 逆序打印链表
 *
 */

void LinkListReversePrint(LinkNode* phead)
{
   if(phead == NULL) 
   {
       return;
   }
   LinkListReversePrint(phead -> next);
   printf("%c   ", phead -> data);
}

////////////////////////////////////////////////////////////////////////////////////////////
/////LinkListReversePrint的测试代码
////////////////////////////////////////////////////////////////////////////////////////////

void TestReversePrint()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListReversePrint(head);
}

/*
 *
 *约瑟夫环
 *
 */

LinkNode* LinkListJosephCircle(LinkNode** phead, int M)
{
    if(phead == NULL)
    {
        return NULL;//非法输入
    }
    if(*phead == NULL)
    {
        return NULL;//链表为空
    }
    if((*phead) -> next == *phead)
    {
        return *phead;//只有一个元素
    }
    LinkNode* cur = *phead;
    LinkNode* to_delete;
    int i = 0;
    while(cur -> next != cur)
    {
        for(i = 1; i < M; i ++)
        {
            cur = cur -> next;
        }
        cur -> data = (cur -> next) -> data;
        to_delete = cur -> next;
        cur -> next = to_delete -> next;
        LinkNodeDestroy(&to_delete);
    }
    return cur;
}

////////////////////////////////////////////////////////////////////////////////////
/////LinkListJosephCircle的测试代码
/////////////////////////////////////////////////////////////////////////////////////

void TestJosephCircle()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkListPushFront(&head, 'g');
    LinkNode* cur = head;
    while(cur -> next != NULL)
    {
        cur = cur -> next;
    }
    cur -> next = head;
    LinkNode* surviver = LinkListJosephCircle(&head, 5);
    printf("surviver = %c\n", surviver -> data);
}
/*
 *
 *链表的逆置
 *
 */

void LinkListReverse(LinkNode** phead)
{
    if(phead == NULL)
    {
        return;//非法输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    if((*phead) -> next == NULL)
    {
        return;//只有一个元素
    }
    LinkNode* cur = *phead;
    LinkNode* to_delete = cur;
    while(cur -> next != NULL)
    {
        to_delete = cur -> next;
        cur -> next = to_delete -> next;
        to_delete -> next = *phead;
        *phead = to_delete;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
/////LinkListReverse的测试代码
///////////////////////////////////////////////////////////////////////////////////////////

void TestReverse()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'a');
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    TestPrintChar(head, "逆置单链表之前");
    LinkListReverse(&head);
    TestPrintChar(head, "逆置单链表之后");
    
}

/*
 *
 * 链表的销毁
 *
 */
void LinkListDestroy(LinkNode** phead)
{
    if(phead == NULL)
    {
        return;//得发输入
    }
    if(*phead == NULL)
    {
        return;//链表为空
    }
    LinkNode* cur = *phead;
    LinkNode* to_delete;
    while(cur -> next != NULL)
    {
        to_delete = cur -> next;
        cur -> next = to_delete -> next;
        LinkNodeDestroy(&to_delete);
    }
    LinkNodeDestroy(&cur);
    *phead = NULL;
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///LinkListDestroy的测试代码
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void TestDestory()
{
    TESTHEADER;
    LinkNode* head;
    LinkListInit(&head);
    LinkListPushFront(&head, 'b');
    LinkListPushFront(&head, 'c');
    LinkListPushFront(&head, 'd');
    LinkListPushFront(&head, 'e');
    LinkListPushFront(&head, 'f');
    LinkListPushFront(&head, 'g');
    LinkListDestroy(&head);
    TestPrintChar(head, "销毁链表");
}

