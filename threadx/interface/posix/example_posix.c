/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts_posix_test.cpp
 * @brief:
 * @date:    2019/7/10
 * @version: 1.0
 */
#include <stdio.h>
#include "ql_rtos.h"
#include "ql_application.h"
#include "posix.h"

#define posix_log(fmt, args...)	printf(fmt, ##args)

////////////////////////////////////////////////////////////////////////////////////////////////////////
// checkpoint: make sure POSIX API supported
/*
int _tgcts_posix_string(void)
{
    int ret = _TGCTS_FAIL;
    // string
    posix_log("strncasecmp result=%d\n", strncasecmp("Test", "test", strlen("test")));
    posix_log("strncmp result=%d\n", strncmp("Test", "test", strlen("Test")));

    char str2[] = "ab,cd,ef";
    char *ptr;
    char *p;

    posix_log("before strtok:  str2=%s\n", str2);
    posix_log("begin:\n");
    ptr = strtok_r(str2, ",", &p);
    while (ptr != NULL) {
        posix_log("str2=%s\n", str2);
        posix_log("ptr=%s\n", ptr);
        ptr = strtok_r(NULL, ",", &p);
    }

    ret = _TGCTS_SUCCESS;

    return ret;
}

char * _make_message(const char *fmt, ...)
{
    int size = 1024;
    char *p;
    va_list ap;

    p = (char *)malloc(size * sizeof(char));

    if (p == NULL)
        return NULL;

    while (1) {
        int n;
        char *tmp;

        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);

        if (n > -1 && n < size)
            return p;

        size *= 2;
        tmp = (char *)realloc(p, size * sizeof(char));
        if (tmp == NULL) {
            free(p);
            return NULL;
        }
        return tmp;
    }
}

int _tgcts_posix_memory_io(void)
{
    int ret = _TGCTS_FAIL;
    // memory, stdio
    char *str = _make_message("%d,%d,%d,%d", 5, 6, 7, 8);

    if (str != NULL) {
        posix_log("%s\n", str);

        if (strncmp(str, "5,6,7,8", strlen(str)) == 0)
            ret = _TGCTS_SUCCESS;

        free(str);
    }

    return ret;
}

int _tgcts_posix_time(void)
{
    int ret = _TGCTS_FAIL;
    // time
    struct timeval tv;
    struct tm timeinfo;
    //char buffer[64];
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &timeinfo);
    posix_log("get_now_time_str  sec:%ld  timeinfo hour:%d\n", tv.tv_sec, timeinfo.tm_hour);
    //strftime(buffer, 64, "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
    //_TGCTS_LOGD("buffer:%s\n", buffer);

    ret = _TGCTS_SUCCESS;

    return ret;
}

int _tgcts_posix_unistd(void)
{
    int ret = _TGCTS_FAIL;
    // unistd
    ql_rtos_task_sleep_s(1);
    ql_rtos_task_sleep_ms(10);

    ret = _TGCTS_SUCCESS;

    return ret;
}

int _tgcts_posix_stdlib(void)
{
    int ret = _TGCTS_FAIL;
    // stdlib
    srand(time(NULL));
    int irand = rand() % 10;

    posix_log("irand=%d\n", irand);

    ret = _TGCTS_SUCCESS;

    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// checkpoint: make sure POSIX pthread API supported
static int test_num;
static int test_num_mutex;
static int test_count = 10000;
static pthread_t test_thread0;
static pthread_t test_thread1;
static pthread_t test_thread2;
static pthread_mutex_t test_mutex;
static bool run_pthread;

static void _thread_construct(pthread_t *test_thread, void *(*thread_loop)(void *), const char *name)
{
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    struct sched_param sched;

    void * base;
    int size;
    base = (void *) malloc(1024);
    pthread_attr_setstack(&attr, base,1024);
    
    posix_log("base:%p",base);
    sched.sched_priority = 4;
    
    pthread_attr_setschedparam(&attr, &sched);
    pthread_attr_setstacksize(&attr, 1024);
   // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(test_thread, &attr, thread_loop, NULL);    
    //pthread_create(test_thread, NULL, thread_loop, NULL);  
      
    pthread_setname_np(*test_thread, name);
    pthread_attr_destroy(&attr);
}

static void * _thread_loop2(void *arg)
{
    int i;
    posix_log("************_thread_loop2 start************");
    while (run_pthread) {

        pthread_mutex_lock(&test_mutex);
        for ( i = 0; i < test_count; i++)
            test_num_mutex--;
        pthread_mutex_unlock(&test_mutex);

        posix_log("_thread_loop2 test_num_mutex=%d\n", test_num_mutex);     
        ql_rtos_task_sleep_ms(100);
        pthread_t t =  pthread_self();
        posix_log("_thread_loop2 pthread_id:%p",t);
    }
    posix_log("************_thread_loop2 end**************");
    //pthread_exit(NULL);
    return ;
}

static void * _thread_loop1(void *arg)
{  
    posix_log("************_thread_loop1 start************");
    while (run_pthread) {
        int i;
        
        for ( i = 0; i < test_count; i++)
            test_num--;

        posix_log("_thread_loop1 test_num=%d\n", test_num);
        ql_rtos_task_sleep_ms(100);
    }
        pthread_t t =  pthread_self();
        posix_log("_thread_loop1 pthread_id:%p",t);
    posix_log("************_thread_loop1 end**************");
    //pthread_exit(NULL);
   return;
}

static void * _thread_loop0(void *arg)
{
    posix_log("************_thread_loop0 start************");

     _thread_construct(&test_thread1, _thread_loop1, "thread1");
     posix_log("_thread_loop1 pthread_id:%p",test_thread1);
     _thread_construct(&test_thread2, _thread_loop2, "thread2");
     posix_log("_thread_loop2 pthread_id:%p",test_thread2);
    
    while (run_pthread) {
        int i;
        pthread_mutex_lock(&test_mutex);
        for ( i = 0; i < test_count; i++)
            test_num_mutex++;
        pthread_mutex_unlock(&test_mutex);
        for (i = 0; i < test_count; i++)
            test_num++;

        posix_log("test_num_mutex=%d\n", test_num_mutex);
        posix_log("test_num=%d\n", test_num);
        ql_rtos_task_sleep_ms(100);
    }
    

    pthread_join(test_thread2, NULL);
    posix_log("###test_thread2 end###");
    
    pthread_join(test_thread1, NULL); 
    posix_log("###test_thread1 end###");

  
    posix_log("************_thread_loop0 end**************");
    
    //pthread_exit(NULL);
    return ;
}

ql_task_t  taskRef1;



void test()
{
    while(1)
    {
        ql_rtos_task_sleep_ms(100);
         posix_log("******************test1***********");
    }

    return ;
}
int _tgcts_posix_pthread(void)
{


   // ql_rtos_task_create(&taskRef1, 1024, 100, "zfwtest", test, -1);
    // ql_rtos_task_sleep_s(2);
   // ql_rtos_task_delete(taskRef1);


  //  ql_rtos_task_sleep_s(10);

    int ret = _TGCTS_FAIL;
    run_pthread = true;
    pthread_mutex_init(&test_mutex, NULL);
     _thread_construct(&test_thread0, _thread_loop0, "thread0");
    
    ql_rtos_task_sleep_s(1);
    
    posix_log("run_pthread charge");
    run_pthread = false;
    pthread_join(test_thread0, NULL);   
    posix_log("###test_thread0 end###");
    

    if (test_num_mutex == 0)
        ret = _TGCTS_SUCCESS;
    //ql_rtos_task_sleep_s(5);
    
    
    //tgcts_log("temp1->tx_thread_name:%s %s %s",temp0->tx_thread_name, temp1->tx_thread_name, temp2->tx_thread_name);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// checkpoint: make sure POSIX filesystem API supported
#define nmemb 2
struct test_fs {
    char name[20];
    int size;
} test_fs_struct[nmemb];

int _tgcts_posix_filesystem()
{
    int ret = _TGCTS_FAIL;

    int i;
    char *filename = "test_filesystem.txt";
    char *str0 = "alibaba\n";
    char *str1 = "ailabs\n";

    FILE *stream1;
    strcpy(test_fs_struct[0].name, str0);
    test_fs_struct[0].size = strlen(str0);
    strcpy(test_fs_struct[1].name, str1);
    test_fs_struct[1].size = strlen(str1);
    stream1=fopen(filename, "w");
    //for(i=0; i<nmemb; i++)
        //printf("name[%d]=%-20s:size[%d]=%d\n", i, test_fs_struct[i].name, i, test_fs_struct[i].size);
    fwrite(test_fs_struct, sizeof(struct test_fs), nmemb, stream1);
    fclose(stream1);

    //sync();

    memset(test_fs_struct, 0, sizeof(struct test_fs) * nmemb);

    FILE *stream2;
    stream2=fopen(filename, "r");
    fread(test_fs_struct, sizeof(struct test_fs), nmemb, stream2);
    fclose(stream2);
    //for(i=0; i<nmemb; i++)
        //printf("name[%d]=%-20s:size[%d]=%d\n", i, test_fs_struct[i].name, i, test_fs_struct[i].size);

    if((strncmp(test_fs_struct[0].name, str0, strlen(str0)) == 0)
        && (strncmp(test_fs_struct[1].name, str1, strlen(str1)) == 0))
        ret = _TGCTS_SUCCESS;

    return ret;
}

pthread_t test_thread3;
sem_t sem;


void _thread_loop3()
{
    ql_rtos_task_sleep_s(2);
    sem_post(&sem);
    posix_log("loop3 sem post");
   // pthread_exit(NULL);
}

void _tgcts_posix_sem()
{
    
    int pshared = 1;
    int value = 0;
    
    sem_init(&sem, pshared, value);
    _thread_construct(&test_thread3, _thread_loop3, "thread3");
    
    posix_log("wait for loop3 sem post");
    sem_wait(&sem);

    sem_destroy(&sem);
    return ;
   
}

TGCTS_TEST_CASE tgcts_posix_test_cases[] = {
   //{(char *)"posix string API", _tgcts_posix_string},
   // {(char *)"posix memory, io API", _tgcts_posix_memory_io},
    //{(char *)"posix time API", _tgcts_posix_time},
    //{(char *)"posix unistd API", _tgcts_posix_unistd},
   // {(char *)"posix stdlib API", _tgcts_posix_stdlib},
    {(char *)"posix pthread API", _tgcts_posix_pthread},
   // {(char*)"posix sem API", _tgcts_posix_sem},
   // {(char *)"posix filesystem API", _tgcts_posix_filesystem},
};
    void tgcts_run_test(TGCTS_TEST_CASE *test_case)
    {
        if (test_case != NULL) {
            if (test_case->func() == _TGCTS_SUCCESS)
                posix_log("AliGenie Compatibility Test: %s supported\n",
                            test_case->name);
            else
                posix_log("AliGenie Compatibility Test: %s not supported\n",
                            test_case->name);
        } else {
            posix_log("AliGenie Compatibility Test: no test cases yet!\n");
        }
    }

void posix_test(void)
{
    _TGCTS_TEST_BEGIN

    int i;

    for (i = 0; i < sizeof(tgcts_posix_test_cases) / sizeof(TGCTS_TEST_CASE); i++) {
        tgcts_run_test(&tgcts_posix_test_cases[i]);
    }

    _TGCTS_TEST_END
}

*/


