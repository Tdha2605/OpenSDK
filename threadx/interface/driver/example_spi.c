/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
#include <stdio.h>
#include <string.h>

#include "ql_type.h"
#include "ql_rtos.h"
#include "ql_application.h"
#include "ql_spi.h"



/* Fias-2022/04/08: (spi ql_spi_write_read)*/
/*Note: For this test you need to connect TX and RX of SPI*/
static void quec_spi_test(void * argv)
{
	printf("spi ql_spi_write_read test \r\n");

	int ret = -1;
	unsigned char buf_in[256] = {0};
	unsigned char buf_out[128] = "=========This is the spi DMA test !!===========\r\n";

	ql_spi_init(QL_SPI_PORT1, QL_SPI_MODE0,QL_SPI_CLK_500KHZ);
	//ql_rtos_task_sleep_s(1);

	while(1)
	{	
		printf("ql_spi_buf_out:  %s \r\n ",buf_out);	
		ret= ql_spi_write_read(QL_SPI_PORT1, buf_in, buf_out, sizeof(buf_out));
		if(ret != 0)
		{
			printf("ql_spi_read error, ret = %d \r\n",ret);
			break;
		}
		
		printf("ql_spi_buf_in:  %s \r\n",buf_in);

		ql_rtos_task_sleep_ms(500);
		
	}
}

//application_init(quec_spi_test, "quec_spi_test", 4, 20);

