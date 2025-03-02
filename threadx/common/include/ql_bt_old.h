/*============================================================================
  Copyright (c) 2022 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/

#ifndef _QL_BT_OLD_H
#define _QL_BT_OLD_H
/*> include header files here*/
#include "ql_bt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSA_SUSPEND             0xFFFFFFFF
#define MAX_BT_PHONE_NUMBER         20
#define MAX_BT_DEVICE_RECORD        10

#define MAX_BT_OBEX_FILE_NAME       256
#define MAX_BT_OBEX_FILE_TYPE_SIZE  32

#define OBEX_RESPONSE_CONTINUE          0x90
#define OBEX_RESPONSE_NOT_ACCEPTABLE    0xc6

typedef enum bttask_cmd_module {
    BTTASK_CMD_MODULE_BASE      = 0,
    BTTASK_CMD_MODULE_LE        = 1000,
    BTTASK_CMD_MODULE_CLASSIC   = 2000,
    BTTASK_CMD_MODULE_TEST      = 3000,
    BTTASK_CMD_MODULE_CUSTOM    = 4000,
} bttask_cmd_module_e;

#define BTTASK_CMD_PROFILE_CMD_START (BTTASK_CMD_MODULE_BASE+50)
typedef enum bttask_cmd_base{
    BTTASK_CMD_ACL_CONNECT = BTTASK_CMD_MODULE_BASE,
    BTTASK_CMD_ACL_DISCONNECT,
    BTTASK_CMD_CANCEL_ACL_CONNECT,
    BTTASK_CMD_INQUIRY,
    BTTASK_CMD_INQUIRY_CANCEL,
    BTTASK_CMD_SET_VISIBLE,
    BTTASK_CMD_SET_VISIBLE_CONNECTABLE,
    BTTASK_CMD_SET_LOCAL_NAME,
    BTTASK_CMD_BONDING_ACCEPT,
    BTTASK_CMD_BONDING,
    BTTASK_CMD_UNBONDING,
    BTTASK_CMD_PIN_REPLY, // 10
    BTTASK_CMD_PIN_NEGATIVE_REPLY,
    BTTASK_CMD_USER_PASSKEY_REQUEST_REPLY,
    BTTASK_CMD_USER_PASSKEY_REQUEST_NEGATIVE_REPLY,
    BTTASK_CMD_SEND_KEYPRESS_NOTIFICATION,

    //a2dp
    BTTASK_CMD_CONNECT_A2DP = BTTASK_CMD_PROFILE_CMD_START,
    BTTASK_CMD_DISCONNECT_A2DP,
    BTTASK_CMD_A2DP_SEND_START,
    BTTASK_CMD_A2DP_SEND_SUSPEND,
    BTTASK_CMD_A2DP_SEND_MEDIA_DATA,
    //avrcp
    BTTASK_CMD_CONNECT_AVRCP,
    BTTASK_CMD_DISCONNECT_AVRCP,
    BTTASK_CMD_AVRCP_PLAY,
    BTTASK_CMD_AVRCP_PAUSE,
    BTTASK_CMD_AVRCP_STOP,
    BTTASK_CMD_AVRCP_FORWARD, //BTTASK_CMD_PROFILE_CMD_START+10
    BTTASK_CMD_AVRCP_BACKWARD,
    //hfp
    BTTASK_CMD_CONNECT_HFP,
    BTTASK_CMD_DISCONNECT_HFP,
    BTTASK_CMD_HFP_ACCEPT_CONNECTION,
    BTTASK_CMD_HFP_REJECT_CONNECTION, //30
    BTTASK_CMD_HFP_UPDATE_CALLSTATUS,
    BTTASK_CMD_HFP_UPDATE_PHONE_NUMBER,
    BTTASK_CMD_HFP_UPDATE_CALLHOLD,
    BTTASK_CMD_HFP_SET_SPEAKER_GAIN,
    BTTASK_CMD_HFP_SET_MICROPHONE_GAIN, //BTTASK_CMD_PROFILE_CMD_START+20
    BTTASK_CMD_HFP_SEND_AT_RAWDATA,
    //for btstack
    BTTASK_CMD_HFP_BS_CALL_STATUS_UPDATE,
    //obex
    BTTASK_CMD_CONNECT_OBEX_OPP,
    BTTASK_CMD_DISCONNECT_OBEX_OPP,
    BTTASK_CMD_OBEX_OPP_SEND_FILE_START,
    BTTASK_CMD_OBEX_OPP_SEND_FILE,
    BTTASK_CMD_OBEX_RESPONSE,
    //sco
    BTTASK_CMD_SETUP_SCO,
    BTTASK_CMD_DISCONNECT_SCO,
    BTTASK_CMD_SCO_DATA_SEND, //BTTASK_CMD_PROFILE_CMD_START+30
    //spp
    BTTASK_CMD_SPP_CONNECT,
    BTTASK_CMD_SPP_DISCONNECT,
    BTTASK_CMD_SPP_SEND_DATA,
    //hid
    BTTASK_CMD_HID_CONNECT,
    BTTASK_CMD_HID_DISCONNECT, //50
    //HFP HF
    BTTASK_CMD_CONNECT_HFP_HF,
    BTTASK_CMD_DISCONNECT_HFP_HF,
    BTTASK_CMD_COPS,
    BTTASK_CMD_ATA,
    BTTASK_CMD_ATH, //BTTASK_CMD_PROFILE_CMD_START+40
    BTTASK_CMD_DIAL,
    BTTASK_CMD_DIAL_MEMORY,
    BTTASK_CMD_SPEAKER_GAIN,
    BTTASK_CMD_MIC_GAIN,
    BTTASK_CMD_CHLD0,
    BTTASK_CMD_CHLD1,
    BTTASK_CMD_CHLD2,
    BTTASK_CMD_CHLD3,
    BTTASK_CMD_CHLD4,
    BTTASK_CMD_BLDN, //BTTASK_CMD_PROFILE_CMD_START+50
    BTTASK_CMD_CLCC,
    BTTASK_CMD_CHLD1X,
    BTTASK_CMD_CHLD2X,
    //a2dp sink
    BTTASK_CMD_A2DP_SINK_CONNECT,
    BTTASK_CMD_A2DP_SINK_DISCONNECT,
    BTTASK_CMD_SNIFF_MODE_ENABLE,
    // New command for custom
    BTTASK_CMD_SET_CHECK_CONTROLLER_ALIVE,

    BTTASK_CMD_LE_SET_RANDOM_ADDRESS = BTTASK_CMD_MODULE_LE,
    BTTASK_CMD_LE_SET_ADV_PARAMETERS,
    BTTASK_CMD_LE_SET_ADV_DATA,
    BTTASK_CMD_LE_SET_ADV_ENABLE,
    BTTASK_CMD_LE_SCAN,
    BTTASK_CMD_LE_SCAN_STOP,
    BTTASK_CMD_LE_SET_SCAN_RESPONSE,
    BTTASK_CMD_LE_REGISTER_SERVICE,
    BTTASK_CMD_LE_UNREGISTER_SERVICE,
    BTTASK_CMD_LE_UPDATE_ATTRIBUTE,
    BTTASK_CMD_LE_NOTIFY,
    BTTASK_CMD_LE_INDICATE,
    BTTASK_CMD_LE_CONNECT,
    BTTASK_CMD_LE_DISCONNECT,
    BTTASK_CMD_LE_MTU_REQUEST,
    BTTASK_CMD_LE_CLIENT_READ_BY_TYPE_REQUEST,
    BTTASK_CMD_LE_CLIENT_READ_REQUEST,
    BTTASK_CMD_LE_CLIENT_READ_BLOB_REQUEST,
    BTTASK_CMD_LE_CLIENT_READ_MULTIPLE_REQUEST,
    BTTASK_CMD_LE_CLIENT_READ_BY_GROUP_TYPE_REQUEST,
    BTTASK_CMD_LE_FIND_INFO_REQUEST,
    BTTASK_CMD_LE_FIND_BY_TYPE_VALUE_REQUEST,
    BTTASK_CMD_LE_WRITE_REQUEST,
    BTTASK_CMD_LE_WRITE_COMMAND,
    BTTASK_CMD_LE_SIGNED_WRITE_COMMAND,
    BTTASK_CMD_TEST_DUT,
    BTTASK_CMD_TEST_TX_TONE,
    BTTASK_CMD_TEST_TX_PACKET,
    BTTASK_CMD_TEST_RX_PACKET,
    BTTASK_CMD_TEST_LE_ENHANCED_RECEIVER,
    BTTASK_CMD_TEST_LE_ENHANCED_TRANSMITTER,
    BTTASK_CMD_TEST_LE_END,
    BTTASK_CMD_LE_SET_ADV_SET_RANDOM_ADDRESS,
    BTTASK_CMD_LE_SET_EXT_ADV_PARAMETERS,
    BTTASK_CMD_LE_SET_EXT_ADV_DATA,
    BTTASK_CMD_LE_SET_EXT_SCAN_RESPONSE,
    BTTASK_CMD_LE_SET_EXT_ADV_ENABLE,
    BTTASK_CMD_LE_CLEAR_WHITE_LIST,
    BTTASK_CMD_LE_SET_WHITE_LIST,
    BTTASK_CMD_LE_SMP_SECURITY_REQUEST,
    BTTASK_CMD_LE_READ_ADV_PHY_TXPOWER,
    BTTASK_CMD_RAW_VENDOR_CMD,
    BTTASK_CMD_TEST_READ_RSSI,
    BTTASK_CMD_TEST_HCI_RESET,
    BTTASK_CMD_TEST_SET_COEXIST_MODE,
    BTTASK_CMD_LE_CLEAN_PAIR_INFO,
    BTTASK_CMD_LE_DUMP_PAIR_INFO,
    BTTASK_CMD_LE_RELOAD_PAIR_INFO,
    BTTASK_CMD_LE_SET_PASSKEY_FROM_LOCAL,
    BTTASK_CMD_LE_SET_PAIR_ENABLE, // enable normal pair rsp
} bttask_cmd_base_e;


