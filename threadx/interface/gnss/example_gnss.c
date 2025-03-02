
/*==========================================================================================================
  Copyright (c) 2021, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
===========================================================================================================*/
/*�°汾����Ҫ��ͬ��ntp*/
#include <stdio.h>
#include "ql_application.h"
#include "ql_log.h"
#include "ql_gnss.h"
#include "ql_rtos.h"
#include "ql_ntp.h"
#include "ql_data_call.h"
#include "sockets.h"
#include "netdb.h"
#include "ql_http_client.h"
#include <string.h>
#include <stdlib.h>
#include "ql_nw.h"
#include "ql_uart.h"

#define QL_WIFISCAN_SUPPORT
#define test_log(fmt, args...)	printf(fmt, ##args)
#define KNOTS_TO_KMH(knots) ((knots) * 1.852)
#define PROFILE_IDX 1
#define JSON_START                          "{"
#define JSON_FIELD(field_name, format)      "\"" #field_name "\"" ":" "\"" #format "\""
#define JSON_SEP                            ","
#define JSON_STOP							"}"

ql_task_t mQl_Task, mQl_Task2, wifi_Task;
char mData[256];

typedef struct {
    char time[11];      // HHMMSS.SSS format
    char status;        // A = Active, V = Void
    double latitude;    // Decimal degrees format
    char lat_dir;       // N or S
    double longitude;   // Decimal degrees format
    char long_dir;      // E or W
    double speed;       // Speed in knots
    char date[7];       // DDMMYY format
    char isoDateTime[21];
} GNRMC_Data;

typedef struct {
    char time[11];      // HHMMSS.SSS format
    double latitude;    // Latitude in decimal degrees
    char lat_dir;       // N or S
    double longitude;   // Longitude in decimal degrees
    char long_dir;      // E or W
    int fix_quality;    // Fix quality
    int num_satellites; // Number of satellites being tracked
    double horizontal_dil; // Horizontal dilution of position
    double altitude;    // Altitude, Meters, above mean sea level
} GNGGA_Data;

typedef struct {
    char isoDateTime[32];
    float lat, lon, speed;
    int numOfSat;

} httpPackage;
httpPackage mHttpPackage;

ql_timer_t mTimer;
ql_queue_t mQueue;
int mTick;

void quec_usbcdc_callback(QL_UART_PORT_NUMBER_E port, void *para)
{
	int read_len = 0;
	char r_data[1024] = {0};
	read_len = ql_uart_read(port, r_data, sizeof(r_data));
	ql_uart_write(QL_USB_CDC_PORT, r_data, read_len);
}
static void timer_Callback () {
    mTick++;
	if(mTick > 20){
		int value = 1;
		mTick = 0;
		ql_rtos_queue_release(mQueue, 8, &value, QL_NO_WAIT);
	}
	
}
/*int parse_GNRMC(char *nmea, GNRMC_Data *data) {
	int strLength = strlen(nmea);
    char *token = strtok(nmea, ",");
    if (!token || strncmp(token, "$GNRMC", 6) != 0) return -1;

    // Extracting each field from the GNRMC sentence
	if(strLength > 68) {
		token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    	token = strtok(NULL, ","); if (token) data->status = token[0];
    	token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->speed = atof(token);
		token = strtok(NULL, ",");
    	token = strtok(NULL, ","); if (token) strcpy(data->date, token);
		return 0;
	} else {
		token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    	token = strtok(NULL, ","); if (token) data->status = token[0];
    	token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->speed = atof(token);
    	token = strtok(NULL, ","); if (token) strcpy(data->date, token);
    	return 0;
	}
   
}*/

int parse_GNRMC(char *nmea, GNRMC_Data *data) {
	int strLength = strlen(nmea);
	printf("string length: %d\n", strLength);
    char *token = strtok(nmea, ",");
    if (!token || strncmp(token, "$GNRMC", 6) != 0) return -1;

    // Extracting each field from the GNRMC sentence
	if(strLength > 69) {
		token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    	token = strtok(NULL, ","); if (token) data->status = token[0];
    	token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->speed = atof(token);
		token = strtok(NULL, ",");
    	token = strtok(NULL, ","); if (token) strcpy(data->date, token);
		return 0;
	} else 
		if (strLength < 68) {
		token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    	token = strtok(NULL, ","); if (token) data->status = token[0];
    	token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    	//token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    	//token = strtok(NULL, ","); if (token) data->speed = atof(token);
		data->speed = 0;
		token = strtok(NULL, ",");
    	token = strtok(NULL, ","); if (token) strcpy(data->date, token);
		return 0;
	} 
	else {
		token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    	token = strtok(NULL, ","); if (token) data->status = token[0];
    	token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    	token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    	token = strtok(NULL, ","); if (token) data->speed = atof(token);
    	token = strtok(NULL, ","); if (token) strcpy(data->date, token);
    	return 0;
	}
   
}

int checkNMEA(const char *nmea) {
    if (strncmp(nmea, "$GNRMC", 6) != 0) return -1;
    if (strncmp(nmea, "$GNRMC", 6) == 0) {
        if (strstr(nmea, ",,,,") != NULL) {
            printf("No position information in GNRMC\n");
            return -1;
        }
    }
    return 0;
}

static int datacall_satrt(void);

