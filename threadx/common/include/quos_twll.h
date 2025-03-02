#ifndef __QUOS_TWLL_H__
#define __QUOS_TWLL_H__
#include "quos_config.h"

#if (1 == SDK_ENABLE_TWLL)

typedef struct __TWLLHead
{
    struct __TWLLHead *prev;
    struct __TWLLHead *next;
}TWLLHead_T;

/* 对节点轮询 */
#define TWLIST_FOR_DATA(LISTHEAD,LISTTEMP,NEXTLIST)    \
    for((LISTTEMP) = (LISTHEAD),(NEXTLIST) = (LISTHEAD)?(LISTHEAD)->next:NULL;   \
        NULL != (LISTTEMP);   \
        (LISTTEMP) = (NEXTLIST),(NEXTLIST) = (NEXTLIST)?(NEXTLIST)->next:NULL) 

/**************************************************************************
**@brief : 双向链表尾部增加节点
**@param : list[in]:链表的首节点地址 
**         node[in] 需要添加的链表节点
**@retval: void
***************************************************************************/
void Quos_twllHeadAdd(TWLLHead_T **twList,TWLLHead_T *twNode);

/**************************************************************************
**@brief : 双向链表头部增加节点
**@param : list[in]:链表的首节点地址 
**         node[in]:需要添加的链表节点
**@retval: void
***************************************************************************/
void Quos_twllHeadAddFirst(TWLLHead_T **list, TWLLHead_T *node);

/**************************************************************************
**@brief : 删除双向链表指定节点
**@param : list[in]:链表的首节点地址
**         node[in]: 需要删除链表节点
**@retval: void
***************************************************************************/
void Quos_twllHeadDelete(TWLLHead_T **twList,TWLLHead_T *twNode);

/**************************************************************************
**@brief : 往双向链表指定节点前面插入节点
**@param : list[in]:链表的首节点地址
**         referNode[in]:参考节点
**         node[in]:需要插入的链表节点
**@retval:TRUE：插入成功，FALSE:插入失败
***************************************************************************/
qbool Quos_twllHeadInsertFront(TWLLHead_T **list, TWLLHead_T *referNode, TWLLHead_T *node);

/**************************************************************************
**@brief : 往双向链表指定节点后面插入节点
**@param : list[in]:链表的首节点地址
**         referNode[in]:参考节点
**         node 需要插入的链表节点
**@retval: TRUE：插入成功，FALSE:插入失败
***************************************************************************/
qbool Quos_twllHeadInsertBehind(TWLLHead_T **list, TWLLHead_T *referNode, TWLLHead_T *node);

/**************************************************************************
**@brief : 正向查找双向链表第N个节点
**@param : list[in]:链表的首节点地址
**         nodeId[in]:节点编号(第一个节点为0)
**@retval: 查找得到的链表节点
***************************************************************************/
TWLLHead_T *Quos_twllHeadFineNodeByNodeId(TWLLHead_T *list, quint32_t nodeId);

/**************************************************************************
**@brief : 获取指定双向链表节点数
**@param : list[in]:链表的首节点地址
**@retval: 双向链表节点数量
***************************************************************************/
quint32_t Quos_twllHeadGetNodeCount(TWLLHead_T *list);

#endif/*SDK_ENABLE_TWLL */

#endif/*__QUOS_TWLL_H__ */
