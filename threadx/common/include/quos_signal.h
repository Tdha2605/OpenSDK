#ifndef __QUOS_SIGNAL_H__
#define __QUOS_SIGNAL_H__
#include "quos_config.h"

#if (1 == SDK_ENABLE_SIGNAL)
//信号回调函数类型定义
typedef void (*SignalCB_f)(const void *arg, quint32_t argLen);

/**************************************************************************
**@brief : 信号初始化
**@param : void
**@retval: void
***************************************************************************/
void Quos_signalInit(void);

/**************************************************************************
**@brief : 设置信号
**@param : arg[in]:信号数据指针(此入参为eventCB回调函数的参数1)
**         len[in]:信号数据长度(此入参为eventCB回调函数的参数2)
**         eventCB[in]:信号处理回调函数
**@retval: TRUE:设置成功，FALSE:设置失败
***************************************************************************/
qbool Quos_signalSet(void *arg, quint32_t len, SignalCB_f eventCB);

/**************************************************************************
**@brief : 处理信号
**@param : void
**@retval: TRUE:信号处理完成，FALSE:没有需要处理的信号
***************************************************************************/
qbool Quos_signalTask(void);
#endif/*SDK_ENABLE_SIGNAL */
#endif/*__QUOS_SIGNAL_H__ */
