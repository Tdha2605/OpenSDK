
/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/


#ifndef __QL_USB_H__
#define __QL_USB_H__


//states: 1>USB插入 0>USB拔出
typedef void (*usbdectcb)(unsigned int states);

/*************************************************
*注册USB插入与拔出的检测回调函敿
*函数成功返回0，否则返囿1
/************************************************/
int ql_usbdect_register_cb(usbdectcb cb);

extern int ql_usb_set_pid(UINT16 PID);
/*****************************************************************
* Function: ql_usb_set_pstring
*
* Description:
* 	设置USB的product类型。 
* 
* Parameters:
* 	string        [in] 需要设置的	  	product	  
* Return:
* 	1				USB正常枚举。
*	0		        USB未能正常枚举,非USB,可能是适配器。
*
NOTES:此接口耗时2s
*****************************************************************/
extern int ql_usb_set_pstring(UINT8      *string,UINT8 len,UINT8 id);

/*****************************************************************
* Function: ql_usb_get_type
*
* Description:
* 	获取USB的类型。 
* 
* Parameters:
* 	void	  		  
* Return:
* 	1				USB正常枚举。
*	0		        USB未能正常枚举,非USB,可能是电源适配器。
*
NOTES:此接口耗时2s
*****************************************************************/
extern int ql_usb_get_type(void);


/*****************************************************************
* Function: ql_usb_connect_state
*
* Description:
* 	获取USB的连接状态,d+,d-。 [!!!此接口目前有bug,不建议使用@2022/04/26]
* 
* Parameters:
* 	void	  		  
* Return:
* 	1				USB正常枚举且可以正常通讯。
*	0		        USB未连接。
*
*****************************************************************/
int ql_usb_connect_state(void);


#endif


