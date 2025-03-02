/**
  @file
  ql_wifi_spi.c

  @brief
  Quectel wifi spi api.

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

#include "ql_log.h"
#include "ql_rtos.h"

//#include "quec_common.h"

#include "ql_gpio.h"
#include "ql_spi.h"
#include "ql_api_wifi.h"

#include "ql_wifi_msg.h"
#include "ql_wifi_net.h"
#include "ql_wifi_spi.h"
#include "ql_wifi_uart.h"

#include "lwipopts.h"


#define QL_OSI_SUCCESS            kNoErr
#define QL_SPI_SUCCESS            kNoErr

#define SWIFI_SPI_PRIORITY        60

/*========================================================================
 *	Macro Definition
 *========================================================================*/

#define QL_WIFI_SPI_LOG(msg, ...) ql_app_log(msg, ##__VA_ARGS__)
//#define QL_WIFI_SPI_LOG(msg, ...) printf(msg, ##__VA_ARGS__)

#define QL_WIFI_SPI_TASK_EVENT_CNT           5

#define QL_WIFI_SPI_MAGIC0                   0xAA
#define QL_WIFI_SPI_MAGIC1                   0x55
#define QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE   9
#define QL_WIFI_SPI_DATA_LEN                 1600

#if 0
#define QL_WIFI_SPI_PORT QL_SPI_PORT1      // EC600M OPNE DKB + WIFI FC41D DKB
#define QL_SPI_Tx_GPIO   GPIO_PIN_NO_18    // 临时:    P60  客户需自定义PIN
#define QL_SPI_Rx_GPIO   GPIO_PIN_NO_19    // 临时:    P59   客户需自定义PIN
#define QL_SPI_Cs_GPIO   GPIO_PIN_NO_34    // CS PIN GPIO :SSPX_FRM  CS 引脚选择
//#define QL_SPI_In_GPIO   GPIO_PIN_NO_x   // SPI 发送防冲突检测: 预留 注意设计硬件需要预留PIN 软件暂时未使用
#else
#define QL_WIFI_SPI_PORT QL_SPI_PORT2    // GPIO4-7 //CUST
#define QL_SPI_Tx_GPIO   GPIO_PIN_NO_2   // 客户需自定义PIN AUX 
#define QL_SPI_Rx_GPIO   GPIO_PIN_NO_3   // 客户需自定义PIN AUX 
#define QL_SPI_Cs_GPIO   GPIO_PIN_NO_5   // CS PIN GPIO :SSPX_FRM 
//#define QL_SPI_In_GPIO   GPIO_PIN_NO_x // SPI 发送防冲突检测:预留

#endif

/*========================================================================
 *	Enumeration Definition
 *========================================================================*/

/*========================================================================
 *  Type Definition
 *=========================================================================*/
typedef struct
{
	ql_task_t recv_task;
	ql_sem_t recv_sem;
	//ql_mutex_t recv_mutex;
	unsigned char *recv_data;
	ql_mutex_t spi_io_mutex;
} ql_wifi_spi_manager_s;

typedef struct
{
	unsigned char header[2];
	unsigned short int data_len;
	unsigned short int seq_num;
	unsigned short int reserved;
	unsigned char checksum;
} ql_wifi_spi_data_header_s;

/*========================================================================
 *   Global Variable
 *========================================================================*/
ql_wifi_spi_manager_s ql_wifi_spi_manager = {0};

/*========================================================================
 *	function Definition
 *========================================================================*/
static void spi_delay_us(unsigned int time)
{
	volatile long i = 0;
	i = time * 50;
	while (i > 0){
		i--;
	}
}

static void _gpioint_callback(void *ctx)
{
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	//PIN_LEVEL_E gpio_lvl = PIN_LEVEL_HIGH;
	//ql_gpio_get_level(QL_SPI_Rx_GPIO, &gpio_lvl);
	QL_WIFI_SPI_LOG("G[%d]", QL_SPI_Rx_GPIO);
	if(manager->recv_sem){
		ql_rtos_semaphore_release(manager->recv_sem);
	}
}

