#ifndef __QUOS_SHA1_H__
#define __QUOS_SHA1_H__
#include "quos_config.h"
#if (SDK_ENABLE_SHA1 == 1)
#define QUOS_SHA1_DIGEST_LENGTH (20)
#define QUOS_SHA1_KEY_IOPAD_SIZE (64)


typedef struct
{
    quint32_t total[2];  /*!< number of bytes processed  */
    quint32_t state[5];  /*!< intermediate digest state  */
    quint8_t buffer[64]; /*!< data block being processed */
    quint8_t k_opad[QUOS_SHA1_KEY_IOPAD_SIZE];
    qbool isHmac;
} Quos_SHA1_ctx_t;

void Quos_sha1init(Quos_SHA1_ctx_t *ctx);
void Quos_sha1key(Quos_SHA1_ctx_t *ctx, quint8_t *key, quint8_t key_len);
void Quos_sha1update(Quos_SHA1_ctx_t *ctx, const quint8_t *input, quint32_t ilen);
void Quos_sha1finish(Quos_SHA1_ctx_t *ctx, quint8_t output[QUOS_SHA1_DIGEST_LENGTH]);
#endif/*SDK_ENABLE_SHA1 */
#endif/*__QUOS_SHA1_H__*/
