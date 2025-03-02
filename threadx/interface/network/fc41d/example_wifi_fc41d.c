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

/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdbool.h"

#include "ql_log.h"
#include "ql_type.h"
#include "ql_rtos.h"

#include "ip4_addr.h"
#include "ip6_addr.h"
#include "inet.h"
#include "netdb.h"
#include "sockets.h"
#include "tcp.h"

#include "ql_application.h"
#include "ql_wifi_app.h"
#include "ql_api_wifi.h"
#include "ql_data_call.h"
#include "ql_gpio.h"
#include "ql_ping.h"
#include "ql_ntp.h"

/*========================================================================
 *	Macro Definition
 *========================================================================*/

//#define QL_WIFI_DEMO_LOG(msg, ...)           ql_app_log(msg, ##__VA_ARGS__)
#define QL_WIFI_DEMO_LOG(msg, ...)           printf(msg, ##__VA_ARGS__)

#define QL_WIFI_DEMO_MSG_QUEUE_CNT          (10)
#define QL_WIFI_DEMO_DISCONNECT_RETRY_TIME  (30 * 1000)
#define QL_WIFI_DEMO_RETRY_CNT_MAX          (3)

//#define QL_WIFI_DEMO_SIM_ID           0
#define QL_WIFI_DEMO_PROFILE_IDX            (7)
#define QL_WIFI_DEMO_SOCKET_BUF_SIZE        (128)
#define QL_WIFI_DEMO_TCP_CLIENTS_CNT        (1)
#define APP_PRIORITY_NORMAL                 (100)

#ifndef kNoErr
#define kNoErr                              (0)
#endif
#ifndef kGeneralErr
#define kGeneralErr                         (-1)
#endif
#define QL_OSI_SUCCESS                      (0)
#define QL_SPI_SUCCESS                      (0)

//PING TEST
#define PING_HOST                           "www.baidu.com"
#define PING_TO_MS                           4000
#define PING_DATA_SIZE                       64
#define PING_NUM                             5
//NTP TEST
#define WIFI_NTP_SERVER_DOMAIN              "ntp.aliyun.com"

//for customer wifi power supply enable ctrl
#define QL_WIFI_PWK_EN_GPIO                  GPIO_PIN_NO_27   //UART_DTR:-> FC41D PWK EN

/*========================================================================
 *	Enumeration Definition
 *========================================================================*/
typedef enum
{
    WIFI_DEMO_RETRY_CONNECT_START = 0,
    WIFI_DEMO_RETRY_CONNECT_END,
} wifi_demo_retry_state_e;

typedef enum
{
    WIFI_DEMO_EVENT_INIT = 0,
    WIFI_DEMO_EVENT_TRY_CONNECT,
    WIFI_DEMO_EVENT_CONNECTED,
    WIFI_DEMO_EVENT_DISCONNECT,
    WIFI_DEMO_EVENT_APP_TCP_CONNECT,
    WIFI_DEMO_EVENT_APP_TCP_DISCONNECT,
    WIFI_DEMO_EVENT_GET_STATUS,
} wifi_demo_event_e;

/*========================================================================
 *  Type Definition
 *=========================================================================*/


typedef struct
{
	int id;
	int fd;
	ql_task_t task;
	uint8_t close_flag;
} wifi_demo_tcp_client_s;


typedef struct
{
	ql_task_t task;
	ql_wifi_state_e state;
	ql_queue_t msg_queue;
	ql_timer_t retry_timer;
	wifi_demo_retry_state_e retry_state;
	int retry_cnt;
	// tcp client
	wifi_demo_tcp_client_s tcp_client[QL_WIFI_DEMO_TCP_CLIENTS_CNT];
	int tcp_client_cnt;

} wifi_demo_manager_s;

typedef struct
{
	uint32_t id;
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
	
} wifi_demo_event_s;
/*========================================================================
 *   Global Variable
 *========================================================================*/
wifi_demo_manager_s wifi_demo_manager = {0};

