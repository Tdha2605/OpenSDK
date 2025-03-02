/**
 ******************************************************************************
 * @file    example_nand_flash.c
 * @author  august.yang
 * @version V1.0.0
 * @date    2020/10/13
 ******************************************************************************
 */
#include <stdio.h>
#include "ql_fs.h"
#include "ql_application.h"
#include "ql_rtos.h"
//#include "ql_spi_nor.h"
enum{
    QL_SPI_DEV0 = 0,
    QL_SPI_DEV2 = 2
};
enum{
    QL_PIN_16_19 ,
    QL_PIN_33_36 
};
enum{
    SPI_CLOCK_13M = 0x0,
	SPI_CLOCK_26M = 0x1,
	SPI_CLOCK_52M = 0x2,  
};

typedef struct  file {
   unsigned int cnt;  
   unsigned char buf[64];  
} file_contex;

static void list_dir(const char *path)
{
	FILE_ID dp = NULL;
	NandFs_info info = {0};
	int ret;
	
	dp = ql_nand_opendir(path);
	if(dp == NULL)
	{
		printf("[FS] *** dir open fail: %s ***\r\n", path);
		return;
	}
	printf("[FS] dir opened: %s\r\n", path);

	printf("[FS] ----- start dir list -----\r\n");
	printf("[FS] type\tsize\tname\r\n");
	while(1)
	{
		ret = ql_nand_readddir(dp, &info);
		if(ret < 0)
		{
			printf("[FS] *** dir read fail: %s ***\r\n", path);
			break;
		}
		else if(ret == 0)
		{
			printf("[FS] ----- end of dir list -----\r\n");
			break;
		}
		else
		{
			printf("[FS] %-4d\t%-4d\t%s\r\n", info.type, info.size, info.name);
		}
	}

	if(dp)
	{
		ql_nand_closedir(dp);
		printf("[FS] dir closed: %s\r\n", path);
	}
}

void nand_flash_dir_test(void *argv)
{
    int ret = 0;
    
    FILE_ID filep;
    size_t status = 0;
    file_contex write_buffer={0};
    ql_rtos_task_sleep_s(5);
    ql_spi_nand_init(QL_SPI_DEV0, QL_PIN_33_36,SPI_CLOCK_26M);
    ret = ql_nand_mkdir("quectel");
    printf("ql_nand_mkdir %d\n",ret);
    filep =ql_nand_fopen("quectel/TEST1.TXT", "wb");
    write_buffer.cnt = 30965;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
    status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);
    
    filep =ql_nand_fopen("quectel/TEST2.TXT", "wb");
    write_buffer.cnt = 30966;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
        status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);
    
    filep =ql_nand_fopen("quectel/TEST3.TXT", "wb");
    write_buffer.cnt = 30967;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
    status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);
    
    ret = ql_nand_mkdir("quectel/pony");
    printf("ql_nand_mkdir1 %d\n",ret);
    filep =ql_nand_fopen("quectel/pony/TEST4.TXT", "wb");
    write_buffer.cnt = 30968;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
        status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);
    
    filep =ql_nand_fopen("quectel/pony/TEST5.TXT", "wb");
    write_buffer.cnt = 30969;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
        status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);
    
    filep =ql_nand_fopen("quectel/pony/TEST6.TXT", "wb");
    write_buffer.cnt = 30970;
    sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
        status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
	/* Check the file write status.  */
	if (status != 68)
	{
		printf("fx_file_write is failede.!!\n");
			/* Error writing to a file, break the loop.  */
		
	}
    ql_nand_fclose(filep);

    list_dir("quectel");
    ql_rtos_task_sleep_ms(50);
    list_dir("quectel/pony");
}

