#include "task_timed.h"
#include <time.h>
#include "JsTimed.h"

#define TIMEDTASK_FILE_PATH "./config/timed.json"

typedef enum _TimedSta{
    TIMED_INIT = 0,     
    TIMED_RUN,        
    TIMED_WAITRECALC,   
}TimedSta;

static cJSON *pJsTimedTaskArray = NULL;
static pthread_t th_Timed; 
static pthread_mutex_t JsTimedLock;
static int running = 0;

static int mTimedTaskDel(int pDelId)
{    
    int num,i;
    cJSON *pJsItem,*pJsObject;

    DALITEK_DEBUG(DBG,"TimedTaskDel id = %d\n",pDelId);
    
    if(!pJsTimedTaskArray){
        return -1;
    }
    pthread_mutex_lock(&JsTimedLock);
    num = cJSON_GetArraySize(pJsTimedTaskArray);
    for(i = 0;i < num; i++){
        pJsObject = cJSON_GetArrayItem(pJsTimedTaskArray,i);
        if(!pJsObject)
            continue;
        pJsItem   = cJSON_GetObjectItem(pJsObject,"id");
        if(!pJsItem)
            continue;
        if(pDelId == pJsItem->valueint){
            cJSON_DeleteItemFromArray(pJsTimedTaskArray,i);
            break;
        }
    }
    if(pDelId == 0){
         for(i = num-1;i >=0; i--){
            cJSON_DeleteItemFromArray(pJsTimedTaskArray,i);
         }
    }
    pthread_mutex_unlock(&JsTimedLock);
    if(i == num){
        return -1;
    }else{
        return 0;
    }
}


static int mTimedTaskAdd(cJSON *pmTask)
{
    cJSON *pJsTask,*pJsItem,*pJsTaskId;
    if(!pmTask)
        return -1;
    pJsItem = cJSON_GetObjectItem(pmTask,"id");
    if(!cJSON_IsNumber(pJsItem))
        return -1;
    mTimedTaskDel(pJsItem->valueint);
    pthread_mutex_lock(&JsTimedLock);
    pJsTask = cJSON_Duplicate(pmTask,1);
    pJsItem = cJSON_CreateNumber(TIMED_INIT);
    cJSON_AddItemToObject(pJsTask,"state",pJsItem);
    pJsItem = cJSON_CreateNumber(0);
    cJSON_AddItemToObject(pJsTask,"cntdown",pJsItem);
    pJsTaskId = cJSON_GetObjectItem(pJsTask,"id");
    cJSON_AddItemToArray(pJsTimedTaskArray,pJsTask);

    /*
    char *cJsStr = NULL;
    cJsStr = cJSON_PrintUnformatted(pJsTimedTaskArray);
    DALITEK_DEBUG(DBG,"pJsTimedTaskArray:%s\n",cJsStr);
    free(cJsStr);
    */
    pthread_mutex_unlock(&JsTimedLock);
    return 0;
}


int TimedTaskAdd(cJSON *pmTask)
{
    if(JsTimed_AddTask(pmTask)<0)
        return -1;
    JsTimed_SaveFile();
    if(mTimedTaskAdd(pmTask) < 0)
        return -1;
    return 0;
}

int TimedTaskDel(int pDelId)
{
    if(JsTimed_DelTask(pDelId)<0)
        return -1;
    JsTimed_SaveFile();
    if(mTimedTaskDel(pDelId) < 0)
        return -1;
    return 0;
}



