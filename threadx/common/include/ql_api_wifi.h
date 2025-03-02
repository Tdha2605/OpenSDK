/**
  @file
  ql_api_wifi.h

  @brief
  This file provides the definitions for wifi, and declares the
  API functions.

*/
/*============================================================================
  Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/
#ifndef QL_API_WIFI_H
#define QL_API_WIFI_H

#include "ql_type.h"
#include "netdb.h"
#include "ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
 *  Marco Definition
 *========================================================================*/
#define QL_WIFI_SSID_MAX_LEN      32
#define QL_WIFI_PASS_MAX_LEN      64
#define QL_WIFI_IPV4_MAX_LEN      16

#define LWIP_NETIF_IP_TYPE_MAX   (2)
#define LWIP_NETIF_DNS_MAX       (2)
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef enum
{
	QL_WIFI_SUCCESS           = 0 ,
	QL_WIFI_EXECUTE_ERR       = 1 ,
	QL_WIFI_INVALID_PARAM_ERR,
	QL_WIFI_INVALID_PARAM_LEN_ERR,
	QL_WIFI_NO_MEM_ERR,
	QL_WIFI_MEM_ADDR_NULL_ERR = 5 ,
	QL_WIFI_INVALID_IP_ERR,
	QL_WIFI_INVALID_GW_ERR,
	QL_WIFI_INVALID_NETMASK_ERR,
	QL_WIFI_INVALID_DNS_ERR,
	QL_WIFI_INVALID_DHCP_ERR = 10 ,
	QL_WIFI_INVALID_MAC_ERR,
	QL_WIFI_INVALID_CID_ERR,
	QL_WIFI_STATE_ERR,

} ql_wifi_errcode_e;

// Wi-Fi options
typedef enum
{
	QL_WIFI_OPT_IP = 0,   // val_len: sizeof(ip4_addr_t),static ip
	QL_WIFI_OPT_GW,       // val_len: sizeof(ip4_addr_t),static gateway
	QL_WIFI_OPT_NETMASK,  // val_len: sizeof(ip4_addr_t),static netmask
	QL_WIFI_OPT_DNS,      // val_len: sizeof(ql_wifi_dns_s),static dns
	QL_WIFI_OPT_DHCP,     // val_len: sizeof(char)
	QL_WIFI_OPT_MAC,      // val_len: ETHARP_HWADDR_LEN
	QL_WIFI_OPT_STATE,    // val_len: sizeof(ql_wifi_state_e),netcard status
	QL_WIFI_OPT_CID,      // val_len: sizeof(ql_wifi_cid_s),wifi cid
	QL_WIFI_OPT_MAX
} ql_wifi_opt_e;

typedef enum
{
	QL_WIFI_STATE_NONE = 0,   // wifinet initialization state
	QL_WIFI_STATE_CONNECTING, // wifinet connecting
	QL_WIFI_STATE_CONNECTED,  // wifinet connected
	QL_WIFI_STATE_DISCONNECT, // wifi disconnect
	QL_WIFI_STATE_MAX,
} ql_wifi_state_e;

/*========================================================================
 *  Type Definition
 *========================================================================*/
typedef void (*ql_wifi_status_notify_cb)(void *ctx);
typedef int (*ql_wifi_linkoutput_cb)(void *ctx);

typedef struct
{
	unsigned int data_len;
	unsigned char *data;
	
} ql_wifi_msg_info_s;

typedef struct
{
	uint8_t sim_id;
	int cid;
	uint8_t enable;
	
} ql_wifi_cid_s;

typedef struct
{
	ip_addr_t dns_server[LWIP_NETIF_IP_TYPE_MAX][LWIP_NETIF_DNS_MAX];
	
} ql_wifi_dns_s;


/*========================================================================
 *	function Definition
 *========================================================================*/

/*****************************************************************
 * Function: ql_wifi_opt_set
 *
 * Description: Set Wi-Fi option.See ql_wifi_opt_e.
 *
 * Parameters:
 * ql_wifi_opt_e        [in]    Option type.See ql_wifi_opt_e.
 * val                  [in]    The address of input parameter.
 * val_len              [in]    The length of input parameter.See ql_wifi_opt_e.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_opt_set(ql_wifi_opt_e opt, void *val, int val_len);

/*****************************************************************
 * Function: ql_wifi_opt_get
 *
 * Description: Get Wi-Fi option.See ql_wifi_opt_e.
 *
 * Parameters:
 * ql_wifi_opt_e        [in]        Option type.See ql_wifi_opt_e.
 * val                  [Out]       The address of output parameter.
 * val_len              [in]        The length of output parameter.
 *                      [out]       Some options will return a valid "val_len".See ql_wifi_opt_e.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_opt_get(ql_wifi_opt_e opt, void *val, int *val_len);

/*****************************************************************
 * Function: ql_wifi_status_notify_cb_register
 *
 * Description: Register Wi-Fi status notification callback.
 *
 * Parameters:
 * cb        [in]        Wi-Fi status notification callback.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_status_notify_cb_register(ql_wifi_status_notify_cb cb);

/*****************************************************************
 * Function: ql_wifi_sta_ip_start
 *
 * Description: Start Wi-Fi STA dhcp or set Wi-Fi STA static IP information from the parameter of ql_wifi_opt_set.
 *
 * Parameters:
 * argv        [in]        No defined yet.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_sta_ip_start(void *argv);

/*****************************************************************
 * Function: ql_wifi_sta_ip_down
 *
 * Description: Stop Wi-Fi STA dhcp and set Wi-Fi STA down.
 *
 * Parameters:
 * argv        [in]        No defined yet.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_sta_ip_down(void *argv);

/*****************************************************************
 * Function: ql_wifi_wlan_create
 *
 * Description: Create Wi-Fi netcard.
 *
 * Parameters:
 * argv        [in]        Netcard MAC.Handle as char mac[6]
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_wlan_create(void *argv);

/*****************************************************************
 * Function: ql_wifi_wlan_destroy
 *
 * Description: Destroy Wi-Fi netcard.
 *
 * Parameters:
 * argv        [in]        No defined yet.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_wlan_destroy(void *argv);

/*****************************************************************
 * Function: ql_wifi_linkoutput_cb_register
 *
 * Description: Register the link layer output callback for LWIP.
 *
 * Parameters:
 * ql_wifi_linkoutput_cb        [in]        Link layer output callback for LWIP.
 *                                          Callback is called when a raw link packet is ready to be transmitted.
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_linkoutput_cb_register(ql_wifi_linkoutput_cb cb);

/*****************************************************************
 * Function: ql_wifi_data_input
 *
 * Description: Input Wi-Fi data streams to LWIP.
 *
 * Parameters:
 * data                          [in]    Input data    
 * len                           [in]    Input data length
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/
ql_wifi_errcode_e ql_wifi_data_input(unsigned char *data, unsigned int len);

/*****************************************************************
 * Function: ql_wifi_getaddrinfo_though_wifi
 *
 * Description: dns query by wifi.
 *
 * Parameters:
 * nodename                          [in]     query domian name string
 * servname                          [in]     query domian name server name string
 * hints                             [in]     query hints;
 * res                               [out]    domain name address info return;
 *
 * Return:
 *  0           success
 *  other       error code
 *
 *****************************************************************/

int ql_wifi_getaddrinfo_though_wifi(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);

#ifdef __cplusplus
} /*"C" */
#endif

#endif