int parse_GNGGA(char *nmea, GNGGA_Data *data) {
    char *token = strtok(nmea, ",");
    if (!token || strncmp(token, "$GNGGA", 6) != 0) return -1;

    // Extracting each field from the GNGGA sentence
    token = strtok(NULL, ","); if (token) strcpy(data->time, token);
    token = strtok(NULL, ","); if (token) data->latitude = atof(token);
    token = strtok(NULL, ","); if (token) data->lat_dir = token[0];
    token = strtok(NULL, ","); if (token) data->longitude = atof(token);
    token = strtok(NULL, ","); if (token) data->long_dir = token[0];
    token = strtok(NULL, ","); if (token) data->fix_quality = atoi(token);
    token = strtok(NULL, ","); if (token) data->num_satellites = atoi(token);
    token = strtok(NULL, ","); if (token) data->horizontal_dil = atof(token);
    token = strtok(NULL, ","); if (token) data->altitude = atof(token);
    return 0;
}

static void ql_nw_status_callback(int profile_idx, int nw_status)
{
	printf("profile(%d) status: %d\r\n", profile_idx, nw_status);
}

static void datacall_start(void* argv)
{
	printf("wait for network register done\r\n");

	if(ql_network_register_wait(30) != 0)
	{
		printf("*** network register fail ***\r\n");
	}
	else
	{

		printf("doing network activing ...\r\n");
		
		ql_wan_start(ql_nw_status_callback);
		ql_set_auto_connect(1, TRUE);
		ql_start_data_call(1, 0, NULL, NULL, NULL, 0);

	}
}

int countSatellites(const char *gngsaSentence) {
    int count = 0;
    const char *token = gngsaSentence;
    int field = 0;
    
    // Duyệt qua từng token được phân tách bởi dấu phẩy
    while ((token = strchr(token, ',')) != NULL) {
        token++;  // Di chuyển qua dấu phẩy
        field++;  // Tăng số trường
        
        // Các ID vệ tinh nằm từ trường 4 đến 15
        if (field >= 4 && field <= 15) {
            if (*token != ',' && *token != '*') {  // Kiểm tra xem trường này có phải là rỗng không
                count++;
            }
        }
        
        // Khi đến trường checksum, dừng việc đếm
        if (*token == '*') {
            break;
        }
    }

    return count;
}

void convertDateTime(const char *time, const char *date, char *output) {
    // Extract and format the date and time directly
    output[0] = '2';
    output[1] = '0';
    output[2] = date[4]; // Year tenths (assumes 2000+)
    output[3] = date[5]; // Year units
    output[4] = '-';
    output[5] = date[2]; // Month tens
    output[6] = date[3]; // Month units
    output[7] = '-';
    output[8] = date[0]; // Day tens
    output[9] = date[1]; // Day units
    output[10] = 'T';
    output[11] = time[0]; // Hour tens
    output[12] = time[1]; // Hour units
    output[13] = ':';
    output[14] = time[2]; // Minute tens
    output[15] = time[3]; // Minute units
    output[16] = ':';
    output[17] = time[4]; // Second tens
    output[18] = time[5]; // Second units
    output[19] = 'Z';
    output[20] = '\0'; // Null-terminate the string
}

double convertNMEAToDecimal(double nmeaCoordinate) {
    int degrees = (int)(nmeaCoordinate / 100);
    double minutes = nmeaCoordinate - (degrees * 100); 

    return degrees + (minutes / 60.0);
}

void quec_gnss_callback(char *data, int len)
{
    int i=0;
	char str[64];
    for(i = 0; i < len; i++)
        printf("%c", data[i]);
    char *lines[15];
    char *line;
    int lineCount = 0;

    line = strtok(data, "\n");
    while (line != NULL && lineCount < 15) {
        lines[lineCount++] = line;
        line = strtok(NULL, "\n");
    }

    // In ra các dòng để kiểm tra
    for (int i = 0; i < lineCount; i++) {
        printf("Line %d: %s\n", i + 1, lines[i]);
    }
    if(!checkNMEA(lines[0])) {
        GNRMC_Data rmc_data = {0};
    GNGGA_Data gga_data = {0};
    parse_GNRMC(lines[0], &rmc_data);
    parse_GNGGA(lines[1], &gga_data);

    int total_Satellietes = countSatellites(lines[3]) + countSatellites(lines[4]);
    double tmp = rmc_data.latitude;
    rmc_data.latitude = convertNMEAToDecimal(tmp);
    tmp = rmc_data.longitude;
    rmc_data.longitude = convertNMEAToDecimal(tmp);


    convertDateTime(rmc_data.time, rmc_data.date, rmc_data.isoDateTime);
    printf("GNRMC Data:\nTime: %s\nStatus: %c\nLatitude: %.8f %c\nLongitude: %.8f %c\nSpeed (km/h): %.3f\nDate: %s\nISODateTime: %s\n",
           rmc_data.time, rmc_data.status, rmc_data.latitude, rmc_data.lat_dir, rmc_data.longitude, rmc_data.long_dir, KNOTS_TO_KMH(rmc_data.speed), rmc_data.date, rmc_data.isoDateTime);
    printf("\nGNGGA Data:\nTime: %s\nLatitude: %.8f %c\nLongitude: %.8f %c\nFix Quality: %d\nNumber of Satellites: %d\nHDOP: %.2f\nAltitude: %.1f m\n",
           gga_data.time, gga_data.latitude, gga_data.lat_dir, gga_data.longitude, gga_data.long_dir, gga_data.fix_quality, gga_data.num_satellites, gga_data.horizontal_dil, gga_data.altitude);       
    
    strcpy(mHttpPackage.isoDateTime, rmc_data.isoDateTime);
    mHttpPackage.lat = rmc_data.latitude;
    mHttpPackage.lon = rmc_data.longitude;
    mHttpPackage.speed = KNOTS_TO_KMH(rmc_data.speed);
    mHttpPackage.numOfSat = total_Satellietes;
    ql_rtos_task_resume(mQl_Task);
    } else {
		ql_rtos_timer_start(mTimer, 1000, 0, timer_Callback, NULL);
		sprintf(str, "reTry Count: %d\n", mTick);
		//printf("reTry Count: %d\n", mTick);
		ql_uart_write(QL_USB_CDC_PORT, str, strlen(str));
	}
    
}