static int mTimedTaskSetEnable(int TaskId,int enable)
{
    int num,i;
    cJSON *pJsItem,*pJsObject,*pJsEnable;
    
    if(!pJsTimedTaskArray){
        return -1;
    }
    pthread_mutex_lock(&JsTimedLock);
    num = cJSON_GetArraySize(pJsTimedTaskArray);
    for(i = 0;i < num; i++){
        pJsObject = cJSON_GetArrayItem(pJsTimedTaskArray,i);
        if(!pJsObject)
            continue;
        pJsItem   = cJSON_GetObjectItem(pJsObject,"id");
        if(!pJsItem)
            continue;
        if(TaskId == pJsItem->valueint){
            pJsEnable = cJSON_GetObjectItem(pJsObject,"enable");
            if(pJsEnable && pJsEnable->type==cJSON_Number)
                cJSON_SetIntValue(pJsEnable,enable);
            else
                return -1;
            break;
        }
    }
    pthread_mutex_unlock(&JsTimedLock);
    if(i == num){
        return -1;
    }else{
        return 0;
    }
}
int TimedTaskSetEnable(int TaskId,int Enable)
{
    if(mTimedTaskSetEnable(TaskId,Enable) < 0)
        return -1;
    if(JsTimed_SetEnable(TaskId,Enable) < 0)
        return -1;
    JsTimed_SaveFile();
    return 0;
}
static int TimedTaskInit(void) 
{
    int ret = -1;
    int num = 0,i;
    
    cJSON *pmTask;
    if(pJsTimedTaskArray)
        cJSON_Delete(pJsTimedTaskArray);
    pJsTimedTaskArray = cJSON_CreateArray();

    num = JsTimed_GetNum();
    //DALITEK_DEBUG(DBG,"num = %d \n",num);
    for(i = 0;i < num;i++){
        pmTask = JsTimed_GetTaskByIndex(i);
        if(!pmTask){
            continue;
        }
        mTimedTaskAdd(pmTask);
    }
    return 0;
}

