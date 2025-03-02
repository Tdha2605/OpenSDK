#ifndef __QHAL_TYPES_H__
#define __QHAL_TYPES_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include "ql_log.h"
#include "Qhal_config.h"
#if 0
#include <strings.h>
#include <pthread.h>
#include <net/if.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/time.h>
#include <sys/types.h>
#include <math.h>
#include <dirent.h>
#include <setjmp.h>
#include <netdb.h>
#include <mcheck.h>
#endif
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif
/*#define  NULL           ((void*)0)*/
#define quint8_t uint8_t
#define qint8_t int8_t
#define quint16_t uint16_t
#define qint16_t int16_t
#define quint32_t uint32_t
#define qint32_t int32_t
#define quint64_t uint64_t
#define qint64_t int64_t
#define pointer_t unsigned long int

//typedef enum{FALSE=0,TRUE=1}qbool;

typedef unsigned char qbool;

#ifdef __x86_64__
#define PRINTF_U64      "%lu"
#define PRINTF_S64      "%ld"
#else
#define PRINTF_U64      "%llu"
#define PRINTF_S64      "%lld"
#endif
#define PRINTF_FD    "%ld"

#define FUNCTION_ATTR_ROM
#define FUNCTION_ATTR_RAM

#define HAL_VSNPRINTF             vsnprintf
#define HAL_MEMCPY(a,b,l)        memcpy((quint8_t*)(a),(quint8_t*)(b),l)
#define HAL_MEMCMP(a,b,l)        memcmp((quint8_t*)(a),(quint8_t*)(b),l)
#define HAL_MEMSET               memset
#define HAL_MEMMOVE              memmove
#define HAL_SPRINTF              sprintf
#define HAL_SNPRINTF             snprintf
#define HAL_STRCHR(X,Y)          strchr(X,Y)
#define HAL_STRCPY(a,b)          strcpy((char*)a,(char*)b)
#define HAL_STRNCPY(a,b,l)       strncpy((char*)a,(char*)b,l)
#define HAL_STRCMP(a,b)          strcmp((char*)a,(char*)b)
#define HAL_STRNCMP(a,b,l)       strncmp((char*)a,(char*)b,l)
#define HAL_STRSTR(a,b)          strstr((char*)a,(char*)b)
#define HAL_STRLEN(a)            ((NULL==a)?0:strlen((char*)a))
#define HAL_STRNCASECMP(a,b,l)   strncasecmp((char*)a,(char*)b,l)
#define HAL_PRINTF(format,...)   ql_app_log(format,##__VA_ARGS__)
#define HAL_SSCANF               sscanf
#define HAL_ATOI				 atoi
#define HAL_STRTOUL              strtoul
#define HAL_STRTOD               strtod
#define HAL_LOCK_DEF(S,X)       S void* X;
#define HAL_LOCK_INIT(X)        ql_rtos_mutex_create(&X)     
#define HAL_LOCK(X)             ql_rtos_mutex_lock(X, 0xFFFFFFFF)
#define HAL_UNLOCK(X)           ql_rtos_mutex_unlock(X)
#define  PRINTF_MAXLEN   1024

#define HAL_LOCK_INITDATA        
#define HAL_MALLOC(LEN)          qhal_malloc(LEN,__FUNCTION__,__LINE__)
#define HAL_FREE(ADDR)           qhal_free(ADDR,__FUNCTION__,__LINE__)
#define HAL_STRDUP(X)            qhal_strdup(X,__FUNCTION__,__LINE__)
#define HAL_MEMDUP(X,Y)          qhal_memdup(X,Y,__FUNCTION__,__LINE__)

extern void *qhal_malloc(quint32_t len,const char *funName,qint32_t line);
extern void qhal_free(void *addr,const char *funName,qint32_t line);
extern void *qhal_memdup(const void *buf, quint32_t len, const char *funName, qint32_t line);
extern char *qhal_strdup(const char *str, const char *funName, qint32_t line);

#endif
