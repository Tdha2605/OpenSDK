#include "posix.h"
#include "poll.h"
#include "sockets.h"


#define posix_log(...)  //printf(__VA_ARGS__)
#define posix_mem(...)  //printf(__VA_ARGS__)





//���߳�id���߳̾������һ��

static thread_cb *tcb_head;
static bool thread_list_is_init= false;
thread_cb* init_thread_cb_head()
{
    tcb_head  = (thread_cb*)malloc(sizeof(thread_cb));
    posix_mem("[zfw]init_thread_cb_head malloc head:%p\r\n",tcb_head);
    if(tcb_head == NULL)
    {
        printf("init_thread_cb malloc failed\r\n");
        return NULL;
    }
   memset(tcb_head, 0x0, sizeof(thread_cb));
    
    tcb_head->p_tid =0;
    tcb_head->tid = 0;
    tcb_head->detachstate = 0;
    tcb_head->next = NULL;
    
    thread_list_is_init = true;
    return tcb_head;
}

 void list_add_thread_cb(pthread_t p_tid, pthread_t tid,  int  detachstate)
{
    if(tcb_head == NULL)
        return ;
    thread_cb *node  = (thread_cb*)malloc(sizeof(thread_cb));
    posix_mem("[zfw]list_add_thread_cb malloc cb:%p\r\n",node);
    if(node == NULL)
    {
        printf("init_thread_cb malloc failed\r\n");
        return ;
    }
    memset(node, 0x0, sizeof(thread_cb));
    
    sem_init(&node->sem, 1, 0);
    node->p_tid =p_tid;
    node->tid = tid;
    node->detachstate = detachstate;    
    node->next = NULL;
    
    posix_log("list_add_thread_cb node:%p p_tid:%p %p %d",node, p_tid, tid, detachstate);

    //β��
    thread_cb *temp_node=tcb_head;
    while(temp_node->next != NULL)
        temp_node =temp_node->next;
    temp_node->next = node;
    
    return ;
}


void traverse_list()
{
    thread_cb *temp_node=tcb_head->next;
    if(temp_node == NULL)
    {
        posix_log("list NULL");
        return ;
    }
        
    do{
        temp_node = temp_node->next;
    }while(temp_node != NULL);
        
}
 thread_cb * list_get_thread_cb(pthread_t p_tid, pthread_t tid)
 {
     thread_cb *temp_node=tcb_head->next;
     if(temp_node == NULL)
         return NULL;
     do
     {
         if(temp_node->p_tid == p_tid && temp_node->tid == tid)
         {
             return temp_node;
         }
         temp_node =temp_node->next;
     }while(temp_node != NULL);
     
     return NULL;    
 }
 
  thread_cb * list_get_thread_cb_by_tid( pthread_t tid)
 {
     thread_cb *temp_node=tcb_head->next;
     if(temp_node == NULL)
         return NULL;
     do
     {
         if(temp_node->tid == tid)
         {
             return temp_node;
         }
         temp_node =temp_node->next;
     }while(temp_node != NULL);
     
     return NULL;    
 }

 
thread_cb * list_del_thread_cb(pthread_t p_tid, pthread_t tid)
{
    thread_cb *temp_node=tcb_head->next;
    thread_cb *temp_node1=tcb_head;
    if(temp_node == NULL)
        return NULL;
    do
    {
        if(temp_node->p_tid == p_tid && temp_node->tid == tid)
        {
            temp_node1->next = temp_node->next;
            return temp_node;
        }
        temp_node1 = temp_node;
        temp_node =temp_node->next;
    }while(temp_node != NULL);
    
    return NULL;    
}

void free_thread_cb(thread_cb * thread_cb)
{
    posix_mem("[zfw]free_thread_cb free cb:%p\r\n",thread_cb);
    if(thread_cb == NULL)
    {
        printf("free_thread_cb error\r\n");
        return;
    }
       
    sem_destroy(&thread_cb->sem);
    free(thread_cb);
    thread_cb = NULL;
}


typedef struct thread_t
{
    pthread_t p_id;
    void*  (* p_func)(void*);
    void *p_arg;
}thread_t;

pthread_t pthread_self(void)						//����߳������ID
{
    ql_task_t taskRef  =NULL;
    ql_rtos_task_get_current_ref(&taskRef);
   
    return (pthread_t)taskRef;
}

