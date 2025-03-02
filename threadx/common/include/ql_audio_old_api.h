/*================================================================
  Copyright (c) 2022, Quectel Wireless Solutions Co., Ltd. All rights reserved.
  Quectel Wireless Solutions Proprietary and Confidential.
=================================================================*/

#ifndef _QL_AUDIO_OLD_API_H_
#define _QL_AUDIO_OLD_API_H_

typedef struct ql_pcm_hadle
{  
    unsigned int handle;
    char record_or_play;
}ql_pcm_hadle_t;

typedef ql_pcm_hadle_t * PCM_HANDLE_T ;
typedef struct {
	unsigned int channels;
	unsigned int samplerate;
	unsigned int periodcnt; // 20ms buffersize/per period, value:2~8
} QL_PCM_CONFIG_T;

 typedef struct aud_input_para{
     int channels;
     int rate;
     int format;
     int hsize;
     int type;
     int file_size;
     int fmt_size;
 }aud_input_para_t;

 struct riff_chunk{
	unsigned int riff;
	unsigned int file_size;
	unsigned int wave;
};

struct format_chunk{
	unsigned int format;
	unsigned int format_size;
	unsigned short aud_fmt;
	unsigned short channel;
	unsigned int sample;
	unsigned int bytes;
	unsigned short block_align;
	unsigned short bits_sample;
	unsigned short others;
};
 typedef enum{
     QUEC_PCM_FORMAT_S8 = 0,
     QUEC_PCM_FORMAT_S16_LE,
     QUEC_PCM_FORMAT_S24_LE,
     QUEC_PCM_FORMAT_S32_LE,
 }QUEC_PCM_FMT_T;

 struct data_chunk{
     unsigned int data;
     unsigned int data_size;
 };
 typedef enum{
     AUD_STREAM_FORMAT_MP3 = 0,
     AUD_STREAM_FORMAT_AMR = 1,
     AUD_STREAM_FORMAT_PCM = 2,
     AUD_STREAM_FORMAT_AMRNB,
     AUD_STREAM_FORMAT_AMRWB,
     AUD_STREAM_FORMAT_END
 }aud_stream_format;

typedef enum
{
    AUD_PLAYER_ERROR = -1,
    AUD_PLAYER_START = 0,
    AUD_PLAYER_PAUSE,
    AUD_PLAYER_RECOVER,
    AUD_PLAYER_PLAYING,
    AUD_PLAYER_NODATA,
    AUD_PLAYER_LESSDATA,
    AUD_PLAYER_MOREDATA,
    AUD_PLAYER_FINISHED,
}ql_aud_player_state;

typedef void(*ql_cb_on_player)(char *p_data, int len, ql_aud_player_state state);

#endif