enum {
    BTTASK_IND_TYPE_COMMON,
    BTTASK_IND_TYPE_ACL,
    BTTASK_IND_TYPE_SCO,
    BTTASK_IND_TYPE_A2DP,
    BTTASK_IND_TYPE_AVRCP,
    BTTASK_IND_TYPE_HFP,
    BTTASK_IND_TYPE_OBEX,
    BTTASK_IND_TYPE_SPP,
    BTTASK_IND_TYPE_LE,
    BTTASK_IND_TYPE_A2DP_SINK,
    BTTASK_IND_TYPE_HFP_HF,
    BTTASK_IND_TYPE_HID,
};

enum {
    BTTASK_IND_INQUIRY_RESULT,
    BTTASK_IND_INQUIRY_COMPLETE,
    BTTASK_IND_PAIRING_REQUEST,
    BTTASK_IND_PAIRED,
    BTTASK_IND_PIN_REQUEST,
    BTTASK_IND_USER_PASSKEY_NOTIFICATION,
    BTTASK_IND_USER_PASSKEY_REQUEST,
    BTTASK_IND_POWERUP_COMPLETE,
    BTTASK_IND_POWERUP_FAILED,
    BTTASK_IND_SHUTDOWN_COMPLETE,
    BTTASK_IND_BTFIRMWARE_ASSERT,
    BTTASK_IND_HCI_COMPLETE_EVENT,
    BTTASK_IND_NULL, // message was eaten by upper layer
    BTTASK_IND_NAME,
    BTTASK_IND_RSSI,
}; // common define

