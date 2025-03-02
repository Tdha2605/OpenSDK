/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#include <stdio.h>
#include "ql_rtos.h"
#include "ql_application.h"
#include "ql_adc.h"
#include "ql_power.h"
#include "ql_uart.h"
#include <string.h>

#define test_log(fmt, args...)	printf(fmt, ##args)

void quec_usbcdc_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	int read_len = 0;
	char r_data[1024] = {0};
	read_len = ql_uart_read(port, r_data, sizeof(r_data));
	test_log("uart[%d] callback read data, data:%s, read_len:%d \n", port, r_data, read_len);
	ql_uart_write(QL_USB_CDC_PORT, r_data, read_len);
}

static void quec_usbcdc_test(void * argv)
{
    int ret = -1;
    char r_data[128] = {0};
	int write_bytes = 0;
	unsigned char data[] = "hello, this is uart test!!!, abcdefghijklmnopqrstuvwxyz, 1234567890\n";
	ql_uart_config_t dcb;

    ql_uart_open(QL_USB_CDC_PORT, QL_UART_BAUD_115200, QL_FC_NONE);

    ql_uart_register_cb(QL_USB_CDC_PORT, quec_usbcdc_callback);	//use callback to read uart data
    
	while (1)
	{   
	    test_log("I still alive...%s\r\n",r_data);
        ql_uart_open(QL_USB_CDC_PORT, QL_UART_BAUD_115200, QL_FC_NONE);
        ret = ql_uart_write(QL_USB_CDC_PORT, data, sizeof(data));
		ql_rtos_task_sleep_s(5);
        ql_uart_close(QL_USB_CDC_PORT);	
	}    
}

/**********
pm802(guilin:adc0��adc1��vbat): 600S��600N_LD��100N_AB
pm813(ningbo:adc0��adc1��vbat): 100Y
pm803(adc0��vbat):               600N_LC��100N_AA
*************/
static void quec_adc_test(void * argv)
{
	unsigned char at_channel = 0,times=10,i=0;
	unsigned short adc_vol = 0;
    unsigned short batvol_sample[10]={0};

    unsigned short vbat_vol = 0;
    unsigned short vbat_sample[10]={0};
    unsigned char data[] = "hello, this is uart test!!!, abcdefghijklmnopqrstuvwxyz, 1234567890\n";
	ql_adc_init();
    ql_uart_config_t dcb;

    ql_uart_open(QL_USB_CDC_PORT, QL_UART_BAUD_115200, QL_FC_NONE);
    char mStr[128];
    ql_uart_register_cb(QL_USB_CDC_PORT, quec_usbcdc_callback);	//use callback to read uart data
    ql_uart_write(QL_USB_CDC_PORT, data, sizeof(data));
	while(1)
	{
	    times = 10;i=0;
	    while(times--)
	    {
	        ql_adc_onff(1);
            ql_rtos_task_sleep_ms(5);
            ql_adc_onff(0);
            
		    //adc_vol = ql_get_battery_vol();
            ql_adc_read(at_channel, &adc_vol);
            batvol_sample[i]=adc_vol;

            vbat_vol = ql_get_battery_vol();
            vbat_sample[i] = vbat_vol;
            
            i++;
        }
        {  //���˺�ƽ��adc0
            unsigned short max=batvol_sample[0], min=batvol_sample[0];
            unsigned short j=0, vol_all=0;
            for(j=1; j<10; j++){    //ȡ�����С??
                if(max < batvol_sample[j])
                    max=batvol_sample[j];

                if(min > batvol_sample[j])
                    min=batvol_sample[j];
            }
            
            for(j=0; j<10; j++){
                vol_all += batvol_sample[j];
            }

            vol_all = vol_all - min - max;  //ȥ�������С??
            adc_vol = vol_all/8;
        }   
        {   //���˺�ƽ��vbat
            unsigned short max=vbat_sample[0], min=vbat_sample[0];
            unsigned short j=0, vol_all=0;
            for(j=1; j<10; j++){    //ȡ�����С??
                if(max < vbat_sample[j])
                    max=vbat_sample[j];

                if(min > vbat_sample[j])
                    min=vbat_sample[j];
            }
            
            for(j=0; j<10; j++){
                vol_all += vbat_sample[j];
            }

            vol_all = vol_all - min - max;  //ȥ�������С??
            vbat_vol = vol_all/8;
        }   
        printf("Pham Tuan Kiet\n");
        sprintf(mStr,"adc0=%d mV  vbat=%d mV\n", adc_vol, vbat_vol);
        ql_uart_write(QL_USB_CDC_PORT, mStr, strlen(mStr));
		test_log(" adc0=%d mV  vbat=%d mV\n", adc_vol, vbat_vol);
		ql_rtos_task_sleep_s(1);
        
	}

}

application_init(quec_adc_test, "quec_adc_test", 2, 0);

