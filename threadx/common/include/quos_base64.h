#ifndef __QUOS_BASE64_H__
#define __QUOS_BASE64_H__
#include "quos_config.h"

#define QUOS_BASE64_DSTDATA_LEN(SRCLEN) (((SRCLEN+2)/3 << 2) + 1)
#if (1 == SDK_ENABLE_BASE64)
/**************************************************************************
**@brief : base64编码
**@param : srcData[in]:待编码数据缓冲区指针
**         srcLen[in]:待编码数据长度
**         dstData[out]:编码后数据缓冲区指针
**         该缓冲区大小>=1+(srcLen/3*4)+((srcLen%3)?4:0)
**@retval: base64编码后的数据长度
***************************************************************************/
quint16_t Quos_base64Encrypt(const quint8_t *srcData, quint16_t srcLen, quint8_t *dstData);

/**************************************************************************
**@brief : base64解码
**@param : srcData[in]:base64编码数据缓冲区指针
**         srcLen[in]:base64编码数据长度
**         dstData[out]:解码后数据缓冲区指针
**@retval: base64解码后的数据长度
***************************************************************************/
quint16_t Quos_base64Decrypt(const quint8_t *srcData, quint16_t srcLen, quint8_t *dstData);
#endif/*SDK_ENABLE_BASE64 */
#endif/*__QUOS_BASE64_H__ */