enum {
    BTTASK_IND_ACL_CONNECTED,
    BTTASK_IND_ACL_DISCONNECTED,
    BTTASK_IND_ACL_CONNECT_FAILED,
}; // ACL define

enum {
    BTTASK_IND_SCO_CONNECTED,
    BTTASK_IND_SCO_DISCONNECTED,
    BTTASK_IND_SCO_CONNECT_FAILED,
}; // SCO define

enum {
    BTTASK_IND_A2DP_CONNECTED,
    BTTASK_IND_A2DP_MEIDA_CONNECTED,
    BTTASK_IND_A2DP_DISCONNECTED,
    BTTASK_IND_A2DP_START,
    BTTASK_IND_A2DP_MEDIA_SEND_COMPLETE,
    BTTASK_IND_REMOTE_SBC_CAPABILITIES,
    BTTASK_IND_A2DP_START_PRIVATE,
    BTTASK_IND_A2DP_MEDIA_MTU,
    BTTASK_IND_A2DP_SUSPEND,
}; // a2dp define

enum {
    BTTASK_IND_A2DP_SINK_CONNECTED,
    BTTASK_IND_A2DP_SINK_MEIDA_CONNECTED,
    BTTASK_IND_A2DP_SINK_DISCONNECTED,
    BTTASK_IND_A2DP_SINK_REMOTE_SBC_CAPABILITIES,
    BTTASK_IND_A2DP_SINK_MEDIA_PAYLOAD,
    BTTASK_IND_A2DP_SINK_PLAY_STARTED,
    BTTASK_IND_A2DP_SINK_PLAY_SUSPENDED,
    BTTASK_IND_A2DP_SINK_PLAY_STOPPED,
}; // a2dp sink define

