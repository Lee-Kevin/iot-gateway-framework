#include "manager.h"
#include "common.h"
#include "debug.h"
#include "task_lds.h"

#include "task_base.h"
#include "task_mqtt.h"
#include "task_broadcast.h"

//#include "task_modbus.h"
//#include "JsModule.h"

#include "version.h"

int anna_log_level = LOG_MAX;
bool anna_log_timestamps = true;
bool anna_log_colors = true;

static bool running = false;
static bool reboot_flag = false;

static int msg_queue_id;
static pthread_mutex_t msg_queue_lock;
static pthread_t tick_thread_id;
static Module_t module[MODULE_MAX]={
    {MOD_BASE_ID,"BaseModule",BaseModuleInit,NULL,NULL,BaseModuleExit},
    {MOD_BROADCAST_ID,"BroadCastModule",BroadcastModuleInit,NULL,NULL,BroadcastModuleExit},
};

static unsigned int module_cnt = 0;

static unsigned int module_count(void)
{
    int i = 0;
    for(i=0;i<MODULE_MAX;i++){
        if(!module[i].ModId)
            break;
    }
    return i;
}

static int SendDevDpMsg2Module(char *pMsg)
{
    if(pMsg == NULL)
        return -1;
    DEVDPCTRLMSG *pDpCtrlMsg = (DEVDPCTRLMSG *)pMsg;
    int i;
    for(i=MODULE_MIN;i<module_cnt;i++){
        if(module[i].ModId == GETMODID(pDpCtrlMsg->DesDevId)){
            //ANNA_LOG(LOG_DBG, "modid=%d", module[i].ModId);
            if(module[i].MsgProc){
                module[i].MsgProc(DEVDPCTRL,(void *)pMsg);
            }
            break;
        }
    }
    return 0;
}

static int SendParamMsg2Module(char *pMsg)
{
	if(pMsg == NULL)
			return -1;
	DEVPARAMMSG *pParamMsg = (DEVPARAMMSG *)pMsg;
	int i;
	for(i=MODULE_MIN;i<module_cnt;i++){
		if(module[i].ModId == GETMODID(pParamMsg->DesDevId)){
			if(module[i].MsgProc){
				module[i].MsgProc(DEVPARAMCTRL,(void *)pMsg);
			}
			break;
		}
	}
	return 0;

}
static void *tick_looping(void *param)
{
	(void *)param;
	int i;
	while(running){
		for(i = MODULE_MIN;i < module_cnt;i++){
			if(module[i].ModuleTick){
				module[i].ModuleTick(NULL);
                
			}
		}
		sleep(1);
	}
	return NULL;
}

