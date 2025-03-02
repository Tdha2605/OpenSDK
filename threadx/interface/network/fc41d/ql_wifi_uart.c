/**
  @file
  ql_wifi_uart.c

  @brief
  Quectel wifi uart api.

*/
/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/

/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "quec_debug.h"
#include "ql_log.h"

#include "ql_rtos.h"

#include "ql_type.h"
#include "ql_uart.h"

#include "ql_wifi_net.h"
#include "ql_wifi_msg.h"
#include "ql_wifi_uart.h"
#include "ql_api_wifi.h"

#ifndef kNoErr
#define kNoErr          (0)
#endif
#ifndef kGeneralErr
#define kGeneralErr     (-1)
#endif

/*========================================================================
 *	Macro Definition
 *========================================================================*/


#define QL_WIFI_UART_LOG(msg, ...)   ql_app_log(msg, ##__VA_ARGS__)
//#define QL_WIFI_UART_LOG(msg, ...)     printf(msg, ##__VA_ARGS__)


#define QL_WIFI_UART_RX_BUFF_SIZE   2048
#define QL_WIFI_UART_TX_BUFF_SIZE   2048

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define QL_WIFI_UART_PORT      QL_MAIN_UART_PORT

#define MAX_DUMP_BUFFER_SIZE   256

/*========================================================================
 *	Enumeration Definition
 *========================================================================*/

/*========================================================================
 *  Type Definition
 *=========================================================================*/
typedef struct
{
	ql_wifi_uart_notify_cb notify_cb;
	ql_mutex_t write_mutex;
} ql_wifi_uart_manager_s;

/*========================================================================
 *   Global Variable
 *========================================================================*/
ql_wifi_uart_manager_s ql_wifi_uart_manager = {0};

/*========================================================================
 *	function Definition
 *========================================================================*/
static void wifi_dump_buffer(const char *name, char *buf, int len)
{

	int i, hexStringLen = 0, size = 0;
	char *outBuf = NULL ;
	/*assume the max name len is 32*/
	hexStringLen = MAX_DUMP_BUFFER_SIZE * 3 + MAX_DUMP_BUFFER_SIZE/16 + 32 + 32;
	outBuf = malloc(hexStringLen);
	if (outBuf == NULL){
		return;
	}
	memset(outBuf, 0, hexStringLen);
	size = snprintf(outBuf, hexStringLen, "[%s][%d]:", name, len);
	for(i = 0; i < MAX_DUMP_BUFFER_SIZE && i < len; i++)
	{
		size += snprintf(outBuf + size, hexStringLen - size, "%02x ", buf[i]);
	}
	size += snprintf(outBuf + size, hexStringLen - size, "\n");
	QL_WIFI_UART_LOG("%s", outBuf);
	
	if (outBuf){
		free(outBuf);
	}
}


unsigned char npto_CRC(void *src, int ilen)
{
	int i;
	unsigned char crc = 0, *ptr = (unsigned char *)src;

	for (i = 0; i < ilen; i++)
	{
		crc += ptr[i];
	}

	return crc;
}

void ql_wifi_uart_cb(QL_UART_PORT_NUMBER_E port, void *para)
{
	unsigned int size = 0;
	unsigned char *recv_buff = 0;
	unsigned int real_size = 0;
	int read_len = 0;
	ql_wifi_uart_manager_s *manager = &ql_wifi_uart_manager;
	
	size = *(unsigned int *)(para);
	QL_WIFI_UART_LOG("ql_wifi_uart_cb size =%d", size);

	while (size > 0)
	{
		real_size = MIN(size, QL_WIFI_UART_RX_BUFF_SIZE);
		recv_buff = malloc(real_size + 1);
		if (recv_buff == NULL){
			QL_WIFI_UART_LOG("malloc failed");
			return;
		}
		read_len = ql_uart_read(port, recv_buff, real_size);
		QL_WIFI_UART_LOG("read_len=%d, recv_data=%p", read_len, recv_buff);
		wifi_dump_buffer("UR", recv_buff, read_len);
		
		if (manager->notify_cb != NULL){
			ql_wifi_msg_info_s info = {.data = recv_buff,.data_len = read_len};
			manager->notify_cb(&info);
		}else{
			free(recv_buff);
			recv_buff = NULL;
		}
	
		if ((read_len > 0) && (size >= read_len)){
			size -= read_len;
		}else{
			break;
		}
	}

}

int ql_wifi_uart_write(char *data, int data_len)
{
	int write_len = 0;
	ql_wifi_uart_manager_s *manager = &ql_wifi_uart_manager;

	if (manager->write_mutex == NULL){
		return 0;
	}
	
	QL_WIFI_UART_LOG("uart write enter\r\n");
	ql_rtos_mutex_lock(manager->write_mutex, QL_WAIT_FOREVER);
	write_len = ql_uart_write(QL_WIFI_UART_PORT, (unsigned char *)data, data_len);
	QL_WIFI_UART_LOG("write_len:%d", write_len);
	wifi_dump_buffer("US",data,data_len);
	ql_rtos_mutex_unlock(manager->write_mutex);
	QL_WIFI_UART_LOG("uart write level\r\n");
	
	return data_len;
}

