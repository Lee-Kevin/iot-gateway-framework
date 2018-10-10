#include "task_linkage.h"
#include "JsLinkage.h"
#include "common.h"
#include "libmain.h"
#include "debug.h"


#define LINKAGE_FILE_PATH "./config/Linkage.json"

#define TRIGEER_TRUE            1
#define TRIGEER_FALSE           2
#define TRIGEER_MAX             3
#define TRIGEER_MIN             4
#define TRIGEER_RANGER          5
#define TRIGEER_OUT_OF_RANGER   6

static pthread_t th_linkage;
static int running = 1;

void RunLinkageTask(cJSON *JsLinkageTask)
{
    cJSON *pJsArray,*pJsObject,*pJsItem,*pJsAction;
    int arraySize,i;
    static DEVDPCTRLMSG mDpmsg;
    if(!JsLinkageTask) return;
    pJsArray = cJSON_GetObjectItem(JsLinkageTask,"action");
    if(!pJsArray) return;
    
	
    pJsItem = cJSON_GetObjectItem(JsLinkageTask,"id");
    if(!pJsItem)
        return;
    mDpmsg.SrcDevID = SETDEVID(MOD_LINKAGE_ID,pJsItem->valueint);/*mod << 24 + linkage_id*/
    
    arraySize = cJSON_GetArraySize(pJsArray);
    for(i = 0; i < arraySize; i++){
        mDpmsg.SrcDpID  = i;
        pJsObject = cJSON_GetArrayItem(pJsArray,i);
        if(!pJsObject) continue;

        pJsItem = cJSON_GetObjectItem(pJsObject,"devid");
        if(!cJSON_IsNumber(pJsItem)) continue;
        mDpmsg.DesDevID = pJsItem->valueint;
        
        pJsItem = cJSON_GetObjectItem(pJsObject,"dpid");
        if(!cJSON_IsNumber(pJsItem)) continue;
        mDpmsg.DesDpID = pJsItem->valueint;
        
        pJsItem = cJSON_GetObjectItem(pJsObject,"data");
        if(!cJSON_IsString(pJsItem)) continue;
        strcpy(mDpmsg.Data,pJsItem->valuestring);
        SendMsg2Main(DEVDPCTRL, (char*)&mDpmsg, sizeof(mDpmsg));
    }
}
int LinkageConditionIsTriggered(cJSON *JsCondition)
{
	cJSON *pJsArray,*pJsObject,*pJsItem;
	int devid,dpid,data,mode;
	int num,i,flag=0;
	pJsArray = JsCondition;
	if(!pJsArray) return 0;
	
	num = cJSON_GetArraySize(pJsArray);
	for(i=0;i<num;i++){
		pJsObject = cJSON_GetArrayItem(pJsArray,i);
		if(!pJsObject) return 0;
		pJsItem = cJSON_GetObjectItem(pJsObject,"devid");
		if(!cJSON_IsNumber(pJsItem)) return 0;
		devid = pJsItem->valueint;
	
		pJsItem = cJSON_GetObjectItem(pJsObject,"dpid");
		if(!cJSON_IsNumber(pJsItem)) return 0;
		dpid = pJsItem->valueint;
	
		pJsItem = cJSON_GetObjectItem(pJsObject,"mode");
		if(!cJSON_IsNumber(pJsItem)) return 0;
		mode = pJsItem->valueint;
		
		if(disk_get_dpvalue(devid,dpid,&data)<0)
			return 0;
		
		switch(mode){
			case TRIGEER_TRUE:{
				if(data == 1)
					flag++;
				break;
			}
			case TRIGEER_FALSE:{
				if(data == 0)
					flag++;
				break;
			}
			case TRIGEER_MAX:{
				int dataset=-1;
				pJsItem = cJSON_GetObjectItem(pJsObject,"dataset");
				if(cJSON_IsString(pJsItem)) return 0;
				dataset = atoi(pJsItem->valuestring);
				if(data > dataset)
					flag++;
				break;
			}
			case TRIGEER_MIN:{
				int dataset;
				pJsItem = cJSON_GetObjectItem(pJsObject,"dataset");
				if(cJSON_IsString(pJsItem)) return 0;
				dataset = atoi(pJsItem->valuestring);
				if(data < dataset)
					flag++;
				break;
			}
			case TRIGEER_RANGER:{
				int min,max;
				pJsItem = cJSON_GetObjectItem(pJsObject,"min");
				if(cJSON_IsString(pJsItem)) return 0;
				min = atoi(pJsItem->valuestring);
				pJsItem = cJSON_GetObjectItem(pJsObject,"max");
				if(cJSON_IsString(pJsItem)) return 0;
				max = atoi(pJsItem->valuestring);
				if(data >= min && data <= max)
					flag++;
				break;
			}
			case TRIGEER_OUT_OF_RANGER:{
				int min,max;
				pJsItem = cJSON_GetObjectItem(pJsObject,"min");
				if(cJSON_IsString(pJsItem)) return 0;
				min = atoi(pJsItem->valuestring);
				pJsItem = cJSON_GetObjectItem(pJsObject,"max");
				if(cJSON_IsString(pJsItem)) return 0;
				max = atoi(pJsItem->valuestring);
				if(data < min && data > max)
					flag++;
				break;
			}
			default:{
				break;
			}
		}
	}
	return (flag == num) ? 1:0;
}
void *LinkageTaskThread(void *param)
{
	int TaskNum;
	int i;
	cJSON *pJsObject,*pJsItem;
	while(running){
		usleep(100000);
		TaskNum = JsLinkage_GetTaskNum();
		for(i=0;i<TaskNum;i++){
			pJsObject = JsLinkage_GetTaskByIndex(i);
			if(!pJsObject) continue;
			pJsItem = cJSON_GetObjectItem(pJsObject,"enable");
			if(!cJSON_IsNumber(pJsItem)) continue;
			if(pJsItem->valueint == 0) continue;
			pJsItem = cJSON_GetObjectItem(pJsObject,"condition");
			if(!pJsItem) continue;
			if(LinkageConditionIsTriggered(pJsItem)){
				RunLinkageTask(pJsObject);
			}
		}
	}
	return NULL;
}

int LinkageModuleInit(void *param)
{
    JsLinkage_Open(LINKAGE_FILE_PATH);
    running =1;
    if(pthread_create(&th_linkage, NULL, LinkageTaskThread, 0) != 0){
        DALITEK_PRINT("pthread_create:%s\n",strerror(errno));  
        return -1;
    }
    return 0;
}
int LinkageModuleExit(void *param)
{
    return JsLinkage_Close();;
}

int Send2LinkageModule(int type,void *msg)
{
    return 0;
}