void real_task_entry(void *argv)
{    
    posix_log("############real_task_entry start###### \r\n");
    thread_t *t = (thread_t *)argv;
    t->p_func(t->p_arg);
    
    
    
    //��ȡ��ǰ�߳�״̬�ǷǷ��뻹�Ƿ���̬
    pthread_t curr_pid = pthread_self();
    
    //ͨ��pid��ȡ�̷߳���״̬
    thread_cb *t_cb = list_get_thread_cb(t->p_id, curr_pid); 
    posix_log("t_cb:%p P_tid:%p tid:%p detachstate:%d\r\n", t_cb, t_cb->p_tid, t_cb->tid, t_cb->detachstate);
    if(t_cb->detachstate == PTHREAD_CREATE_JOINABLE) //�Ƿ���̬
    {
       posix_log("-----PTHREAD_CREATE_JOINABLE-----\r\n");
       posix_mem("[zfw] free t:%p\r\n",t);
       free(t);
       t=NULL;
       posix_log("sem_post1\r\n");
       sem_post(&t_cb->sem);
	   posix_log("#######real_task_entry end############# \r\n");
       ql_rtos_task_delete(NULL);
    }
    else if(t_cb->detachstate == PTHREAD_CREATE_DETACHED) //����̬
    {
        posix_log("-----PTHREAD_CREATE_DETACHED-----\r\n");
        thread_cb * t_cb = list_del_thread_cb(t->p_id, curr_pid);
        free_thread_cb(t_cb);
        posix_mem("[zfw] free t:%p\r\n",t);
        free(t);
        t=NULL;
		posix_log("#######real_task_entry end############# \r\n");
        ql_rtos_task_delete(NULL);
    }
    

}
//pthread
int  pthread_create(pthread_t *tidp, const     pthread_attr_t *attr,start_rtn func, void  *arg)
{ 

   pthread_attr_t _attr;
   ql_task_t p_task = NULL; 
   int detachstate = -1;

       //�����߳̿�����
    if(!thread_list_is_init)
        init_thread_cb_head();

    pthread_t curr_pid = pthread_self();
    thread_t *t =(thread_t *)malloc(sizeof(thread_t));   
    posix_mem("[zfw] malloc t:%p\r\n",t);
    t->p_id = curr_pid;
    t->p_arg = arg;
    t->p_func = func; 
    
    if(attr && attr->stackaddr != NULL)
    {
        (attr->detachstate == -1) ? (detachstate=PTHREAD_CREATE_JOINABLE) :(detachstate = attr->detachstate);
        posix_log("attr->stackaddr:%p attr->stacksize:%d attr->schedparam.sched_priority:%d", attr->stackaddr, attr->stacksize, attr->schedparam.sched_priority);
        ql_rtos_task_create_withptr(&p_task, attr->stackaddr, attr->stacksize, attr->schedparam.sched_priority+100,"test", real_task_entry,t);
        
        list_add_thread_cb(curr_pid, (pthread_t)p_task, detachstate);

    }
    else if(attr && attr->stackaddr == NULL)
    {   
        (attr->detachstate == -1) ? (detachstate=PTHREAD_CREATE_JOINABLE) :(detachstate = attr->detachstate);
        ql_rtos_task_create(&p_task, attr->stacksize, attr->schedparam.sched_priority+100,"test", real_task_entry,t);
        list_add_thread_cb(curr_pid, (pthread_t)p_task, detachstate);
    }
    else 
    {
        pthread_attr_init(&_attr);
       // posix_log("attr->stackaddr:%p attr->stacksize:%d sched_priority:%d", _attr.stackaddr, _attr.stacksize, _attr.schedparam.sched_priority);
        ql_rtos_task_create(&p_task, _attr.stacksize, _attr.schedparam.sched_priority+100,"test", real_task_entry,t);
        
        list_add_thread_cb(curr_pid, (pthread_t)p_task, PTHREAD_CREATE_JOINABLE);
    }

    
       
    *tidp = (pthread_t)p_task;  
  
    if(attr == NULL)
    {
        pthread_attr_destroy(&_attr);
    }
    
  return 0;
}

//�߳��˳�


/*Fias 2021/07/28  ���ӿͻ����Ƶ� pthread_suspend��pthread_resume�ӿ�*/
int pthread_suspend(pthread_t *_pthread)
{
   ql_task_t taskRef = *_pthread;
   return ql_rtos_task_suspend(taskRef);
}

int pthread_resume(pthread_t *_pthread)
{
   ql_task_t taskRef = *_pthread;
   return ql_rtos_task_resume(taskRef);
}

