//.h文件
#include<stdio.h>

#pragma once

typedef int LinkNodeType;

typedef struct LinkNode
{
   LinkNodeType data;
   struct LinkNode* next;
}LinkNode,*list;//结点定义

typedef LinkNode* PLinkNode;

void LinkListInit(LinkNode** phead);//初始化链表

void LinkListPushBack(LinkNode** phead, LinkNodeType value);//尾插一个元素

LinkNode* LinkNodeCreate(LinkNodeType value);//创建一个结点

void LinkListPopFront(LinkNode** phead);//头删一个元素

void LinkListPushFront(LinkNode** phead, LinkNodeType value);//头插一个元素

void LinkListPopBack(LinkNode** phead);//尾删一个元素

void LinkNodeDestroy(LinkNode** phead);//销毁一个结点

LinkNode* LinkListFind(LinkNode* phead, LinkNodeType to_find);//查找值为to_find的结点

void LinkListInsert(LinkNode** phead, LinkNode* pos, LinkNodeType value);//往pos处插入值为value的新结点


void LinkListInsertBefor(LinkNode** phead, LinkNode* pos, LinkNodeType value);//往pos前插入一个值为value的新结点

void LinkListEraser(LinkNode** phead, LinkNode* pos);//删除pos处的结点（遍历链表）

void LinkListEraser2(LinkNode** phead, LinkNode* pos);//删除pos处的结点（不遍历链表）

void LinkListRemove(LinkNode** phead, LinkNodeType to_delet);//删除值为to_delete的结点

void LinkListRemoveAll(LinkNode** phead, LinkNodeType value);//删除链表中值为value的所有结点

void LinkListSize(LinkNode* phead);//链表大小

void LinkListReversePrint(LinkNode* head);//逆置打印链表

int LinkListIsEmpty(LinkNode* phead);//链表判空

void LinkListReversePrint(LinkNode* phead);

void LinkListRevers(LinkNode** phead);//链表逆置