void quec_gnss_agps_callback(quec_gnss_sate_num_t * gps_sate)
{
    printf("gps_num:%d;gps_vailed_num:%d\nbds_num:%d;bds_vailed_num:%d\n",
        gps_sate->gps_sate_num,gps_sate->gps_sate_vaild_num,
        gps_sate->bds_sate_num,gps_sate->bds_sate_vaild_num);
}
static void quec_gnss_test(void * argv)
{
    int i;
    ql_gnss_location_t *gps_location = NULL;
    struct ql_data_call_info info = {0};
	char ip4_addr_str[16] = {0};
     u32 flags = 0;
	int ret =0 ; 
    unsigned int reset_type = 0;

    

	ql_gnss_cfgsys_set(5);							//����GPS+BDS
	ql_gnss_cfgmsg_set(31);							//����GGA/RMC/GSV/GSA/VTG

	ql_gnss_agps_cfg(AGPS_GNSS_ENABLE,0);				//ʹ��AGPS

	ql_gnss_apflash_cfg(APFLASH_GNSS_ENABLE);		//ʹ��APFLASH

    ql_set_gnss_nmea_mode(PACKET_MODE);
	
    ql_gnss_get_location(&gps_location);
   /*if(datacall_satrt()!=0)
		return;

	ql_get_data_call_info(1, 0, &info);

	printf("info.profile_idx: %d\r\n", info.profile_idx);
	printf("info.ip_version: %d\r\n", info.ip_version);
	printf("info.v4.state: %d\r\n", info.v4.state);
	printf("info.v4.reconnect: %d\r\n", info.v4.reconnect);

	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
*/
    while(1)
    {
        printf("[GPS ]  GPS  heap(%d)\r\n",ql_rtos_get_free_heap_size());
        printf("========GPS OPEN :[%d]s===== !\n");
        ql_gnss_open(quec_gnss_callback);
        //ql_get_agps_sate_num(quec_gnss_agps_callback);  //ע���ȡagps���������ص�����
        ql_rtos_task_sleep_s(10);
       // printf("========GPS CLOSE!!===== !\n");
        //ql_gnss_close();
		//ql_rtos_task_resume(mQl_Task);
        //ql_rtos_task_sleep_s(1);
    }
}
static void quec_gnss_monitor_task(void * argv)
{
    while(1){
        
     ql_monitor_param_print();
     ql_rtos_task_sleep_s(5);
    }
}

application_init(quec_gnss_test, "quec_gnss_test", 10, 2);
//application_init(datacall_start, "data_call", 10, 0);
//application_init(quec_gnss_monitor_task, "xxd", 10, 0);

// Function to remove colons from a string
void remove_colons(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != ':') {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0'; // Null-terminate the string
}
char jsonOutput[2048];  // Sử dụng bộ nhớ tĩnh

char *convert_to_json2(const char *inputString) {
    int len = sprintf(jsonOutput, "{\n\"RequestId\": \"82cb34c9-88c3\",\n\"DeviceId\": \"device:W12345\",\n\"WiFiMeasurements\": [\n");
    const char *line = inputString;
    const char *next_line;
    char bssid[20];
    int signalStrength, channel;

    while ((next_line = strchr(line, '\n')) != NULL) {
        sscanf(line, "+QWIFISCAN: (-,-,%d,\"%[^\"]\",%d)", &signalStrength, bssid, &channel);
        remove_colons(bssid);
        len += snprintf(jsonOutput + len, sizeof(jsonOutput) - len, "    {\"BSSID\":\"%s\",\"signalStrength\":%d,\"channel\":%d},\n",
                       bssid, signalStrength, channel);
        line = next_line + 1; // Move past the newline character
    }

    if (len > 0 && jsonOutput[len - 2] == ',') {
        jsonOutput[len - 2] = '\n'; // Replace the last comma with a newline
        jsonOutput[len - 1] = ']';  // Close the array
        len += sprintf(jsonOutput + len, "}\n"); // Close the JSON object
    }

    return jsonOutput;  // Return the pointer to the static buffer
}

void ql_wifiscan_callback ( unsigned char scan_count, const char *scan_info)
{
	//printf ("=[APP]==wifiscan:%d\n %s===\r\n", scan_count, scan_info);
	convert_to_json2(scan_info);
    if (jsonOutput != NULL) {
        printf("%s", jsonOutput);
       
    }
	ql_rtos_task_resume(mQl_Task2);
}

void ql_nw_test_wifiscan_task()
{
	QL_NW_ERROR_CODE ret = 0;
#if 0
    struct ql_data_call_info info = {0};
	char ip4_addr_str[16] = {0};
	if(datacall_satrt()!=0)
		return;

	ql_get_data_call_info(1, 0, &info);

	printf("info.profile_idx: %d\r\n", info.profile_idx);
	printf("info.ip_version: %d\r\n", info.ip_version);
	printf("info.v4.state: %d\r\n", info.v4.state);
	printf("info.v4.reconnect: %d\r\n", info.v4.reconnect);

	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);

	printf("\n\n============ql nw wifiscan start============\n");