static unsigned int TimedCntDownCalc(cJSON *pJsTask,struct tm *pt)
{
    cJSON *pJsCondition,*pJsMode,*pJsItem;
    int item = 0;
    int temp = 0;

    if(!pJsTask)
        return -1;
    pJsCondition = cJSON_GetObjectItem(pJsTask,"condition");
    if(!pJsCondition)
        return -1;
    pJsMode = cJSON_GetObjectItem(pJsTask,"mode");
    if(!pJsMode)
        return -1;
    
    switch (pJsMode->valueint) {
    case 1:
    case 2:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        
        return pJsItem->valueint;

    case 9:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        if(pJsItem->valueint != (pt->tm_year + 1900))
            return -1;
    case 8:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        if(pJsItem->valueint != (pt->tm_mon + 1))
            return -1;
    case 7:
        if(pJsMode->valueint == 7) {
            pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
            if(!pJsItem)
                return -1;
            temp = pt->tm_wday;
            if(0 == temp) {
                temp = 7;
            }
            if(!(pJsItem->valueint & (1<<(temp-1))))
                return -1;
        }
    case 6:
        if(pJsMode->valueint != 7) {
            pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
            if(!pJsItem)
                return -1;
            if(pJsItem->valueint != pt->tm_mday)
                return -1;
        }
    case 5:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        if(pJsItem->valueint < pt->tm_hour)
            return -1;
        temp = (pJsItem->valueint - pt->tm_hour)*3600;
    case 4:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        if(pJsItem->valueint <= pt->tm_min) {
            if(pJsMode->valueint != 4) {
                temp -= 3600;
            }
            temp += (pJsItem->valueint + 60 - pt->tm_min)*60;
        } else {
            temp += (pJsItem->valueint - pt->tm_min)*60;
        }
        if(temp < 0) {
            return -1;
        }
    case 3:
        pJsItem = cJSON_GetArrayItem(pJsCondition,item++);
        if(!pJsItem)
            return -1;
        DALITEK_DEBUG(DBG,"pJsMode->valueint = %d,pJsItem->valueint = %d,pt->tm_sec=%d",pJsMode->valueint,pJsItem->valueint,pt->tm_sec);
        if(pJsItem->valueint <= pt->tm_sec) {
             if(pJsMode->valueint != 3) {
                 temp -= 60;
             }
            temp += (pJsItem->valueint + 60 - pt->tm_sec);
        } else {
            temp += (pJsItem->valueint - pt->tm_sec);
        }
        if(temp < 0) {
            return -1;
        }
        return temp*1000;

    default:
        return -1;
    }
}
static void RunTimedTask(cJSON *pJsTask)
{
    DALITEK_DEBUG(DBG, "RunTimedTask");
    static DEVDPCTRLMSG mDpmsg;
    cJSON *pJsArray,*pJsObject,*pJsItem;
    int arraySize,i;
    char *pcData;
    
    if(!pJsTask)
        return;
    pJsArray = cJSON_GetObjectItem(pJsTask,"devdpmsg");
    if(!pJsArray)
        return;
    
    pJsItem = cJSON_GetObjectItem(pJsTask,"id");
    if(!pJsItem)
        return;
    mDpmsg.SrcDevID = SETDEVID(MOD_TIMED_ID,pJsItem->valueint);
    
    arraySize = cJSON_GetArraySize(pJsArray);
    //DALITEK_DEBUG(DBG, "cJSON_GetArraySize %d #######################",arraySize);
    for(i = 0; i < arraySize; i++){
        mDpmsg.SrcDpID  = i;
        pJsObject = cJSON_GetArrayItem(pJsArray,i);
        if(!pJsObject)
            continue;
        
        pJsItem = cJSON_GetObjectItem(pJsObject,"devid");
        if(!pJsItem)
            continue;
        mDpmsg.DesDevID = pJsItem->valueint;
        
        pJsItem = cJSON_GetObjectItem(pJsObject,"dpid");
        if(!pJsItem)
            continue;
        mDpmsg.DesDpID = pJsItem->valueint;
        
        pJsItem = cJSON_GetObjectItem(pJsObject,"data");// ???
        if(!pJsItem || pJsItem->type != cJSON_String)
            continue;
        //pcData = cJSON_Print(pJsItem);
        //pcData = cJSON_PrintUnformatted(pJsItem);
        if(strlen(pcData) >= sizeof(mDpmsg.Data)){
            //free(pcData);
            continue;
        }
        strcpy(mDpmsg.Data,pJsItem->valuestring);
        SendMsg2Main(DEVDPCTRL, (char*)&mDpmsg, sizeof(mDpmsg));
        //free(pcData);
    }
}
static void TimedTaskOnTimed()
{   
    static cJSON  *pTTask;
    static time_t           sec;
    static time_t           secpre;
    static struct tm        *pTm;
    static int              mscnt = 0;
    cJSON         *pTNext,*pJsMod,*pJsState,*pJsCnt,*pJsEnable;
    
    if(!pJsTimedTaskArray){
        return;
    }

    mscnt += 100;

    if(mscnt >= 1000) {
        mscnt = 0;
        sec = time(NULL);
        pTm = localtime(&sec);
        
        if( (sec != (secpre + 1))                                   
            || (!(pTm->tm_hour || pTm->tm_min || pTm->tm_sec))) {   

           // DALITEK_PRINT("Recalculate the count\n");
            pTTask = pJsTimedTaskArray->child;
            while(pTTask) {
                pJsMod = cJSON_GetObjectItem(pTTask,"mode");
                if(pJsMod && (pJsMod->valueint > 2)) {
                    pJsState = cJSON_GetObjectItem(pTTask,"state");
                    if(pJsState){
                        cJSON_SetIntValue(pJsState,TIMED_INIT);
                    }
                }
                pTTask = pTTask->next;
            }
        }
        secpre = sec;
        

        

        pTTask = pJsTimedTaskArray->child;
        while(pTTask) {
            pJsEnable = cJSON_GetObjectItem(pTTask,"enable");
            if(!cJSON_IsNumber(pJsEnable))
                continue;
            if(pJsEnable->valueint ==0){
                pTTask = pTTask->next;
                continue;
            }
            pJsState = cJSON_GetObjectItem(pTTask,"state");
            pJsCnt   = cJSON_GetObjectItem(pTTask,"cntdown");
            if(pJsState && pJsCnt && (pJsState->valueint == TIMED_INIT)) {
                int cnt;
                cnt = TimedCntDownCalc(pTTask,pTm);
                cJSON_SetIntValue(pJsCnt,cnt);
                if(cnt < 0) {
                    cJSON_SetIntValue(pJsState,TIMED_WAITRECALC);
                } else {
                    cJSON_SetIntValue(pJsState,TIMED_RUN);
                    //DALITEK_PRINT("TimedTask 0x%x: CntDownCalc= %d\n",pTTask->task.id,pTTask->cntdown);
                }
            }
            pTTask = pTTask->next;
        }
    }
    pTTask = pJsTimedTaskArray->child;
    while(pTTask) {
        pJsEnable = cJSON_GetObjectItem(pTTask,"enable");
        pJsState = cJSON_GetObjectItem(pTTask,"state");
        pJsCnt   = cJSON_GetObjectItem(pTTask,"cntdown");
        pJsMod  = cJSON_GetObjectItem(pTTask,"mode");
        if(!(pJsState && pJsCnt && pJsMod && pJsEnable))
        {
            pTTask = pTTask->next;
            continue;
        }
        if(pJsEnable->valueint ==0){
                pTTask = pTTask->next;
                continue;
        }
        if(pJsState->valueint == TIMED_RUN) {
            if(pJsCnt->valueint >= 100) {
               // DALITEK_DEBUG(DBG,"##########Cnt=%d########",pJsCnt->valueint);
                pJsCnt->valueint -= 100;
            } else{
                cJSON *pJsId;
                //DALITEK_DEBUG(DBG,"##########mode = %d,Cnt=%d########",pJsMod->valueint,pJsCnt->valueint);
                RunTimedTask(pTTask);//execute 
                if((pJsMod->valueint == 1) || (pJsMod->valueint == 9)){
                    pTNext = pTTask->next;
                    pJsId = cJSON_GetObjectItem(pTTask,"id");
                    if(pJsId) {
                        JsTimed_DelTask(pJsId->valueint);       
                        JsTimed_Save();
                        mTimedTaskDel(pJsId->valueint);        
                    }
                    pTTask = pTNext;
                    continue;
                } else {
                    int cnt;
                    //DALITEK_DEBUG(DBG,"##########sec = %d########",sec);
                    sec++;
                    pTm = localtime(&sec);
                    cnt = TimedCntDownCalc(pTTask,pTm);
                    //DALITEK_DEBUG(DBG,"##########pJsCnt->valueint = %d########",pJsCnt->valueint);
                    cJSON_SetIntValue(pJsCnt,cnt);
                    //DALITEK_DEBUG(DBG,"##########pJsCnt->valueint = %d########",pJsCnt->valueint);
                    if(cnt < 0) {
                        cJSON_SetIntValue(pJsState,TIMED_WAITRECALC);
                    } else {
                        cJSON_SetIntValue(pJsState,TIMED_RUN);
                    }
                }
            }
        }
        pTTask = pTTask->next;
    }
}    
static void* TimedTaskThread(void *t)
{
    struct timeval tvget;
    int x,y=0;
    
    TimedTaskInit();
    running = 1;
    while(running) {
        usleep(10000);
        gettimeofday(&tvget,NULL);
        x = tvget.tv_usec / 100000;
        if(x != y) {
            y = x;
            usleep(20000);
            TimedTaskOnTimed();
        }
    }

    return NULL;
}