ql_mutex_t wifi_demo_mutex = NULL;
ql_sem_t   ql_wifi_ping_sem = NULL;

static char send_buf[QL_WIFI_DEMO_SOCKET_BUF_SIZE] = {0};
static int  send_len = 0;
static char recv_buf[QL_WIFI_DEMO_SOCKET_BUF_SIZE] = {0};
static int  recv_len = 0;


/*========================================================================
 *	function Definition
 *========================================================================*/
int ql_wifi_pwk_en_gpio_init(void)
{
	int ret;
	ql_gpio_deinit(QL_WIFI_PWK_EN_GPIO);
	//spi Tx ctrl pin
	ret = ql_gpio_init(QL_WIFI_PWK_EN_GPIO, PIN_DIRECTION_OUT, PIN_PULL_DISABLE, PIN_LEVEL_HIGH);
	if (ret != 0){
		QL_WIFI_DEMO_LOG("wifi pwk en gpio tx init err! ");
		return ret;
	}
	
	return ret;
}

void wifi_demo_send_event(ql_task_t task, uint32_t id, uint32_t param1, uint32_t param2, uint32_t param3)
{
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	ql_queue_t *msg_queue = NULL;
	if (task == manager->task){
		msg_queue = manager->msg_queue;
	}else{
		return;
	}

	QlOSStatus err = QL_OSI_SUCCESS;
	wifi_demo_event_s event = {.id = id, .param1 = param1, .param2 = param2, .param3 = param3};
	err = ql_rtos_queue_release(msg_queue, sizeof(wifi_demo_event_s), (uint8_t *)&event, QL_NO_WAIT);
	if (err){
		QL_WIFI_DEMO_LOG("release failed\r\n");
	}
	return;
}

static void wifi_demo_retry_timer_callback(void *ctx)
{
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	QL_WIFI_DEMO_LOG("timer retry\r\n");
	wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_TRY_CONNECT, 0, 0, 0);
	return;
}

static void wifi_demo_connect_status(void* ctx)
{
	uint32_t value = *((uint32_t*)ctx);
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	QL_WIFI_DEMO_LOG("wifi connect %d\r\n", value);
	wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_GET_STATUS, value, 0, 0);
	return;
}

