#ifndef _COMMON_H__
#define _COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/ipc.h>   
#include <sys/time.h>   
#include <sys/msg.h> 
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

typedef enum
{
    /*JSON*/
    ERR_SUCCESS = 0,
    ERR_JSON,
    ERR_HEAD,
    ERR_BODY,
    ERR_METHOD,
    ERR_SESSION,
    ERR_TS,
    ERR_STATUS,
    ERR_VERSION,
    ERR_UKEY,
    ERR_FIELD,
    ERR_TOPIC,
    ERR_FROMTOPIC,
    ERR_ERRORCODE,
    /*DISK*/
	ERR_DB_NAME = 100,
    ERR_NO_RECORD,
    ERR_RECORD_EXITED,
    ERR_NO_FILE,
    /*USER*/
    ERR_USER_OR_PASSWD=200,
    ERR_USER_EXITED,
    ERR_NO_USER,
    ERR_PASSWD,
    ERR_RIGHT,//È¨ÏÞ´íÎó

    /*SCENE*/
    ERR_ADD_SCENE,
    ERR_DEL_SCENE,
    ERR_MODIFY_SCENE,
    ERR_SCENE_ADD_TASK,
    /*OTHER*/
    ERR_PARAMTER,
    ERR_UNKNOW=254,
    ERR_BROAD=255,
    ERR_NO_RETURN,
    ERR_DOWNLOAD,
    ERR_PACKAGE_MD5,
    ERR_APP_MD5,
    ERR_NO_DB,
}DALITEK_ERR_CODE;


#endif