enum {
    BTTASK_IND_AVRCP_CONNECTED,
    BTTASK_IND_AVRCP_DISCONNECTED,
    BTTASK_IND_AVRCP_KEY_PRESSED,
    BTTASK_IND_AVRCP_KEY_RELEASED,
}; // avrcp define

enum {
    BTTASK_IND_HFP_CONNECTION_REQUEST, // HF initate a connection to AG
    BTTASK_IND_HFP_CONNECTED, // connection complete, HF or AG initate this connection
    BTTASK_IND_HFP_DISCONNECTED,
    BTTASK_IND_HFP_ATA,
    BTTASK_IND_HFP_ATCHUP,
    BTTASK_IND_HFP_DIAL,
    BTTASK_IND_HFP_ATCHLD,
    BTTASK_IND_HFP_VOICE_RECOGNITION,
    BTTASK_IND_HFP_SPEAKER_VOLUME,
    BTTASK_IND_HFP_MICROPHONE_GAIN,
    BTTASK_IND_HFP_SDP_FAILED, // remote device not support HFP
}; // hfp ag define

enum {
    BTTASK_IND_HFP_HF_CONNECTED, // connection complete, HF or AG initate this connection
    BTTASK_IND_HFP_HF_DISCONNECTED,
    BTTASK_IND_HFP_HF_SPEAKER_VOLUME,
    BTTASK_IND_HFP_HF_MICROPHONE_GAIN,
    BTTASK_IND_HFP_HF_RING,
    BTTASK_IND_HFP_HF_AG_INDICATOR_STATUS_CHANGED,
    BTTASK_IND_HFP_HF_NETWORK_OPERATOR_CHANGED,
    BTTASK_IND_HFP_HF_CLIP,
    BTTASK_IND_HFP_HF_CLCC,
	BTTASK_IND_HFP_SCO_DATA,
}; // hfp hf define

enum {
    BTTASK_IND_OBEX_OPP_SERVER_CONNECTED,
    BTTASK_IND_OBEX_OPP_SERVER_DISCONNECTED,
    BTTASK_IND_OBEX_OPP_SERVER_OBJECT_FILE_INFO,
    BTTASK_IND_OBEX_OPP_SERVER_OBJECT_RECIEVE_DATA,
    BTTASK_IND_OBEX_OPP_SERVER_OBJECT_RECIEVE_FINISH,
    BTTASK_IND_OBEX_OPP_SERVER_OBJECT_RECIEVE_ABORT,
    BTTASK_IND_OBEX_OPP_CLIENT_CONNECTED,
    BTTASK_IND_OBEX_OPP_CLIENT_DISCONNECTED,
    BTTASK_IND_OBEX_OPP_CLIENT_OBJECT_PUT_FAILED,
    BTTASK_IND_OBEX_OPP_CLIENT_OBJECT_PUT_SUCCESS,
    BTTASK_IND_OBEX_OPP_CLIENT_OBJECT_PUT_CONTINUE,
    BTTASK_IND_OBEX_PBAP_SERVER_CONNECTED,
}; // obex define