void wifi_demo_tcp_client_thread(void *arg)
{
	int ret = 0;
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	wifi_demo_tcp_client_s *ctx = (wifi_demo_tcp_client_s *)arg;
	ctx->close_flag = 0;
	
	int i = 0;
	int flags = 0;
	int connected = 0;
	int socket_fd = -1;
	fd_set read_fds;
	fd_set write_fds;
	fd_set exp_fds;
	int fd_changed = 0;
	int closing = false;
	char ip4_addr_str[16] = {0};
	struct sockaddr_in local4, server_ipv4;

	uint8_t destip[] = {220, 180, 239, 212};
	uint16_t destport = 8305;
	ip4_addr_t int_destip;
	IP4_ADDR(&int_destip, destip[0], destip[1], destip[2], destip[3]);

	int val_len = sizeof(ip_addr_t);
	ip_addr_t ip = {0};
	ret = ql_wifi_opt_get(QL_WIFI_OPT_IP, &ip, &val_len);
	if (ret != QL_WIFI_SUCCESS){
		QL_WIFI_DEMO_LOG("get ip faild!\r\n");
		goto exit;
	}
	QL_WIFI_DEMO_LOG("get ip [%04x]\r\n", ip.addr);
	memset(&local4, 0x00, sizeof(struct sockaddr_in));
	local4.sin_family = AF_INET;
	local4.sin_port = 2345;
	memcpy(&local4.sin_addr, &ip.addr, sizeof(ip_addr_t));
	//inet_aton(ip_ntoa(&ip.addr), (ip_addr_t*)&local4.sin_addr);

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exp_fds);

	QL_WIFI_DEMO_LOG("socket start!\r\n");
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0)
	{
		goto exit;
	}
	ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
	if(ret < 0)
	{
		close(socket_fd);
		socket_fd = -1;
		goto exit;
	}
	flags |= O_NONBLOCK;
	fcntl(socket_fd, F_SETFL,flags);
	memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
	server_ipv4.sin_family = AF_INET;
	server_ipv4.sin_port = htons(destport);
	memcpy(&(server_ipv4.sin_addr),&int_destip,sizeof(int_destip));	
	ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));	
	if(ret == 0)
	{
		connected = 1;
	}
	else
	{
		if(ql_get_soc_errno() != EINPROGRESS){
			close(socket_fd);
			socket_fd = -1;
			goto exit;
		}
	}

	if (connected == 1)
	{
		QL_WIFI_DEMO_LOG("tcp connected\r\n");
		ql_rtos_mutex_try_lock(wifi_demo_mutex);
		ctx->fd = socket_fd;
		ql_rtos_mutex_unlock(wifi_demo_mutex);
		char con[128] = {0};
		int len = 0;
		FD_SET(socket_fd, &read_fds);
		len = snprintf(con, 128, "tcp connected\r\n");

		if (write(socket_fd, con, len) != len)
		{
			FD_SET(socket_fd, &write_fds);
		}
	}
	else
	{
		FD_SET(socket_fd, &write_fds);
	}
	FD_SET(socket_fd, &exp_fds);
	
	while (1)
	{
		fd_changed = select(socket_fd + 1, &read_fds, &write_fds, &exp_fds, NULL);
		if (fd_changed > 0)
		{
			if (FD_ISSET(socket_fd, &write_fds))
			{
				FD_CLR(socket_fd, &write_fds);
				if (connected == 0)
				{
					int value = 0;
					char con[128] = {0};
					u32_t len = sizeof(value);
					getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &value, &len);
					if (value == 0 || value == EISCONN)
					{
						QL_WIFI_DEMO_LOG("tcp connected");
						ql_rtos_mutex_try_lock(wifi_demo_mutex);
						ctx->fd = socket_fd;
						ql_rtos_mutex_unlock(wifi_demo_mutex);
						connected = 1;
						FD_SET(socket_fd, &read_fds);
						len = snprintf(con, 128, "tcp connected\r\n");
						write(socket_fd, con, len);
					}
					else
					{
						QL_WIFI_DEMO_LOG("tcp connect failed\r\n");
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}
				else
				{
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA#!&*1234end", i);
					write(socket_fd, send_buf, send_len);
					i++;
				}
			}
			else if (FD_ISSET(socket_fd, &read_fds))
			{
				FD_CLR(socket_fd, &read_fds);
				memset(recv_buf, 0x00, 128);
				recv_len = read(socket_fd, recv_buf, 128);
				if (recv_len > 0) {
					QL_WIFI_DEMO_LOG("recv: %s\r\n", recv_buf);
					memset(send_buf, 0x00, 128);
					send_len = snprintf(send_buf, 128, "%d%s%d\r\n", i, "startAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA#!&*1234end", i);
					write(socket_fd, send_buf, send_len);
					i++;
					ql_rtos_task_sleep_ms(500);
					FD_SET(socket_fd, &read_fds);
				}
				else if (recv_len == 0)
				{
					if (closing == false){
						QL_WIFI_DEMO_LOG("shut down\r\n");
						shutdown(socket_fd, SHUT_WR);
						closing = true;
						FD_SET(socket_fd, &read_fds);
					}
					else
					{
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}
				else
				{
					if (ql_get_soc_errno() == EAGAIN){
						FD_SET(socket_fd, &read_fds);
					}else{
						close(socket_fd);
						socket_fd = -1;
						break;
					}
				}
			}
			else if (FD_ISSET(socket_fd, &exp_fds))
			{
				FD_CLR(socket_fd, &exp_fds);
				close(socket_fd);
				socket_fd = -1;
				break;
			}
		}

	}
	
exit:
	QL_WIFI_DEMO_LOG("wifi tcp client %d end,%p\r\n", ctx->id, ctx->task);
	close(socket_fd);
	ql_rtos_mutex_try_lock(wifi_demo_mutex);
	memset(ctx, 0, sizeof(wifi_demo_tcp_client_s));
	ctx->id = -1;
	ctx->fd = -1;
	manager->tcp_client_cnt--;
	ql_rtos_mutex_unlock(wifi_demo_mutex);
	ql_rtos_task_delete(NULL);
	
}

