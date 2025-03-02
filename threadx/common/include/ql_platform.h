/*================================================================
  Copyright (c) 2022, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
#ifndef _QL_CPU_FRE_H
#define _QL_CPU_FRE_H

typedef struct
{
    char fname[16];
    unsigned char* EEbuf;
    unsigned int buflen;    
}QL_DUMP_BIN_FILE;

typedef enum QL_CPU_FREQUENCY_PP_ENUM
{
	QL_PP1 = 1,//312M
	QL_PP2,//416M
	QL_PP3,//499M
	QL_PP4,//624M
	QL_PP5,//832M
}QL_CPU_FREQUENCY_PP_E;
typedef void(*ql_dump_cb)(void);

int ql_cpu_fre_set(unsigned int cpu_fre);

unsigned int ql_cpu_fre_get(void);
/*****************************************************************
* Function: ql_geu_random_number
*
* Description:
* 	获取硬件真随机数。
* 
* Parameters:
* 	无。  
* Return:
* 	返回硬件真随机数。
*
*****************************************************************/
unsigned int ql_geu_random_number(void);
/*****************************************************************
* Function: ql_rtos_get_current_task_name
*
* Description:
* 	获取软件假随机数。
* 
* Parameters:
* 	无。  
* Return:
* 	软件假随机数。
*
*****************************************************************/
unsigned int ql_pseudo_random_number(void);  
/*****************************************************************
* Function: ql_get_dump_cnt
*
* Description:
* 	获取dump次数。
* 
* Parameters:
* 	无。  
* Return:
* 	获取dump次数。
*
*****************************************************************/
int ql_get_dump_cnt(void);
/*****************************************************************
* Function: ql_get_dump_file
*
* Description:
* 	 获取简单dump信息的bufffer
* 
* Parameters:
* 	  EEbuf dump： buf承载体，EEbuf.buflen参考example
*     EEbufnum：需要dump的文件数最大为3
* Return:
* 	
*
*****************************************************************/
int ql_get_dump_file(QL_DUMP_BIN_FILE* EEbuf,unsigned int EEbufnum);
/*****************************************************************
* Function: ql_flash_save_dump_enable
*
* Description:
* 	使能dump文件暂存到flash功能。
* 
* Parameters:
* 	EnableFlag：1使能 0关闭。  
* Return:
* 	。
*
*****************************************************************/
void ql_flash_save_dump_enable(unsigned char EnableFlag);
/*****************************************************************
* Function: ql_del_dump_file
*
* Description:
* 	释放保存dump的flash空间。
* 
* Parameters:
* 	无。  
* Return:
* 	。
*
*****************************************************************/
int ql_del_dump_file(void);
/*****************************************************************
* Function: ql_clean_dump_cnt
*
* Description:
* 	将记录的dump次数清0。
* 
* Parameters:
* 	无。  
* Return:
* 	
*
*****************************************************************/
int ql_clean_dump_cnt(void);

/*****************************************************************
* Function: ql_set_dump_cb
*
* Description:
* 	注册进入dump前的回调（进入dump前会先执行注册的回调，用来提示上层）。
* 
* Parameters:
* 	dump_cb：注册的回调函数  
* Return:
* 	0：成功
*   -1：失败
*****************************************************************/
int ql_set_dump_cb(ql_dump_cb dump_cb);

/*****************************************************************
* Function: ql_asr_property_get
*
* Description:
* 	queue the <value> with index key <name>
* 
* Parameters:
* 	name：index key  （注意要是常量字符串，且长度要小于20）
* Return:
* 	指定index key下的地址
*   
*****************************************************************/
const char*  ql_asr_property_get(const char *name);

/*****************************************************************
* Function: ql_asr_property_get_buffer
*
* Description:
* 	load the <value> into [arg=buffer] with index key <name>
* 
* Parameters:
* 	name：index key  （注意要是常量字符串，且长度要小于20）
*  buffer：存储获指定index key下地址获取到的值 （注意buffer的大小要大于返回的数据长度）
* Return:
* 	0: 失败
*	len:数据长度   
*****************************************************************/
int ql_asr_property_get_buffer(const char *name, char *buffer);


#endif