int TimedModuleInit(void *param)
{
    if(JsTimed_Open(TIMEDTASK_FILE_PATH)<0)
        return -1;
    if(pthread_mutex_init(&JsTimedLock, NULL) != 0){
        DALITEK_PRINT("Timed module pthread_mutex_init fail!\n");
        return -1;
    }
    if(pthread_create(&th_Timed, NULL, TimedTaskThread, 0) != 0){
        DALITEK_PRINT("pthread_create:%s\n",strerror(errno));  
        return -1;
    }
    //DALITEK_PRINT("TimedModuleInit success\n");
    usleep(200000);
    return 0;
}

int TimedModuleExit(void *param)
{

    running = 0;
    pthread_cancel(th_Timed);
    pthread_join(th_Timed,NULL);
    pthread_mutex_destroy(&JsTimedLock);
    return JsTimed_Close();
}

int Send2TimedModule(int type,void *msg)
{
    int ret;
    DEVPARAMMSG *pDevParamMsg;
    DEVDPCTRLMSG *pDevCtrlMsg;
    DEVPARAMMSG mDevParamMsg;    
    switch(type){
        case DEVDPCTRL:{
            int modid,taskid,value=-1;
            pDevCtrlMsg = (DEVDPCTRLMSG*)msg;
            modid = GETMODID(pDevCtrlMsg->DesDevId);
            if(modid != MOD_TIMED_ID)
                break;
            taskid = GETDEVID(pDevCtrlMsg->DesDevId);
            if(sscanf(pDevCtrlMsg->Data,&value) != 1)
                break;
            if(pDevCtrlMsg->DesDpId == 1)
                TimedTaskSetEnable(taskid,value);    
            break;
            }
        case DEVPARAMCTRL:
            pDevParamMsg = (DEVPARAMMSG*)msg;
            if(pDevParamMsg->DesParamIndex != GETMODID(pDevParamMsg->DesParamIndex)){
                return -1;
            }
            switch(pDevParamMsg->DesParamIndex) {
                case 1:
                    //TODO add timed task
                    break;
                case 2:
                    //TODO delete timed task
                    break;    
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return 0;
}