#endif
	int event[1] ={0x00};
	while(1)
	{
		ql_rtos_queue_wait(mQueue, event, sizeof(int), QL_WAIT_FOREVER);
		if(event[0] == 1) {
			ql_nw_set_wifiscan_callback(ql_wifiscan_callback);
			ret = ql_nw_get_wifiscan_info(15000,2,6,5,0,0);//��ʱʱ��15s,����10��
			printf("Start wifiscan ret(%d), wait\r\n",ret);
			//ql_rtos_task_sleep_s(5);
			ql_rtos_timer_stop(mTimer);
			event[0] = 0;
		}
	}
}

//application_init(ql_nw_test_wifiscan_task, "ql_nw_test_wifiscan_task", 10, 2);

#define PRINT_BUF_SIZE 65
static char print_buf[PRINT_BUF_SIZE];

void print_string(const char * string,int len)
{
	int printed = 0;
	while (printed != len) 
	{
		if ((len - printed) > (PRINT_BUF_SIZE - 1)) 
		{
			memcpy(print_buf, string + printed, (PRINT_BUF_SIZE - 1));
			printed += (PRINT_BUF_SIZE - 1);
			print_buf[PRINT_BUF_SIZE - 1] = '\0';
		} 
		else 
		{
			snprintf(print_buf,len - printed+1, "%s", string + printed);
			printed = len;
		}
		printf("%s", print_buf);
	}
	printf("\n");
}

