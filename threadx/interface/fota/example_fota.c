/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/


#include <stdio.h>
#include "ql_rtos.h"
#include "ql_application.h"
#include "ql_fota.h"
#include "ql_power.h"
#include "ql_fs.h"
#include "ql_uart.h"
#include "ql_spi_nor.h"

#define fota_exam_log(fmt, ...) printf("[FOTA_EXAM][%s, %d] "fmt"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#define QUEC_EXTERNAL_NORFLASH_TEST
static ql_flag_t fota_uart_flag;
#define QL_FOTA_UART_READ 0x1


void quec_fota_uart_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	static char read_flag = 1;
	if(read_flag){
		read_flag = 0;
		ql_rtos_flag_release(fota_uart_flag, QL_FOTA_UART_READ, QL_FLAG_OR);
	}
}

void ql_fota_test(void)
{
		int ret = -1,event = 0;
		int write_bytes = 0;
		unsigned char data[] = "hello, this is file fota test!!!, please Transfer dfota.bin\n";
		unsigned char read_buf[4096] = {0};
		ql_uart_config_t dcb;
		int count = 0, res = 0,file_len = 0;
		QFILE *fp = NULL;
		qlFotaImgProcCtxPtr ctx = NULL;
		int file_size = 0;
#ifdef QUEC_EXTERNAL_NORFLASH_TEST
		unsigned char *id;
		int port_index = EXTERNAL_NORFLASH_PORT33_36;
		int clk = _APBC_SSP_FNCLKSEL_1_625MHZ_;
		
		printf("[FS] ========== exflash init  \r\n");
		ql_spi_nor_init(port_index, clk);	
	
		id = ql_spi_nor_read_id(port_index);
		printf("[NORFLASH]=====norflash ID:0x%02x 0x%02x 0x%02x\r\n", id[0], id[1], id[2]);
	
		ret = qextfs_init('C', "external_fs", 1, port_index, 0, 0x100000);
		printf("[FS] ========== exfs init : %d	\r\n", ret);
		
		ql_fota_set_package_path("C:/FotaFile.bin");
		ql_remove("C:/FotaFile.bin");
#endif	
		ret = ql_uart_open(QL_MAIN_UART_PORT, QL_UART_BAUD_115200, QL_FC_HW);
		if (ret) {
			printf("open uart[%d] failed! \n", QL_MAIN_UART_PORT);
			return;
		}
		
		ql_rtos_flag_create(&fota_uart_flag);
		/*****************************************/
		ql_uart_get_dcbconfig(QL_MAIN_UART_PORT, &dcb);
		printf("[%s][%d]baudrate:%d, data_bit:%d, stop_bit:%d, parity_bit:%d, flow_ctrl:%d \r\n", __func__, __LINE__, dcb.baudrate, dcb.data_bit, dcb.stop_bit, dcb.parity_bit, dcb.flow_ctrl);
		dcb.baudrate = QL_UART_BAUD_115200;
		ql_uart_set_dcbconfig(QL_MAIN_UART_PORT, &dcb);
		/*****************************************/
	
		ql_uart_register_cb(QL_MAIN_UART_PORT, quec_fota_uart_callback);	//use callback to read uart data
	
	
		ql_uart_write(QL_MAIN_UART_PORT, data, sizeof(data));
		ql_rtos_flag_wait(fota_uart_flag, QL_FOTA_UART_READ, QL_FLAG_OR, &event, QL_WAIT_FOREVER);
	
		ctx = ql_fota_init();
		if(!ctx)
		{
			printf("*** fota init fail ***");
			return;
		}
		
		count = 0;
		while((ret >= 0) && (count < 60)){
			ret = ql_uart_read(QL_MAIN_UART_PORT, read_buf, sizeof(read_buf));
			if(ret <= 0){
				count++;
				ql_rtos_task_sleep_ms(5);
				continue;
			}else{
				count = 0;
				file_len += ret;
				res = ql_fota_image_write(ctx, (void *)read_buf, ret);
				if(res < 0){
					printf("ql_fwrite error:%d \n",res);
				}
			}
		}
	
		if(count >= 60){
			printf("Transfer end:%d ,file_len:%d\n",ret, file_len);
		}
		ql_uart_close(QL_MAIN_UART_PORT);
	
		ret = ql_fota_image_flush(ctx);
		if(ret)
		{
			printf("*** fota image flush fail ***");
			return;
		}
		printf("ql_fota_image_flush end\n");
		ret = ql_fota_image_verify(ctx);
		if(ret)
		{
			printf("*** fota image verify fail ***");
			return;
		}
		printf("ql_fota_image_verify success! \n");
	
	
		printf("ready to power reset!!\n");
		ql_power_reset();

}

#define FOTA_SERVER_ADDRESS_STEP "http://123.60.181.47/app.bin"
#define FOTA_APN "uninet"
#define FOTA_USERNAME ""
#define FOTA_PASSWORD ""
void ql_full_app_test(void)
{
    int ret = 0, app_type = -1;
    char resp[64] ={0};
    char para[8]={0};
	QL_FULLAPP_APNS_UNAME_UPWD_T  *fullapp_apn_info=calloc(1,sizeof(QL_FULLAPP_APNS_UNAME_UPWD_T));

	ql_rtos_task_sleep_s(5);
	// fota_exam_log("========== full fota test success !!!!!!!!!!!!!!!!!!!! ==========");
	fota_exam_log("==========full fota test start ==========");

    memcpy(fullapp_apn_info->apn, FOTA_APN, strlen(FOTA_APN));
    memcpy(fullapp_apn_info->username, FOTA_USERNAME, strlen(FOTA_USERNAME));
    memcpy(fullapp_apn_info->password, FOTA_PASSWORD, strlen(FOTA_PASSWORD));

	ret = ql_fullfota1_app_ext_qiact_with_apn(1, fullapp_apn_info);
	fota_exam_log("ql_fullfota1_app_ext_qiact_with_apn()=%d\r\n",ret);
	fota_exam_log("%s: apn=%s, username=%s, password=%s\r\n",__FUNCTION__, fullapp_apn_info->apn, \
		fullapp_apn_info->username, fullapp_apn_info->password);
	ret = ql_fullfota_app_start(FOTA_SERVER_ADDRESS_STEP);
	if(ret != 0){
		fota_exam_log("==========full fota test end ==========");
	}
}

//application_init(ql_fota_test, "quec_fota_test", 50, 10);
//application_init(ql_full_app_test, "ql_full_app_test", 2, 10);