int main(int argc,char *argv[])
{
	int i, module_index=0;
	MsgQueue_t msgs={0};
	DEVPARAMMSG *pDevParamMsg;
	DEVDPCTRLMSG *pDevDpCtrlMsg;

    /*日志初始化*/
    bool daemonize=false;
    bool terminal=true;
	char *logfile = "./log/log.txt";
    if(anna_log_init(daemonize,terminal,logfile) < 0){
		return -1;
	}

    /*IPC消息队列初始化*/
	msg_queue_id = msgget(MSG_KEYID,IPC_EXCL);
	if(msg_queue_id < 0){
		msg_queue_id = msgget(MSG_KEYID,MSG_FLAG);
		if(msg_queue_id < 0){
			return -1;
		}
	}
    /*消息锁初始化*/
	if(pthread_mutex_init(&msg_queue_lock,NULL) != 0){
		return -1;
	}
	
	/*打印软件版本*/
	ANNA_LOG(LOG_INFO,"[ software version ] : %s\n",SOFTWARE_VERSION);

    /*模块接口初始化*/
    if(!JsModule_Open("./json/module.json")){
        if(JsGetModuleValue("protocol_bus","Lds")){
			module_index = module_count();
			if(module_index >= MODULE_MIN && module_index < MODULE_MAX){
				module[module_index].ModId = MOD_LDS_ID;
				module[module_index].Name = "LdsModule";
				module[module_index].ModuleInit = LdsModuleInit;
				module[module_index].MsgProc = Send2LdsModule;
				module[module_index].ModuleTick = LdsModuleTick;
				module[module_index].ModuleExit = LdsModuleExit;
			}else{
				;
			}
        }
    
#if 1
		if(JsGetModuleValue("network","RemoteMosquitto")){
			module_index = module_count();
			if(module_index >= MODULE_MIN && module_index < MODULE_MAX){
				module[module_index].ModId = MOD_MQTT_ID;
				module[module_index].Name = "MqttModule";
				module[module_index].ModuleInit = MqttModuleInit;
				module[module_index].MsgProc = Send2MqttModule;
				module[module_index].ModuleTick = MqttModuleTick;
				module[module_index].ModuleExit = MqttModuleExit;
			}
		}
#endif
		JsModule_Close();
    }
    
    /*模块接口数量统计*/
    module_cnt = module_count();
    ANNA_LOG(LOG_INFO,"module count : %d\n",module_cnt);

    /*模块初始化*/
    for(i = 0;i < module_cnt;i++){
        if(!module[i].ModuleInit){
            continue;
        }
        if(module[i].ModuleInit(NULL) != 0){
            //;
        }
    }
    
    /*初始化运行状态*/
    running = true;

    /*创建tick线程*/
    if(pthread_create(&tick_thread_id,NULL,tick_looping,0) != 0){
        return -1;
    }

    /*消息任务：1.控制类消息2.参数类消息3.告警类消息（暂未定义）4.系统类消息（程序重启、系统重启）*/
    while(running){
        if(msgrcv(msg_queue_id,(void *)&msgs,sizeof(MsgQueue_t),0,0) < 0){
            break;
        }
        switch(msgs.type){
            case DEVDPCTRL:{
                pDevDpCtrlMsg = (DEVDPCTRLMSG *)msgs.buf;
                if(pDevDpCtrlMsg->DesDevId){
                    SendDevDpMsg2Module(msgs.buf);
                }
                break;
            }
            case DEVPARAMCTRL:{
                pDevParamMsg = (DEVPARAMMSG*)msgs.buf;
                if(pDevParamMsg->DesDevId){
                    SendParamMsg2Module(msgs.buf);
                }
                break;  
            }
            case SYSREBOOT:{
                reboot_flag = true;
            }
            case APPEXIT:{
                running = false;
                pthread_cancel(tick_thread_id);
                pthread_join(tick_thread_id,NULL);
                sleep(2);
                goto Exit;
            }
        }
    }
Exit:
    /*模块退出*/
	for(i = 0;i < module_cnt;i++){
		if(module[i].ModuleExit){
			module[i].ModuleExit(NULL);
		}
	}
    /*销毁队列、锁、日志关闭*/
	msgctl(msg_queue_id,IPC_RMID,0);
	pthread_mutex_destroy(&msg_queue_lock);
	anna_log_destroy();
	if(reboot_flag){
		system("reboot");
	}
	return 0;
}


int SendMsg2Manager(MSGTYPE mtype,char *pData,int len)
{
	MsgQueue_t msgs;
	int ret=-1;

	if(msg_queue_id < 0){
		return ret;
	}
	if(sizeof(msgs.buf)<len){
		return ret;
	}
    
	pthread_mutex_lock(&msg_queue_lock);
	msgs.type = mtype;
	if(pData && len){
		memcpy(msgs.buf,pData,len);
	}
	ret = msgsnd(msg_queue_id,&msgs,len,IPC_NOWAIT);
	pthread_mutex_unlock(&msg_queue_lock);
    
	if(ret < 0){
		ANNA_LOG(LOG_ERR,"msgsnd:%s",strerror(errno));
	}
    
	return ret;
}