static int response_cb(
	QL_HTTP_CLIENT_T *client,
	QL_HTTP_CLIENT_EVENT_E event,
	int status_code, 
	char *data, 
	int data_len, 
	void *private_data)
{
	int ret=0;
	char para[64]={0};
	printf("event = %d\n", event);
	printf("Client status: %d \n", client->response.status_code);
	switch(event)
	{
		case QL_HTTP_CLIENT_EVENT_SEND_FAIL:
			printf("http send failed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_SEND_SUCCESSED:
			printf("http send successed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_HEADER_FAIL:
			printf("http parse response header failed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_HEADER_FINISHED:
			printf("http recv header status_code:%d;header_len:%d!\n",status_code,data_len);
			//print_string(data,data_len);
			ql_uart_write(QL_USB_CDC_PORT, data, data_len);
			ret=ql_http_client_get_header(data,"Content-Length",0,para,sizeof(para));
			if(ret>0)
				printf("Content-Length[len:%d]:%s!\n",ret,para);
			if(status_code==200)
				printf("http recv response ok!\n");
			ret=1; //继续接受body数据，返回0，不继续接受body数据
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_BODY:
			printf("http recv body status_code:%d;recv_body_len:%d!\n",status_code,data_len);
			//print_string(data,data_len);
			ret=1; //继续接受body数据，返回0，不继续接受body数据
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_BODY_FINISHED:
			printf("http recv body finished!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_DISCONNECTED:
			printf("http be closed by server!\n");
			break;
		default:
			break;
	}
	return ret;
}

static void ql_datacall_status_callback(int profile_idx, int status)
{
	printf("profile(%d) status: %d\r\n", profile_idx, status);
}

static int datacall_satrt(void)
{
	int i=0;
	printf("wait for network register done\r\n");

	while(i<10)
	{
		if(ql_network_register_wait(120) == 0)
		{
			break;
		}
		else 
		{
			i++;
		}
		
	}

	if(i>=10)
	{
		printf("*** network register fail ***\r\n");
		return 1;
	}
	
	printf("doing network activing ...\r\n");
	
	ql_wan_start(ql_datacall_status_callback);
	ql_set_auto_connect(1, TRUE);
	ql_start_data_call(1, 0, NULL, NULL, NULL, 0);

	return 0;
}

const static SSLCertPathPtr rootCA_path = 
"\
-----BEGIN CERTIFICATE-----\n\
MIIJrzCCCJegAwIBAgIMLO4ZPBiCeOo+Q3VzMA0GCSqGSIb3DQEBCwUAMGYxCzAJ\n\
BgNVBAYTAkJFMRkwFwYDVQQKExBHbG9iYWxTaWduIG52LXNhMTwwOgYDVQQDEzNH\n\
bG9iYWxTaWduIE9yZ2FuaXphdGlvbiBWYWxpZGF0aW9uIENBIC0gU0hBMjU2IC0g\n\
RzIwHhcNMTkwNTA5MDEyMjAyWhcNMjAwNjI1MDUzMTAyWjCBpzELMAkGA1UEBhMC\n\
Q04xEDAOBgNVBAgTB2JlaWppbmcxEDAOBgNVBAcTB2JlaWppbmcxJTAjBgNVBAsT\n\
HHNlcnZpY2Ugb3BlcmF0aW9uIGRlcGFydG1lbnQxOTA3BgNVBAoTMEJlaWppbmcg\n\
QmFpZHUgTmV0Y29tIFNjaWVuY2UgVGVjaG5vbG9neSBDby4sIEx0ZDESMBAGA1UE\n\
AxMJYmFpZHUuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtMa/\n\
2lMgD+pA87hSF2Y7NgGNErSZDdObbBhTsRkIsPpzRz4NOnlieGEuVDxJfFbawL5h\n\
VdVCcGoQvvW9jWSWIQCTYwmHtxm6DiA+SchT7QKPRgHroQeTc7vt8bPJ4vvd8Dkq\n\
g630QZi8huq6dKim49DlxY6zC7LSrJF0Dv+AECM2YmUItIf1VwwlxwDY9ahduDNB\n\
pypf2/pwniG7rkIWZgdp/hwmKoEPq3Pj1lIgpG2obNRmSKRv8mgKxWWhTr8EekBD\n\
HNN1+3WsGdZKNQVuz9Vl0UTKawxYBMSFTx++LDLR8cYo+/kmNrVt+suWoqDQvPhR\n\
3wdEvY9vZ8DUr9nNwwIDAQABo4IGGTCCBhUwDgYDVR0PAQH/BAQDAgWgMIGgBggr\n\
BgEFBQcBAQSBkzCBkDBNBggrBgEFBQcwAoZBaHR0cDovL3NlY3VyZS5nbG9iYWxz\n\
aWduLmNvbS9jYWNlcnQvZ3Nvcmdhbml6YXRpb252YWxzaGEyZzJyMS5jcnQwPwYI\n\
KwYBBQUHMAGGM2h0dHA6Ly9vY3NwMi5nbG9iYWxzaWduLmNvbS9nc29yZ2FuaXph\n\
dGlvbnZhbHNoYTJnMjBWBgNVHSAETzBNMEEGCSsGAQQBoDIBFDA0MDIGCCsGAQUF\n\
BwIBFiZodHRwczovL3d3dy5nbG9iYWxzaWduLmNvbS9yZXBvc2l0b3J5LzAIBgZn\n\
gQwBAgIwCQYDVR0TBAIwADBJBgNVHR8EQjBAMD6gPKA6hjhodHRwOi8vY3JsLmds\n\
b2JhbHNpZ24uY29tL2dzL2dzb3JnYW5pemF0aW9udmFsc2hhMmcyLmNybDCCA0kG\n\
A1UdEQSCA0AwggM8ggliYWlkdS5jb22CEmNsaWNrLmhtLmJhaWR1LmNvbYIQY20u\n\
cG9zLmJhaWR1LmNvbYIQbG9nLmhtLmJhaWR1LmNvbYIUdXBkYXRlLnBhbi5iYWlk\n\
dS5jb22CEHduLnBvcy5iYWlkdS5jb22CCCouOTEuY29tggsqLmFpcGFnZS5jboIM\n\
Ki5haXBhZ2UuY29tgg0qLmFwb2xsby5hdXRvggsqLmJhaWR1LmNvbYIOKi5iYWlk\n\
dWJjZS5jb22CEiouYmFpZHVjb250ZW50LmNvbYIOKi5iYWlkdXBjcy5jb22CESou\n\
YmFpZHVzdGF0aWMuY29tggwqLmJhaWZhZS5jb22CDiouYmFpZnViYW8uY29tgg8q\n\
LmJjZS5iYWlkdS5jb22CDSouYmNlaG9zdC5jb22CCyouYmRpbWcuY29tgg4qLmJk\n\
c3RhdGljLmNvbYINKi5iZHRqcmN2LmNvbYIRKi5iai5iYWlkdWJjZS5jb22CDSou\n\
Y2h1YW5rZS5jb22CCyouZGxuZWwuY29tggsqLmRsbmVsLm9yZ4ISKi5kdWVyb3Mu\n\
YmFpZHUuY29tghAqLmV5dW4uYmFpZHUuY29tghEqLmZhbnlpLmJhaWR1LmNvbYIR\n\
Ki5nei5iYWlkdWJjZS5jb22CEiouaGFvMTIzLmJhaWR1LmNvbYIMKi5oYW8xMjMu\n\
Y29tggwqLmhhbzIyMi5jb22CDiouaW0uYmFpZHUuY29tgg8qLm1hcC5iYWlkdS5j\n\
b22CDyoubWJkLmJhaWR1LmNvbYIMKi5taXBjZG4uY29tghAqLm5ld3MuYmFpZHUu\n\
Y29tggsqLm51b21pLmNvbYIQKi5zYWZlLmJhaWR1LmNvbYIOKi5zbWFydGFwcHMu\n\
Y26CESouc3NsMi5kdWFwcHMuY29tgg4qLnN1LmJhaWR1LmNvbYINKi50cnVzdGdv\n\
LmNvbYISKi54dWVzaHUuYmFpZHUuY29tggthcG9sbG8uYXV0b4IKYmFpZmFlLmNv\n\
bYIMYmFpZnViYW8uY29tggZkd3ouY26CD21jdC55Lm51b21pLmNvbYIMd3d3LmJh\n\
aWR1LmNughB3d3cuYmFpZHUuY29tLmNuMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggr\n\
BgEFBQcDAjAdBgNVHQ4EFgQUdrXm1kn4+DbqdaltXk1VWzdc/ccwHwYDVR0jBBgw\n\
FoAUlt5h8b0cFilTHMDMfTuDAEDmGnwwggEEBgorBgEEAdZ5AgQCBIH1BIHyAPAA\n\
dgC72d+8H4pxtZOUI5eqkntHOFeVCqtS6BqQlmQ2jh7RhQAAAWqaLuGaAAAEAwBH\n\
MEUCICx7TcD5hUeKLQrAeTvWtLVm+Kr7glitIzb+Frymg5khAiEAwC/NnJkgy32R\n\
X9KLxhMQc7XBVAMzQZ+masUUk89pK2sAdgBvU3asMfAxGdiZAKRRFf93FRwR2QLB\n\
ACkGjbIImjfZEwAAAWqaLt5PAAAEAwBHMEUCIAMyaJ450OtfGWHbpxJpbyhEgQKl\n\
PMKjE9V+mCZfIBqgAiEAp4tis7C0RDLiEf9FjVURLDarKZNEyDRcznw1VzGuqxIw\n\
DQYJKoZIhvcNAQELBQADggEBAKq5zVKO3DZdR9SL8zIXBkaDYKMnBUkpsRtGbjj+\n\
k/4JQ2zSoVgkEkK3q0H4Rwp9ZLV13FpFFLKkGGuctzuPs37SvcBySzUFrg0tGR9Q\n\
c3Ja35cYO9sq895EzmQtwR6EzHYkPjBnIyboT/cL9uxp139RqaBvuMQU4sBKSsQA\n\
XVdqyUHEJSsyGKpiqB5JgXMcgV9e+uSUMsNQbY6qzGxMUwz6j040eZ+lYMD4UHW4\n\
oZ0B5qslIww7JAJAWCT/NAKLlGEQaC+2gOPQX0oKpwLSwJg+HegCyCdxJrKoh7bb\n\
nRBHS8ITYjTG0Dw5CTklj/6i9PP735snPfzQKOht3N0X0x8=\n\
-----END CERTIFICATE-----\n\
";
/*const char *json_string = "{"
    "\"token\":\"pk.c2eed62095b562dc8d1fc5b889c2241e\","
    "\"radio\":\"lte\","
    "\"mcc\":452,"
    "\"mnc\":4,"
    "\"cells\":[{\"lac\":13006,\"cid\":2608649}],"
    "\"address\":1"
"}";*/
char *json_string = "{\n"
                       "  \"RequestId\": \"82cb34c9-88c3\",\n"
                       "  \"DeviceId\": \"device:W12345\",\n"
                       "  \"WiFiMeasurements\": [\n"
                       "    {\"BSSID\":\"F40D9F26D225\",\"signalStrength\":-42,\"channel\":1},\n"
                       "    {\"BSSID\":\"10394EE04130\",\"signalStrength\":-50,\"channel\":1}\n"
                       "  ]\n"
                       "}";
static void ql_http_client_test(void *arg)
{
    char private_data[]="hello world!";
    int response_code,run_num=0,ret=0;
    QL_HTTP_CLIENT_T * client = NULL;
    struct http_client_list * header = NULL;
	struct ql_data_call_info info = {0};
	char ip4_addr_str[16] = {0};
	char url[64]="https://210.245.54.60:1443";
	char data[]="{uaername:test;password:test}";

	SSLConfig sslConfig = 
	{
		.en = 0,
		.profileIdx = PROFILE_IDX,
		.serverName = "210.245.54.60",
		//.serverName = "locationapi.services.u-blox.com",
		.serverPort = 1443,
		.protocol = 0,
		.dbgLevel = 0,
		.sessionReuseEn = 0,
		.vsn = SSL_VSN_ALL,
		.verify = SSL_VERIFY_MODE_OPTIONAL,
		.cert.from = SSL_CERT_FROM_BUF,
		.cert.path.rootCA = rootCA_path,
		.cert.path.clientKey = NULL,
		.cert.path.clientCert = NULL,
		.cert.clientKeyPwd.data = NULL,
		.cert.clientKeyPwd.len = 0,
		.cipherList = "ALL",
		.CTRDRBGSeed.data = NULL,
		.CTRDRBGSeed.len = 0
	};
	
    //ql_log_mask_set(QL_LOG_APP_MASK,QL_LOG_PORT_USB);
#if 0
	if(datacall_satrt()!=0)
		return;

	ql_get_data_call_info(1, 0, &info);

	printf("info.profile_idx: %d\r\n", info.profile_idx);
	printf("info.ip_version: %d\r\n", info.ip_version);
	printf("info.v4.state: %d\r\n", info.v4.state);
	printf("info.v4.reconnect: %d\r\n", info.v4.reconnect);

	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
#endif
	
    client = ql_http_client_init();

	while (client) 
	{
		
		printf("\n\n==============http_send_position[%d]================\n",run_num);
		client = ql_http_client_init();

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PDP_CID, PROFILE_IDX);              /*set PDP cid,if not set,using default PDP*/
		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PROTOCOL_VER, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_ENABLE_COOKIE, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/
		printf("ql_http_client_setopt: QL_HTTP_CLIENT_OPT_ENABLE_COOKIE    ret :%d  \n",ret);

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_SSL_CTX, &sslConfig);
	

		//if(run_num==0) {
			header = ql_http_client_list_append(header, "Content-Type: application/json\r\n");
		//	header = ql_http_client_list_append(header, "Connection: keep-alive\r\n");
			int len = sprintf(mData,
                       "{"
                       "\"time\":\"%s\","
                       "\"device\":\"%s\","
                       "\"event\":\"%s\","
                       "\"battery\":\"%s\","
                       "\"longitude\":\"%.8f\","
                       "\"latitude\":\"%.8f\","
                       "\"speed\":\"%.3f\","
                       "\"sat\":\"%d\""
                       "}",
                       mHttpPackage.isoDateTime, "353500722222223", "1", "15", mHttpPackage.lon, mHttpPackage.lat, mHttpPackage.speed, mHttpPackage.numOfSat);
            printf("Num of Sat: %d\n", mHttpPackage.numOfSat);
			ret = ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_HTTPHEADER, header);
			ret=ql_http_client_request(client, "https://210.245.54.60:1443/tracking/internal/status", QL_HTTP_CLIENT_REQUEST_POST,QL_HTTP_CLIENT_AUTH_TYPE_NONE,NULL,NULL,mData,strlen(mData),response_cb,NULL);
			
		
		printf("\n\n==============http_send_position[%d] end================\n",run_num);
		run_num++;
		ql_http_client_list_destroy(header);
		header = NULL;
		ql_rtos_task_sleep_s(5);
		ql_http_client_release(client); /*release http resources*/
        ql_rtos_task_suspend(mQl_Task);
	}
	
}
char lat[32], lon[32], datetime[64];
void parse_json(const char *json, const char *key, char *buffer) {
    const char *key_start = strstr(json, key);
    if (key_start) {
        key_start += strlen(key) + 1; // Skip key and ":"
        const char *value_start = key_start;
        while (*value_start == ' ' || *value_start == '\"') value_start++; // Skip spaces and the quote
        const char *value_end = value_start;
        while (*value_end != ',' && *value_end != '}' && *value_end != '\"') value_end++;
        strncpy(buffer, value_start, value_end - value_start);
        buffer[value_end - value_start] = '\0'; // Ensure null-terminated
    }
}
static int response_cb2(
	QL_HTTP_CLIENT_T *client,
	QL_HTTP_CLIENT_EVENT_E event,
	int status_code, 
	char *data, 
	int data_len, 
	void *private_data)
{
	int ret=0;
	char para[64]={0};
	printf("Client status: %d \n", client->response.status_code);
	switch(event)
	{
		case QL_HTTP_CLIENT_EVENT_SEND_FAIL:
			printf("http send failed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_SEND_SUCCESSED:
			printf("http send successed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_HEADER_FAIL:
			printf("http parse response header failed!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_HEADER_FINISHED:
			//printf("http recv header status_code:%d;header_len:%d!\n",status_code,data_len);
			//print_string(data,data_len);
			ret=ql_http_client_get_header(data,"Content-Length",0,para,sizeof(para));
			if(ret>0)
				printf("Content-Length[len: %d]: %s!\n",ret,para);
			if(status_code==200)
				printf("http recv response ok!\n");
			ret=1; //继续接受body数据，返回0，不继续接受body数据
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_BODY:
			//printf("http recv body status_code:%d;recv_body_len:%d!\n",status_code,data_len);
			ql_uart_write(QL_USB_CDC_PORT, data, data_len);

            parse_json(data, "\"Lat\"", lat);
            parse_json(data, "\"Lon\"", lon);
            parse_json(data, "\"DateTime\"", datetime);
            
            mHttpPackage.lat = atof(lat);
            mHttpPackage.lon = atof(lon);
            strcpy(mHttpPackage.isoDateTime, datetime); 
            mHttpPackage.isoDateTime[strlen(datetime)]= 'Z';
            mHttpPackage.isoDateTime[strlen(datetime) + 1] = 0;   
            mHttpPackage.numOfSat = -1;
            mHttpPackage.speed = 0;

           /*printf("Latitude: %s\n", lat);
            printf("Longitude: %s\n", lon);
            printf("DateTime: %s\n", datetime);*/


            printf(" Google Maps Link: https://www.google.com/maps?q=%s,%s\n", lat, lon);
			//print_string(data,data_len);
            ql_rtos_task_resume(mQl_Task);
			ret=1; //继续接受body数据，返回0，不继续接受body数据
			break;
		case QL_HTTP_CLIENT_EVENT_RECV_BODY_FINISHED:
			printf("http recv body finished!\n");
			break;
		case QL_HTTP_CLIENT_EVENT_DISCONNECTED:
			printf("http be closed by server!\n");
			break;
		default:
			break;
	}
	return ret;
}

static void ql_http_client_2(void *arg)
{
    char private_data[]="hello world!";
    int response_code,run_num=0,ret=0;
    QL_HTTP_CLIENT_T * client = NULL;
    struct http_client_list * header = NULL;
	struct ql_data_call_info info = {0};
	char ip4_addr_str[16] = {0};
	char url[64]="https://210.245.54.60:1443";
	char data[]="{uaername:test;password:test}";

	SSLConfig sslConfig = 
	{
		.en = 0,
		.profileIdx = PROFILE_IDX,
		//.serverName = "210.245.54.60",
		.serverName = "locationapi.services.u-blox.com",
		.serverPort = 443,
		.protocol = 0,
		.dbgLevel = 0,
		.sessionReuseEn = 0,
		.vsn = SSL_VSN_ALL,
		.verify = SSL_VERIFY_MODE_OPTIONAL,
		.cert.from = SSL_CERT_FROM_BUF,
		.cert.path.rootCA = rootCA_path,
		.cert.path.clientKey = NULL,
		.cert.path.clientCert = NULL,
		.cert.clientKeyPwd.data = NULL,
		.cert.clientKeyPwd.len = 0,
		.cipherList = "ALL",
		.CTRDRBGSeed.data = NULL,
		.CTRDRBGSeed.len = 0
	};
	
    //ql_log_mask_set(QL_LOG_APP_MASK,QL_LOG_PORT_USB);

	//if(datacall_satrt()!=0)
	//	return;

	/*ql_get_data_call_info(1, 0, &info);

	printf("info.profile_idx: %d\r\n", info.profile_idx);
	printf("info.ip_version: %d\r\n", info.ip_version);
	printf("info.v4.state: %d\r\n", info.v4.state);
	printf("info.v4.reconnect: %d\r\n", info.v4.reconnect);

	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);*/

	
    client = ql_http_client_init();
	//ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PDP_CID, PROFILE_IDX);              /*set PDP cid,if not set,using default PDP*/
	//ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PROTOCOL_VER, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/

	//ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_ENABLE_COOKIE, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/
	//printf("ql_http_client_setopt: QL_HTTP_CLIENT_OPT_ENABLE_COOKIE    ret :%d  \n",ret);

	//ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_SSL_CTX, &sslConfig);
	//header = ql_http_client_list_append(header, "Content-Type: application/json\r\n");
	//header = ql_http_client_list_append(header, "Connection: keep-alive\r\n");
	//ret = ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_HTTPHEADER, header);
	while (client) 
	{
		
		printf("\n\n==============http_get_position[%d]================\n",run_num);
		client = ql_http_client_init();

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PDP_CID, PROFILE_IDX);              /*set PDP cid,if not set,using default PDP*/
		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_PROTOCOL_VER, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_ENABLE_COOKIE, 1);              /*"0" is HTTP 1.1, "1" is HTTP 1.0*/
		printf("ql_http_client_setopt: QL_HTTP_CLIENT_OPT_ENABLE_COOKIE    ret :%d  \n",ret);

		ret=ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_SSL_CTX, &sslConfig);
	

		//if(run_num==0) {
			header = ql_http_client_list_append(header, "Content-Type: application/json\r\n");
		//	header = ql_http_client_list_append(header, "Connection: keep-alive\r\n");
			/*int len = sprintf(mData,
                       "{"
                       "\"time\":\"%s\","
                       "\"device\":\"%s\","
                       "\"event\":\"%s\","
                       "\"battery\":\"%s\","
                       "\"longitude\":\"%.8f\","
                       "\"latitude\":\"%.8f\","
                       "\"speed\":\"%.3f\","
                       "\"sat\":\"%d\""
                       "}",
                       mHttpPackage.isoDateTime, "353500722222222", "1", "15", mHttpPackage.lon, mHttpPackage.lat, mHttpPackage.speed, mHttpPackage.numOfSat);*/
			ret = ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_HTTPHEADER, header);
			//if(ret==QL_HTTP_CLIENT_ERR_LAST_REQUEST_NOT_FINISH)
		//	{
			//	printf("last request not finish, not to set!\n");
			//}
			//ret=ql_http_client_request(client, "https://210.245.54.60:1443/tracking/internal/status", QL_HTTP_CLIENT_REQUEST_POST,QL_HTTP_CLIENT_AUTH_TYPE_NONE,NULL,NULL,mData,strlen(mData),response_cb,NULL);
			//ret=ql_http_client_request(client, "https://us1.unwiredlabs.com/v2/process", QL_HTTP_CLIENT_REQUEST_POST,QL_HTTP_CLIENT_AUTH_TYPE_NONE,NULL,NULL,json_string,strlen(json_string),response_cb,NULL);
			ret=ql_http_client_request(client, "https://locationapi.services.u-blox.com/v1/unipos/s2s/location/get", QL_HTTP_CLIENT_REQUEST_POST,QL_HTTP_CLIENT_AUTH_TYPE_BASE,"XT13W8SYDPXF0J8FD3IL","FyKUzUN7vD0bM6CxTF+IME6Co94hLbCaRW2Szj/i",jsonOutput,strlen(jsonOutput),response_cb2,NULL);
	//	}
        	
		/*else {
				header = ql_http_client_list_append(header, "Content-Type: application/json\r\n");
				ql_http_client_setopt(client, QL_HTTP_CLIENT_OPT_HTTPHEADER, header);
				ret = 0;
				if(ret==QL_HTTP_CLIENT_ERR_LAST_REQUEST_NOT_FINISH)
				{
					printf("last request not finish, not to set!\n");
				}

				ret=ql_http_client_request(client, "https://locationapi.services.u-blox.com/v1/unipos/s2s/location/get", QL_HTTP_CLIENT_REQUEST_POST,QL_HTTP_CLIENT_AUTH_TYPE_BASE,"XT13W8SYDPXF0J8FD3IL","FyKUzUN7vD0bM6CxTF+IME6Co94hLbCaRW2Szj/i",json_string,strlen(json_string),response_cb,NULL);
				if(ret == QL_HTTP_CLIENT_ERR_LAST_REQUEST_NOT_FINISH)
				{
					printf("last request not finish, can not to request again!\n");
				}
		
			printf("ql_http_client_request ret=%d!\n",ret);
		}*/
		
		
		printf("\n\n==============http_get_position[%d] end================\n",run_num);
		run_num++;
		ql_http_client_list_destroy(header);
		header = NULL;
		ql_rtos_task_sleep_s(5);
		ql_http_client_release(client); /*release http resources*/
        ql_rtos_task_suspend(mQl_Task2);
	}
	
}