enum {
    BTTASK_IND_SPP_CONNECT_IND,
    BTTASK_IND_SPP_CONNECT_CNF,
    BTTASK_IND_SPP_DISCONNECT_IND,
    BTTASK_IND_SPP_DISCONNECT_CNF,
    BTTASK_IND_SPP_DATA_IND,
    BTTASK_IND_SPP_DATA_CNF,
    BTTASK_IND_SPP_FLOW_IND,
 }; // SPP define

enum {
    BTTASK_IND_HID_CONNECTED,
    BTTASK_IND_HID_DISCONNECTED,
};


enum {
    BTTASK_IND_LE_SCAN_EVENT,
    BTTASK_IND_LE_GATT_CONNECTED,
    BTTASK_IND_LE_GATT_DISCONNECTED,
    BTTASK_IND_LE_HANDLE_VALUE_CFM, // indicate confirm response
    BTTASK_IND_LE_ERROR_RESPONSE,
    BTTASK_IND_LE_MTU_EXCHANGED, // this indication means MTU be changed (no matter master or slave)
    BTTASK_IND_LE_CLIENT_MTU_EXCHANGED, // if MTU exchang finished, GATT client can do next action
    BTTASK_IND_LE_CLIENT_READ_BY_GROUP_TYPE_RSP,
    BTTASK_IND_LE_CLIENT_READ_BY_TYPE_RSP,
    BTTASK_IND_LE_CLIENT_FIND_INFOMATION_RSP,
    BTTASK_IND_LE_CLIENT_READ_RSP,
    BTTASK_IND_LE_CLIENT_READ_BLOB_RSP,
    BTTASK_IND_LE_CLIENT_HANDLE_NOTIFY,
    BTTASK_IND_LE_CLIENT_HANDLE_INDIATION,
    BTTASK_IND_LE_WHITE_LIST_SIZE,
    BTTASK_IND_SMP_PASSKEY,
    BTTASK_IND_ADV_PHY_TXPOWER,
    BTTASK_IND_LE_CLIENT_WRITE_RSP,	//indicate get write resp
    BTTASK_IND_SLAVE_LE_BOND_COMPLETE, //slave role, bond

    // add for btstack
    BTTASK_IND_LE_ATT_EVENT_CAN_SEND_NOW,
    BTTASK_IND_LE_ATT_EVENT_CAN_WRITE_NOW,
    BTTASK_IND_LE_IDENTITY_INFO,
    BTTASK_IND_LE_DB_CONTROL_GET_INFO,
    BTTASK_IND_LE_ATT_EVENT_READ_DATA_RESULT,
    BTTASK_IND_LE_ATT_EVENT_READ_OVER_RESULT,
    BTTASK_IND_LE_GATT_SCAN_RESULT,
    BTTASK_IND_LE_GATT_SCAN_DUMP_SERVICE,
    BTTASK_IND_LE_GATT_SCAN_DUMP_CHARACTERISTIC,
    BTTASK_IND_LE_GATT_SCAN_DUMP_DESCRIPTOR,
}; // le define


enum {
    AVRCP_KEY_ENTER = 0x2B,
    AVRCP_KEY_VOLUME_UP = 0x41,
    AVRCP_KEY_VOLUME_DOWN,
    AVRCP_KEY_MUTE,
    AVRCP_KEY_PLAY,
    AVRCP_KEY_STOP,
    AVRCP_KEY_PAUSE,
    AVRCP_KEY_FORWARD = 0x4B,
    AVRCP_KEY_BACKWARD,
};


