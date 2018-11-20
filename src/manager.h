#ifndef _COMMON_H
#define _COMMON_H

/*
 *gateway_id(1byte) + module_id(1byte) + device_id (2byte)
 *gateway_id default 0
 */

#define SETDEVID(ModId,Dev)		((ModId<<16) | Dev)
#define GETGWID(devid)			((devid>>24) & 0xFF)
#define GETMODID(devid)			((devid>>16) & 0x00FF)
#define GETDEVMID(devid)		((devid>>8) & 0x0000FF)
#define GETDEVSID(devid)		(devid & 0x000000FF)
#define GETDEVID(devid)			(devid & 0x0000FFFF)

/*base protocol and network protocol*/
#define MOD_BASE_ID				1
#define MOD_USER_ID				2
#define MOD_PUSH_ID				3
#define MOD_BROADCAST_ID		4
#define MOD_MQTT_ID				5
#define MOD_LOCAL_MQTT_ID		6
#define MOD_TCP_ID				7
#define MOD_LOCAL_TCP_ID		8
#define MOD_HTTP_ID				9
#define MOD_LOCAL_HTTP_ID		10

/*busness protocol*/
#define MOD_GROUP_ID			32
#define MOD_TIMED_ID			33
#define MOD_SCENE_ID			33
#define MOD_LINKAGE_ID			34

/*device protocol*/
#define MOD_WIFI_ID				64
#define MOD_MODBUS_ID           65
#define MOD_LDS_ID              66


#define MSG_KEYID				0x6767
#define MSG_FLAG				IPC_CREAT|0666
#define USER_DATA_LEN			128
#define MAX_DATA_LEN			100

#define MODULE_MIN              2
#define MODULE_MAX              64

typedef enum _MSGTYPE{
	DEVDPCTRL = 1,
	DEVPARAMCTRL,
	APPEXIT,
	SYSREBOOT
}MSGTYPE;


#pragma pack()  // 设定对齐方式
typedef struct _DEVDPCTRLMSG{ // 数据点 来源和目的
	unsigned int 	SrcDevId;
	int 			SrcDpId;
	unsigned int 	DesDevId;
	int 			DesDpId;
	char 			Data[MAX_DATA_LEN]; 
}DEVDPCTRLMSG;


typedef struct _DEVPARAMMSG{
	unsigned int 	SrcDevId;
	int 			SrcParamIndex;
	unsigned int 	DesDevId;
	int 			DesParamIndex;
	char 			Data[MAX_DATA_LEN];
}DEVPARAMMSG;


typedef struct {
	long 	type;
	char	buf[USER_DATA_LEN];
}MsgQueue_t;

typedef struct Module_t{
	unsigned int 	ModId;
	const char 		*Name;
	int (*ModuleInit)(void *);
	int	(*MsgProc)(int,void*);
	int (*ModuleTick)(void*);
	int (*ModuleExit)(void*);
}Module_t;

int SendMsg2Manager(MSGTYPE mtype,char *pData,int len);

#endif