int pthread_detach(pthread_t thread)				//ȡ�������̵߳İ󶨣��˳����Զ��ͷ���Դ��������Դ�����ͷ�
{
    thread_cb *t_cb = list_get_thread_cb_by_tid(thread);
    t_cb->detachstate = PTHREAD_CREATE_DETACHED;
    return 0;
}


int pthread_join(pthread_t thread, void **retval)	//���߳�״̬��Ϊjoinable״̬�������ȴ�һ���̵߳Ľ���,�̼߳�ͬ���Ĳ����������ķ�ʽ�ȴ�threadָ�����߳̽���
{
    //ͨ��thread��ȡ�̵߳��ƶ��̵߳�ָ��
	//�ж��߳��Ƿ����Ƴ�̬
	//��������̬�ȴ��������˳�̬��ready̬������ִ��
    pthread_t p_pid = pthread_self();
    
     thread_cb *t_cb = list_get_thread_cb(p_pid, thread);   
     if(t_cb==NULL)
        return 0;

     posix_log("%s:%d>>t_cb:%p P_tid:%p tid:%p detachstate:%d\r\n", __func__,__LINE__, t_cb, t_cb->p_tid, t_cb->tid, t_cb->detachstate);
     //traverse_list();
     if(t_cb->detachstate == PTHREAD_CREATE_DETACHED) //����̬
        return 0;
     if(t_cb->detachstate == PTHREAD_CREATE_JOINABLE) //�Ƿ���̬
     {
        posix_log("%s:%d>>\r\n", __func__,__LINE__);
        sem_wait(&t_cb->sem);
        posix_log("%s:%d>>\r\n", __func__,__LINE__);
        list_del_thread_cb(p_pid, thread);
        posix_log("%s:%d>>t_cb:%p P_tid:%p tid:%p detachstate:%d\r\n", __func__,__LINE__, t_cb, t_cb->p_tid, t_cb->tid, t_cb->detachstate);

        free_thread_cb(t_cb);
        return 0;
     }
}


int pthread_cancel(pthread_t thread)				//������ֹ�źŸ�thread�̣߳�����ɹ��򷵻�0������Ϊ��0ֵ
{
     return ql_rtos_task_delete((ql_task_t)thread);
}




int pthread_setname_np(pthread_t thread, const char * name)	 	//�����߳���
{
    if(!thread)
        return EINVAL;
        
    return ql_rtos_task_set_name((ql_task_t)thread, (char *)name);

}

int pthread_attr_init(pthread_attr_t *attr)
{
    
   // attr = (pthread_attr_t *)malloc(sizeof(pthread_attr_t));
   // if(attr == NULL)
   //     return EINVAL;
   // memset(attr, 0x0,sizeof(pthread_attr_t));
    
    attr->stacksize = 3072;
    attr->schedparam.sched_priority = 1;
    attr->stackaddr = NULL;
    attr->detachstate = -1; //�Ƿ���̬
    
    return 0;
}
int pthread_attr_destroy(pthread_attr_t *attr)
{
   // if(attr)
    //    free(attr);
    
    return 0;
}
int	pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate)
{
    if(attr == NULL || (detachstate!=PTHREAD_CREATE_JOINABLE && detachstate!=PTHREAD_CREATE_DETACHED))
        return EINVAL;
    attr->detachstate = detachstate;
    return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
    if(attr==NULL || param==NULL)
        return EINVAL;
    
    attr->schedparam.sched_priority = param->sched_priority;
    return 0;
}
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    if(attr == NULL)
        return EINVAL;
    
    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    if(!attr || !stackaddr /*|| stacksize < PTHREAD_STACK_MIN*/ )
      return EINVAL;
    
     attr->stackaddr = stackaddr;
     attr->stacksize = stacksize;
     posix_log("attr->stackaddr:%p attr->stacksize:%d", attr->stackaddr, attr->stacksize);
     return 0;
}
#if 1
//��ʼ����������
int pthread_cond_init(pthread_cond_t *cond,const pthread_condattr_t *attr)
{
   // posix_log("[%s_%d] mutex\r\n",__func__, __LINE__);
    ql_sem_t _sem_t = NULL;
    ql_rtos_semaphore_create(&_sem_t, 0);
    *cond = (pthread_cond_t)_sem_t;
    return 0;
}
int pthread_cond_destroy(pthread_cond_t *cond)
{
    return ql_rtos_semaphore_delete((ql_sem_t)*cond);
}
int pthread_cond_broadcast(pthread_cond_t *cond)
{
    u32 cnt = 0;
    
    do{
        ql_rtos_semaphore_release((ql_sem_t)*cond);
        ql_rtos_semaphore_get_cnt((ql_sem_t)*cond, &cnt);
        printf("pthread_cond_broadcast cnt:%d\r\n",cnt);
        
    }while(cnt==0);
            
    ql_rtos_semaphore_wait((ql_sem_t)*cond, 0xFFFFFFFF);
        
    return 0;
}
int pthread_cond_signal(pthread_cond_t *cond) //+1
{
    posix_log("[%s_%d] mutex\r\n",__func__, __LINE__);
    return ql_rtos_semaphore_release((ql_sem_t)*cond);

}
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)  //-1
{
   // posix_log("[%s_%d] mutex\r\n",__func__, __LINE__);
    ql_rtos_mutex_unlock((ql_mutex_t)*mutex);


    ql_rtos_semaphore_wait((ql_sem_t)*cond, abstime->tv_sec*1000+abstime->tv_nsec/1000000);  	//ms
    ql_rtos_mutex_lock((ql_mutex_t)*mutex, 0xFFFFFFFF);

}
//������������һֱ����
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
   // posix_log("[%s_%d] mutex\r\n",__func__, __LINE__);
    ql_rtos_mutex_unlock((ql_mutex_t)*mutex);

    ql_rtos_semaphore_wait((ql_sem_t)*cond, 0xFFFFFFFF);
    
    ql_rtos_mutex_lock((ql_mutex_t)*mutex, 0xFFFFFFFF);

}


