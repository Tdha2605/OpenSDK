#ifndef _QHAL_CONFIG_H_
#define _QHAL_CONFIG_H_

/**************************************** quecthing function switch start *********************************************/
#ifndef QTH_ENABLE_AT
#define QTH_ENABLE_AT               0 /* AT功能 */ 
#endif
#ifndef QTH_ENABLE_LOCIN
#define QTH_ENABLE_LOCIN            1 /* 设备内置定位功能 */
#endif
#ifndef QTH_ENABLE_AT_CACHE
#define QTH_ENABLE_AT_CACHE         1 /* AT方案下透传或物模型缓存功能 */
#endif
#ifndef QTH_ENABLE_BINDCODE
#define QTH_ENABLE_BINDCODE         1 /* 设备绑定功能 */
#endif
#ifndef QTH_ENABLE_DKDS
#define QTH_ENABLE_DKDS             1 /* 本地修改dk ds功能 */
#endif
#ifndef QTH_ENABLE_RST
#define QTH_ENABLE_RST              1 /* 设备重置功能 */
#endif
#ifndef QTH_ENABLE_SESSION
#define QTH_ENABLE_SESSION          1 /* 通信数据协议层加密功能 */
#endif
#ifndef QTH_ENABLE_HTTP_OTA
#define QTH_ENABLE_HTTP_OTA         1 /* httpOta功能 */ 
#endif
#ifndef QTH_ENABLE_GATEWAY
#define QTH_ENABLE_GATEWAY          1 /* 网关功能 */
#endif
#ifndef QTH_ENABLE_MODBUS
#define QTH_ENABLE_MODBUS           1 /* MODBUS功能 */
#endif
#ifndef QTH_ENABLE_QTH_OTA
#define QTH_ENABLE_QTH_OTA          0 /* FOTA下载服务源，1：Quecthing,0：模组自身 */
#endif
#ifndef QTH_ENABLE_NTP_REQ
#define QTH_ENABLE_NTP_REQ          1 /* NTP功能 */
#endif
#ifndef QTH_ENABLE_ODOS
#define QTH_ENABLE_ODOS             1 /* 一机一密功能 */
#endif
#ifndef QTH_ENABLE_DATACOLL
#define QTH_ENABLE_DATACOLL         1 /* 数采功能 */   
#endif
/**************************************** quecthing function switch end ***********************************************/

/************************************** quecthing base function switch *************************************************/
#define QTH_ENABLE_TSL              1   /* 物模型功能 */
#define QTH_ENABLE_TSL_SERVER       1   /* 物模型-服务功能 */
#define QTH_ENABLE_PASSTRANS        1   /* 透传数据功能 */
#define QTH_ENABLE_OTA              1   /* OTA 功能 */
/*******************************************************************************************************************/


/**************************************** kernel function switch start ************************************************/
#define SDK_ENABLE_TLS              1
#define SDK_ENABLE_HTTP             1
#define SDK_ENABLE_MQTT             1
#define SDK_ENABLE_COAP             1
#define SDK_ENABLE_SHA256           1
#define SDK_ENABLE_MD5              1
#define SDK_ENABLE_BASE64           1
#define SDK_ENABLE_AES              1
#define SDK_ENABLE_EVENT            1
#define SDK_ENABLE_SIGNAL           1
#define SDK_ENABLE_TIMER            1
#define SDK_ENABLE_SYSTICK          1
#define SDK_ENABLE_SOCKET           1
#define SDK_ENABLE_JSON             1
#define SDK_ENABLE_DATASAFE         1
#define SDK_ENABLE_TWLL             1
#define SDK_ENABLE_NET_MGR          1
#define SDK_ENABLE_DNS_CACHE        1
/**************************************** kernel function switch end ***********************************************/

/**************************************** LOG模块 PRINTF等级配置 ****************************************************/
#define LQTH_BUS                    LL_DBG
#define LQTH_OTA                    LL_DBG
#define LQTH_SYS                    LL_DBG
#define LQTH_IOT                    LL_DBG
#define LQTH_CONN                   LL_DBG
#define LQTH_DP                     LL_DBG
#define LQTH_LOC                    LL_DBG
#define LQTH_SECURE                 LL_ERR
#define LQTH_AT                     LL_DBG
#define LQTH_BIND                   LL_DBG
#define LQTH_TTLV                   LL_DBG
#define LQTH_GW                     LL_DBG
#define LQTH_MBSUB                  LL_DBG
#define LQTH_MBFILE                 LL_DBG
#define LQTH_MBDEV                  LL_DBG
#define LQTH_GWDM                   LL_DBG
#define LQTH_MBDOWN                 LL_DBG
#define LQTH_RLOG                   LL_DBG
#define LQTH_DATACOLL               LL_DBG

#define LSDK_STORE                  LL_DBG
#define LSDK_COAP                   LL_DBG
#define LSDK_ENCRP                  LL_DBG
#define LSDK_EVENT                  LL_DBG
#define LSDK_HTTP                   LL_DBG
#define LSDK_MQTT                   LL_DBG
#define LSDK_SIG                    LL_DBG
#define LSDK_SOCK                   LL_DBG
#define LSDK_TIMER                  LL_DBG
#define LSDK_LWM2M                  LL_DBG
#define LSDK_NET                    LL_DBG

#define HAL_DEV                     LL_DBG
#define HAL_FLASH                   LL_DBG
#define HAL_TCP                     LL_DBG
#define HAL_TLS                     LL_DBG
#define HAL_DTLS                    LL_DBG
#define HAL_UDP                     LL_DBG
#define HAL_UART                    LL_DBG
#define HAL_SOCK                    LL_DBG
#define HAL_PROP                    LL_DBG
#define HAL_NET                     LL_DBG
/**************************************** LOG模块 PRINTF等级配置 end ***********************************************/

/**************************************** 功能互斥控制 ***********************************************/
#define NO_SUPPORT_MESS "SDK not support this function"
#if (0 == QTH_ENABLE_TSL && 1 == QTH_ENABLE_TSL_SERVER)
#error "tls_server need tls func"
#endif/*0 == QTH_ENABLE_TSL && 1 == QTH_ENABLE_TSL_SERVER*/

#if(1 == QTH_ENABLE_AT)
#define QUEC_IOT_SEND_MAX_LEN (1024)
#define QUEC_ATCMD_BUF_MAX_LEN  (QUEC_IOT_SEND_MAX_LEN+64)
#if(1 == QTH_ENABLE_MODBUS)
#pragma message("modbus can not support to AT,auto to close it")
#undef QTH_ENABLE_MODBUS
#define QTH_ENABLE_MODBUS 0
#endif/*QTH_ENABLE_MODBUS*/
#else
#undef QTH_ENABLE_AT_CACHE
#define QTH_ENABLE_AT_CACHE 0
#endif/*QTH_ENABLE_AT */
/**************************************** 功能互斥控制 end ***********************************************/



#endif/*_QHAL_CONFIG_H_ */