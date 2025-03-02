/*================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
#include <stdio.h>
#include <string.h>

#include "ql_log.h"
#include "ql_func.h"
#include <stdarg.h>
#include "ql_api_map_common.h"

#define PRINTF_MAX_LEN 2049

typedef void(*_api_seagull_uart_putc_t)(const char);

typedef void (*_api__ql_os_log_print_t)(const char* fmt, va_list arg_ptr);

typedef void (*_api__ql_fs_log_print_t)(const char* fmt, va_list arg_ptr);

typedef void (*_api__ql_app_log_print_t)(const char* fmt, va_list arg_ptr);

typedef void (*_api_ql_log_mask_set_t)(UINT64 module_mask, UINT32 port_mask);
typedef int(* _api_debug_printf_lock_t)(void);
typedef int(* _api_debug_printf_unlock_t)(void);
typedef void (*_api_ql_debug_log_enable_t)(void);
typedef void (*_api_ql_debug_log_disable_t)(void);


static _api__ql_os_log_print_t m__ql_os_log_print = NULL;
static _api__ql_fs_log_print_t m__ql_fs_log_print = NULL;
static _api__ql_app_log_print_t m__ql_app_log_print = NULL;
static _api_ql_log_mask_set_t m_ql_log_mask_set = NULL;
static _api_seagull_uart_putc_t m_seagull_uart_putc = NULL;
static _api_debug_printf_lock_t m_debug_printf_lock = NULL;
static _api_debug_printf_unlock_t m_debug_printf_unlock = NULL;
static _api_ql_debug_log_enable_t m_ql_debug_log_enable = NULL;
static _api_ql_debug_log_disable_t m_ql_debug_log_disable = NULL;


qL_debug_log_mode_e ql_debug_log_mode = QL_DEBUG_LOG_OPEN;

static void _ql_os_log_print(const char* fmt, va_list arg_ptr)
{
	if(NULL == m__ql_os_log_print)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m__ql_os_log_print = (_api__ql_os_log_print_t)ptr;
	}
	
	return m__ql_os_log_print(fmt, arg_ptr);
}



static void _ql_fs_log_print(const char* fmt, va_list arg_ptr)
{
	if(NULL == m__ql_fs_log_print)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m__ql_fs_log_print = (_api__ql_fs_log_print_t)ptr;
	}
	
	return m__ql_fs_log_print(fmt, arg_ptr);
}



static void _ql_app_log_print(const char* fmt, va_list arg_ptr)
{
	if(NULL == m__ql_app_log_print)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m__ql_app_log_print = (_api__ql_app_log_print_t)ptr;
	}
	return m__ql_app_log_print(fmt, arg_ptr);
}

void seagull_uart_putc(const char ch)
{
	if(NULL == m_seagull_uart_putc)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m_seagull_uart_putc = (_api_seagull_uart_putc_t)ptr;
	}
	return m_seagull_uart_putc(ch);
}


char * _ql_log_get_short_file_name(char * file_name)
{
	return (strrchr(file_name, '/') == NULL ? (strrchr(file_name, '\\') == NULL ? file_name : strrchr(file_name, '\\')+1) : strrchr(file_name, '/')+1);
}



void _ql_os_log(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);	

	_ql_os_log_print(fmt, ap);

	va_end(ap);
}



void _ql_fs_log(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);	
	
	_ql_fs_log_print(fmt, ap);

	va_end(ap);		
}



void _ql_app_log(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);	
	
	_ql_app_log_print(fmt, ap);
	
	va_end(ap);		
}


void ql_debug_log_enable(void)
{
	if(NULL == m_ql_debug_log_enable)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m_ql_debug_log_enable = (_api_ql_debug_log_enable_t)ptr;
	}
	ql_debug_log_mode = QL_DEBUG_LOG_OPEN;

	return m_ql_debug_log_enable();
}

void ql_debug_log_disable(void)
{
	if(NULL == m_ql_debug_log_disable)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m_ql_debug_log_disable = (_api_ql_debug_log_disable_t)ptr;
	}
	ql_debug_log_mode = QL_DEBUG_LOG_CLOSE;

	return m_ql_debug_log_disable();
}

void ql_log_mask_set(UINT64 module_mask, UINT32 port_mask)
{
	if(NULL == m_ql_log_mask_set)
	{
		s32 ptr = m_get_api_ptr((char*)__FUNCTION__);
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return;
		}
		m_ql_log_mask_set = (_api_ql_log_mask_set_t)ptr;
	}
	
	return m_ql_log_mask_set(module_mask, port_mask);
}

static int debug_printf_lock(void)
{
	if(NULL == m_debug_printf_lock)
	{
		s32 ptr = m_get_api_ptr((char*)"quec_debug_port_lock");
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return -1;
		}
		m_debug_printf_lock = (_api_debug_printf_lock_t)ptr;
	}
	return m_debug_printf_lock();
}

static int debug_printf_unlock(void)
{
	if(NULL == m_debug_printf_unlock)
	{
		s32 ptr = m_get_api_ptr((char*)"quec_debug_port_unlock");
		if(0 == ptr)
		{
			SDK_API_DEBUG_NOTSUP();
			return -1;
		}
		m_debug_printf_unlock = (_api_debug_printf_unlock_t)ptr;
	}
	return m_debug_printf_unlock();
}

static char print_buf[PRINTF_MAX_LEN];

int printf( const char *format, ... )
{
	if(ql_debug_log_mode == QL_DEBUG_LOG_CLOSE)
	{
		return 0;
	}
	va_list arg_ptr;
	int len = 0;

	debug_printf_lock();
	
	va_start(arg_ptr, format);
	len = vsnprintf(print_buf, PRINTF_MAX_LEN, format, arg_ptr);
	va_end(arg_ptr);

	if(len <= 0)
		goto exit;

	len = 0;

	while(print_buf[len])
	{
		seagull_uart_putc(print_buf[len++]);
	}
exit:
	debug_printf_unlock();
	return len;
}