static int wifi_demo_tcp_client_create()
{
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	wifi_demo_tcp_client_s *ctx = NULL;
	// User defined.Only support one client here.
	if (manager->tcp_client_cnt >= 1){
		QL_WIFI_DEMO_LOG("only support one client\r\n");
		return -1;
	}
	// Find NULL task
	int i = 0;
	for (i = 0; i < QL_WIFI_DEMO_TCP_CLIENTS_CNT; i++)
	{
		ctx = &(manager->tcp_client[i]);
		QL_WIFI_DEMO_LOG("ctx->task %p\r\n", ctx->task);
		if (ctx->task == NULL){
			break;
		}
	}

	if (i >= QL_WIFI_DEMO_TCP_CLIENTS_CNT)
	{
		QL_WIFI_DEMO_LOG("client full\r\n");
		return -1;
	}
	if (QL_OSI_SUCCESS != ql_rtos_task_create(&(ctx->task), 4 * 1024, APP_PRIORITY_NORMAL, "wifi_tcp_client", wifi_demo_tcp_client_thread, ctx, 5))
	{
		QL_WIFI_DEMO_LOG("task created failed\r\n");
		return -1;
	}
	ctx->id = i;
	ctx->fd = -1;
	manager->tcp_client_cnt++;
	return 0;
}

