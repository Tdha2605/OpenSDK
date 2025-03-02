#ifndef __QUOS_SUPPORTTOOL_H__
#define __QUOS_SUPPORTTOOL_H__
#include "quos_config.h"

typedef struct
{
    char *path; /* path指向url的地址 */
    char hostname[QUOS_DNS_HOSTNANE_MAX_LENGHT + 1];
    qbool isSecure;
    quint16_t port;
} urlAnalyze_t;
enum
{
    QUOS_CHAR_09 = (1 << 0),
    QUOS_CHAR_af = (1 << 1),
    QUOS_CHAR_gz = (1 << 2),
    QUOS_CHAR_AF = (1 << 3),
    QUOS_CHAR_GZ = (1 << 4),
};

#define __ENDIANCHANGE(x) ((sizeof(x) == 2) ? (quint16_t)((x >> 8) | (x << 8)) : ((sizeof(x) == 4) ? (quint32_t)((((x) >> 24) & 0x000000FF) | (((x) >> 8) & 0x0000FF00) | (((x) << 8) & 0x00FF0000) | (((x) << 24) & 0xFF000000)) : (x)))

/* 字节对齐 */
#define __BYTE_TO_ALIGN(X, Y) ((X) % (Y) ? ((X) + (Y) - (X) % (Y)) : (X))

/*大小写转换 */
#define __TO_UPPER(X) ((X) & (~0x20))
#define __TO_LOWER(X) ((X) | 0x20)

#define __IS_LETTER(X) (('A' <= (X) && (X) <= 'Z') || ('a' <= (X) && (X) <= 'z'))
#define __IS_DIGIT(X) ('0' <= (X) && (X) <= '9')
#define __IS_XDIGIT(X) (('0' <= (X) && (X) <= '9') || ('a' <= (X) && (X) <= 'f') || ('A' <= (X) && (X) <= 'F'))

/*求最大值和最小值 */
#define __GET_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define __GET_MIN(x, y) (((x) < (y)) ? (x) : (y))

/*得到一个field在结构体(struct)中的偏移量 */
#define __GET_POS_ELEMENT(type, field) ((pointer_t) & (((type *)0)->field))
/*得到一个结构体中field所占用的字节数 */
#define __GET_SIZE_ELEMENT(type, field) sizeof(((type *)0)->field)
/*根据元素地址得到结构体 */
#define __GET_STRUCT_BY_ELEMENT(ptr, type, field) ((type *)((pointer_t)ptr - __GET_POS_ELEMENT(type, field)))
/*返回一个比X小的最接近的n的倍数 */
#define __GET_SMALL_N(x, n) ((x) / (n) * (n))

/*返回一个比X大的最接近的n的倍数 */
#define __GET_BIG_N(x, n) (((x) + (n)-1) / (n) * (n))

/* 转换宏为字符串 */
#define _MACRO2STR_1(s) #s
#define _MACRO2STR_2(s) _MACRO2STR_1(s)

/* 字符串拼接 */
#define _STRCAT_STR_1(A, B) A##B
#define _STRCAT_STR_2(A, B) _STRCAT_STR_1(A, B)

#define _BOOL2STR(X) ((X) ? "TRUE" : "FALSE") /* qbool 转字符串 */
#define _STR2BOOL(X) (0 == Quos_strncasecmp(X, "TRUE", __GET_MAX(Quos_strlen(X), Quos_strlen("TRUE"))) ? TRUE : FALSE)
#define _STR2PRINT(X) (NULL == X ? "" : X)
#define _DATA2BOOL(X, Y) (((X >> Y) & 1) ? TRUE : FALSE)

#define _ARRAY01_U16(ARRAY) (((quint16_t)(((quint8_t *)(ARRAY))[0]) << 8) | \
                             ((quint16_t)(((quint8_t *)(ARRAY))[1]) << 0))

