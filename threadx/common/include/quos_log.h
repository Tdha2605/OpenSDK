#ifndef __QUOS_LOG_H__
#define __QUOS_LOG_H__
#include "quos_config.h"

#define LL_OFF  (0X07) /* 关闭所有日志 */
#define LL_FAIL (0X06) /* 将导致程序退出的错误及其以上 */
#define LL_ERR  (0X05) /* 发生错误但不会导致程序退出及其以上 */
#define LL_WARN (0X04) /* 警告级别错误及其以上 */
#define LL_INFO (0X03) /* 粗粒度级别log及其以上 */
#define LL_DBG  (0X02) /* 细粒度级别log及其以上 */
#define LL_DUMP (0X01) /* dump数据，仅用于Quos_logHexDump打印 */

#ifndef QUOS_LOGL
#define QUOS_LOGL LL_DBG
#endif/*QUOS_LOGL*/

#define Quos_logPrintf(TYPE, LEVEL, format, ...) quos_logPrint(TYPE, LEVEL, #TYPE, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define Quos_logHexDump(TYPE, LEVEL, HEAD, DAT, DATLEN) quos_logHexDump(TYPE, LEVEL, #TYPE, __FUNCTION__, __LINE__, HEAD, DAT, DATLEN, 8)
#define Quos_logHexDumpU16(TYPE, LEVEL, HEAD, DAT, DATLEN) quos_logHexDump(TYPE, LEVEL, #TYPE, __FUNCTION__, __LINE__, HEAD, DAT, DATLEN, 16)

void Quos_logInit(void);
void Quos_logSetCb(void (*printfCb)(qint8_t type, qint8_t level,const char *typeString, const void *buff, quint32_t buffLen));
void quos_logPrint(qint8_t type, qint8_t level, const char *typeString, const char *funName, qint32_t line, const char *format, ...);
void quos_logHexDump(qint8_t type, qint8_t level, const char *typeString, const char *funName, qint32_t line, const char *head, const void *dat, quint32_t datLen, quint8_t size);
#endif/*__QUOS_LOG_H__ */
