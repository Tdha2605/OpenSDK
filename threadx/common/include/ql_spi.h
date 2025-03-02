/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#ifndef _QL_SPI_H_
#define _QL_SPI_H_
	 
#ifdef __cplusplus
	 extern "C" {
#endif

typedef enum QL_SPI_CLK_ENUM
{	
	QL_SPI_CLK_INVALID=-1,
	QL_SPI_CLK_100KHZ=0,
	QL_SPI_CLK_200KHZ,
	QL_SPI_CLK_300KHZ,
	QL_SPI_CLK_400KHZ,
	QL_SPI_CLK_500KHZ,
	QL_SPI_CLK_600KHZ,
	QL_SPI_CLK_700KHZ,
	QL_SPI_CLK_812_5KHZ,
	QL_SPI_CLK_1_625MHZ,
	QL_SPI_CLK_3_25MHZ,
	QL_SPI_CLK_6_5MHZ,
	QL_SPI_CLK_13MHZ,
	QL_SPI_CLK_26MHZ,
	QL_SPI_CLK_52MHZ,
	QL_SPI_CLK_MAX,
}QL_SPI_CLK_E;

typedef enum QL_SPI_MODE_ENUM
{	
	QL_SPI_MODE_INVALID=-1,
	QL_SPI_MODE0=0,
	QL_SPI_MODE1,
	QL_SPI_MODE2,
	QL_SPI_MODE3,
	QL_SPI_MODE_MAX,
}QL_SPI_MODE_E;

typedef enum QL_SPI_PORT_ENUM
{
	QL_SPI_PORT_INVALID = -1,
	QL_SPI_PORT0 = 0,	//GPIO16-GPIO19
	QL_SPI_PORT1 = 1,	//GPIO33-GPIO36
	QL_SPI_PORT2 = 2,	//GPIO4-GPIO7
	QL_SPI_PORT_MAX,
}QL_SPI_PORT_E;
	
typedef enum QL_SPI_OpMode_ENUM        /* The Operation Mode that the SPI could work */
{
	QL_SPI_OP_PIO=0,              /* PIO mode */
    QL_SPI_OP_DMA                 /* DMA mode */
}QL_SPI_OpMode;

/*****************************************************************
* Function: ql_spi_init
* Description: SPI_INIT
*
* Parameters:
* 		 port	  		[in] 	SPI引脚选择SS0\SS2。 
* 		spimode	  		[in] 	控制器模式：QL_SPI_MODE0,QL_SPI_MODE1,QL_SPI_MODE2,QL_SPI_MODE3,
*		spiclk			[in] 	传输时钟
* Return:
* 	0			成功。
*	-1 			失败。
*****************************************************************/	
int ql_spi_init(QL_SPI_PORT_E port, QL_SPI_MODE_E spimode, QL_SPI_CLK_E spiclk);

/*****************************************************************
* Function: ql_spi_write
* Description:SPI写数据
*
* Parameters:
* 		port	  		[in] 	SPI引脚选择SS0\SS2。 
* 		buf	  			[in] 	存放写入的数据
*		len				[in] 	写数据的长度
* Return:
* 	0			成功。
*	-1 			失败。
*****************************************************************/
int ql_spi_write(QL_SPI_PORT_E port, unsigned char *buf, unsigned int len);

/*****************************************************************
* Function: ql_spi_read
* Description:SPI读数据
*
* Parameters:
* 		port	  		[in] 	SPI引脚选择SS0\SS2。 
* 		buf	  			[in] 	存放读取的数据
*		len				[in] 	读取数据的长度
* Return:
* 	0			成功。
*	-1 			失败。
*****************************************************************/
int ql_spi_read(QL_SPI_PORT_E port, unsigned char *buf, unsigned int len);

/*****************************************************************
* Function: ql_spi_write_read
* Description:SPI数据读写
*
* Parameters:
* 		port	  		[in] 	SPI引脚选择SS0\SS2。 
* 		inbuf	  		[in] 	存放写入的数据
*		outbuf			[in] 	存放读出的数据
*		len				[in] 	读写数据的长度
* Return:
* 	0			成功。
*	-1 			失败。
*****************************************************************/
int ql_spi_write_read(QL_SPI_PORT_E port, unsigned char *inbuf, unsigned char *outbuf, unsigned int len);

#ifdef __cplusplus
	} /*"C" */
#endif

#endif /* _QL_UART_H_ */