void nand_flash_test(void *argv)
{
    int ret = 0 ;
	FILE_ID filep;
	size_t status = 0;	
	file_contex	local_buffer={0};
    file_contex write_buffer={0};
    unsigned int cnt = 35;
    ql_rtos_task_sleep_s(5);
    ql_spi_nand_init(QL_SPI_DEV0, QL_PIN_33_36,SPI_CLOCK_26M);
	while(1)	
	{
        ret = ql_nand_access("TEST.TXT");
        if(ret != 1){//����ļ������ڿ�ס
            filep = ql_nand_fopen("TEST.TXT", "wb");
            ql_nand_fseek(filep, 0, SEEK_SET);
    		/* Write a string to the test file.  */
            memset(&write_buffer,0,sizeof(write_buffer));
            write_buffer.cnt = 30965;
            sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
    		status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
    		/* Check the file write status.  */
    		if (status != 68)
    		{
    			printf("fx_file_write is failede.!!\n");
     			/* Error writing to a file, break the loop.  */
    			break;
    		}
            /* Close the test file.  */
    	    status =  ql_nand_fclose(filep);
            if (status != NULL)
        	{
    			/* Error deleting the file, break the loop.  */
    			printf("fx_file remove is failede.!!\n");
    			break;
        	}
            printf("file is not exists");  
            break;
        }
        else
        {
            printf("file exists");
            filep = ql_nand_fopen("TEST.TXT", "rb+");	
            if (filep == NULL)
    		{
    			printf("fx_file_create is failede.!!\n");
    			/* Create error, break the loop.  */
    			break;
    		}
            ql_nand_fseek(filep, 0, SEEK_SET);
            /* Read the first 28 bytes of the test file.  */
		    status =  ql_nand_fread(&local_buffer, 1, 68, filep);
            printf("fx_file Reading[%d].!!\n", status);
            /* Check the file read status.	*/
    		if (status != 68)
    		{
    			printf("fx_file checking is failede.!!\n");
    		}
            else
            {
                cnt = local_buffer.cnt;
                cnt++;
            }
			printf("File Context.[%d_%s].!!\n",local_buffer.cnt,local_buffer.buf );
            /* Seek to the beginning of the test file.	*/
    		ql_nand_fseek(filep, 0, SEEK_SET);
    		/* Write a string to the test file.  */
            memset(&write_buffer,0,sizeof(write_buffer));
            write_buffer.cnt = cnt;
            sprintf(write_buffer.buf,"QUECTEL_BUILED_A_SMARTER_WORLD");
    		status =  ql_nand_fwrite( &write_buffer, 1, 68,  filep);
    		/* Check the file write status.  */
    		if (status != 68)
    		{
    			printf("fx_file_write is failede.!!\n");
     			/* Error writing to a file, break the loop.  */
    			break;
    		}
            /* Close the test file.  */
		    status =  ql_nand_fclose(filep);		
            if (status != NULL)
    		{
    			/* Error deleting the file, break the loop.  */
    			printf("fx_file remove is failede.!!\n");
    			break;
    		}
        }
         ql_rtos_task_sleep_ms(1);
	}


}

#define QL_PCM_STATUS_FG_01  0x01
#define QL_PCM_STATUS_FG_02  0x02
#define QL_PCM_TASK_MASK_FG  (QL_PCM_STATUS_FG_01|QL_PCM_STATUS_FG_02)

#define LOG_FILE "867862055278381-02-211124105928" //��־�ļ�
#define LOG_MANAGE_FILE "artc_manage.txt"  //�����ļ�

ql_flag_t save_log_flag_ref = NULL;  //������־��flag
ql_mutex_t log_rd_wr_mutex_lock;    // ��дlog��־�Ļ�����  

static void test_log_thread(void *argv)
{
    unsigned int  save_log_flag  = 0;
    while(1)
    {
        ql_rtos_flag_wait(save_log_flag_ref, QL_PCM_TASK_MASK_FG, QL_FLAG_OR_CLEAR, &save_log_flag, QL_WAIT_FOREVER);   
        ql_rtos_mutex_lock(log_rd_wr_mutex_lock,QL_WAIT_FOREVER); //�����
       
        log_write_file(); //ģ�ⵥ�ļ�׷��д��־

        ql_rtos_mutex_unlock(log_rd_wr_mutex_lock); //����
        ql_rtos_task_sleep_ms(30);
    }
}

void start_test_log_task(void)  //����������־������
{
	ql_task_t test_log_task = NULL;
	ql_rtos_task_create(&test_log_task, 1024*10, 100, "save_log_task", test_log_thread, NULL);
}

void log_write_file()
{
	FILE_ID fp_test;
	char test_str[51] = "hardware03211124203908Terminal self-test completed";

	if(ql_nand_access(LOG_FILE) == 1) 
	{   
        printf("before write log, file size:%d\r\n", ql_nand_fsize(LOG_FILE));  

		fp_test = ql_nand_fopen(LOG_FILE, "rb+");
		if(fp_test == NULL)
		{
			printf("open LOG_FILE fail");
			return;
		}

		ql_nand_fseek(fp_test, 0, SEEK_END);		//ƫ�Ƶ��ļ�ĩ

		ql_nand_fwrite(test_str, 1, strlen(test_str), fp_test);

		ql_nand_fclose(fp_test);

		printf("after write log, file size:%d\r\n", ql_nand_fsize(LOG_FILE));   
	}
	else
	{
		printf("LOG_FILE NO EXIST AND CREATE\r\n");
		fp_test = ql_nand_fopen(LOG_FILE, "wb");
		ql_nand_fclose(fp_test);
	}
	printf("write log file end\r\n");
}


static void demo_main(void * argv)
{
    ql_autosleep_enable(1);
}
//application_init(demo_main, "demo_main", 10, 1);

//application_init(nand_flash_dir_test, "nand_flash_test", 10, 6);