struct bt_event_spp_event {
	unsigned char addr[6];
	unsigned char port;
	unsigned char result;
	unsigned short int max_frame_size;
	unsigned short int data_len;
	unsigned char* data;
};

struct bt_event_le_mtu_exchange {
    int mut;
    int acl_handle;
};
struct bt_event_le_read_by_group_type_rsp {
    unsigned char size;
    unsigned char value[255];
};
struct bt_event_le_smp_passkey {
    struct bt_addr addr;
    unsigned int passkey_value;
};
struct bt_event_le_adv_phy_txpower {
    unsigned char power_level;
};

struct bt_task_le_set_adv_random_address {
    unsigned char handle;
    struct bt_addr addr;
};

struct bt_event_pairing_request {
    unsigned char name[MAX_BT_NAME];
    struct bt_addr addr;
    unsigned int numeric_value;
    unsigned int cod;
    unsigned char io_capability;
};

struct bt_event_paired {
    unsigned char name[MAX_BT_NAME];
    unsigned char addr[6];
    unsigned linkey[16];
    int linkkey_type;
    unsigned int cod;
};

struct bt_event_pin_request {
    struct bt_addr addr;
    unsigned char name[MAX_BT_NAME];
    unsigned int cod;
};

struct bt_event_acl_connect {
    unsigned char addr[6];
    unsigned short int handle;
};

struct bt_event_acl_disconnect {
    unsigned short int reason;
    unsigned short int handle;
};

struct bt_event_le_bond_complete {
    unsigned char address_type;
    struct bt_addr address;
};


struct bt_event_sco {
    unsigned char addr[6];
    unsigned int handle;
};

struct bt_event_a2dp_connect {
    int cid;
    unsigned char addr[6];
};

struct bt_event_a2dp_disconnect {
    int cid;
};

struct bt_event_avrcp_connect {
    unsigned char addr[6];
};

struct bt_event_hfp_connect {
    unsigned char addr[6];
};

struct bt_event_hfp_dial {
    int type; // 0: number; 1: redial; 2: memory
    unsigned char number[MAX_BT_PHONE_NUMBER];
    int length;
};

struct bt_event_obex_file_info {
    char file_name[MAX_BT_OBEX_FILE_NAME];
    int file_name_size;
    char file_type[MAX_BT_OBEX_FILE_TYPE_SIZE]; // UTF-8 not need size
    int file_size;
};

struct bt_event_name_indication {
    struct bt_addr addr;
    unsigned char name[MAX_BT_NAME];
    int name_length;
};

struct bt_event_obex_receive_finish {
    //file_info
    struct bt_event_obex_file_info file_info;

    //data info
    int data_len;
    void* data;
};

struct bt_event_le_att_connected {
    struct bt_addr addr;
    unsigned char addr_type;
    int acl_handle;
    int role;
	unsigned char peer_irk[16];
};


enum {
	GATT_SERVICE_DISCOVER_NONE,
	GATT_SERVICE_DISCOVER_MTU,
	GATT_SERVICE_DISCOVER_PRIMARY,
	GATT_SERVICE_DISCOVER_CHARACTERISTIC,
	GATT_SERVICE_DISCOVER_CCC,
	GATT_SERVICE_DISCOVER_MAX
};
	
struct bt_task_le_ext_adv_enable {
	unsigned char enable;
	unsigned char number;
	unsigned char *handle;
	unsigned short *duration;
	unsigned char *max_event;
};

struct bt_task_le_ext_adv_data {
    unsigned char data[251];
    unsigned char length;
    unsigned char handle;
    unsigned char operation;
    unsigned char fragment;
};

struct gatt_service_discover_context {
	int status;
	struct gatt_service_record *service;
	struct gatt_characteristic_record *characteristic;
	unsigned short process_handle;
};

typedef int (*le_att_event_handle_t)(struct bt_task_event * msg);


#ifdef __cplusplus
} /*"C" */
#endif

#endif

