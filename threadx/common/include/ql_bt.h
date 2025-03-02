/*============================================================================
  Copyright (c) 2022 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/

#ifndef _QL_BT_H
#define _QL_BT_H
/*> include header files here*/
#include "ql_type.h"


#ifdef __cplusplus
extern "C" {
#endif
#define MAX_BT_NAME                 248//48//forrest.liu@20210315 modify for low layer define update

#define BT_MODE_BR                  0
#define BT_MODE_BR_LE               1
#define BT_MODE_LE                  2

#define BT_HCI_MODE_H4              0
#define BT_HCI_MODE_H5              1

#define BT_FIRMWARE_AT_SD           0
#define BT_FIRMWARE_AT_FLASH        1
#define BT_FIRMWARE_AT_MEMORY       2

#define BAUDRATE_9600               9600
#define BAUDRATE_14400      		14400
#define BAUDRATE_19200      		19200
#define BAUDRATE_38400      		38400
#define BAUDRATE_57600      		57600
#define BAUDRATE_115200     		115200
#define BAUDRATE_230400     		230400
#define BAUDRATE_460800     		460800
#define BAUDRATE_921600     		921600
#define BAUDRATE_1842000    		1842000
#define BAUDRATE_3000000    		3000000
#define BAUDRATE_3686400    		3686400
#define MAX_BT_OBEX_FILE_NAME       256
#define MAX_BT_OBEX_FILE_TYPE_SIZE  32

typedef enum
{
  BT_PARITY_NONE = 0,
  BT_PARITY_ODD = 1,
  BT_PARITY_EVEN = 3,
} BT_PARITY_TYPE;

typedef enum
{
  BT_DATA_5 = 0,
  BT_DATA_6 = 1,
  BT_DATA_7 = 2,
  BT_DATA_8 = 3,
} BT_DATA_WIDTH;
  
typedef struct bt_addr {
    unsigned char bytes[6];
}bt_addr_struct;

struct bt_device_record {
    char name[MAX_BT_NAME];
    unsigned int cod;
    unsigned char addr[6];
    unsigned char linkkey[16];
    int linkkey_type;
    int hfp_volume;
    int a2dp_volume;
    struct bt_device_record *next;
};

struct bt_event_inquiry {
    char name[MAX_BT_NAME];
    int length;
    unsigned int cod;
    unsigned char addr[6];
    char rssi;
};
struct bt_task_obex_file_send {
    unsigned int total;
    unsigned char *data;
    unsigned int payload_size;
    char type[MAX_BT_OBEX_FILE_TYPE_SIZE];
    char name[MAX_BT_OBEX_FILE_NAME]; // name: unicode type, should be uint16
    unsigned int name_size;
    int final;
    int tid;
};

typedef enum
{
  BT_CONNECT = 0,
  BT_DISCONNECT = 1,
  BLE_CONNECT = 2,
  BLE_DISCONNECT = 3,
} ql_bt_connect_status;

typedef int (*BT_UartIsrCb)(UINT32 port_id);

typedef enum
{
    bt_int_none = 0,
    bt_rx_int,
    bt_tx_int,
    bt_eor_int,
    bt_all_int,
    bt_int_max,
}bt_uartirqtype;

struct appbt_a2dp {
    int sample;
    int channel;
    void (*notify_upper_layer_send_data)(void);
};

typedef struct
{
  UINT32 port_id;
  UINT32 fifo;
  UINT32 dma;
  UINT32 loopback;
  bt_uartirqtype int_tpye;
  UINT32 highspeed;
  UINT32 flowctrl;
  UINT32 baudrate;
  UINT32 datawidth;
  UINT32 parity;
  BT_UartIsrCb rx_callback;
  BT_UartIsrCb tx_callback;
}bt_uart_cfg;

typedef struct _ble_device_info_record{
	unsigned char valid;
	unsigned char address[6];
	unsigned char addr_type;
	unsigned short deviceState;
	unsigned char ltk[16];
	unsigned char keyType;
	unsigned short ediv;
	unsigned char rand[8];
    unsigned char enc_size;
	unsigned char csrk[16];
	unsigned char peer_csrk[16];
	unsigned char signCounter;
	unsigned char irk[16];
	unsigned char peer_irk[16];
}__attribute__((packed)) ble_device_info_record;

struct bt_hardware_cfg {
    int baudrate; // 115200, 921600, 3000000
    bt_uart_cfg *uart_cfg;
    int reset_pin; // -1: not reset BT chip through reset PIN
    int pwdn_pin; // -1: no PWDN pin
    int bt_wakeup_host_pin; // -1: not use wakeup PIN
    int host_wakeup_bt_pin; // -1: not use wakeup PIN
    unsigned int clock32;
};
struct bt_br_common_cfg {
    char name[MAX_BT_NAME];
    unsigned char addr[6];
    unsigned char discoverable; // 0: cannot be searched by other device; 1: can be searched
    unsigned char connectable; // 0: non-connectable; 1: connectable
    unsigned int cod;
    unsigned char io_capability; // 0: DisplayOnly; 1: DisplayYesNo; 2: KeyboardOnly; 3: NoInputNoOutput
    unsigned short int inquiry_scan_interval;
    unsigned short int inquiry_scan_window;
    unsigned short int page_scan_interval;
    unsigned short int page_scan_window;
    /*Note:now only hfp hf support vohci*/
    unsigned char vohci; //0:voice over pcm; 1: voice over hci
    unsigned char reserved[20]; //add reserved buffer here
};


typedef unsigned short (*att_read_callback_t)(unsigned short con_handle, unsigned short attribute_handle, unsigned short offset, unsigned char * buffer, unsigned short buffer_size);
typedef int (*att_write_callback_t)(unsigned short con_handle, unsigned short attribute_handle, unsigned short transaction_mode, unsigned short offset, unsigned char *buffer, unsigned short buffer_size);

struct bt_le_common_cfg {
    unsigned char random_address[6];
    unsigned char smp_io_capability; // 0: DisplayOnly; 1: DisplayYesNo; 2: KeyboardOnly; 3: NoInputNoOutput
    unsigned char smp_secure_connection;
};



struct bt_profile_cfg {
    int hfp; // 0: not have HFP; 1: have HFP profile
    int a2dp; // 0: not have A2DP; 1: have A2DP profile
    int avrcp; // 0: not have AVRCP; 1: have AVRCP profile
    int spp; // 0: not have SPP; 1: have SPP profile
    int obex; // 0: not have OBEX; 1: have OBEX profile
    int pbap; // 0: not have PBAP; 1: have phone book profile
    int hfp_hf;
    int hid;
    //headset profile
    int a2dp_sink;
    unsigned char map;
    unsigned char reserved[19]; //add reserved buffer here
};

struct bt_user_cfg {
    struct bt_hardware_cfg hw;
    struct bt_br_common_cfg br_common;
    struct bt_le_common_cfg le_common;
    struct bt_profile_cfg profile;
    struct bt_device_record *record;
    int firmware_where; // 0: SD; 1: flash; 2: memory
    unsigned char *bt_firmware;
    int bt_firmware_size;
    unsigned char *bt_nvm_lst;
    int bt_nvm_lst_size;
    int h4_h5; // 0: h4; 1: h5.
    int bt_mode; // 0: br only; 1: br + le; 2: le only
    int bt_chip_id; // 5801, 5803
    int xip;
	ble_device_info_record *ble_rec;
};

struct bt_user_init_cfg {
     char name[MAX_BT_NAME];
	unsigned char io_capability; // 0: DisplayOnly; 1: DisplayYesNo; 2: KeyboardOnly; 3: NoInputNoOutput
	unsigned short int inquiry_scan_interval;
    unsigned short int inquiry_scan_window;
	unsigned short int page_scan_interval;
    unsigned short int page_scan_window;
	unsigned int cod;
};


struct ql_bt_task_le_adv_parameters {
    unsigned short int interval_min;
    unsigned short int interval_max;
    unsigned char advertising_type; // LE_ADV_TYPE_XXX
    unsigned char own_address_type;
    unsigned char peer_address_type;
    struct bt_addr peer_address;
    unsigned char filter; /* 0x00: process scan and connection request from all devices
                             0x01: process connection request from all devices
                                   and scan request only from White List
                             0x02: process scan request from all devices
                                   and conneciton request only from White List
                             0x03: process scan and connection reques only from in the White List
                          */
};

struct bt_task_event {
    unsigned short int event_type;
    unsigned short int event_id;
    int payload_length;
    void *payload;
};

typedef void (*bt_event_handle_t)(struct bt_task_event *event);

typedef void (*ql_cb_bt_open_status)(int result);
typedef void (*ql_cb_bt_scan_status)(struct bt_event_inquiry *inquiry,int result);
typedef void (*ql_cb_bt_connect_status)(int result);


struct bt_task_command {
    int command_id;
    void (*execute_cb)(void *arg);
    int payload_length;
    void *payload;
};




#define LE_HIDS_BCDHID		                0x1011
enum
{
	BOOT_MODE = 0,
	REPORT_MODE,
};

#define LE_ERR_OK                       0x00
#define LE_ERR_INVALID_HANDLE           0x01
#define LE_ERR_READ_NOT_PERMITTED       0x02
#define LE_ERR_WRITE_NOT_PERMITTED      0x03
#define LE_ERR_INVALID_PDU              0x04
#define LE_ERR_INSUFFICIENT_AUTHEN      0x05
#define LE_ERR_REQUEST_NOT_SUPPORT      0x06
#define LE_ERR_INVALID_OFFSET           0x07
#define LE_ERR_INSUFFICIENT_AUTHOR      0x08
#define LE_ERR_PREPARE_QUEUE_FULL       0x09
#define LE_ERR_ATTRIBUTE_NOT_FOUND      0x0A
#define LE_ERR_ATTRIBUTE_NOT_LONG       0x0B
#define LE_ERR_INSUFFICIENT_ENCRY_KEY   0x0C
#define LE_ERR_INVALID_ATTRIBUTE_VALUE  0x0D
#define LE_ERR_UNLIKELY_ERROR           0x0E
#define LE_ERR_INSUFFICIENT_ENCRY       0x0F
#define LE_ERR_UNSUPPORTED_GTOUP_TYPE   0x10
#define LE_ERR_INSUFFICIENT_RESOURCES   0x11
	
#define LE_DEVICE_TYPE_CLASSIC          1
#define LE_DEVICE_TYPE_DUAL             3
#define LE_DEVICE_TYPE_LE               2
#define LE_DEVICE_TYPE_UNKNOWN          0
	
#define LE_ADV_TYPE_IND                 0x00
#define LE_ADV_TYPE_DIRECT_IND          0x01
#define LE_ADV_TYPE_SCAN_IND            0x02
#define LE_ADV_TYPE_NONCONN_IND         0x03
#define LE_ADV_TYPE_DIRECT_IND_LOW_DUTY 0x04
	
#define LE_EXT_ADV_PROPERTIES_BIT_CONNECTABLE   0
#define LE_EXT_ADV_PROPERTIES_BIT_SCANNABLE     1
#define LE_EXT_ADV_PROPERTIES_BIT_DIRECTED      2
#define LE_EXT_ADV_PROPERTIES_BIT_HIGH_DUTY_DIRECTED_CONNECTABLE  3
#define LE_EXT_ADV_PROPERTIES_BIT_LEGACY        4
#define LE_EXT_ADV_PROPERTIES_BIT_OMIT          5
#define LE_EXT_ADV_PROPERTIES_BIT_TXPOWER       6
	
#define LE_ADDRESS_TYPE_PUBLIC          0x00
#define LE_ADDRESS_TYPE_RANDOM          0x01
	
#define LE_GAP_TYPE_FLAGS                       0x01
#define LE_GAP_TYPE_INCOMPLETE_SERVICE_16BIT_LIST       0x02
#define LE_GAP_TYPE_COMPLETE_SERVICE_LIST       0x03
#define LE_GAP_TYPE_SHORT_NAME                  0x08
#define LE_GAP_TYPE_COMPLETE_NAME               0x09
#define LE_GAP_TYPE_TX_POWER                    0x0A
#define LE_GAP_TYPE_SERVICE_DATA                0x16
#define LE_GAP_TYPE_APPEARANCE                  0x19
#define LE_GAP_TYPE_SPECIFIC_DATA               0xFF
	
#define LE_PASSIVE_SCAN                 0x00
#define LE_ACTIVE_SCAN                  0x01
	
#define LE_ATT_MASTER                   0
#define LE_ATT_SLAVE                    1
	
#define LE_ATT_CHARC_PROP_READ          0x02
#define LE_ATT_CHARC_PROP_WWP           0x04
#define LE_ATT_CHARC_PROP_WRITE         0x08
#define LE_ATT_CHARC_PROP_NOTIFY        0x10
#define LE_ATT_CHARC_PROP_INDICATE      0x20
	
#define LE_ATT_PM_READABLE              0x0001
#define LE_ATT_PM_WRITEABLE             0x0002
#define LE_ATT_PM_AUTHENT_REQUIRED      0x0004
#define LE_ATT_PM_AUTHORIZE_REQUIRED    0x0008
#define LE_ATT_PM_ENCRYPTION_REQUIRED   0x0010
#define LE_ATT_PM_AUTHENT_MITM_REQUERED 0x0020
	
#define LE_ATT_UUID_PRIMARY             0x2800
#define LE_ATT_UUID_CHARC               0x2803
	
	// standard GATT descriptors
#define LE_ATT_UUID_CLIENT_CHARC_CONFIG 0x2902
#define LE_ATT_UUID_CHAR_PRESENT_FORMAT 0x2904
#define LE_ATT_UUID_EXTERNAL_REF        0x2907
#define LE_ATT_UUID_REPORT_REF          0x2908
	
	
#define LE_ATT_UUID_IMMEDIATE_ALERT     0x1802
#define LE_ATT_UUID_DEVICE_INFO         0x180A
#define	LE_ATT_UUID_BATTERY_SERVICE		0x180F
#define	LE_ATT_UUID_HID					0x1812
#define LE_ATT_UUID_MESH_PRO_SERVICE    0x1827
#define LE_ATT_UUID_MESH_PROXY_SERVICE  0x1828
	
	
#define LE_ATT_UUID_ALERT_LEVEL         0x2A06
#define LE_ATT_UUID_BATTERY   			0x2A19
#define LE_ATT_UUID_HIDBOOT_KB_INPUT	0x2A22
#define LE_ATT_UUID_HIDBOOT_KB_OUTPUT	0x2A32
#define LE_UUID_HIDBOOT_MOUSE_INPUT	    0x2A33
	
#define LE_ATT_UUID_HIDINFO   			0x2A4A
#define LE_ATT_UUID_HIDREPORT_MAP		0x2A4B
	
#define LE_ATT_UUID_HIDCONTROLPOINT   	0x2A4C
#define LE_ATT_UUID_PROTOCOL   		    0x2A4E
	
#define LE_ATT_UUID_HIDREPORT   		0x2A4D
#define LE_ATT_UUID_MANUFACTURER_NAME   0x2A29
#define LE_ATT_UUID_MESH_PRO_DATA_IN    0x2ADB
#define LE_ATT_UUID_MESH_PRO_DATA_OUT   0x2ADC
#define LE_ATT_UUID_MESH_PROXY_DATA_IN  0x2ADD
#define LE_ATT_UUID_MESH_PROXY_DATA_OUT 0x2ADE
#define LE_ATT_UUID_PNP_ID   			0x2A50
	
#define LE_CCC_BIT_INDICATED            0x02
#define LE_CCC_BIT_NOTIFIED             0x01
	
#define LE_ATT_UUID_SYSTEM_ID           0x2A23
#define LE_ATT_UUID_MODEL_NUMBER        0x2A24
#define LE_ATT_UUID_SERIAL_NUMBER       0x2A25
#define LE_ATT_UUID_FIRMWARE_VERSION    0x2A26
#define LE_ATT_UUID_HARDWARE_VERSION    0x2A27
#define LE_ATT_UUID_SOFTWARE_VERSION    0x2A28
#define LE_ATT_UUID_MANUFACTURER_NAME   0x2A29
#define LE_ATT_UUID_IEEE_LIST           0x2A2A
#define LE_ATT_UUID_PNP_ID   			0x2A50
	
#define LE_ATT_UUID_MESH_PRO_DATA_IN    0x2ADB
#define LE_ATT_UUID_MESH_PRO_DATA_OUT   0x2ADC
#define LE_ATT_UUID_MESH_PROXY_DATA_IN  0x2ADD
#define LE_ATT_UUID_MESH_PROXY_DATA_OUT 0x2ADE
#define LE_SUPPORT

struct le_uuid {
    unsigned char type;
};

struct le_uuid_16 {
    struct le_uuid uuid;
    unsigned short value;
};

struct le_uuid_32 {
    struct le_uuid uuid;
    unsigned int value;
};

struct le_uuid_128 {
    struct le_uuid uuid;
    unsigned char value[16];
};



#pragma pack(1)
struct gatt_chrc {
    unsigned char properties;
    unsigned short handle;
    unsigned short uuid;
};


struct gatt_chrc128 {
    unsigned char properties;
    unsigned short handle;
    unsigned char uuid[16];
};
#pragma pack()

typedef struct
{
	unsigned short configurationBits;
	struct bt_addr addr;
}gatt_chara_ccb_ex_t;



//forrest.liu@20201204 add for 010 new change
struct gatt_attr {
    struct le_uuid *uuid;
    void *value;
    int size;
    unsigned short handle;
    unsigned char permission;
    int (*write_cb)(void *arg);
    int (*read_cb)(void *arg);
};

struct gatt_attr_rw_parameters {
    unsigned short handle;
    unsigned short length; /* how many bytes want read or write */
    unsigned short offset; /* read write from where */
    unsigned char *data; /* for write operation: this is contains the data to be written; 
                            for read operation: user should alloc memory and fill data to here, then return buffer size*/
};
							
struct bt_task_le_update_attribute {
	void *attribute;
};

							
struct bt_task_le_notify_indicate {
	unsigned short acl;
	unsigned short att;
	unsigned char *data;
	int size;
};

struct bt_event_le_common_rsp {
	unsigned char size;
	unsigned char value[255];
};
struct bt_event_le_read_rsp {
	unsigned char size;
	unsigned char value[255];
};
struct bt_event_le_client_handle_notify {
	unsigned short acl_handle;
	unsigned short handle;
	unsigned char value[255];
	int size;
};
struct bt_event_le_client_handle_indication {
	unsigned short acl_handle;
	unsigned short handle;
	unsigned char value[255];
	int size;
};

struct le_client_status {
	int handle;
	struct bt_addr addr;
};
struct le_uuid_common {
	unsigned char type;

	union {
		struct le_uuid_16 uuid_16;
		struct le_uuid_32 uuid_32;
		struct le_uuid_128 uuid_128;
	} u;
};

struct le_list {
	struct le_list *next;
	void *data;
};
enum ciev_index {
    CIEV_CALL = 0,
    CIEV_CALLSETUP, /* 0: means not currently in call setup.
                       1: means an incoming call process ongoing
                       2: means an outgoing call setup is ongoing (outgoing call setup in dialing state)
                       3: means remote party being alerted in an outgoing call (outgoing call setup in alerting state) */
    CIEV_SERVICE,
    CIEV_SIGNAL,
    CIEV_ROAMING,
    CIEV_BATTERY_CHAGER,
    CIEV_CALLHELD, /* 0: no call on hold
                      1: a call is placed on hold or active/held calls swapped
                      2: call on hold, no call active */
};

struct gatt_characteristic_record {
	unsigned short handle;
	struct {
		unsigned short handle;
		unsigned char propertis;
		struct le_uuid_common uuid;
	} propertis;
	unsigned short ccc;
	struct gatt_characteristic_record *next;
};

struct gatt_service_record {
	int type; // primary or not
	unsigned short start_handle;
	unsigned short end_handle;
	struct le_uuid_common uuid;
	struct gatt_characteristic_record *characteristic;
	struct gatt_service_record *next;
};

#define LE_UUID_TYPE_16     (2)
#define LE_UUID_TYPE_32     (4)
#define LE_UUID_TYPE_128    (16)

extern struct le_uuid_16 LE_UUID_PERIMARY_SERVICE;

extern struct le_uuid_16 LE_UUID_CLIENT_CHARC_CONFIG;

extern struct le_uuid_16 LE_UUID_CHARACTERISTIC;

extern struct le_uuid_16 LE_UUID_REPORT_REF_CHARC_CONFIG;

extern struct le_uuid_16 LE_UUID_CHAR_PRESENTATION_FORMAT;


#define LE_DECLARE_PRIMARY_SERVICE(_UUID_, _UUID_TYPE_)  \
    {.uuid = (struct le_uuid *)&LE_UUID_PERIMARY_SERVICE, \
     .value = (void *)&(_UUID_.value), .size = _UUID_TYPE_, \
     .permission = LE_ATT_PM_READABLE}
//forrest.liu@20201204 add for 010 new change
#define LE_DECLARE_CHARACTERISTIC(_CHARACTERISTIC_, _UUID_, _PERMISSION_, _WCB_, _RCB_, _VALUE_, _VALUE_SIZE_) \
    {.uuid = (struct le_uuid *)&LE_UUID_CHARACTERISTIC, \
     .value = (void *)&_CHARACTERISTIC_, \
     .size = sizeof(_CHARACTERISTIC_), \
     .permission = LE_ATT_PM_READABLE}, \
    {.uuid = (struct le_uuid *)&_UUID_, \
     .value = (void *)_VALUE_, .size = _VALUE_SIZE_, \
     .permission = _PERMISSION_, \
     .write_cb = _WCB_, .read_cb = _RCB_}

#define LE_DECLARE_CLINET_CHRAC_CONFIG(_VALUE_, _WCB_) \
    {.uuid = (struct le_uuid *)&LE_UUID_CLIENT_CHARC_CONFIG, \
     .value = (void *)&_VALUE_, \
     .size = sizeof(_VALUE_), \
     .permission = LE_ATT_PM_READABLE | LE_ATT_PM_WRITEABLE, \
     .write_cb = _WCB_}

#define LE_DECLARE_REPORT_REF_CHRAC_CONFIG(_VALUE_, _RCB_, SIZE) \
    {.uuid = (struct le_uuid *)&LE_UUID_REPORT_REF_CHARC_CONFIG, \
     .value = (void *)_VALUE_, \
     .size = SIZE, \
     .permission = LE_ATT_PM_READABLE, \
     .read_cb = _RCB_}


#define LE_DECLARE_REPORT_PRESENTATION_FORMAT(_VALUE_, _RCB_, SIZE) \
    {.uuid = (struct le_uuid *)&LE_UUID_CHAR_PRESENTATION_FORMAT, \
     .value = (void *)_VALUE_, \
     .size = SIZE, \
     .permission = LE_ATT_PM_READABLE, \
     .read_cb = _RCB_}

struct ql_bt_task_le_ext_adv_parameters {
    unsigned char handle;
    unsigned short int properties;
    unsigned int interval_min;
    unsigned int interval_max;
    unsigned char channel;
    unsigned char own_address_type;
    unsigned char peer_address_type;
    struct bt_addr peer_address;
    unsigned char filter_policy;
    unsigned char tx_power;
    unsigned char primary_phy;
    unsigned char secondary_adv_max_skip;
    unsigned char secondary_phy;
    unsigned char sid;
    unsigned char scan_request_notification;
};

struct bt_event_le_scan_event {
    unsigned char event_type;
    unsigned char address_type;
    struct bt_addr address;
    unsigned char length;
    unsigned char data[31];
    char rssi;
};

typedef void (*le_scan_event_handle_t)(struct bt_event_le_scan_event *event);

/*****************************************************************
������ql_bt_open
������������
������handler :����һ������� �ײ��ϱ�����Ļص�
      init_cfg:  ��ʹ��
      cfg_confg: ���ô�����ʱ�ĸ�������
����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_open(bt_event_handle_t handler, struct bt_user_init_cfg *init_cfg, struct bt_user_cfg *cfg_confg);

/*****************************************************************
������ql_bt_close
�������ر�����
������NULL


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_close(void);

/*****************************************************************
������ql_bt_get_bt_address
��������ȡ������ַ
������addr:������ַ���λ��


����ֵ:
      NULL
*****************************************************************/
void ql_bt_get_bt_address(struct bt_addr* addr);

/*****************************************************************
* Function: ql_bt_acl_connect
*
* Description:
*	Setup a acl connect
*	
* Parameters:
*	addr			[in]		Bluetooth remote address	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_acl_connect(struct bt_addr addr);


/*****************************************************************
* Function: ql_bt_cancel_acl_connect
*
* Description:
*	Setup a acl connect
*	
* Parameters:
*	addr			[in]		Bluetooth remote address	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/

int ql_bt_cancel_acl_connect(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_acl_disconnect
*
* Description:
*	Disconnect a acl connection
*	
* Parameters:
*	handle			[in]		acl handle	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_acl_disconnect(unsigned short int handle);

/*****************************************************************
������ql_bt_set_bt_address
����������������ַ
������addr:Ҫ���õ�������ַ
      ע��:������ǰ���ô�������ֱ��ʹ�� �����򿪺����� �����������Ч


����ֵ:
      NULL
*****************************************************************/
void ql_bt_set_bt_address(struct bt_addr* addr);

/*****************************************************************������ql_bt_inquiry
������������������
������inquiry_length:����ʱ������
	  num_responses: ������������

����ֵ:
      0      sucess
      ����     failed
*********************
*****************************************************************/
int ql_bt_inquiry(unsigned char inquiry_length, unsigned char num_responses);

/*****************************************************************
������ql_bt_inquiry_cancel
�������ر���������
������NULL


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_inquiry_cancel(void);

/*****************************************************************
������ql_bt_connect_headset
��������������
������addr:Ŀ���豸�ĵ�ַ
	  call_status:����״̬

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_connect_headset(struct bt_addr addr, int call_status);

/*****************************************************************
������ql_bt_set_visiable
���������������Ƿ�ɼ�
������visiable: 1--�ɼ�  0--���ɼ�


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_set_visiable(int visiable);
/*****************************************************************
* Function: ql_bt_bonding
*
* Description:
* 	Make a pair to target device
* 	
* Parameters:
* 	addr		[in]	Bluetooth Device Address
*   
* Return:
* 	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_bonding(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_unbond
*
* Description:
* 	Unbind the target device
* 	
* Parameters:
* 	addr			[in]		Bluetooth Device Address
*
* Return:
* 	0						success
*	-1						fail	
*
*****************************************************************/
int ql_bt_unbond(struct bt_addr addr);
/*****************************************************************
* Function:  ql_bt_pin_negative_reply
*
* Description:
* 	Negative reply PIN to target device
*
* Parameters:
*	addr			[in]		Bluetooth Device Address
*								
* Return:
*	NULL
*
*****************************************************************/
int ql_bt_pin_negative_reply(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_spp_connect
*
* Description:
*	Spp connection function
*	An event will be reported if the connection is successful
*	BTTASK_IND_SPP_CONNECT_CNF
* 
* Parameters:
*	addr			[in]		Target device address
*	
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_spp_connect(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_obex_send_file
*
* Description:
*	Send file to target device
*	
* Parameters:
*	file			[in]		field name pointer
*				
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_obex_send_file(struct bt_task_obex_file_send *file);

/*****************************************************************
* Function: setup a OBEX connect
*
* Description:
*	Send audio data to target device
*	
* Parameters:
*	addr			[in]		address of target device
*					
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/

 int ql_bt_obex_connect(struct bt_addr addr);
/******************************************************************
*	 AVRCP
******************************************************************/

/*****************************************************************
* Function: ql_bt_avrcp_connect
*
* Description:
*	Setup AVRCP connect
*	
* Parameters:
*	addr			[in]		address of target device
*				
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
 int ql_bt_avrcp_connect(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_avrcp_disconnect
*
* Description:
*	Disconnect a AVRCP connection
*	
* Parameters:
*	NULL
*	
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
 int ql_bt_avrcp_disconnect(void);
/*****************************************************************
* Function: ql_bt_switch_voice_to_headset
*
* Description:
*	Switch channel to headset
*	
* Parameters:
*	NULL
*				
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_switch_voice_to_headset(void);
/*****************************************************************
* Function: ql_bt_switch_voice_to_phone
*
* Description:
*	Switch channel to mobile phone
*	
* Parameters:
*	NULL
*			
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_switch_voice_to_phone(void);

/*****************************************************************
������ql_bt_send_bttask_command
��������ײ㷢�Ͷ�Ӧ��command(������ʹ��)
������command:��Ϻ��commandָ��


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_send_bttask_command(struct bt_task_command * command);

/*****************************************************************
������ql_bt_accept_bonding
��������ײ㷢�Ͷ�Ӧ��command(������ʹ��)
������addr:Ŀ���豸��mac��ַ
	  accept_reject:

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_accept_bonding(struct bt_addr addr, int accept_reject);

/*****************************************************************
������ql_bt_sort_device_record
���������豸��������
������addr:Ŀ���豸��mac��ַ


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
void ql_bt_sort_device_record(struct bt_addr addr);

/*****************************************************************
������ql_bt_a2dp_connected_handler
����������a2dpЭ��
������addr:Ŀ���豸��mac��ַ


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
void ql_bt_a2dp_connected_handler(unsigned char *addr);

/*****************************************************************
* Function: ql_bt_a2dp_connect
*
* Description:
*	Setup a a2dp connect
*	
* Parameters:
*	addr			[in]		Bluetooth remote address	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_a2dp_connect(struct bt_addr addr);

/*****************************************************************
������ql_bt_hfp_accept_connection
����������hfpЭ��
������call_status:����״̬�ص�
	  addr:Ŀ���豸��mac��ַ


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_hfp_accept_connection(int call_status, struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_a2dp_SBC_test_send_message
*
* Description:
*	Subband coded transmission data
*	
* Parameters:
*	msg_id			[in]		Custom message flag bit	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_a2dp_SBC_test_send_message(unsigned int msg_id);

/*****************************************************************
* Function: ql_bt_a2dp_disconnect
*
* Description:
*	Disconnect a a2dp connection
*	
* Parameters:
*	NULL
*								
* Return:
*	NULL
*
*****************************************************************/
 int ql_bt_a2dp_disconnect(void);

/*****************************************************************
* Function: ql_bt_a2dp_send_sbc_data
*
* Description:
*	Send SBC data to target device
*	
* Parameters:
*	a2dp			[in]		Audio information settings	
*								
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_a2dp_send_sbc_data(unsigned int length, unsigned char* data,
									unsigned int timestamp, unsigned short int seq_num,
									unsigned char payload_type,
									unsigned char frames);


/*****************************************************************
������ql_bt_mgr_handler
�������ײ��ϱ�������͵���Ӧ�¼�
������msg:�ײ��ϱ���Ϣ



����ֵ:
      NULL
*****************************************************************/
void ql_bt_mgr_handler(struct bt_task_event * msg);

/*****************************************************************
* Function: ql_bt_bt_dut
*
* Description:
* 	Set Bluetooth DUT profile
* 	
* Parameters:
* 	NULL
*
* Return:
* 	NULL	
*
*****************************************************************/
int ql_bt_bt_dut(void);

/*****************************************************************
* Function: ql_bt_get_open_status
*
* Description:
* 	Open the bluetooth symbol, it is suggested that 
*	the open and close bluetooth event shall prevail.
* 	
* Parameters:
* 	NULL
*	
* Return:
* 	1							open flag
*	0							close flag
*
*****************************************************************/
int ql_bt_get_open_status(void);


/*****************************************************************
* Function: ql_bt_change_local_name
*
* Description:
* 	Change bluetooth local name
* 	
* Parameters:
* 	name		[in]		Bluetooth Device name
*   
* Return:
* 	0						success
*	-1						fail	
*
*****************************************************************/
 int ql_bt_change_local_name(const char *name);

/*****************************************************************
* Function: ql_bt_connect_headset
*
* Description:
* 	Disconnect to remote device
* 	disconnect specified device for all profile
*
* Parameters:
* 	addr			[in]		Remote Bluetooth address
*	
* Return:
* 	0							success
*	-1							fail	
*
*****************************************************************/
 int ql_bt_disconnect_headset(struct bt_addr *addr);

/*****************************************************************
������ql_bt_a2dp_disconnected_handler
�������Ͽ�a2dp����
������NULL



      NULL
*****************************************************************/
void ql_bt_a2dp_disconnected_handler(void);

/*****************************************************************
������ql_bt_add_device_record
���������豸mac��ַ���ӵ���¼��
������device:Ŀ���豸��������Ϣ



����ֵ:
      NULL
*****************************************************************/
void ql_bt_add_device_record(struct bt_device_record *device);

/*****************************************************************
������ql_bt_sco_connect
����������scoЭ��
������addr:Ŀ���豸��mac��ַ


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_sco_connect(struct bt_addr addr);

/*****************************************************************
* Function: ql_bt_sco_disconnect
*
* Description:
*	Disconnect SCO connection
*	
* Parameters:
*	handle			[in]		SCO handle
*			
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_sco_disconnect(unsigned short int handle);


/*****************************************************************
������ql_bt_obex_disconnect
�������Ͽ�obexЭ������
������tid:handle


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_obex_disconnect(int tid);

/*****************************************************************
������ql_bt_spp_disconnect
�������Ͽ�sppЭ������
������spp_port:��Ӧ��spp�˿�


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_spp_disconnect(unsigned short spp_port);

/*****************************************************************
������ql_bt_a2dp_start_handler
����������a2dp���ų���
������NULL


����ֵ:
      NULL
      
*****************************************************************/
void ql_bt_a2dp_start_handler(void);

/*****************************************************************
������ql_bt_close_set
�������ر�����ǰ�Ĳ��� ɾ��task��
������NULL


����ֵ:
      NULL
*****************************************************************/
void ql_bt_close_set(void);

/*****************************************************************
������ql_bt_a2dp_remote_sbc_capabilities_handler
������a2dp���Ų����˵�����
������a2dp_capabilites:


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
void ql_bt_a2dp_remote_sbc_capabilities_handler(unsigned int a2dp_capabilites);

/*****************************************************************
������ql_bt_pin_reply
��������������ʱpin code��repley
������addr:Ŀ���豸��ַ
	  pin:repleyֵ

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_pin_reply(struct bt_addr addr, char *pin);

/*****************************************************************


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_a2dp_send_start(struct appbt_a2dp *a2dp);

/*****************************************************************
������ql_bt_obex_send_response
����������obex��Ӧ����Ϣ
������tid:handle
	  response_code��Ӧ����

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_obex_send_response(int tid, unsigned char response_code);

/*****************************************************************
������ql_bt_spp_send_data
������ͨ��sppЭ�鷢������
������data:���͵���������
	  length�����͵����ݳ���
	  spp_port:ʹ�õĶ�Ӧ��spp�˿�
����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_spp_send_data(unsigned char* data, unsigned short length, unsigned short spp_port);

/*****************************************************************
������ql_bt_a2dp_send_suspend
��������ͣa2dp���ݷ���
������NULL


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_a2dp_send_suspend(void);

/*****************************************************************
������ql_bt_a2dp_send_pcm_data
������ͨ��a2dpЭ�鷢��PCM����
������data:���͵���������
	  length�����͵����ݳ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_a2dp_send_pcm_data(unsigned char *data, int length);

/*****************************************************************
������ql_bt_free
�������ͷ�������ڴ�
������p:�����ڴ�ĳ���


����ֵ:
      NULL
      
*****************************************************************/
void ql_bt_free(void *p);
/******************************************************************
*	 HFP
******************************************************************/


/*****************************************************************
* Function: ql_bt_hfp_connect
*
* Description:
*	Setup a HFP connection to other device
*	
* Parameters:
*	addr			[in]		Bluetooth remote address	
*	call_status		[in]		The default is 0							
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_connect(struct bt_addr addr, int call_status);



/*****************************************************************
* Function: ql_bt_hfp_disconnect
*
* Description:
*	Disconnect HFP connection
*	
* Parameters:
*	NULL	
*								
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_disconnect(void);



/*****************************************************************
* Function: ql_bt_hfp_accept_connection
*
* Description:
*	Accept HFP connection, initiate by HF
*	
* Parameters:
*	call_status			[in]		The default is 0	
*	addr				[in]		Bluetooth remote address							
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
 int ql_bt_hfp_accept_connection(int call_status, struct bt_addr addr);



/*****************************************************************
* Function: ql_bt_hfp_reject_connection
*
* Description:
*	Reject HFP connection, initiate by HF
*	
* Parameters:
*	addr			[in]		Bluetooth remote address	
*								
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_reject_connection(struct bt_addr addr);



/*****************************************************************
* Function: ql_bt_hfp_update_callstatus
*
* Description:
*	Send call status to remote device
*	
* Parameters:
*	index			[in]		Bluetooth remote address	
*	value			[in]		0
*	delay			[in]		0
*
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_update_callstatus(enum ciev_index index, int value, int delay);


/*****************************************************************
* Function: ql_bt_hfp_update_phone_number
*
* Description:
*	Send phone number to remote device
*	
* Parameters:
*	num			[in]		SIM card native number, string
*	type		[in]		'AT+CLCC' number type					
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_update_phone_number(const char *num, int type);


/*****************************************************************
* Function: ql_bt_hfp_update_callhold
*
* Description:
*	Send call hold status(+BTRH:xx) to remote device
*	
* Parameters:
*	status			[in]		0 - the incoming call on hold
*								1 - accepting the call 
*								2 - incoming call was rejected	
*								
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
int ql_bt_hfp_update_callhold(int status);


/*****************************************************************
* Function: ql_bt_hfp_callhold
*
* Description:
*	Hold the call
*	
* Parameters:
*	reject			[in]		
*								
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
 int ql_bt_hfp_callhold(int reject);



/*****************************************************************
* Function: ql_bt_hfp_voice_recogonition
*
* Description:
*	Active or deactive voice recogonition function
*	
* Parameters:
*	enable			[in]		0--on	
*								1--off
* Return:
*	0							success
*	-1							fail
*
*****************************************************************/
 int ql_bt_hfp_voice_recogonition(int enable);



/*****************************************************************
* Function: ql_bt_hfp_set_speaker_gain
*
* Description:
*	Set speaker gain of HFP
*	
* Parameters:
*	gain			[in]		Volume gain 0-15
*				
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
 int ql_bt_hfp_set_speaker_gain(int gain);



/*****************************************************************
* Function: ql_bt_hfp_set_microphone_gain
*
* Description:
*	Set microphone gain of HFP
*	
* Parameters:
*	gain			[in]		Volume gain 0-15
*			S
* Return:
*	0							success
*	-1							fail	
*
*****************************************************************/
int ql_bt_hfp_set_microphone_gain(int gain);




/*********************************************BLE***********************************************************/
/*****************************************************************
������ql_bt_le_set_gap_name
������ios������������
������gap_name:��������
	  name_len�����ֳ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_set_gap_name(char *gap_name, int name_len);

/*****************************************************************
������ql_bt_le_create_adv_data
����������BLE�㲥
������gap:����
	  buffer:�㲥����
	  buffer_size:�㲥���ݳ���
	  data:��Ҫ���õ�����
	  size:��Ҫ���õ����ݳ���
����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_create_adv_data(int gap, unsigned char *buffer, int buffer_size,unsigned char *data, int size);

/*****************************************************************
������ql_bt_le_create_adv_data
���������ù㲥����
������
	  data:��Ҫ���õ�����
	  size:��Ҫ���õ����ݳ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_set_adv_data(unsigned char *data, unsigned char length);

/*****************************************************************
������ql_le_set_scan_response_data
����������ɨ��Ӧ������
������
	  data:���õ�Ӧ������
	  size:���õ�Ӧ�����ݳ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_set_scan_response_data(unsigned char *data, unsigned char length);

/*****************************************************************
������ql_bt_le_set_adv_parameters
����������BLE�㲥����
������
	  para:���õĹ㲥����


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_set_adv_parameters(struct ql_bt_task_le_adv_parameters *para);

/*****************************************************************
������ql_bt_le_set_adv_enable
������ʹ�ܹ㲥ʹ��
������
	  enable:1--ʹ�� 0--�ر�


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_set_adv_enable(int enable);

/*****************************************************************
������ql_bt_le_register_service
����������BLE sever
������
	  service:����sever������
	  count:sever�ṹ���С

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_register_service(struct gatt_attr *service, int count);

/*****************************************************************
������ql_bt_le_unregister_service
������ɾ��BLE sever
������NULL



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_unregister_service(void);

/*****************************************************************
������ql_get_bt_open_status
���������ӽ���ص�����
������cb:���ӽ���ص�



����ֵ:
      NULL
*****************************************************************/
void ql_get_bt_open_status(ql_cb_bt_open_status cb);

/*****************************************************************
������ql_get_scan_bt_status
������ɨ�����ص�����
������cb:ɨ�����ص�



����ֵ:
      NULL
*****************************************************************/
void ql_get_scan_bt_status(ql_cb_bt_scan_status cb);

/*****************************************************************
������ql_get_bt_open_status
���������ӽ���ص�����
������cb:���ӽ���ص�



����ֵ:
      NULL
*****************************************************************/
void ql_get_bt_connect_status(ql_cb_bt_connect_status cb);

/*****************************************************************
* Function:  ql_bt_update_device_record
*
* Description:
* 	Update Bluetooth device information
*
* Parameters:
*	NULL
*								
* Return:
*	NULL
*
*****************************************************************/
void ql_bt_update_device_record(void);

/*****************************************************************
* Function:  ql_bt_get_device_records
*
* Description:
* 	Get the classic Bluetooth device that has been paired.
*	
*
* Parameters:
*	info	[out]	Pairing List Information
*								
* Return:
*	0							success
*	-1							fail
*							
*****************************************************************/
int ql_bt_get_device_records(struct bt_device_record **info);

/*****************************************************************
������ql_bt_le_set_random_address
����������BLE�����ַ
������addr:���õĵ�ַ



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_set_random_address(struct bt_addr addr);

/***************�⼸����app��ʵ�ֵ�***************/
int ql_bt_le_set_ext_adv_parameters(struct ql_bt_task_le_ext_adv_parameters *para);
int ql_bt_le_set_adv_set_random_address(unsigned char handle, struct bt_addr addr);
int ql_bt_le_set_ext_adv_data(unsigned char *data, unsigned char length, unsigned char handle, unsigned char operation, unsigned char fragment);
int ql_bt_le_set_ext_scan_response(unsigned char *data, unsigned char length, unsigned char handle, unsigned char operation, unsigned char fragment);
int ql_bt_le_set_ext_adv_enable(unsigned char enable, unsigned char number, unsigned char handle[], unsigned short duration[], unsigned char max_event[]);
int ql_appbt_le_notify(unsigned short conn_handle,unsigned short att_handle, unsigned char *data, int size);

/*****************************************************************
������ql_read_rssi
��������ȡ��ǰ���ӵ�RSSIֵ
������handle:��ǰ�����豸��handle



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_read_rssi(unsigned short handle);

/*****************************************************************
������ql_bt_le_set_pair_enable
������ios��������
������set_enable:1--enable 0--disable



����ֵ:
      0      sucess
      ����     failed
**********************
*****************************************************************/
int ql_bt_le_set_pair_enable(unsigned char set_enable);

/*****************************************************************

������ql_bt_le_disconnect
�������Ͽ�ble����
������NULL



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_disconnect(unsigned short con_handle);

/*****************************************************************
������ql_bt_le_read_by_group_type_request
����������'Read by Group Type Request'��GATT������
������start:��ʼhandle
	  end:����handle
	  uuid:Primary Service 0x2800

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_read_by_group_type_request(unsigned short start, unsigned short end, unsigned short uuid);

/*****************************************************************
������ql_bt_le_get_device_records
������send 'Find Information Request' to GATT server
������start:��ʼhandle
	  end:����handle


����ֵ:
      0      sucess
      ����     failed
*********************
*****************************************************************/
int ql_bt_le_get_device_records(ble_device_info_record **bleInfo);

/*****************************************************************
������ql_le_find_information_request
������send 'Find Information Request' to GATT server
������start:��ʼhandle
	  end:����handle


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_find_information_request    (unsigned short start, unsigned short end);

/*****************************************************************
������ql_bt_le_connect
����������ble����
������addr:Ŀ���豸��mac��ַ
	  type:��ַ���� 0 public; 1 random


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_connect(struct bt_addr addr, int type);

/*****************************************************************
������ql_le_custom_gatt_scan_start
������ɨ������ble���õ�sever�б�
������
      conn_handle:�����豸��Ӧ��handle


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_custom_gatt_scan_start(unsigned short conn_handle);

/*****************************************************************
������ql_le_custom_write_command
����������command����Ӧ��att
������
      conn_handle:�����豸��Ӧ��handle
	  att_handle: ��������Ӧ��handle
	  data:���͵�command
	  size:comand���ֽ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_custom_write_command(unsigned short conn_handle, unsigned short att_handle, unsigned char *data, unsigned short size);

/*****************************************************************
������ql_le_custom_write_request
�������������ݵ���Ӧ��att
������
      conn_handle:�����豸��Ӧ��handle
	  att_handle: ��������Ӧ��handle
	  data:���͵�����
	  size:���ݵ��ֽ���

����ֵ:
      0      sucess
      ����     failed
*****************************************************************/


int ql_le_custom_write_request(unsigned short conn_handle, unsigned short att_handle, unsigned char *data, unsigned short size);

/*****************************************************************
������ql_le_custom_read_request
����������Ӧ��att���Ͷ�����
������
      conn_handle:�����豸��Ӧ��handle
	  att_handle: ��������Ӧ��handle


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_custom_read_request(unsigned short conn_handle, unsigned short att_handle);

/*****************************************************************
������ql_le_custom_mtu_request
����������mtuЭ������
������
      conn_handle:�����豸��Ӧ��handle
	  mtu: Э�̵�һ������ֽ���


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_custom_mtu_request(unsigned short conn_handle, unsigned short mtu);

/*****************************************************************
������ql_le_disconnect_with_handle
�������Ͽ���Ӧ��handle����
������
      con_handle:�����豸��Ӧ��handle


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_disconnect_with_handle(unsigned short con_handle);

/*****************************************************************
������ql_ble_update_device_record
����������ble������Ϣ��¼
������NULL



����ֵ:
      NULL
*****************************************************************/
void ql_ble_update_device_record(void);

/*****************************************************************
������ql_bt_le_read_by_type_request
������send 'read by type Request' to GATT server
������start:��ʼhandle
	  end:����handle
	  uuid:Primary Service 0x2803


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_read_by_type_request(unsigned short start, unsigned short end, unsigned short uuid);

/*****************************************************************
������ql_bt_le_mtu_request
������ͬ��mtu����
������acl_handle:��ǰ���ӵľ��



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_mtu_request(unsigned short acl_handle);

/*****************************************************************
������ql_bt_le_scan
������ֹͣbleɨ��
������
	type			[in]		0 means passive,1 means active
	interval		[in]		The interval of scan, 2.5ms~10.24s
	window			[in]		The window of scan, 2.5ms~10.24s
	own_address_type[in]		Own address type
	handle			[in]		Callback handle after scan result


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_scan(unsigned char type, unsigned short int interval, unsigned short int window, unsigned char own_address_type, le_scan_event_handle_t handle);

/*****************************************************************
������ql_bt_le_scan_stop
������ֹͣbleɨ��
������NULL



����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_scan_stop(void);

/*****************************************************************
������ql_bt_le_register_att_service
��������ջ����BLE sever�ӿ�
������profile_data : server�Ľṹ��
      profile_data_len : �ṹ���С
      read_callback : read�ص�
      write_callback :write�ص�


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_bt_le_register_att_service(unsigned char *profile_data, unsigned short profile_data_len, att_read_callback_t read_callback, att_write_callback_t write_callback);

/*****************************************************************
������ql_le_get_pair_info
��������ȡble��¼�ӿ�
������index   : ��Ҫ��ȡ��¼��ƫ��


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_get_pair_info(int index);

/*****************************************************************
������ql_le_remove_pair_info
������ɾ��ble��¼�ӿ�
������index   : ��Ҫɾ����¼��ƫ��


����ֵ:
      0      sucess
      ����     failed
*****************************************************************/
int ql_le_remove_pair_info(int index);

/*****************************************************************
��������0x01��sever�������� Ŀ���Ǹ���sevice��
������NULL


����ֵ:
      NULL
*****************************************************************/

void ql_le_send_service_change(unsigned short conn_handle);

/*****************************************************************
������ql_le_custom_connection_parameter_update_request
�������޸�ble ���Ӽ��
������
con_handle         :�����豸��Ӧ��handle
conn_interval      :�޸ĵ����Ӽ��
//������Ӽ����ָ��һ�������¼��Ŀ�ʼ����һ�������¼��Ŀ�ʼ��ʱ���������Ӽ����1.25msΪ��λ�����Ӽ���ķ�Χ��6~3200����7.5ms~4s֮�䣩
slave_latency      :���豸�ӳٻ��ߴ��豸ʱ��
//����Slave�����豸����û������Ҫ��������£�����һ����Ŀ�������¼�������Щ�����¼��в��ػظ�Master�����豸���İ����������ܸ���ʡ�磨��Χ������0~499��
supervision_timeout:��س�ʱ
//��������趨��һ����ʱʱ�䣬���BLE�����ʱ����û�з���ͨ�ŵĻ����ͻ��Զ��Ͽ�����λ��10ms����Χ��10~3200��100ms~32s��

���Ӽ�����ӻ�ʱ���Լ���ʱʱ�������߱����������¹�ʽ��

Supervision Timeout >��1 +slaveLatency��*��connectionInterval��

ע��slaveLatency�������� supervision Timeout��ʱ���ڣ��������Ӿͻ᲻�����Ͽ�

���������Ӳ����ڲ�ͬ����¶�ͨ�����ʺ͹��ĵ�Ӱ��

1.Connection Interval���̣�Master��Slaveͨ�Ÿ���Ƶ������������������ٶȣ����������ݷ���ʱ�䣬��Ҳ����˹��ġ�

2.Connection Interval�ӳ���ͨ��Ƶ�ʽ��ͣ����������ٶȽ��ͣ����ݷ��͵ȴ���ʱ����������������ý����˹��ġ�

3.Slave Latency���ٻ�������Ϊ0����ôÿ�������¼��ж���Ҫ�ظ�Master�İ�����Ȼ���Ļ������������ݷ����ٶ�Ҳ����ߡ�

4.Slave Latency�ӳ��������½������ݷ����ٶȽ���


����ֵ:
      INT
*****************************************************************/

int ql_le_custom_connection_parameter_update_request(unsigned short conn_handle, unsigned short conn_interval, unsigned short slave_latency, unsigned short supervision_timeout);

/*****************************************************************
������ql_le_set_adv_mode
����������ģʽ
������
BT_LE_ADV_LEGACY_MODE = 0,
BT_LE_EXTENDED_ADV_LEGACY_MODE= 1,
BT_LE_EXTENDED_ADV_AUX_MODE= 2,


����ֵ:
	  NULL
*****************************************************************/

int ql_le_custom_connection_parameter_update_request(unsigned short conn_handle, unsigned short conn_interval, unsigned short slave_latency, unsigned short supervision_timeout);

/*****************************************************************
������ql_le_set_adv_mode
����������ģʽ
������
BT_LE_ADV_LEGACY_MODE = 0,
BT_LE_EXTENDED_ADV_LEGACY_MODE= 1,
BT_LE_EXTENDED_ADV_AUX_MODE= 2,


����ֵ:
	  NULL
*****************************************************************/
	
void ql_le_set_adv_mode(uint8_t le_adv_mode);
	

#ifdef __cplusplus
} /*"C" */
#endif

#endif