static int wifi_demo_dns_query(char *hostname)
{

	int dns_index = 0;
	struct addrinfo *res = NULL,*cur = NULL;
	struct addrinfo hints;
	struct sockaddr_in *sin_res;
	int ret       = 0;

	memset(&hints,0,sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	
	QL_WIFI_DEMO_LOG("%s\r\n", __FUNCTION__);
	ret = ql_wifi_getaddrinfo_though_wifi(hostname, NULL, &hints, &res);
	if (ret) {
		return -1;
	}

	QL_WIFI_DEMO_LOG("[%s] dns query list=>:\r\n", hostname);
	for(cur = res; cur != NULL; cur = cur->ai_next) {
		dns_index++;
		if ((cur->ai_addr->sa_family) == AF_INET) {
			sin_res = (struct sockaddr_in*)cur->ai_addr;
			QL_WIFI_DEMO_LOG("dns=> %d:%s\r\n", dns_index, inet_ntoa(sin_res->sin_addr));
		}
	}
	
	if(res) {
		freeaddrinfo(res);
	}
	return 0;
}



static void wifi_demo_ping_cb(ql_ping_state_e state, int err, ql_ping_stat_t * statistic)
{
	QL_WIFI_DEMO_LOG("ql_wifi_ping_cb stat %d  \r\n",state);
	
	if(state == QL_PING_INPROGRESS) {
		if(err == 0)
			QL_WIFI_DEMO_LOG("ping:\t ip: %s\t datasize: %3d\t rtt: %4d\t ttl: %4d\r\n",statistic->ipSrc, statistic->dataSize, statistic->rtt, statistic->ttl);
		else
			QL_WIFI_DEMO_LOG("ping:\t error: %d\r\n", err);
	}else{
		QL_WIFI_DEMO_LOG("ping:\t send: %d\t recv: %d\t lost: %d\t minRtt: %d\t maxRtt: %d\r\n", statistic->sentNum, statistic->rcvdNum, statistic->lostNum, statistic->minRtt, statistic->maxRtt);
	}
	
	if(statistic->sentNum >= PING_NUM|| statistic->rcvdNum >= PING_NUM){
		QL_WIFI_DEMO_LOG("ping cb send count %d recv count %d  \r\n",statistic->sentNum,statistic->rcvdNum);
		ql_rtos_semaphore_release(ql_wifi_ping_sem);
	}
}

static void wifi_demo_ping_test()
{
	int ret = 0;
	int val_len = 0;
	ql_wifi_state_e state = 0;
	ql_ping_ctx_t *ctx = NULL;
	ql_ping_cfg_t cfg = 
	{
		.pdpCid = 0xFF,
		.host = PING_HOST,
		.timeout = PING_TO_MS,
		.dataSize = PING_DATA_SIZE,
		.pingNum = PING_NUM,
		.thoughWiFi = true, //though serial wifi ping
	};

	val_len = sizeof(ql_wifi_state_e);
	// get wifi connect state
	ret = ql_wifi_opt_get(QL_WIFI_OPT_STATE, &state, &val_len);
	if (ret != QL_WIFI_SUCCESS || state != QL_WIFI_STATE_CONNECTED){
		QL_WIFI_DEMO_LOG("*** ql_wifi_ping, wifi not connected! ***\r\n");
	}
	
	ctx = ql_ping_init(cfg, wifi_demo_ping_cb);
	QL_WIFI_DEMO_LOG("*** ql_wifi_ping_cb %p ***\r\n", wifi_demo_ping_cb);
	
	if(ctx == NULL)
	{
		QL_WIFI_DEMO_LOG("*** ping wifi init fail! ***\r\n");
		return;
	}

	ql_rtos_semaphore_create(&ql_wifi_ping_sem,0);
	QL_WIFI_DEMO_LOG("*** begin ping wifi ***\r\n");
	ql_ping(ctx);
	ql_rtos_semaphore_wait(ql_wifi_ping_sem,QL_WAIT_FOREVER);
	QL_WIFI_DEMO_LOG("*** end ping wifi ***\r\n");
	ql_ping_deinit(ctx);
	ql_rtos_semaphore_delete(ql_wifi_ping_sem);
}

void wifi_get_ntp_callback(int ntp_status)
{
	QL_WIFI_DEMO_LOG("========== get ntp status:%d ...\r\n", ntp_status);
}

static void wifi_ntp_demo_test(void)
{
	int ret = 0;
	QL_WIFI_DEMO_LOG("========ql_ntp_server set========== !\n");
	ret = ql_ntp_set_server(WIFI_NTP_SERVER_DOMAIN);
	QL_WIFI_DEMO_LOG("========ql_ntp_client set:%d==server %s======== !\n", ret, WIFI_NTP_SERVER_DOMAIN);
	//though wifi do ntp
	ret = ql_ntp_set_though_swifi(TRUE);
	if(ret != QL_WIFI_SUCCESS){
		QL_WIFI_DEMO_LOG("ql_ntp_init failed!!\n");
	}
	//though wifi do ntp set pdp cid invaild; please keep use a free pdp index.
	ql_ntp_set_cid(16);
	QL_WIFI_DEMO_LOG("========ql_ntp_sync!!===== !\n");
	ql_ntp_sync_ex(wifi_get_ntp_callback);

	QL_WIFI_DEMO_LOG("========== NTP test  end ...\r\n");
}
static void wifi_demo_thread(void *arg)
{
	wifi_demo_manager_s *manager = &wifi_demo_manager;

	QL_WIFI_DEMO_LOG("wifi demo task start\r\n");
	int ret = 0;
	//uint8_t sim_id = QL_WIFI_DEMO_SIM_ID;
	int profile_idx = QL_WIFI_DEMO_PROFILE_IDX; // range 1 to 7

	char ssid_str[]     = {"HTI_Local"};
	char password_str[] = {"12345@hti"};
	int i;

	// Initialize client ctx
	manager->tcp_client_cnt = 0;
	for (i = 0; i < QL_WIFI_DEMO_TCP_CLIENTS_CNT; i++)
	{
		manager->tcp_client[i].id = -1;
		manager->tcp_client[i].fd = -1;
		manager->tcp_client[i].task = NULL;
	}

	// Create mutex
	ret = ql_rtos_mutex_create(&wifi_demo_mutex);
	if (ret != QL_OSI_SUCCESS){
		QL_WIFI_DEMO_LOG("create mutex ret: %x\r\n", ret);
		goto exit;
	}
	
	// Create timer
	//ret = ql_rtos_timer_create(&(manager->retry_timer), NULL, wifi_demo_retry_timer_callback, NULL);
	ret = ql_rtos_timer_create(&(manager->retry_timer));
	if (ret != QL_OSI_SUCCESS){
		QL_WIFI_DEMO_LOG("create timer ret: %x\r\n", ret);
		goto exit;
	}

	// Create message queue
	ret = ql_rtos_queue_create(&(manager->msg_queue), sizeof(wifi_demo_event_s), QL_WIFI_DEMO_MSG_QUEUE_CNT);
	if (ret != QL_OSI_SUCCESS){
		QL_WIFI_DEMO_LOG("msg queue ret: %x\r\n", ret);
		goto exit;
	}
	
	// wifi pwk supply init
	ret = ql_wifi_pwk_en_gpio_init();
	if (ret != QL_WIFI_APP_SUCCESS){
	    QL_WIFI_DEMO_LOG("ql_wifi_pwk_en_gpio_init err %x\r\n", ret);
	    goto exit;
	}

	// wifi init
	ret = ql_wifi_app_init(NULL);
	if (ret != QL_WIFI_APP_SUCCESS)
	{
	    QL_WIFI_DEMO_LOG("ql_wifi_app_init err %x\r\n", ret);
	    goto exit;
	}
	QL_WIFI_DEMO_LOG("ql_wifi_app_init done\r\n");
	// enable dhcp with WIFI STA module 
	char dhcp = 1;
	ret = ql_wifi_opt_set(QL_WIFI_OPT_DHCP, &dhcp, sizeof(char));
	if (ret != QL_WIFI_SUCCESS)
	{
	    QL_WIFI_DEMO_LOG("dhcp set err %x\r\n", ret);
	    goto exit;
	}
	QL_WIFI_DEMO_LOG("QL_WIFI_OPT_DHCP done\r\n");
	// ssid & password
	ql_wifi_app_user_ap_info_s *ap_user_info = (ql_wifi_app_user_ap_info_s *)calloc(1,sizeof(ql_wifi_app_user_ap_info_s));
	if(!ap_user_info)
	{
		QL_WIFI_DEMO_LOG("calloc faild!\r\n");
		goto exit;
	}
	int ssid_len = strlen(ssid_str);
	int password_len = strlen(password_str);
	if(ssid_len >= QL_WIFI_SSID_MAX_LEN || password_len >= QL_WIFI_PASS_MAX_LEN)
	{
		goto exit;
	}
	
	memcpy(ap_user_info->ssid,ssid_str,ssid_len);
	memcpy(ap_user_info->password,password_str,password_len);
	ret = ql_wifi_app_set_user_ap_info(ap_user_info);
	if(ret != QL_WIFI_APP_SUCCESS)
	{
		QL_WIFI_DEMO_LOG("get ssid/password faild!\r\n");
		free(ap_user_info);
		goto exit;
	}
	free(ap_user_info);

    wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_INIT, 0, 0, 0);
    while (1)
    {
        wifi_demo_event_s event = {0};
        if (QL_OSI_SUCCESS != ql_rtos_queue_wait(manager->msg_queue, (u8 *)&event, sizeof(wifi_demo_event_s), QL_WAIT_FOREVER))
        {
            QL_WIFI_DEMO_LOG("queue wait error\r\n");
            goto exit;
        }
        QL_WIFI_DEMO_LOG("wifi_demo_thread event [%x]\r\n", event.id);
        switch (event.id)
        {
        case WIFI_DEMO_EVENT_INIT: {
            // wifi software reset,better add hardware reset
            ret = ql_wifi_app_reset(NULL);
            if (ret != QL_WIFI_APP_SUCCESS)
            {
                QL_WIFI_DEMO_LOG("reset err %x\r\n", ret);
                goto exit;
            }
			//ql_wifi_app_spi_test(NULL);
            ql_rtos_task_sleep_ms(10);
            QL_WIFI_DEMO_LOG("wifi init\r\n");
            wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_TRY_CONNECT, 0, 0, 0);
            break;
        }
        case WIFI_DEMO_EVENT_TRY_CONNECT: {
            manager->retry_cnt++;
            if (manager->retry_cnt >= QL_WIFI_DEMO_RETRY_CNT_MAX)
            {
                manager->retry_cnt = 0;
                wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_INIT, 0, 0, 0);
                break;
            }
            // start timer
            ret = ql_rtos_timer_start(manager->retry_timer, QL_WIFI_DEMO_DISCONNECT_RETRY_TIME, 0, wifi_demo_retry_timer_callback, NULL);
            QL_WIFI_DEMO_LOG("timer start %x\r\n", ret);
            if (ret != QL_OSI_SUCCESS)
            {
                QL_WIFI_DEMO_LOG("manager->retry_timer start failed\r\n");
                wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_TRY_CONNECT, 0, 0, 0);
            }

            QL_WIFI_DEMO_LOG("wifi connect start\r\n");
            // start to connect wifi
            ret = ql_wifi_app_sta_disable(NULL);
            if (ret != QL_WIFI_APP_SUCCESS)
            {
                QL_WIFI_DEMO_LOG("sta disable err %x\r\n", ret);
                goto exit;
            }
            ql_rtos_task_sleep_ms(10);
            ret = ql_wifi_app_sta_enable(wifi_demo_connect_status);
            if (ret != QL_WIFI_APP_SUCCESS)
            {
                QL_WIFI_DEMO_LOG("sta enable err %x\r\n", ret);
                goto exit;
            }
            QL_WIFI_DEMO_LOG("ql_wifi_app_sta_enable done\r\n");
            // Set retry_state end.When we get disconnect status,
            // we check this state for ignoring to send WIFI_DEMO_EVENT_TRY_CONNECT again.
            manager->retry_state = WIFI_DEMO_RETRY_CONNECT_END;
            break;
        }
		// for cust test
		case WIFI_DEMO_EVENT_CONNECTED: {
			int val_len = sizeof(ip_addr_t);
			ip_addr_t ip = {0};
			ip_addr_t gw = {0};
			ip_addr_t nm = {0};
			ql_wifi_dns_s dns = {0};
			ql_wifi_state_e state = 0;
			val_len = sizeof(ql_wifi_state_e);
			// get wifi connect state
			ret = ql_wifi_opt_get(QL_WIFI_OPT_STATE, &state, &val_len);
			if (ret == QL_WIFI_SUCCESS && state == QL_WIFI_STATE_CONNECTED){
				//ip info get
				val_len = sizeof(ip_addr_t);
				ret = ql_wifi_opt_get(QL_WIFI_OPT_IP, &ip, &val_len);
				if (ret == QL_WIFI_SUCCESS){
					QL_WIFI_DEMO_LOG("get wifi net ip [%04x]\r\n", ip.addr);
				}
				val_len = sizeof(ip_addr_t);
				ret = ql_wifi_opt_get(QL_WIFI_OPT_GW, &gw, &val_len);
				if (ret == QL_WIFI_SUCCESS){
					QL_WIFI_DEMO_LOG("get wifi net gw [%04x]\r\n", gw.addr);
				}
				val_len = sizeof(ip_addr_t);
				ret = ql_wifi_opt_get(QL_WIFI_OPT_NETMASK, &nm, &val_len);
				if (ret == QL_WIFI_SUCCESS){
					QL_WIFI_DEMO_LOG("get wifi net netmask [%04x]\r\n", nm.addr);
				}
				val_len = sizeof(ql_wifi_dns_s);
				ret = ql_wifi_opt_get(QL_WIFI_OPT_DNS, &dns, &val_len);
				if (ret == QL_WIFI_SUCCESS){
					QL_WIFI_DEMO_LOG("get wifi net dns1 [%04x] dns2 [%04x]\r\n ", dns.dns_server[0][0].addr,dns.dns_server[0][1].addr);
				}
				// dns qurey though serial wifi 
				wifi_demo_dns_query("www.qq.com");
				wifi_demo_dns_query("www.163.com");
				wifi_demo_dns_query("www.quectel.com");
				// ping test
				wifi_demo_ping_test();
				// ntp test
				wifi_ntp_demo_test();
				ql_rtos_task_sleep_s(10);
				wifi_ntp_demo_test();
				ql_rtos_task_sleep_s(10);
				wifi_ntp_demo_test();
				ql_rtos_task_sleep_s(10);
				wifi_ntp_demo_test();
				// tcp test 
				wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_APP_TCP_CONNECT, 0, 0, 0);
				
			}
			break;
		}
		case WIFI_DEMO_EVENT_APP_TCP_CONNECT: {
			ql_rtos_mutex_try_lock(wifi_demo_mutex);
			wifi_demo_tcp_client_create();
			ql_rtos_mutex_unlock(wifi_demo_mutex);
			break;
		}
		case WIFI_DEMO_EVENT_GET_STATUS: {
			uint32_t value = event.param1;
			if (value == QL_WIFI_STATE_CONNECTED){
				manager->retry_state = WIFI_DEMO_RETRY_CONNECT_END;
				ql_rtos_timer_stop(manager->retry_timer);
				//From others to QL_WIFI_STATE_CONNECTED
				if(manager->state != QL_WIFI_STATE_CONNECTED)
				{
					wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_CONNECTED, 0, 0, 0);
				}
			}
			else if (value == QL_WIFI_STATE_DISCONNECT)
			{
				if (manager->retry_state == WIFI_DEMO_RETRY_CONNECT_END)
				{
					int ret = 0;
					manager->retry_state = WIFI_DEMO_RETRY_CONNECT_START;
					ql_rtos_timer_stop(manager->retry_timer);

					ret = ql_rtos_timer_start(manager->retry_timer, QL_WIFI_DEMO_DISCONNECT_RETRY_TIME, 0, wifi_demo_retry_timer_callback, NULL);
					QL_WIFI_DEMO_LOG("timer start %x\r\n", ret);
					if (ret != QL_OSI_SUCCESS){
						QL_WIFI_DEMO_LOG("manager->retry_timer start failed\r\n");
						wifi_demo_send_event(manager->task, WIFI_DEMO_EVENT_TRY_CONNECT, 0, 0, 0);
					}
				}
			}
				manager->state = value;
				manager->retry_cnt = 0;
				break;
		}
			default: {
				break;
			}
		}
	}
exit:
	if (manager->retry_timer){
		ql_rtos_timer_delete(manager->retry_timer);
	}
	if (manager->msg_queue)
	{
		ql_rtos_queue_delete(manager->msg_queue);
		manager->msg_queue = NULL;
	}
	if(wifi_demo_mutex){
		ql_rtos_mutex_delete(wifi_demo_mutex);
		wifi_demo_mutex = NULL;
	}

	QL_WIFI_DEMO_LOG("wifi demo task end\r\n");
	ql_rtos_task_delete(NULL);
}

int ql_wifi_demo_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	wifi_demo_manager_s *manager = &wifi_demo_manager;
	ql_log_mask_set(QL_LOG_APP_MASK, QL_LOG_PORT_USB);
	//ql_rtos_task_sleep_s(20);//for debug trace only
	
	err = ql_rtos_task_create(&(manager->task), 4 * 1024, APP_PRIORITY_NORMAL, "wifi_demo", wifi_demo_thread, NULL, 5);
	if (err != QL_OSI_SUCCESS){
		QL_WIFI_DEMO_LOG("task created failed\r\n");
	}
	return 0;
}

application_init(ql_wifi_demo_init, "ql_wifi_demo_init", 2, 0);