ql_wifi_uart_errcode_e ql_wifi_uart_notify_cb_register(ql_wifi_uart_notify_cb cb)
{
	ql_wifi_uart_manager_s *manager = &ql_wifi_uart_manager;
	manager->notify_cb = cb;
	return QL_WIFI_UART_SUCCESS;
}

ql_wifi_uart_errcode_e ql_wifi_uart_init(void *argv)
{
	int err = 0;
	ql_wifi_uart_errcode_e ret = QL_WIFI_UART_SUCCESS;
	ql_wifi_uart_manager_s *manager = &ql_wifi_uart_manager;
	ql_uart_config_t uart_cfg = {0};
	
	QL_WIFI_UART_LOG("ql_wifi_uart_init");
	uart_cfg.baudrate   = QL_UART_BAUD_921600;
	uart_cfg.flow_ctrl  = QL_FC_NONE;
	uart_cfg.data_bit   = QL_UART_DATABIT_8;
	uart_cfg.stop_bit   = QL_UART_STOP_1;
	uart_cfg.parity_bit = QL_UART_PARITY_NONE;

	memset(manager, 0, sizeof(ql_wifi_uart_manager_s));
	err = ql_rtos_mutex_create(&(manager->write_mutex));
	if (kNoErr != err){
		ret = QL_WIFI_UART_MUTEX_CREATE_ERR;
		goto exit;
	}

	err = ql_uart_set_dcbconfig(QL_WIFI_UART_PORT, &uart_cfg);
	QL_WIFI_UART_LOG("err: 0x%x", err);
	if (QL_UART_SUCCESS != err){
		ret = QL_WIFI_UART_PORT_SET_ERR;
		goto exit;
	}

	err = ql_uart_open(QL_WIFI_UART_PORT, uart_cfg.baudrate, uart_cfg.flow_ctrl);
	QL_WIFI_UART_LOG("err: 0x%x", err);

	if (QL_UART_SUCCESS == err)
	{
		ql_uart_register_cb(QL_WIFI_UART_PORT, ql_wifi_uart_cb);
		memset(&uart_cfg, 0, sizeof(ql_uart_config_t));
		err = ql_uart_get_dcbconfig(QL_WIFI_UART_PORT, &uart_cfg);
		QL_WIFI_UART_LOG("err: 0x%x, baudrate=%d, flow_ctrl=%d, data_bit=%d, stop_bit=%d, parity_bit=%d",
				err, uart_cfg.baudrate, uart_cfg.flow_ctrl, uart_cfg.data_bit, uart_cfg.stop_bit, uart_cfg.parity_bit);
	}
	else
	{
		ret = QL_WIFI_UART_OPEN_ERR;
		goto exit;
	}
exit:
	if (ret != QL_WIFI_UART_SUCCESS)
	{
		ql_wifi_uart_deinit(NULL);
		return ret;
	}
	return QL_WIFI_UART_SUCCESS;
}

ql_wifi_uart_errcode_e ql_wifi_uart_deinit(void *argv)
{
	ql_wifi_uart_manager_s *manager = &ql_wifi_uart_manager;
	ql_wifi_uart_manager_s cmp = {0};
	
	if (0 != memcmp(manager, &cmp, sizeof(ql_wifi_uart_manager_s)))
	{
		ql_uart_close(QL_WIFI_UART_PORT);
		ql_rtos_mutex_delete(manager->write_mutex);
		memset(manager, 0, sizeof(ql_wifi_uart_manager_s));
	}
	else
	{
		QL_WIFI_UART_LOG("wifi uart repeat deinit");
	}
	
	return QL_WIFI_UART_SUCCESS;
}

// SIO output data msg
int ql_wifi_uart_output_data_msg(void *ctx)
{
	ql_wifi_msg_output_data_info_s *info = (ql_wifi_msg_output_data_info_s*)ctx;
	uint8_t *data = info->data;
	int data_len = info->data_len;
	int cmd = info->cmd;

	ql_wifi_msg_protocol_head_info_s head_info = {0};
	char *output_data = NULL;
	unsigned char checksum;

	memset(&head_info, 0, sizeof(ql_wifi_msg_protocol_head_info_s));

	output_data = malloc(data_len + QUEC_PROTOCOL_HEAD_MAX_SIZE + 1);
	if (output_data == NULL){
		return 0;
	}

	head_info.magic0 = QL_WIFI_UART_MAGIC0;
	head_info.magic1 = QL_WIFI_UART_MAGIC1;
	head_info.command = cmd;
	head_info.cmdlen = data_len + QUEC_PROTOCOL_HEAD_MAX_SIZE;

	memcpy(output_data, &head_info, QUEC_PROTOCOL_HEAD_MAX_SIZE);
	memcpy(output_data + QUEC_PROTOCOL_HEAD_MAX_SIZE, data, data_len);

	checksum = npto_CRC(output_data, QUEC_PROTOCOL_HEAD_MAX_SIZE + data_len);
	output_data[QUEC_PROTOCOL_HEAD_MAX_SIZE - 1] = checksum;

	ql_wifi_uart_write(output_data, QUEC_PROTOCOL_HEAD_MAX_SIZE + data_len);
	QL_WIFI_UART_LOG("data_len=%d cmd=%d", data_len, cmd);

	if (output_data != NULL){
		free(output_data);
		output_data = NULL;
	}

	return 0;
}
