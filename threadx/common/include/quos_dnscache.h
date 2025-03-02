#ifndef __QUOS_DNS_CACHA_H__
#define __QUOS_DNS_CACHA_H__
#include "quos_config.h"

#if (SDK_ENABLE_DNS_CACHE == 1)
#define DNS_IP_FROM_HOSTNAME_MAX_NUM        (3)			/* 从DNS中获取ip个数 */
/**************************************************************************
**@brief : 设置域名默认ip信息
**@param : hostname[in]:域名地址
**         ip[in]:默认ip(为NULL时,则清除hostname对应的默认ip地址)
**@retval: void
***************************************************************************/
void Quos_netHostnameSetDefault(const char *hostname, const char *ip);

/**************************************************************************
**@brief : 根据域名获取ip值
**@param : hostname[in]:域名地址
**@retval: 域名对应的ip(NULL:获取失败)
***************************************************************************/
quint8_t *Quos_netGetIpFromHostname(const char *hostname);

/**************************************************************************
** @brief : 应用层连接成功后告知当前域名对应的有效ip序号(该函数在接收到服务连接应答后调用)
** @param : chlFd[in]:
** @retval: void
***************************************************************************/
void Quos_netHostnameValidIpNumSet(void* chlFd);

/**************************************************************************
** @brief : socket通道fd与域名绑定
** @param : chlFd[in]:socket 通道fd
**          hostname[in]:域名地址
** @retval: void
***************************************************************************/
void Quos_netHostnameWaittingConnect(void* chlFd, const char *hostname);

/**************************************************************************
**@brief : 根据域名获取有效ip
**@param : hostname[in]:域名地址
**         ip[out]:IPd地址
**@retval: TRUE:ip获取成功，FALSE:获取失败
***************************************************************************/
qbool Quos_netHostnameValidIpGet(const char *hostname, char *ip);

/**************************************************************************
**@brief : 删除指定域名节点
**@param : hostname[in]:待删除的域名节点地址
**@retval: void
***************************************************************************/
void Quos_netHostnameDelete(const char *hostname);
#endif/*SDK_ENABLE_DNS_CACHE */
#endif/*__QUOS_DNS_CACHA_H__ */