#define _ARRAY10_U16(ARRAY) (((quint16_t)(((quint8_t *)(ARRAY))[1]) << 8) | \
                             ((quint16_t)(((quint8_t *)(ARRAY))[0]) << 0))

#define _ARRAY0123_U32(ARRAY) (((quint32_t)(((quint8_t *)(ARRAY))[0]) << 24) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[1]) << 16) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[2]) << 8) |  \
                               ((quint32_t)(((quint8_t *)(ARRAY))[3]) << 0))

#define _ARRAY1032_U32(ARRAY) (((quint32_t)(((quint8_t *)(ARRAY))[1]) << 24) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[0]) << 16) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[3]) << 8) |  \
                               ((quint32_t)(((quint8_t *)(ARRAY))[2]) << 0))

#define _ARRAY3210_U32(ARRAY) (((quint32_t)(((quint8_t *)(ARRAY))[3]) << 24) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[2]) << 16) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[1]) << 8) |  \
                               ((quint32_t)(((quint8_t *)(ARRAY))[0]) << 0))

#define _ARRAY2301_U32(ARRAY) (((quint32_t)(((quint8_t *)(ARRAY))[2]) << 24) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[3]) << 16) | \
                               ((quint32_t)(((quint8_t *)(ARRAY))[0]) << 8) |  \
                               ((quint32_t)(((quint8_t *)(ARRAY))[1]) << 0))

#define _ARRAY76543210_U64(ARRAY) (((quint64_t)(((quint8_t *)(ARRAY))[7]) << 56) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[6]) << 48) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[5]) << 40) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[4]) << 32) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[3]) << 24) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[2]) << 16) | \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[1]) << 8) |  \
                                   ((quint64_t)(((quint8_t *)(ARRAY))[0]) << 0))

#define _ARRAY012345678_U64(ARRAY) (((quint64_t)(((quint8_t *)(ARRAY))[0]) << 56) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[1]) << 48) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[2]) << 40) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[3]) << 32) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[4]) << 24) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[5]) << 16) | \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[6]) << 8) |  \
                                    ((quint64_t)(((quint8_t *)(ARRAY))[7]) << 0))

#define _U16_ARRAY01(INT, ARRAY) (((quint8_t *)ARRAY)[0] = ((INT) >> 8) & 0xFF, ((quint8_t *)ARRAY)[1] = (INT)&0xFF)
#define _U16_ARRAY10(INT, ARRAY) (((quint8_t *)ARRAY)[0] = (INT)&0xFF, ((quint8_t *)ARRAY)[1] = ((INT) >> 8) & 0xFF)
#define _U32_ARRAY0123(INT, ARRAY) (((quint8_t *)ARRAY)[0] = ((INT) >> 24) & 0xFF, ((quint8_t *)ARRAY)[1] = ((INT) >> 16) & 0xFF, ((quint8_t *)ARRAY)[2] = ((INT) >> 8) & 0xFF, ((quint8_t *)ARRAY)[3] = (INT)&0xFF)
#define _U32_ARRAY3210(INT, ARRAY) (((quint8_t *)ARRAY)[0] = (INT)&0xFF, ((quint8_t *)ARRAY)[1] = ((INT) >> 8) & 0xFF, ((quint8_t *)ARRAY)[2] = ((INT) >> 16) & 0xFF, ((quint8_t *)ARRAY)[3] = ((INT) >> 24) & 0xFF)
#define _U64_ARRAY01234567(INT, ARRAY) (((quint8_t *)ARRAY)[0] = ((INT) >> 56) & 0xFF, ((quint8_t *)ARRAY)[1] = ((INT) >> 48) & 0xFF, ((quint8_t *)ARRAY)[2] = ((INT) >> 40) & 0xFF, ((quint8_t *)ARRAY)[3] = ((INT) >> 32) & 0xFF, \
                                        ((quint8_t *)ARRAY)[4] = ((INT) >> 24) & 0xFF, ((quint8_t *)ARRAY)[5] = ((INT) >> 16) & 0xFF, ((quint8_t *)ARRAY)[6] = ((INT) >> 8) & 0xFF, ((quint8_t *)ARRAY)[7] = ((INT) >> 0) & 0xFF)
