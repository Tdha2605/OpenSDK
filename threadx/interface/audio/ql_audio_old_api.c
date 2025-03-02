
/*兼容旧同步接口*/
/*这些接口灵活性很低不建议客户使用*/
/*在kernel使用接口请自行移植到kernel*/
#include "ql_audio.h"
#include "ql_audio_old_api.h"
#include "ql_log.h"
#include "ql_fs.h"

#define true 1
#define false 0
#define SEEK_SET 0
#define ID_RIFF 	0x46464952
#define ID_WAVE		0x45564157
#define ID_FMT  	0x20746d66
#define ID_DATA 	0x61746164
#define ID_IOS		0x524c4c46//wav data id by IOS
#define QUEC_PCM_BLOCK_FLAG		(0x01)
#define QUEC_PCM_NONBLOCK_FLAG	(0x02)
#define QUEC_PCM_READ_FLAG    	(0x04)
#define QUEC_PCM_WRITE_FLAG		(0x08)
#define AUDIO_PLAY 1
#define AUDIO_RECORD 2

static char is_support_samplerate(unsigned int rate)
{
    switch(rate) {
        case 8000:
        case 16000:
            return true;
        }
	return false;
}

static int check_wav_file(unsigned int fd, aud_input_para_t *para)
{
	int ret = 0,i = 0;
	struct riff_chunk riff_hdr;
	struct format_chunk format_hdr;
	struct data_chunk data_hdr;
	unsigned int chunk_id = 0,chunk_size = 0,chunk_len = 0;
	unsigned int data = 0,data_size = 0;
	
	if(!para)
	{
		return -1;
	}

	ret = ql_fseek(fd, 0, SEEK_SET);
	if(ret < 0)
	{
		return -1;
	}
	ret = ql_fread(&riff_hdr, sizeof(riff_hdr),1,fd);							//riff
	if(ret < sizeof(riff_hdr))
	{
		ql_fseek(fd, 0, SEEK_SET);
		return -1;
	}
	if( (riff_hdr.riff != ID_RIFF) || (riff_hdr.wave!= ID_WAVE))
	{
		return -1;
	}

	for(i = 0;i < riff_hdr.file_size;){										//format
		ql_fread(&chunk_id, sizeof(chunk_id),1,fd);
		if(chunk_id == ID_FMT){
			ql_fread(&format_hdr.format_size, sizeof(unsigned int),1,fd);
			format_hdr.format = chunk_id;
			break;
		}else{
			ql_fread(&chunk_size, sizeof(chunk_size),1,fd);
			i += (chunk_size + 8);
			if(i > riff_hdr.file_size)
				return -1;
			ret = ql_fseek(fd, chunk_size, 1);
			if(ret < chunk_size)
				return -1;
			chunk_len = i;
		}
				
	}
	ql_fread(&format_hdr.aud_fmt, format_hdr.format_size,1,fd);
	if(is_support_samplerate(format_hdr.sample) == false){
		return -1;
	}


	for(i = 0;i < (riff_hdr.file_size-chunk_len-format_hdr.format_size);){			//data
		ql_fread(&data, sizeof(data),1,fd);
		if(data == ID_DATA){
			ql_fread(&data_hdr.data_size, sizeof(unsigned int),1,fd);
			data_hdr.data = data;
			break;
		}else{
			ql_fread(&data_size, sizeof(data_size),1,fd);
			i += (data_size + 8);
			if(i > riff_hdr.file_size-chunk_len-format_hdr.format_size)
				return -1;
			ret = ql_fseek(fd, data_size, 1);
			if(ret < data_size)
				return -1;
		}
	}

	switch (format_hdr.bits_sample)
	{
		case 8:
			para->format = QUEC_PCM_FORMAT_S8;
			break;
		case 16:
			para->format = QUEC_PCM_FORMAT_S16_LE;
			break;
		case 24:
			para->format = QUEC_PCM_FORMAT_S24_LE;
			break;
		case 32:
			para->format = QUEC_PCM_FORMAT_S32_LE;
			break;
		default:
			return -1;
	}

	para->rate = format_hdr.sample;
	para->channels = format_hdr.channel;
	para->hsize = 4;
	para->type = AUD_STREAM_FORMAT_PCM;
	para->file_size = data_hdr.data_size;

	return 0;	
}

