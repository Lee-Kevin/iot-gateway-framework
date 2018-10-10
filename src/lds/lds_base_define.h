#ifndef LDS_BASE_DEFINE_H
#define LDS_BASE_DEFINE_H
#include "queue.h"
#include <semaphore.h>
#define eps 0.001

#define ISZERO(d) (((d)<eps||(d)>-eps)?0:1)
#define DEVTYPE_LDS_LIGHT_ONOFF 1
#define LDS_QUEUE_SIZE 1024

#define LDS_REPORT_TYPE_VALUE   1
#define LDS_REPORT_TYPE_GROUP   2
#define LDS_REPORT_TYPE_LINKAGE 3

#define LDS_CONTROL_TYPE_READ   1
#define LDS_CONTROL_TYPE_WRITE  2

typedef struct lds_base
{
    unsigned short dev_type;
    unsigned int devid;
    int (*dpinit)(struct lds_base *,int ,int ,int ,int);
    int (*presetinit)(struct lds_base *,int ,int,int);
    int (*uninit)(struct lds_base *);
    unsigned char (*getdpvalue)(struct lds_base *,int);
    int (*getdptype)(struct lds_base *,int);
    int (*getdpcode)(struct lds_base *,int);
    int (*setdpvalue)(struct lds_base *,int,unsigned char);
    int (*savepreset)(struct lds_base *,int);
    int (*control_preset)(struct lds_base *,int);
    int (*read)(struct lds_base *);
    int (*write)(struct lds_base *,int,int);
}lds_base_t;

typedef struct LdsMsg
{
    unsigned short type;
    unsigned int devid;
    int dpid;
    int value;
}LdsMsg_t;

typedef struct LdsQueue{
//定义modbus消息队列
    queue_t* send_queue;//上行消息
    queue_t* recv_queue;//下行消息
//定义信号量
    sem_t send_sem;  
    sem_t recv_sem; 
}LdsQueue_t;

extern int LDS_UART_ID;
extern struct LdsQueue gLdsQueue;
#endif