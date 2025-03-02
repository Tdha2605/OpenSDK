#ifndef __QUOS_MQTT_H__
#define __QUOS_MQTT_H__
#include "quos_config.h"
#if (SDK_ENABLE_MQTT == 1)
#include "quos_socket.h"

enum
{
    QUOS_MQTT_ERR_INSIDE = -5,
    QUOS_MQTT_ERR_NET = -4,
    QUOS_MQTT_ERR_PING = -3,
    QUOS_MQTT_ERR_SUBSCRI = -2,
    QUOS_MQTT_ERR_CONNECT = -1,
    QUOS_MQTT_OK_CONNECT = 0,
    QUOS_MQTT_UNNACCEPTABLE_PROTOCOL = 1,
    QUOS_MQTT_CLIENTID_REJECTED = 2,
    QUOS_MQTT_SERVER_UNAVAILABLE = 3,
    QUOS_MQTT_BAD_USERNAME_OR_PASSWORD = 4,
    QUOS_MQTT_NOT_AUTHORIZED = 5,
    QUOS_MQTT_SERVER_UNAVAILABLE_BAK = 6, /* 解决qthSDK2.9.0之前的SERVER_UNAVAILABLE错误时清空DS问题，作用同QUOS_MQTT_SERVER_UNAVAILABLE */
    QUOS_MQTT_OK_SUBSCRIBE = 100,
};

enum
{
    CONNECT = 1,
    CONNACK,
    PUBLISH,
    PUBACK,
    PUBREC,
    PUBREL,
    PUBCOMP,
    SUBSCRIBE,
    SUBACK,
    UNSUBSCRIBE,
    UNSUBACK,
    PINGREQ,
    PINGRESP,
    DISCONNECT
};

typedef struct
{
    char *topic;
    quint8_t *msg;
    quint16_t msgLen;
    qbool retained;
    quint8_t qos;
} Quos_mqttwill;

#define MQTT_ERR_STRING(X)                                                                                                          \
    (                                                                                                                               \
        (X == QUOS_MQTT_ERR_INSIDE) ? "MQTT_ERR_INSIDE" : (X == QUOS_MQTT_ERR_NET)                ? "MQTT_ERR_NET"                  \
                                                      : (X == QUOS_MQTT_ERR_PING)                 ? "MQTT_ERR_PING"                 \
                                                      : (X == QUOS_MQTT_ERR_SUBSCRI)              ? "MQTT_ERR_SUBSCRI"              \
                                                      : (X == QUOS_MQTT_ERR_CONNECT)              ? "MQTT_ERR_CONNECT"              \
                                                      : (X == QUOS_MQTT_OK_CONNECT)               ? "MQTT_OK_CONNECT"               \
                                                      : (X == QUOS_MQTT_UNNACCEPTABLE_PROTOCOL)   ? "MQTT_UNNACCEPTABLE_PROTOCOL"   \
                                                      : (X == QUOS_MQTT_CLIENTID_REJECTED)        ? "MQTT_CLIENTID_REJECTED"        \
                                                      : (X == QUOS_MQTT_SERVER_UNAVAILABLE)       ? "MQTT_SERVER_UNAVAILABLE"       \
                                                      : (X == QUOS_MQTT_BAD_USERNAME_OR_PASSWORD) ? "MQTT_BAD_USERNAME_OR_PASSWORD" \
                                                      : (X == QUOS_MQTT_SERVER_UNAVAILABLE_BAK)   ? "MQTT_SERVER_UNAVAILABLE_BAK"   \
                                                      : (X == QUOS_MQTT_NOT_AUTHORIZED)           ? "MQTT_NOT_AUTHORIZED"           \
                                                      : (X == QUOS_MQTT_OK_SUBSCRIBE)             ? "MQTT_OK_SUBSCRIBE"             \
                                                                                                  : "Unknown")

typedef void (*MqttEventCb_f)(void *chlFd, qint32_t event);
typedef void (*MqttpublishRecv_f)(char *topicName, quint8_t *payload, quint32_t payloadlen);

qbool Quos_mqttInit(void **chlFdPoint,
                    const char *url,
                    const char *clientID,
                    const char *username,
                    const char *password,
                    quint16_t keepAlive,
                    quint8_t topicCount,
                    char *topicString[],
                    quint8_t requestedQoSs[],
                    MqttEventCb_f eventCB,
                    MqttpublishRecv_f pubRecv);
void Quos_mqttDeinit(void *chlFd);

qint32_t Quos_mqttPublish(const void *chlFd, char *topic, qint32_t qos, void *buf, quint16_t bufLen, socketRecvNodeCb_f recvCB, qbool isAck);
#endif/*SDK_ENABLE_MQTT */
#endif/*__QUOS_MQTT_H__ */