static int check_mp3_file(unsigned int fd)
{
	int ret = 0;
	char head[10] = {0};

	ret = ql_fseek(fd, 0, SEEK_SET);
	if(ret < 0)
	{
		return -1;
	}

	ret = ql_fread(head, sizeof(head),1,fd);
	if(ret < sizeof(head))
	{
		ql_fseek(fd, 0, SEEK_SET);
		return -1;
	}

	if(memcmp(head, "ID3", 3) == 0)
	{
		return 0;
	}
	else if( (head[0] == 0xFF) && (head[1] & 0xF0) == 0xF0 )
	{
		return 0;
	}
	
	return 1;
}

static int check_amr_file(unsigned int fd, aud_input_para_t *para)
{
	int bytesRead = 0;
	int ret = 0;
	char header[6] = {0};
	ret = ql_fseek(fd, 0, SEEK_SET);
	if(ret < 0)
	{
		return -1;
	}
	// Validate the input AMR file
	bytesRead = ql_fread(header, 6,1,fd);
	if (bytesRead != 6 || memcmp(header, "#!AMR\n", 6)) {
		return -1;
	}

	return 0;
}

int check_audio_format(unsigned int fd, aud_input_para_t *para)
{
	
	if(!check_wav_file(fd, para))
	{
		printf("Wav file: rate(%d), channels(%d), format(%d)\n",
					para->rate, para->channels, para->format);
		return AUD_STREAM_FORMAT_PCM;
	}
	else if(!check_mp3_file(fd))
	{
		return AUD_STREAM_FORMAT_MP3;
	}
	else if(!check_amr_file(fd, para))
	{
		return AUD_STREAM_FORMAT_AMR;
	}else{
		printf("check_file error\n");
		return -1;
	}
}

/***********************************************************************
 *
 * Name:       
 *
 * Description: 播放音频文件 ，文件类型可以是mp3，pcm ，wav
 *
 * Parameters:
 *
 * Returns:
 *
 * Notes: 
 *
 ***********************************************************************/
static Mp3PlaybackHandle  mp3_handle;
static Mp3PlayConfigInfo  mp3_configInfo;
static unsigned char is_mp3_playing = 0;
static audio_track_handle wav_handle;
static AmrPlaybackHandle amr_handle;
static AmrPlaybackConfigInfo amr_config;
static struct audio_track_config wav_config;
static unsigned char is_wav_file_playing;
static unsigned char is_amr_file_playing;
static void wav_play_callback(acm_audio_track_handle handle, acm_audio_track_event_t event)
{
    if(handle == wav_handle)
    {
        if(event == AUDIO_TRACK_EVENT_STARTED)
        {
            is_wav_file_playing = 1;
        }
        if(event == AUDIO_TRACK_EVENT_UNDERRUN)
        {
            is_wav_file_playing = 0;
        }
    }
    
}
static void ql_Mp3PlaybackEvent(Mp3PlayEventType event, int val)
{
    if(event == MP3_PLAYBACK_EVENT_STATUS){
       if(val == MP3_PLAYBACK_STATUS_STARTED)
       {
            is_mp3_playing = 1;
       }
       if(val == MP3_PLAYBACK_STATUS_ENDED)
       {
            is_mp3_playing = 0;          
       }
    }
}

