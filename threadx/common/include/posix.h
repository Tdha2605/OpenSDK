/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts.h
 * @brief:
 * @date:    2019/7/10
 * @version: 1.0
 */
#ifndef __POSIX_H__
#define __POSIX_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "ql_rtos.h"
#include "errno.h"
#include "ql_fs.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

typedef void*(* start_rtn)(void *);

typedef long sem_t;


typedef struct thread_cb_struct{   

    unsigned int  detachstate;   //本线程的分离状态
	pthread_t p_tid;    //父线程的id
    pthread_t tid;      //本线程的id
    sem_t sem;
    
    struct thread_cb_struct *next;
}thread_cb;

thread_cb* init_thread_cb_head();
void list_add_thread_cb(pthread_t p_tid, pthread_t tid,  int  detachstate);
thread_cb * list_get_thread_cb(pthread_t p_tid, pthread_t tid);
thread_cb * list_get_thread_cb_by_tid( pthread_t tid);
thread_cb * list_del_thread_cb(pthread_t p_tid, pthread_t tid);
void free_thread_cb(thread_cb * thread_cb);
void traverse_list();

#define pthread_exit(X) return X


#define NAME_MAX 255
typedef  QDIR DIR; 

struct dirent
{
  long            d_ino;		/* Always zero. */
  unsigned short  d_reclen;		/* Always sizeof struct dirent. */
  unsigned short  d_namlen;		/* Length of name in d_name. */
  unsigned        d_type;		/* File attributes */
  char            d_name[FILENAME_MAX]; /* File name. */
};

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
void seekdir(DIR *dirp, long loc);
off_t lseek(int fildes, off_t offset, int whence);
int stat(const char *path, struct stat *buf);
int statfs(const char *path, struct statfs *buf);
int link(const char *path1, const char *path2);
int unlink(const char *path);
int remove(const char *path);
int rename(const char *oldname, const char *newname);
#ifndef MBEDTLS_FS_ALT_H
int fsync(int fd);
#endif
int mkdir(const char *path, mode_t mode);
int rmdir(const char *path);
int access(const char *filenpath, int mode);
int creat(const char *path, mode_t mode);
int telldir(DIR *dir);


int pthread_setname_np(pthread_t thread, const char *name);

#if 1
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_post(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);
#endif
char*  strtok_r(char* string_org,const char* demial,char** last);

char* strdup(const char* str);




#endif