//��ʼ��һ����������           PTHREAD_PROCESS_SHARED�ù�������ǿ�Խ���̵� Ĭ��PTHREAD_PROCESS_PRIVATE
int pthread_condattr_init(pthread_condattr_t *attr)
{
   return 0; 
}
int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    return 0;
}


//mutex
int pthread_mutex_init(pthread_mutex_t *p_mutex, const pthread_mutexattr_t *attr) 
{  
    ql_mutex_t _mutex = NULL;  
    
    ql_rtos_mutex_create(&_mutex);
    *p_mutex = (pthread_mutex_t)_mutex;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *p_mutex)
{
    return ql_rtos_mutex_delete((ql_mutex_t)*p_mutex);
}
int pthread_mutex_lock(pthread_mutex_t *p_mutex)
{
    return ql_rtos_mutex_lock((ql_mutex_t)*p_mutex, 0xFFFFFFFF);
}
int pthread_mutex_unlock(pthread_mutex_t *p_mutex)
{
    return ql_rtos_mutex_unlock((ql_mutex_t)*p_mutex);
}
int pthread_mutex_trylock(pthread_mutex_t *p_mutex)
{
    return ql_rtos_mutex_try_lock((ql_mutex_t)*p_mutex);
}

//sem
int sem_init(sem_t *sem, int pshared, unsigned int value) 
{
    ql_sem_t _sem = NULL;
    ql_rtos_semaphore_create(&_sem, value);
    *sem = (sem_t)_sem;
    return 0;

}
int sem_post(sem_t *sem)  //+1
{
    return ql_rtos_semaphore_release((ql_sem_t)*sem);
}
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
   return ql_rtos_semaphore_wait((ql_sem_t)*sem, abs_timeout->tv_sec*1000+abs_timeout->tv_nsec/1000000);  //ms
}
int sem_wait(sem_t *sem) //-1
{
   return ql_rtos_semaphore_wait((ql_sem_t)*sem, QL_WAIT_FOREVER);

}
int sem_destroy(sem_t *sem)
{
    return ql_rtos_semaphore_delete((ql_sem_t)*sem);
}

int pthread_mutexattr_init(pthread_mutexattr_t *mattr)
{
    //todo
    return 1;
}
int pthread_mutexattr_destroy(pthread_mutexattr_t *mattr)
{
    //todo
    return 1;

}

/*
int poll(struct pollfd* fds, nfds_t nfds, int timeout)
     - fds     : ��Ҫ�����ļ�����������
				-int fd;		 //�ļ�������
				-short events;	 //�ȴ����¼�
				-short revents;  //ʵ�ʷ������¼�
     - nfds    : ���ǵ�һ���������������һ����ЧԪ�ص��±�+1
     - timeout : ����ʱ����0��ʾ��������-1��ʾ����������⵽��Ҫ�����ļ������������仯ʱ���Ӵ�������>0��n������ʱ��n����λΪms 
     ����ֵ��
       -1��ʾʧ��
       >0��n����ʾ�ɹ�����ʾ��⵽��n���ļ������������˱仯
       
NOTES:Ŀǰ��֧��Socket�������ļ��,�������ļ�ϵͳ���ļ���������֧��
��ǰ��poll��ʹ��select()������װʵ��
*/