static void ql_amr_palyback_event(AmrPlaybackEventType event, int val)
{
    if(event == AMR_PLAYBACK_EVENT_STATUS){
       if(val == AMR_PLAYBACK_STATUS_STARTED)
       {
            is_amr_file_playing = 1;
       }
       if(val == AMR_PLAYBACK_STATUS_ENDED)
       {
            is_amr_file_playing = 0;          
       }
    }
}
int ql_play_file_start(char *file_name)
{
    int rc = 0,ret=0;
    unsigned int fd = 0;
    aud_input_para_t para;
    rc = ql_access(file_name,0);
    if(rc == 1)
    {
        fd = ql_fopen(file_name, "rb");
        if(fd == NULL)
        {
            printf("ql_fopen failed fd = %x\n", fd);
            return -1;
        }
    }
    else
    {
        printf("not find %s\n", file_name);
        return -1;
    }
    ret = check_audio_format(fd, &para);
    ql_fclose(fd);
    if(AUD_STREAM_FORMAT_MP3 == ret)
	{
       if(is_mp3_playing == 1) {
	    printf("error, mp3 is in playing\n");
        return -2;
        }
        mp3_configInfo.listener = ql_Mp3PlaybackEvent;
        ql_play_mp3(file_name,&mp3_handle,&mp3_configInfo);
	}
	else if(AUD_STREAM_FORMAT_PCM == ret)
	{
        if(is_wav_file_playing == 1)
        {
           printf("%s:wav file is in playing\r\n", __func__);
           return -2; 
        }
        wav_config.event_cb = wav_play_callback;
		ql_wav_play(file_name,&wav_config,&wav_handle);
	}
	else if(AUD_STREAM_FORMAT_AMR == ret)
	{
        if(is_amr_file_playing == 1)
        {
           printf("%s:wav file is in playing\r\n", __func__);
           return -2; 
        }
        amr_config.listener = ql_amr_palyback_event;
        ret = ql_play_amr(file_name,&amr_handle,&amr_config);
	}

	return ret;
}

 int ql_play_file_stop(void)
{
	return 0;
}

int ql_wav_file_play(char *file_name)
{
    if(!file_name){
	    printf("%s:file_name NULL\r\n", __func__);
		return -1;
	}
    if(is_wav_file_playing == 1)
    {
       printf("%s:wav file is in playing\r\n", __func__);
       return -2; 
    }
    wav_config.event_cb = wav_play_callback;
    return ql_wav_play(file_name,&wav_config,&wav_handle); 
}

void ql_wav_file_stop(void)
{
    ql_wav_play_stop(wav_handle,1);
}

int ql_mp3_file_play(char *file_name)
{
     if(!file_name){
		printf("mp3Start_nameNULL\n");
		return -1;
	}
	
    if(is_mp3_playing == 1) {
	    printf("error, mp3 is in playing\n");
        return -2;
    }
    mp3_configInfo.listener = ql_Mp3PlaybackEvent;
    ql_play_mp3(file_name,&mp3_handle,&mp3_configInfo);
}

void ql_mp3_file_stop(void)
{
    ql_stop_mp3_play(mp3_handle,0);
}

int ql_amr_file_play(char *file_name)
{
     if(!file_name){
		printf("mp3Start_nameNULL\n");
		return -1;
	}
	
    if(is_amr_file_playing == 1) {
	    printf("error, mp3 is in playing\n");
        return -2;
    }
    amr_config.listener = ql_amr_palyback_event;
    ql_play_amr(file_name,&amr_handle,&amr_config);
}

void ql_amr_file_stop(void)
{
    ql_stop_amr_play(amr_handle,0);
}

 long long QUEC_2_ASR_AUDIO_TRACK_OPTION(unsigned int flags)
{
    AUDIO_PLAY_OPTION option ;
    long long option1;
    memset(&option,0,sizeof(long long));
    if(QUEC_PCM_BLOCK_FLAG&flags)
    {
        option.write_mode = 1;
    }
    else if(QUEC_PCM_NONBLOCK_FLAG&flags)
    {
        option.write_mode = 0;
    }
    option.active_point = 1;
    memcpy(&option1,&option,sizeof(long long));
    return  option1; 
}


/***********************************************************************
 *
 * Name:       
 *
 * Description: 播放音频文件 ，文件类型可以是mp3，pcm ，wav
 *
 * Parameters:
 *
 * Returns:
 *
 * Notes: 
 *
 ***********************************************************************/
 char record_work_mode=0;
