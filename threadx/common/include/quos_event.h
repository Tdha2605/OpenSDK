#ifndef __QUOS_EVENT_H__
#define __QUOS_EVENT_H__
#include "quos_config.h"
#if (SDK_ENABLE_EVENT == 1) 

typedef void (*EventCB_f)(qint32_t event, void *arg);

/**************************************************************************
**@brief : 事件注册 (该函数接口支持多个事件对应同一回调函数,
**         也支持通过多次调用实现单一事件对应多个回调函数的使用方法)
**@param : eventArray[in]:待注册的事件数组
**         eventCnt[in]:待注册事件的数量
**         eventCb[in]:事件回调函数
**@retval: void
***************************************************************************/
void Quos_eventCbReg(const qint32_t eventArray[], quint32_t eventCnt, EventCB_f eventCb);

/**************************************************************************
**@brief : 事件消息投递
**@param : evnt[in]:需要投递消息的事件
**         arg[in]:需要投递的消息(该消息将投递到参数evnt对应的事件中)
** 输出	@retval: TRUE:事件消息投递成功，FALSE:事件消息投递失败
***************************************************************************/
qbool Quos_eventPost(qint32_t event,void *arg);
#endif/*SDK_ENABLE_EVENT */
#endif/*__QUOS_EVENT_H__ */
