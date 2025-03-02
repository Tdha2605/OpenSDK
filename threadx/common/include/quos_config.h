#ifndef __QUOS_CONFIG_H__
#define __QUOS_CONFIG_H__
#include "Qhal_driver.h"

#define SOCKET_FD_INVALID                   ((pointer_t)-1)
#define QUOS_DNS_HOSTNANE_MAX_LENGHT        (64) /* DNS规定域名不能超过63字符*/
#define QUOS_IP_ADDR_MAX_LEN                (46)
enum
{
    QUOS_SYSTEM_EVENT_NETWORK = -1,
};
enum
{
    QUOS_SEVENT_NET_CONNECTED = 0,
    QUOS_SEVENT_NET_DISCONNECT = 1,
    QUOS_SEVENT_NET_CONNTIMEOUT = 2,
    QUOS_SYSTEM_NET_RECOVER = 3,
};
/* SDK 支持功能配置 */
#ifndef SDK_ENABLE_TLS
#define SDK_ENABLE_TLS      0
#endif/*SDK_ENABLE_TLS */
#ifndef SDK_ENABLE_HTTP
#define SDK_ENABLE_HTTP     0
#endif/*SDK_ENABLE_HTTP */
#ifndef SDK_ENABLE_LWM2M
#define SDK_ENABLE_LWM2M    0
#endif/*SDK_ENABLE_LWM2M */
#ifndef SDK_ENABLE_COAP
#define SDK_ENABLE_COAP     0
#endif/*SDK_ENABLE_COAP */
#ifndef SDK_ENABLE_MQTT
#define SDK_ENABLE_MQTT     0
#endif/*SDK_ENABLE_MQTT */
#ifndef SDK_ENABLE_SHA1
#define SDK_ENABLE_SHA1     0
#endif/*SDK_ENABLE_SHA1 */
#ifndef SDK_ENABLE_SHA256
#define SDK_ENABLE_SHA256   0
#endif/*SDK_ENABLE_SHA256 */
#ifndef SDK_ENABLE_MD5
#define SDK_ENABLE_MD5      0
#endif/*SDK_ENABLE_MD5 */
#ifndef SDK_ENABLE_BASE64
#define SDK_ENABLE_BASE64   0
#endif/*SDK_ENABLE_BASE64 */
#ifndef SDK_ENABLE_AES
#define SDK_ENABLE_AES      0
#endif/*SDK_ENABLE_AES */
#ifndef SDK_ENABLE_EVENT
#define SDK_ENABLE_EVENT    0
#endif/*SDK_ENABLE_EVENT */
#ifndef SDK_ENABLE_FIFO
#define SDK_ENABLE_FIFO     0
#endif/*SDK_ENABLE_FIFO */
#ifndef SDK_ENABLE_SIGNAL
#define SDK_ENABLE_SIGNAL   0
#endif/*SDK_ENABLE_SIGNAL */
#ifndef SDK_ENABLE_SOCKET
#define SDK_ENABLE_SOCKET    0
#endif/*SDK_ENABLE_SOCKET */
#ifndef SDK_ENABLE_TIMER
#define SDK_ENABLE_TIMER    0
#endif/*SDK_ENABLE_TIMER */
#ifndef SDK_ENABLE_SYSTICK
#define SDK_ENABLE_SYSTICK   0
#endif/*SDK_ENABLE_SYSTICK */
#ifndef SDK_ENABLE_JSON
#define SDK_ENABLE_JSON     0
#endif/*SDK_ENABLE_JSON */
#ifndef SDK_ENABLE_DATASAFE
#define SDK_ENABLE_DATASAFE 0
#endif/*SDK_ENABLE_DATASAFE */
#ifndef SDK_ENABLE_TWLL
#define SDK_ENABLE_TWLL     0
#endif/*SDK_ENABLE_TWLL */
#ifndef SDK_ENABLE_NET_MGR
#define SDK_ENABLE_NET_MGR       0
#endif/*SDK_ENABLE_NET_MGR */
#ifndef SDK_ENABLE_DNS_CACHE
#define SDK_ENABLE_DNS_CACHE     0
#endif/*SDK_ENABLE_DNS_CACHE */
#ifndef SDK_ENABLE_MEMCHK
#define SDK_ENABLE_MEMCHK     0
#endif/*SDK_ENABLE_DNS_CACHE */

/* LOG模块 PRINTF等级配置 */
#ifndef LSDK_STORE
#define LSDK_STORE LL_DBG
#endif/*LSDK_STORE */
#ifndef LSDK_COAP
#define LSDK_COAP  LL_DBG
#endif/*LSDK_COAP */
#ifndef LSDK_ENCRP
#define LSDK_ENCRP LL_DBG
#endif/*LSDK_ENCRP */
#ifndef LSDK_EVENT
#define LSDK_EVENT LL_DBG
#endif/*LSDK_EVENT */
#ifndef LSDK_HTTP
#define LSDK_HTTP  LL_DBG
#endif/*LSDK_HTTP */
#ifndef LSDK_MQTT
#define LSDK_MQTT  LL_DBG
#endif/*LSDK_MQTT */
#ifndef LSDK_SIG
#define LSDK_SIG   LL_DBG
#endif/*LSDK_SIG */
#ifndef LSDK_SOCK
#define LSDK_SOCK  LL_DBG
#endif/*LSDK_SOCK */
#ifndef LSDK_TIMER
#define LSDK_TIMER LL_DBG
#endif/*LSDK_TIMER */
#ifndef LSDK_LWM2M
#define LSDK_LWM2M LL_DBG
#endif/*LSDK_LWM2M */
#ifndef LSDK_NET
#define LSDK_NET   LL_DBG
#endif/*LSDK_NET */
#ifndef LSDK_MEM
#define LSDK_MEM   LL_ERR
#endif/*LSDK_MEM */
#endif/*__QUOS_CONFIG_H__ */
