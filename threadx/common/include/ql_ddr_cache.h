/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#ifndef __QL_DDR_CACHE_H__
#define __QL_DDR_CACHE_H__

/*****************************************************************
* Function: ql_ddr_cache_flush_memory
*
* Description:
* 	ˢcache���ڴ档 
* 
* Parameters:
* 	mem	  			[in] 	��Ҫˢ��cache��ddr�ĵ�ַ�� 
* 	size	  		[in] 	ˢ���ݵĴ�С
*
*
* Return:
*		NULL
*
*****************************************************************/
void ql_ddr_cache_flush_memory (void *mem, unsigned int size);

/*****************************************************************
* Function: ql_ddr_cache_invalidate_memory
*
* Description:
* 	���ö�Ӧ��ַcache������Ч�� 
* 
* Parameters:
* 	mem	  			[in] 	ʹ��Чcache���ݵĵ�ַ�� 
* 	size	  		[in] 	ʹ��Чcache���ݵĴ�С
*
* Return:
*		NULL
*
*****************************************************************/
void ql_ddr_cache_invalidate_memory(void *mem, unsigned int size);

#endif