int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	int ret = -1,i;
	int max_socket_fd = -1;
	fd_set read_fds, write_fds;
	struct timeval	t;
	int  event = 0 ;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	//1��������ֵ����fd����Ҫ��Ϊselect()���������
	max_socket_fd = fds[0].fd;
	for ( i = 0;i < nfds;i++)  
	{  
		if (max_socket_fd < fds[i].fd)  
		{    
			max_socket_fd = fds[i].fd;
		} 
		
		FD_SET(fds[i].fd, &read_fds);
		FD_SET(fds[i].fd, &write_fds);
	} 

    printf("max_socket_fd :%d\n",max_socket_fd);   
	if(-1 == timeout)
	{
      	ret = select(max_socket_fd + 1, &read_fds, &write_fds, NULL, NULL);
	}
	else
	{
		t.tv_sec = timeout/1000;
		t.tv_usec = (timeout%1000)*1000;
		ret = select(max_socket_fd + 1, &read_fds, &write_fds, NULL, &t);
		
	}	
	if(ret <= 0)
	{
		printf("*** select timeout or error ***\r\n");
		return -1 ;
	}

	
    for (i = 0;i < nfds; i++)  
    {    
       if(fds[i].events == POLLOUT)
       	{

           if(FD_ISSET(fds[i].fd, &write_fds))
           {
             fds[i].revents =  POLLOUT ;
	       }
	    }
	   	
	    if(fds[i].events == POLLIN)
	    {
            if(FD_ISSET(fds[i].fd, &read_fds))
	        {        
	           fds[i].revents =  POLLIN ;
			}	
		}
			
        if( fds[i].revents ==  (POLLIN  + POLLOUT) )
        {
         	if(FD_ISSET(fds[i].fd, &read_fds) &&  FD_ISSET(fds[i].fd, &write_fds))
			{
			   fds[i].revents = (POLLIN + POLLOUT) ;						   			
			}		  
		}
			
	}
	
	return ret;
	
}






/*
char* strdup(const char* str)
{
  char* dst;

  dst = (char*) malloc (strlen(str) + 1);
  if (dst != NULL)
  {
    strcpy(dst, str);
  }

  return dst;
}
*/
#endif
#if 0
unsigned int sleep(unsigned int seconds)
{
    ql_rtos_task_sleep_s((u32)seconds);
    return 1;
}
int usleep(long int time)
{
   u32 val = ((time/1000)>5) ? (time/1000) : 5; 
   ql_rtos_task_sleep_ms(val);
   return 1;
}
#endif 
#if 0
char*  strtok_r(char* string_org,const char* demial,char** last)
 {
 const char* spanp; //span��ʾ�ָ���p��ʾָ��
 char c, sc; //c��ʾchar�ַ���sc��ʾ span char
 char* tok;  //token��ʾ�ָ��Ķ�
  
 //����ʼ��β��ΪNULL��ʱ��˵��û���ַ������ң����Է���NULL
 if (string_org == NULL  && (string_org = *last) == NULL)
     {
     return (NULL);
     }
  
 //��goto��ɵ�ѭ������ɨ���ַ�����ʱ�򣬵���������Ҫƥ����ַ�ʱ���Թ�����ַ���        
 count:
 c = *string_org++;
     
 for (spanp = demial; (sc = *spanp++) != 0; )
     {
     if (c == sc)
         {
         goto count;
         }
     }
  
 //��һ���ַ�Ϊ0�����ʾ�����������������last��ΪNULL��������NULL            
 if (c == 0)
     {
     *last = NULL;
     return (NULL);
     }
  
 //��ԭʼ���ַ���ָ����ˡ�            
 tok = string_org -1;
  
 //��ʼɨ���ַ������Ƿ���Ҫƥ����ַ���֮������ƥ���ַ�֮ǰ�Ĳ��ַ��ء�
 //�⿴���Ǹ�����ѭ��������Դ�ַ�����ƥ���ַ������ߵ���βʱ��Ҳ����string_org��sc��ΪNULLʱ�������ѭ�������ߵ�return(tok)����ѭ����
 for (;;)
     {
     c = *string_org++;
     spanp = demial;
     
     do 
         {
         if ((sc = *spanp++) == c) 
             {
             if (c == 0)
                 {
                 string_org = NULL;
                 }
             else
                 {
                 string_org[-1] = 0;
                 }
             *last = string_org;
             return (tok);
             }
         } while (sc != 0);
     }
     
 }
#endif
