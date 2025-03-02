#ifndef __QUOS_HTTP_H__
#define __QUOS_HTTP_H__
#include "quos_config.h"
#if (SDK_ENABLE_HTTP == 1)
#define QUOS_HTTP_MULTIPART_BOUNDARY              "450d2e46-73fc11eaad264b91df3ae910"
#define QUOS_HTTP_MULTIPART_NODE_START            "--" QUOS_HTTP_MULTIPART_BOUNDARY "\r\n"
#define QUOS_HTTP_MULTIPART_NODE_END              "\r\n--" QUOS_HTTP_MULTIPART_BOUNDARY "--\r\n"

#define QUOS_HTTP_HEAD_CONTENT_LENGHT             "Content-Length: %u\r\n"

#define QUOS_HTTP_CONTENT_TYPE_KEY                "Content-Type: "
#define QUOS_HTTP_CONTENT_TYPE_VALUE_JSON         "application/json\r\n"
#define QUOS_HTTP_CONTENT_TYPE_VALUE_MULTIPART    "multipart/form-data; boundary=" QUOS_HTTP_MULTIPART_BOUNDARY "\r\n"
#define QUOS_HTTP_CONTENT_TYPE_VALUE_OCTET_STREAM "application/octet-stream\r\n"

#define QUOS_HTTP_CONTENT_DISPOSITION_KEY         "Content-Disposition: "

enum
{
    QUOS_HTTP_CODE_ERR_DATA = -255,
    QUOS_HTTP_CODE_ERR_RAM,
    QUOS_HTTP_CODE_ERR_NET,
};

typedef struct
{
    char *rawHeaders; /* 每项以"\r\n"结束 */
    char *payload;
    quint16_t payloadLen;
} HttpReqData_t;

typedef struct
{
    char *txName;
    char *rxName;
    quint32_t size;
    quint32_t offset;
    qbool isPostForm;
} HttpReqFile_t;

/* 在非下载文件时，只有完成整个HTTP才CB一次
   在下载文件时，下载提取到header数据时先CB一次，recvLen=0,
   根据CB返回结果判断是否继续，为TRUE时则继续下载文件，
   HTTP结束后再CB一次，此时recvLen为下载到的文件大小 */
typedef qbool (*httpEventCB_f)(qint32_t httpCode,const char *retHeader,const quint8_t *recvBuf, quint32_t recvLen);

qbool Quos_httpRequest(void **httpFd, const char *opt, const char *url, httpEventCB_f eventCB, const HttpReqData_t *reqData, const HttpReqFile_t *reqFile);
qbool Quos_httpGetDownload(void **httpFd, const char *url, httpEventCB_f eventCB, const HttpReqData_t *reqData, const char *filename, quint32_t offset);
qbool Quos_httpPostForm(void **httpFd, const char *url, httpEventCB_f eventCB, const HttpReqData_t *reqData, const char *filename, quint32_t fileSize);
qbool Quos_httpFileRate(const void *httpFd, quint32_t *bodyLen, qint32_t *bodyType, char **retHeader);

#define Quos_httpGet(URL, EVENTCB, REQDATA) Quos_httpRequest(NULL, "GET", URL, EVENTCB, REQDATA, NULL)
#define Quos_httpPost(URL, EVENTCB, REQDATA) Quos_httpRequest(NULL, "POST", URL, EVENTCB, REQDATA, NULL)
#define Quos_httpPut(URL, EVENTCB, REQDATA) Quos_httpRequest(NULL, "PUT", URL, EVENTCB, REQDATA, NULL)
#define Quos_httpDelete(URL, EVENTCB, REQDATA) Quos_httpRequest(NULL, "DELETE", URL, EVENTCB, REQDATA, NULL)
#endif/*SDK_ENABLE_HTTP */
#endif/*__QUOS_HTTP_H__ */