int ql_init(void *arg) {
    QlOSStatus status;
    
    struct ql_data_call_info info = {0};
	char ip4_addr_str[16] = {0};
	ql_uart_config_t dcb;

    ql_uart_open(QL_USB_CDC_PORT, QL_UART_BAUD_115200, QL_FC_NONE);

    ql_uart_register_cb(QL_USB_CDC_PORT, quec_usbcdc_callback);
	ql_uart_write(QL_USB_CDC_PORT, "create http task\n", 18);
    printf("create http task\n");

    if(datacall_satrt()!=0)
		return;

	ql_get_data_call_info(1, 0, &info);

	printf("info.profile_idx: %d\r\n", info.profile_idx);
	printf("info.ip_version: %d\r\n", info.ip_version);
	printf("info.v4.state: %d\r\n", info.v4.state);
	printf("info.v4.reconnect: %d\r\n", info.v4.reconnect);

	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	printf("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
    
	ql_rtos_timer_create(&mTimer);
    //ql_rtos_timer_start(mTimer, 1000, 0, timer_Callback, NULL);
    ql_rtos_queue_create(&mQueue, sizeof(int), 8);

    status = ql_rtos_task_create(&mQl_Task, 4096, 100,"ql_http_test", ql_http_client_test, NULL);
    ql_rtos_task_create(&wifi_Task, 10240, 100, "wifi_scan_task",ql_nw_test_wifiscan_task, NULL );
	ql_rtos_task_create(&mQl_Task2, 4096, 100,"ql_http_test", ql_http_client_2, NULL);

    ql_rtos_task_suspend(mQl_Task);
    ql_rtos_task_suspend(mQl_Task2);
}
application_init(ql_init, "ql_http_client_test", 2, 3);

