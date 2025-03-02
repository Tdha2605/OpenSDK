#ifndef __QHAL_FLASHOPT_H__
#define __QHAL_FLASHOPT_H__
#include "Qhal_types.h"
#include "ql_fs.h"
#define QIOT_FILE_CONFIG "qiot_config.ini"
#define QIOT_FILE_OTA "qiot_ota.bin"
#if (1 == QTH_ENABLE_MODBUS)
#define MODBUS_FILE_CONFIG "modbus.bin"
#define MODBUS_FILE_CONFIG_BAK "modbus_BAK.bin"
#define MODBUS_FILE_SUB "modbus_SUB.bin"
#endif/*QTH_ENABLE_MODBUS */
#define U_DISK "U:/"

pointer_t Qhal_fileOpen(const char *filename, qbool onlyRead);
qbool Qhal_fileWrite(pointer_t sockFd,quint32_t offset,void *buf,quint16_t bufLen);
quint32_t Qhal_fileRead(pointer_t sockFd,quint32_t offset,void *buf,quint16_t bufLen);
void Qhal_fileClose(pointer_t sockFd);
qbool Qhal_fileErase(const char *filename);
#endif/*__QHAL_FLASHOPT_H__ */