int ql_wifi_spi_gpio_init(void)
{
	int ret;

	ql_gpio_deinit(QL_SPI_Tx_GPIO);
	ql_gpio_deinit(QL_SPI_Rx_GPIO);
	ql_gpio_deinit(QL_SPI_Cs_GPIO);
	
	//spi Tx ctrl pin
	ret = ql_gpio_init(QL_SPI_Tx_GPIO, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_HIGH);
	if (ret != 0){
		QL_WIFI_SPI_LOG("spi gpio tx init err! ");
		return ret;
	}
	//spi cs pin
	ret = ql_gpio_init(QL_SPI_Cs_GPIO, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_HIGH);
	if (ret != 0){
		QL_WIFI_SPI_LOG("spi gpio cs init err! ");
		return ret;
	}
	
	//spi Rx ctrl pin
	ret = ql_eint_register(QL_SPI_Rx_GPIO, PIN_FALLING_EDGE, PIN_PULL_PU, NULL, _gpioint_callback);
	if (ret) {
		QL_WIFI_SPI_LOG("spi gpio rx init err! ");
		return ret;
	}
	ql_eint_enable_wakeup(QL_SPI_Rx_GPIO, PIN_FALLING_EDGE);

	return ret;
}


void ql_wifi_spi_gpio_deinit(void)
{
	ql_eint_disable(QL_SPI_Rx_GPIO);
	ql_eint_unregister(QL_SPI_Rx_GPIO);
	ql_gpio_deinit(QL_SPI_Rx_GPIO);
	ql_gpio_deinit(QL_SPI_Tx_GPIO);
	ql_gpio_deinit(QL_SPI_Cs_GPIO);

}


int ql_wifi_spi_read(unsigned char *data, unsigned short inlen)
{
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	unsigned char *indata = NULL;
	int ret;

	if (data == NULL || inlen <= 0){
		return kGeneralErr;
	}

	indata = (unsigned char *)malloc(inlen);
	if (indata == NULL){
		QL_WIFI_SPI_LOG("spi read malloc err");
		return kGeneralErr;
	}

	//QL_WIFI_SPI_LOG("spi read enter \r\n");
	//spi_delay_us(5);
	memset(indata, 0, inlen);
	//ql_rtos_mutex_lock(manager->recv_mutex,QL_WAIT_FOREVER);
	ql_rtos_mutex_lock(manager->spi_io_mutex, QL_WAIT_FOREVER);
	ql_gpio_set_level(QL_SPI_Cs_GPIO, PIN_LEVEL_LOW);
	ret = ql_spi_read(QL_WIFI_SPI_PORT, indata, inlen);
	ql_gpio_set_level(QL_SPI_Cs_GPIO, PIN_LEVEL_HIGH);
	ql_rtos_mutex_unlock(manager->spi_io_mutex);
	//ql_rtos_mutex_unlock(manager->recv_mutex);
	if (ret == QL_SPI_SUCCESS){
		memcpy(data, indata, inlen);
	}else{
		QL_WIFI_SPI_LOG("spi read ret = %d \n", ret);
	}
	
	//QL_WIFI_SPI_LOG("spi read level \r\n");

	free(indata);
	return ret;
}

int ql_wifi_spi_write(unsigned char *data, unsigned short outlen)
{
	unsigned char *outdata = NULL;
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;

	int ret;

	if (data == NULL || outlen <= 0){
		return kGeneralErr;
	}
	outdata = (unsigned char *)malloc(outlen);
	if (outdata == NULL){
		QL_WIFI_SPI_LOG("spi write malloc err");
		return kGeneralErr;
	}
	//QL_WIFI_SPI_LOG("spi write enter \r\n");
	memcpy(outdata, data, outlen);
	ql_rtos_mutex_lock(manager->spi_io_mutex, QL_WAIT_FOREVER);
	ql_gpio_set_level(QL_SPI_Cs_GPIO, PIN_LEVEL_LOW);
	ret = ql_spi_write(QL_WIFI_SPI_PORT, outdata, outlen);
	ql_gpio_set_level(QL_SPI_Cs_GPIO, PIN_LEVEL_HIGH);
	ql_rtos_mutex_unlock(manager->spi_io_mutex);
	if (ret != QL_SPI_SUCCESS){
		QL_WIFI_SPI_LOG("spi write ret = %d \n", ret);
	}
	//QL_WIFI_SPI_LOG("spi write level \r\n");
	free(outdata);
	return ret;
}