pthread_t  test_thread ;
pthread_t  test_thread2 ;
pthread_t  test_thread3 ;
pthread_mutex_t  mutex_t;
pthread_cond_t   cond_t  ;



void thread_loop1(void    *arg)   /*101*/
{
   uint8_t  count = 0 ;
   pthread_t id ;
   while(1)
   {
 
	 // pthread_mutex_lock(&mutex_t);  
	 // posix_log("get_mutex_t\n");	

	 // pthread_mutex_unlock(&mutex_t);
	  posix_log("thread_loop1\n");
	 // id = pthread_self();
	 // posix_log("id :%d\n",id);
	 // pthread_cond_broadcast(&cond_t);
	  ql_rtos_task_sleep_ms(500);
   }
}


void thread_loop2(void *arg)  /*101*/
{
   while(1)
   {
      posix_log("thread_loop2\n");
	 // pthread_cond_signal(&cond_t);
	  ql_rtos_task_sleep_ms(500);
   }

}


void thread_loop3(void *arg)  /*102*/
{
   while(1)
   {

	  //pthread_mutex_lock(&mutex_t);
	  // pthread_mutex_trylock(&mutex_t);
	   posix_log("loop3_get_mutex_t\n");	
       pthread_join(test_thread,NULL);
	  //pthread_mutex_unlock(&mutex_t);
	  ql_rtos_task_sleep_ms(500);
   }

}