PCM_HANDLE_T ql_pcm_open(QL_PCM_CONFIG_T *config, unsigned int flags)
{
    PCM_HANDLE_T quec_audio_handle = malloc(sizeof(ql_pcm_hadle_t));
    long long option = 0;
    if (flags&QUEC_PCM_WRITE_FLAG) 
    {
        quec_audio_handle->record_or_play = AUDIO_PLAY;
        option = QUEC_2_ASR_AUDIO_TRACK_OPTION(flags);
        quec_audio_handle->handle = ql_audio_track_open(config->samplerate, config->channels,option, NULL);
    }
    else if(flags&QUEC_PCM_READ_FLAG)
    {
        if(QUEC_PCM_BLOCK_FLAG)
            record_work_mode = 1;
        else 
            record_work_mode = 0;
        quec_audio_handle->record_or_play = AUDIO_RECORD;
         quec_audio_handle->handle = ql_audio_record_open(config->samplerate, config->channels, option,NULL);
    }
    return (PCM_HANDLE_T)quec_audio_handle;
}

int ql_pcm_read(PCM_HANDLE_T handle, void *data, unsigned int count)
{
    ql_pcm_hadle_t *quec_audio_handle = handle;
    int ret = -1,CNT=0;
    unsigned int count1=0;
    if(count == 0 || data == NULL)
        return ret;
    count1 = count;
    if(record_work_mode == 1)/*由于asr的录音不带阻塞模式，故在此增加阻塞*/
    {
        while(ret&&CNT<10){
            ret = ql_audio_record_read(quec_audio_handle->handle,  data, &count1);
            ql_rtos_task_sleep_ms(5);
            CNT++;
        }
        if(ret<0)
            count1 = 0; 
    }
    else
    {
        ret = ql_audio_record_read(quec_audio_handle->handle,  data, &count1);
        if(ret<0)
            count1 = 0; 
    }

    return count1;
}

int ql_pcm_write(PCM_HANDLE_T handle, void *data, unsigned int count)
{
    ql_pcm_hadle_t *quec_audio_handle = handle;
    return ql_audio_track_write(quec_audio_handle->handle,  data, count);
}

int ql_pcm_close(PCM_HANDLE_T handle) 
{
    ql_pcm_hadle_t *quec_audio_handle = handle;
    int ret = 0;
    if(quec_audio_handle->record_or_play==AUDIO_PLAY)
    {
        ret = ql_audio_track_close(quec_audio_handle->handle,1);
    }
    else if(quec_audio_handle->record_or_play==AUDIO_RECORD)
    {
        ret = ql_audio_record_close(quec_audio_handle->handle,1);
    }
    else 
    {
        ret = -1;
    }
    free(quec_audio_handle);
    return ret;
}

ql_cb_on_player ql_pcm_play_cb;
audio_track_handle track_handle = 0;

static void pcm_play_callback(acm_audio_track_handle handle, acm_audio_track_event_t event)
{
    ql_aud_player_state ql_aug_event = AUD_PLAYER_ERROR;
    if(handle == track_handle)
    {
        if(event == AUDIO_TRACK_EVENT_STARTED)
        {
             if(ql_pcm_play_cb)
                ql_pcm_play_cb(NULL,0,AUD_PLAYER_START);
        }
        if(event == AUDIO_TRACK_EVENT_UNDERRUN)
        {
            if(ql_pcm_play_cb)
                ql_pcm_play_cb(NULL,0,AUD_PLAYER_NODATA);
        }
        if(event == AUDIO_TRACK_EVENT_NEARLY_OVERRUN)
        {
            if(ql_pcm_play_cb)
                ql_pcm_play_cb(NULL,0,AUD_PLAYER_MOREDATA);
        }
        if(event == AUDIO_TRACK_EVENT_NEARLY_UNDERRUN)
        {
            if(ql_pcm_play_cb)
                ql_pcm_play_cb(NULL,0,AUD_PLAYER_LESSDATA);
        }
       
    }
}


int ql_pcm_play_init(ql_cb_on_player play_cb)
{
   ql_pcm_play_cb=play_cb;
   return 0;
}
int ql_pcm_play(unsigned int hdl, unsigned char *wav_buff, int wav_size)
{
    unsigned long long option = 0x80;//voice算法优化
    track_handle = ql_audio_track_open(8000, 1,option, pcm_play_callback);
    return ql_audio_track_write(track_handle, wav_buff, wav_size);
}

int ql_pcm_play_stop(unsigned int hdl)
{
    return ql_audio_track_close(track_handle,0);
}

int ql_pcm_record_deinit(int hdl)
{
    ql_pcm_play_cb=0;
    return 0;
}