int ql_wifi_spi_port_init(void)
{
	int ret = kNoErr;
	ret  = ql_spi_init(QL_WIFI_SPI_PORT, QL_SPI_MODE0, QL_SPI_CLK_6_5MHZ);
	return ret;
}

void ql_wifi_spi_port_deinit(void)
{
	//ql_spi_release(QL_WIFI_SPI_PORT);
}

// spi output data msg
int ql_wifi_spi_output_data_msg(void *ctx)
{
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	ql_wifi_msg_info_s *info = (ql_wifi_msg_info_s*)ctx;
	uint8_t *data = info->data;
	int data_len = info->data_len;

	unsigned char *output_data = NULL;
	unsigned char checksum;
	ql_wifi_spi_data_header_s ql_wifi_spi_data;
	
	output_data = malloc(QL_WIFI_SPI_DATA_LEN);
	if (output_data == NULL){
		return -1;
	}
	
	memset(output_data, 0, QL_WIFI_SPI_DATA_LEN);
	ql_wifi_spi_data.header[0] = QL_WIFI_SPI_MAGIC0;
	ql_wifi_spi_data.header[1] = QL_WIFI_SPI_MAGIC1;
	ql_wifi_spi_data.data_len = (unsigned short int)data_len;
	ql_wifi_spi_data.seq_num  = 0;
	ql_wifi_spi_data.reserved = 0;
	ql_wifi_spi_data.checksum = 0;

	memcpy(output_data, &ql_wifi_spi_data, QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE);
	memcpy(output_data + QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE, data, data_len);

	checksum = npto_CRC(output_data, QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE + data_len);
	output_data[QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE - 1] = checksum;
	//wifi_dump_buffer("SSEND",output_data, QL_WIFI_SPI_DATA_LEN);
	
	ql_gpio_set_level(QL_SPI_Tx_GPIO, PIN_LEVEL_LOW);
	spi_delay_us(150);
	QL_WIFI_SPI_LOG("spi send len=%d", data_len);
	ql_wifi_spi_write(output_data, QL_WIFI_SPI_DATA_LEN);
	spi_delay_us(10);
	ql_gpio_set_level(QL_SPI_Tx_GPIO, PIN_LEVEL_HIGH);

	if (output_data != NULL){
		free(output_data);
		output_data = NULL;
	}

	return 0;
}


int ql_wifi_spi_msg_parse(unsigned char *src, unsigned int src_len, unsigned char **dst, unsigned int *dst_len)
{

	unsigned char *ptr = src;
	unsigned char checksum;
	ql_wifi_spi_data_header_s ql_wifi_spi_data;
	// data protocol parse 
	ptr++; 
	memcpy(&ql_wifi_spi_data, ptr, QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE);
	if (ql_wifi_spi_data.header[0] != QL_WIFI_SPI_MAGIC0 || ql_wifi_spi_data.header[1] != QL_WIFI_SPI_MAGIC1 || ql_wifi_spi_data.data_len <= 0){
		QL_WIFI_SPI_LOG("header[0]= %x header[1]= %x data len =%d", ql_wifi_spi_data.header[0], ql_wifi_spi_data.header[1], ql_wifi_spi_data.data_len);
		return -1;
	}
	ptr[QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE - 1] = 0;         // set checksum zero for recalc
	checksum = npto_CRC(ptr, ql_wifi_spi_data.data_len + QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE);
	if (checksum != ql_wifi_spi_data.checksum){
		QL_WIFI_SPI_LOG("crc check error checksum=%x ql_wifi_spi_data.checksum=%x", checksum, ql_wifi_spi_data.checksum);
		return -2;
	}
	*dst_len = ql_wifi_spi_data.data_len;
	*dst = &ptr[QL_WIFI_SPI_PROTOCOL_HEAD_MAX_SIZE];
	return 0;
}

