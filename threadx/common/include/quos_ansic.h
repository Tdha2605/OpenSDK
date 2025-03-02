#ifndef __QUOS_ANSIC_H__
#define __QUOS_ANSIC_H__
#include "quos_config.h"

qint32_t Quos_atoi(const char *nptr);
qbool Quos_iota(qint64_t number,char *string);
qint64_t Quos_round(double value);
double Quos_fabs(double value);
qbool Quos_reallocN(void **ptr, quint32_t oldLen, quint32_t newLen);
void *Quos_memdup(const void *buf, quint32_t len);
void *Quos_memset(void *str, qint32_t c, quint32_t n);
qint32_t Quos_memcmp(const void *str1, const void *str2, quint32_t n);
void *Quos_memmove(void *dst, const void *src, quint32_t count);
char *Quos_strdup(const void *str);
char *Quos_strchr(const char *s, char c);
char *Quos_strcpy(char *dst, const char *src);
char *Quos_strncpy(char *dst, const char *src, qint32_t maxlen);
qint32_t Quos_strcmp(const char *str1,const char *str2);
qint32_t Quos_strncmp(const char *str1,const char *str2, quint32_t maxlen);
qint32_t Quos_strncasecmp(const char *str1, const char *str2, qint32_t maxlen);
quint32_t Quos_strlen(const char * str);
char *Quos_strstr(const char *str1, const char *str2);
quint64_t Quos_strtoul(const char *cp, char **endp, quint32_t base);
qint64_t Quos_strtol(const char *cp, char **endp, quint32_t base);
double Quos_strtod(const char *nptr, char **endptr);
#endif