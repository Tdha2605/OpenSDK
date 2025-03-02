#ifndef __QUOS_NET_H__
#define __QUOS_NET_H__
#include "quos_config.h"
#if (SDK_ENABLE_NET_MGR == 1)

/**************************************************************************
**@brief : 获取网络服务状态
**@param : void
**@retval: TURE:网络已连接，FALSE：网络未连接
***************************************************************************/
qbool Quos_netStateGet(void);

/**************************************************************************
**@brief : 打开网络服务
**@param : void
**@retval: void
***************************************************************************/
void Quos_netOpen(void);

/**************************************************************************
**@brief : 关闭网络服务
**@param : void
**@retval: void
***************************************************************************/
void Quos_netClose(void);

/**************************************************************************
**@brief : 恢复网络服务(唤醒后)
**@param : void
**@retval: void
***************************************************************************/
void Quos_netRecover(void);

/**************************************************************************
**@brief : 网络状态变化通知
**@param : isConn[in]:网络连接状态(TRUE:已连接，FALSE:未连接)
**@retval: void
***************************************************************************/
void Quos_netIOStatusNotify(qbool result);
#endif/*SDK_ENABLE_NET_MGR */
#endif/*__QUOS_NET_H__ */
