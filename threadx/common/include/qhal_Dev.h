#ifndef __QHAL_DEV_H__
#define __QHAL_DEV_H__
#include "Qhal_types.h"
//#include "quec_cust_feature.h"
typedef enum
{
    DEV_RESTART_FACTORY = 0, /* 出厂状态 */
    DEV_RESTART_SEGMENTFAULT,
    DEV_RESTART_NORMAL,
    DEV_RESTART_OTA,
    DEV_RESTART_WDT,
    DEV_RESTART_NET_EXCP,
    DEV_RESTART_HW_FAULT,
} DevRestartReason_e;

#define QIOT_MD5_MAXSIZE (32)
typedef struct
{
    uint8_t idex;
    char md5[QIOT_MD5_MAXSIZE + 1];
    char *downloadUrl;
    quint32_t size;
} QIot_otaFilePublicInfo_t;

#define DEV_RESTART_REASON_STRING(X) \
    ( \
    (X == DEV_RESTART_FACTORY) ? "DEV_RESTART_FACTORY" : \
    (X == DEV_RESTART_SEGMENTFAULT) ? "DEV_RESTART_SEGMENTFAULT" : \
    (X == DEV_RESTART_NORMAL) ? "DEV_RESTART_NORMAL" : \
    (X == DEV_RESTART_OTA) ? "DEV_RESTART_OTA" : \
    (X == DEV_RESTART_WDT) ? "DEV_RESTART_WDT" : \
    (X == DEV_RESTART_NET_EXCP) ? "DEV_RESTART_NET_EXCP" : \
    (X == DEV_RESTART_HW_FAULT) ? "DEV_RESTART_HW_FAULT" : \
    "Unknown")

void Qhal_devFeeddog(void);
qbool Qhal_rtcInit(void);
void Qhal_rtcGet(quint32_t *sec, quint32_t *ms);
char *Qhal_softversionGet(void);
char *Qhal_logHeadString(void);
void Qhal_devRestart(void);
qbool Qhal_beforeMain(void);
char *Qhal_devUuidGet(void);
quint32_t Qhal_randomGet(void);
#if (1 == QTH_ENABLE_AT)
void Qhal_urcReport(const quint8_t *data, quint32_t len);
#endif/*QTH_ENABLE_AT */
#if (1 == QTH_ENABLE_QTH_OTA)
quint32_t Qhal_devOtaNotify(const char *filename, quint32_t fileSize);
#else
qint32_t Qhal_devOtaNotify(QIot_otaFilePublicInfo_t info[], quint32_t size);
#endif/*QTH_ENABLE_QTH_OTA */
void Qhal_KernelResume(void);
void Qhal_netOpen(quint32_t *timeout);
void Qhal_netClose(void);
#endif/*__QHAL_DEV_H__ */