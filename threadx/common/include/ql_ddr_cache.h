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
* 	刷cache到内存。 
* 
* Parameters:
* 	mem	  			[in] 	需要刷新cache到ddr的地址。 
* 	size	  		[in] 	刷数据的大小
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
* 	设置对应地址cache数据无效。 
* 
* Parameters:
* 	mem	  			[in] 	使无效cache数据的地址。 
* 	size	  		[in] 	使无效cache数据的大小
*
* Return:
*		NULL
*
*****************************************************************/
void ql_ddr_cache_invalidate_memory(void *mem, unsigned int size);

#endif