void ql_wifi_spi_thread(void *argv)
{
	int err = 0;
	ql_wifi_msg_info_s msg_ptr;
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	manager->recv_data = malloc(QL_WIFI_SPI_DATA_LEN);
	
	if (manager->recv_data == NULL){
		QL_WIFI_SPI_LOG("spi recv_data malloc failed");
		goto exit;
	}

	err = ql_wifi_spi_port_init();
	if (err != QL_OSI_SUCCESS){
		QL_WIFI_SPI_LOG("spi init failed");
		goto exit;
	}
	

	err = ql_wifi_spi_gpio_init();
	if (err != QL_OSI_SUCCESS){
		QL_WIFI_SPI_LOG(" gpio init failed");
		goto exit;
	}


	err = ql_rtos_semaphore_create(&manager->recv_sem, 1);
	if (err != QL_OSI_SUCCESS){
		QL_WIFI_SPI_LOG("spi sem created failed");
		goto exit;
	}

	err = ql_rtos_mutex_create(&manager->spi_io_mutex);
	if( err != QL_OSI_SUCCESS ){
		QL_WIFI_SPI_LOG("spi  mutex created failed");
		goto exit;
	}

	while (1)
	{
		memset(manager->recv_data, 0, QL_WIFI_SPI_DATA_LEN);
		ql_rtos_semaphore_wait(manager->recv_sem, QL_WAIT_FOREVER);
		ql_eint_enable_wakeup( QL_SPI_Rx_GPIO, PIN_FALLING_EDGE);
		if (ql_wifi_spi_read(manager->recv_data, QL_WIFI_SPI_DATA_LEN) == 0)
		{
			QL_WIFI_SPI_LOG("spi  read success");
			if (ql_wifi_spi_msg_parse(manager->recv_data, QL_WIFI_SPI_DATA_LEN, &msg_ptr.data, &msg_ptr.data_len) == 0){
				QL_WIFI_SPI_LOG("spi data parse success");
				ql_wifi_data_input(msg_ptr.data,msg_ptr.data_len);
			}else{
				QL_WIFI_SPI_LOG("spi msg_parse failed");
			}
		}else{
			QL_WIFI_SPI_LOG("spi read failed");
		}
	}

exit:
	ql_rtos_task_delete(NULL);
	return;
}

ql_wifi_spi_errcode_e ql_wifi_spi_init(void *argv)
{
	ql_wifi_spi_errcode_e ret = QL_WIFI_SPI_SUCCESS;
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	QL_WIFI_SPI_LOG("ql_wifi_spi_init");
	if(manager->recv_task)
	{
		return QL_WIFI_SPI_SUCCESS;
	}
	if(QL_OSI_SUCCESS != ql_rtos_task_create(&manager->recv_task, 4096, SWIFI_SPI_PRIORITY, "ql_wspi", ql_wifi_spi_thread, NULL, QL_WIFI_SPI_TASK_EVENT_CNT))
	{
		QL_WIFI_SPI_LOG("ql_wifi_spi_init err");
		ret = QL_WIFI_SPI_TASK_CREATE_ERR;
		return ret;
	}
	return ret;
}

ql_wifi_spi_errcode_e ql_wifi_spi_deinit(void *argv)
{
	ql_wifi_spi_manager_s *manager = &ql_wifi_spi_manager;
	ql_wifi_spi_manager_s cmp = {0};
	
	if(0 != memcpy(manager,&cmp,sizeof(ql_wifi_spi_manager_s)))
	{
		QL_WIFI_SPI_LOG("ql_wifi_spi_deinit");
		ql_wifi_spi_port_deinit();
		ql_wifi_spi_gpio_deinit();

		if (manager->recv_task)
		{
			ql_rtos_task_delete(manager->recv_task);
			manager->recv_task = NULL;
		}

		if (manager->recv_data)
		{
			free(manager->recv_data);
			manager->recv_data = NULL;
		}

		if(manager->spi_io_mutex){
			ql_rtos_mutex_unlock(manager->spi_io_mutex);
			ql_rtos_mutex_delete(manager->spi_io_mutex);
			manager->spi_io_mutex = NULL;
		}

		if (manager->recv_sem)
		{
			ql_rtos_semaphore_delete(manager->recv_sem);
			manager->recv_sem = NULL;
		}
		memset(manager,0,sizeof(ql_wifi_spi_manager_s));
	}
	else
	{
		QL_WIFI_SPI_LOG("wifi spi repeat deinit");
	}
	
	return QL_WIFI_SPI_SUCCESS;
}
