#ifndef __QUOS_MEMORY_H__
#define __QUOS_MEMORY_H__
#include "quos_config.h"

#define QUOS_CALLOC(LEN) quos_calloc(LEN, __FUNCTION__, __LINE__)
#define QUOS_FREE(ADDR) quos_free(ADDR, __FUNCTION__, __LINE__)
void Quos_memInit(void);
void Quos_memTest(void);
void *quos_calloc(quint32_t len, const char *funName, qint32_t line);
void quos_free(void *addr, const char *funName, qint32_t line);
void Quos_memInfoDump(void);
#endif