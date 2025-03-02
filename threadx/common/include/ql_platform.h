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
* 	��ȡӲ�����������
* 
* Parameters:
* 	�ޡ�  
* Return:
* 	����Ӳ�����������
*
*****************************************************************/
unsigned int ql_geu_random_number(void);
/*****************************************************************
* Function: ql_rtos_get_current_task_name
*
* Description:
* 	��ȡ������������
* 
* Parameters:
* 	�ޡ�  
* Return:
* 	������������
*
*****************************************************************/
unsigned int ql_pseudo_random_number(void);  
/*****************************************************************
* Function: ql_get_dump_cnt
*
* Description:
* 	��ȡdump������
* 
* Parameters:
* 	�ޡ�  
* Return:
* 	��ȡdump������
*
*****************************************************************/
int ql_get_dump_cnt(void);
/*****************************************************************
* Function: ql_get_dump_file
*
* Description:
* 	 ��ȡ��dump��Ϣ��bufffer
* 
* Parameters:
* 	  EEbuf dump�� buf�����壬EEbuf.buflen�ο�example
*     EEbufnum����Ҫdump���ļ������Ϊ3
* Return:
* 	
*
*****************************************************************/
int ql_get_dump_file(QL_DUMP_BIN_FILE* EEbuf,unsigned int EEbufnum);
/*****************************************************************
* Function: ql_flash_save_dump_enable
*
* Description:
* 	ʹ��dump�ļ��ݴ浽flash���ܡ�
* 
* Parameters:
* 	EnableFlag��1ʹ�� 0�رա�  
* Return:
* 	��
*
*****************************************************************/
void ql_flash_save_dump_enable(unsigned char EnableFlag);
/*****************************************************************
* Function: ql_del_dump_file
*
* Description:
* 	�ͷű���dump��flash�ռ䡣
* 
* Parameters:
* 	�ޡ�  
* Return:
* 	��
*
*****************************************************************/
int ql_del_dump_file(void);
/*****************************************************************
* Function: ql_clean_dump_cnt
*
* Description:
* 	����¼��dump������0��
* 
* Parameters:
* 	�ޡ�  
* Return:
* 	
*
*****************************************************************/
int ql_clean_dump_cnt(void);

/*****************************************************************
* Function: ql_set_dump_cb
*
* Description:
* 	ע�����dumpǰ�Ļص�������dumpǰ����ִ��ע��Ļص���������ʾ�ϲ㣩��
* 
* Parameters:
* 	dump_cb��ע��Ļص�����  
* Return:
* 	0���ɹ�
*   -1��ʧ��
*****************************************************************/
int ql_set_dump_cb(ql_dump_cb dump_cb);

/*****************************************************************
* Function: ql_asr_property_get
*
* Description:
* 	queue the <value> with index key <name>
* 
* Parameters:
* 	name��index key  ��ע��Ҫ�ǳ����ַ������ҳ���ҪС��20��
* Return:
* 	ָ��index key�µĵ�ַ
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
* 	name��index key  ��ע��Ҫ�ǳ����ַ������ҳ���ҪС��20��
*  buffer���洢��ָ��index key�µ�ַ��ȡ����ֵ ��ע��buffer�Ĵ�СҪ���ڷ��ص����ݳ��ȣ�
* Return:
* 	0: ʧ��
*	len:���ݳ���   
*****************************************************************/
int ql_asr_property_get_buffer(const char *name, char *buffer);


#endif