/*
void thread_loop4(void *arg)
{
   while(1)
   {
      posix_log("thread_loop4\n");
	  ql_rtos_task_sleep_ms(500);
   }

}*/





void posix_test(void *arg)
{

    pthread_create(&test_thread,NULL, thread_loop1, NULL);   
    pthread_create(&test_thread2,NULL, thread_loop2, NULL);    

  	
    pthread_attr_t attr;   
	attr.schedpolicy = 2 ;
	attr.stacksize = 1024 ;
    pthread_create(&test_thread3,&attr, thread_loop3, NULL);   

    pthread_mutex_init(&mutex_t,NULL);
    pthread_cond_init(&cond_t,NULL);

	
  /*
    pthread_attr_t attr1;

    pthread_attr_init(&attr1);
    struct sched_param sched;
    void * base;
    int size;
    base = (void *) malloc(1024);
    pthread_attr_setstack(&attr, base,1024);
    
    sched.sched_priority = 2;
    pthread_attr_setschedparam(&attr, &sched);
    pthread_attr_setstacksize(&attr, 1024);

    pthread_create(&test_thread4, &attr1, thread_loop4, NULL);    
 
      
    pthread_setname_np(test_thread4, "test_thread4_name");
    pthread_attr_destroy(&attr);

   */ 
	
}



//application_init(posix_test, "posix_test", 20, 2);


