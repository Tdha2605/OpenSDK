/*================================================================
  Copyright (c) 2022, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/
/**
 ******************************************************************************
 * @file    example_tts.c
 * @author  august.yang
 * @version V1.0.0
 * @date    2022/04/15
 * @brief   This file tests TTS APIs
 ******************************************************************************
 */


#include <stdio.h>
#include "ql_rtos.h"
#include "ql_application.h"
#include "ql_audio.h"
#include "ql_tts.h"
void test_cb(QL_TTS_ENVENT_E event, char *str)
{
	printf("user_cb :event = %d,str = %s\n",event,str);
}

static void quec_tts_test(void * argv)
{
	//�ȴ�audio��ʼ����
	while(!ql_get_audio_state()){
		ql_rtos_task_sleep_ms(50);
	}
	
	printf("quec_tts_test start ...\r\n");
	
	ql_set_audio_path_receiver();
	
	ql_codec_choose(AUD_INTERNAL_CODEC, NULL);
	ql_tts_init(test_cb);
	ql_set_volume(8);
			
	ql_tts_set_speed(0);									//����tts�����ٶȣ�������Χ-32768~32767
	ql_tts_set_volume(0);									//����tts������������Χ-32768~32767	
	ql_tts_digit_mode(0);									//���ִ�����ԣ�0�Զ��жϣ�1���������봦��2��������ֵ����
	//while(1)
	{		
        /*mode1->utf16 mode2->gbk mode3->utf8*/
		ql_tts_play(2,"hello,123��ӭʹ����Զģ��");					//mode2,ʹ��GBK���뷽ʽ
		ql_rtos_task_sleep_s(3); 							//�ȴ�5S,����ttsΪ�첽���ţ��������û�ֱ�Ӵ��
		//ql_tts_play(3,"6B228FCE4F7F752879FB8FDC6A215757");	//mode3,ʹ��UNICODE-8���뷽ʽ
		//ql_rtos_task_sleep_s(5); 							//�ȴ�5S
		ql_tts_stop();
	}
}
// application_init(quec_tts_test, "quec_tts_test", 2, 0);
