#ifndef __QUOS_LWM2M_H__
#define __QUOS_LWM2M_H__

#include "quos_config.h"
#if (1 == SDK_ENABLE_LWM2M)

#include "liblwm2m.h"
typedef struct
{
    struct
    {
        struct
        {
            quint8_t *val;
            quint16_t len;
        } payload;
    } bs;
    quint32_t stepPeriod;
} lwm2mUsedata_t;

void *quos_lwm2mInit(char *name, lwm2m_object_t *objects[], quint16_t count, lwm2mUsedata_t *userdata);
#endif/*SDK_ENABLE_LWM2M */
#endif/*__QUOS_LWM2M_H__ */