#define _U64_ARRAY76543210(INT, ARRAY) (((quint8_t*)ARRAY)[0] = (INT)&0xFF,((quint8_t*)ARRAY)[1] = ((INT)>>8)&0xFF,((quint8_t*)ARRAY)[2] = ((INT)>>16)&0xFF,((quint8_t*)ARRAY)[3] = ((INT)>>24)&0xFF, \
                                (((quint8_t*)ARRAY)[4] = ((INT)>>32)&0xFF,((quint8_t*)ARRAY)[5] = ((INT)>>40)&0xFF,((quint8_t*)ARRAY)[6] = ((INT)>>48)&0xFF,((quint8_t*)ARRAY)[7] = ((INT)>>56)&0xFF)

#define IP2STR "%hhu.%hhu.%hhu.%hhu"
#define IP2STR_(IP) (quint8_t)((IP) >> 24), (quint8_t)((IP) >> 16), (quint8_t)((IP) >> 8), (quint8_t)((IP) >> 0)

#define TIME_SEC2UTC "%08u.%02u.%02u"
#define TIME_SEC2UTC_(SEC) (quint32_t)(SEC / 3600), (quint8_t)(SEC % 3600 / 60), (quint8_t)(SEC % 60)

#ifndef INT_MAX
#define INT_MAX (2147483647)
#endif/*INT_MAX */
#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif/*INT_MIN */
#ifndef DBL_EPSILON
#define DBL_EPSILON (2.2204460492503131e-016)
#endif/*DBL_EPSILON */

#define CPL_BIT(value, bit) (value ^= (1 << bit))  /* 取反指定位 */
#define SET_BIT(value, bit) (value |= (1 << bit))  /* 置位指定位 */
#define CLR_BIT(value, bit) (value &= ~(1 << bit)) /* 清零指定位 */
#define GET_BIT(value, bit) (value & (1 << bit))   /* 读取指定位 */

void Quos_RandomGen(void *data, quint32_t len, quint8_t type);
void Quos_stringPrintScrt(char *dst, const char *src, quint32_t len);

qbool Quos_numIsDouble(double value);
qbool Quos_urlAnalyze(const char *url, urlAnalyze_t *result);
qbool Quos_strIsInt(char *src, quint32_t len, qint32_t *value);
qbool Quos_strIsUint(char *src, quint32_t len, quint32_t *value);
qbool Quos_fileMd5(const char *filename, quint32_t fileLen, char md5[]);
qbool Quos_stringCheck(const void *data, quint32_t len, quint8_t type);
qbool Quos_keyValueInsert(char *srcStr, quint32_t maxLen, const char *keyword, const char *separator, const char *value, char *endStr);

char *Quos_stringRemoveMarks(char *strVal);
quint8_t Quos_decimalDigits(double value);
quint8_t Quos_convertToBase(quint32_t value, quint32_t base);

quint32_t Quos_ip2Int(const char *ipStr);
quint32_t Quos_str2Hex(void *srcStr, quint8_t RetHex[]);
quint32_t Quos_intPushArray(quint64_t intValue, quint8_t *array);
quint32_t Quos_crcCalculate(quint32_t crc, const void *buf, quint32_t len);
quint32_t Quos_hex2Str(quint8_t hex[], quint32_t hexLen, char *retStr, qbool isUpper);
quint32_t Quos_stringSplit(char *src, quint32_t srcLen, char **words, quint32_t maxSize, const char *delim, qbool keepEmptyParts);
qint32_t Quos_keyValueExtract(char *srcStr, const char *keyword, const char *separator, char **dstStr, const char *endStr);
double Quos_powInt(double base, qint32_t exp);

#endif/*__QUOS_SUPPORTTOOL_H__ */
