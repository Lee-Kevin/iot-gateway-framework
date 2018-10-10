#include "task_group.h"
#include <semaphore.h>
#define GROUP_FILE_PATH "./config/group.json"
#define GROUP_QUEUE_SIZE 1024

static pthread_t th_group; 
static int running = 0;

static sem_t sem_group;
static queue_t *queue_group;

struct group_msg_t
{
  int devid;
  int dpid;
  double value;
};
static void* GroupTaskThread(void *param)
{
    DEVDPCTRLMSG sendMsg;
    int groupnum,i,gaddr;
    group_msg_t *pItem=NULL;
    
    while(running)
    {
        sem_wait(&sem_group);
        if(queue_pop(queue_group, (Q_AnyValue)pItem)==0){
            int devid,dpid;
            double value;
            devid = pItem->devid;
            dpid = pItem->dpid;
            value = pItem->value;
            groupnum = JsGroup_GetNum();
            for(i = 0; i < groupnum; i++)
            {
                cJSON *pJsDpList,*pJsObject,*pJsDev,*pJsDp;
        
                gaddr = JsGroup_GetAddrByIndex(i);
                pJsDpList = JsGroup_GetDpList(gaddr);
                if(!pJsDpList)
                    continue;
                pJsObject = pJsDpList->child;
                for(;pJsObject;pJsObject = pJsObject->next)
                {
                    pJsDev = cJSON_GetObjectItem(pJsObject,"devid");
                    pJsDp  = cJSON_GetObjectItem(pJsObject,"dpid");
                    if(    cJSON_IsNumber(pJsDev)
                        && (pJsDev->valueint == devid)
                        && cJSON_IsNumber(pJsDp)
                        && (pJsDp->valueint == dpid))
                    {
                        break;
                    }
                }

                if(!pJsObject)
                    continue;
        
                pJsObject = pJsDpList->child;
                for(;pJsObject;pJsObject = pJsObject->next)
                {
                    pJsDev = cJSON_GetObjectItem(pJsObject,"devid");
                    pJsDp  = cJSON_GetObjectItem(pJsObject,"dpid");
                    if( !( cJSON_IsNumber(pJsDev) && cJSON_IsNumber(pJsDp) ) )
                        continue;
                    if( (pJsDev->valueint == devid)
                        &&(pJsDp->valueint == dpid))
                    {
                        continue;
                    }
                    sendMsg.SrcDevId = MOD_GROUP_ID<<16+gaddr;
                    sendMsg.SrcDpId = 0;
                    sendMsg.DesDevId = pJsDev->valueint;
                    sendMsg.DesDpId = pJsDp->valueint;
                    sprintf(sendMsg.Data,"%f",value);
                    SendMsg2Manager(DEVDPCTRL,(char *)sendMsg,sizeof(sendMsg));
                }
            }

        }

    }
    return NULL;
}


int GroupModuleInit(void *param)
{
    if(JsGroup_Open(GROUP_FILE_PATH) < 0)
        return -1;
    
    if(pthread_mutex_init(&JsGroupLock, NULL) != 0){
        printf("group module pthread_mutex_init fail!\n");
        return -1;
    }
    if(sem_init(&sem_group,0,0) == -1)
        return -1;
    queue_group = queue_new(GROUP_QUEUE_SIZE);
    if(!queue_group)
        return -1;
    running = 1; 
    if(pthread_create(&th_group, NULL, GroupTaskThread, 0) != 0){
        printf("pthread_create:%s\n",strerror(errno));  
        return -1;
    }
    return 0;   
}
int GroupModuleExit(void *param)
{
    running = 0;
    sem_destroy(&sem_group);
    pthread_cancel(th_group);
    pthread_join(th_group,NULL);
    queue_delete(queue_group);
    return 0;
}
int Send2GroupModule(int type,void *msg)
{
    DEVDPCTRLMSG *pDpCtrl;
    if(msg == NULL){
        DALITEK_LOG(LOG_DBG,"msg is NULL");
        return -1;
        }
    if(group_num <= 0 || group_arr == NULL){
        DALITEK_LOG(LOG_DBG,"group data is NULL");
            return -1;
        }

    switch(type){
        case DEVDPCTRL:{
            pDpCtrl= (DEVDPCTRLMSG *)msg;
            if(GETMODID(pDpCtrl->DesDevID) != MOD_GROUP_ID) {
                break;
            }
            group_msg_t *pItem=NULL;
            pItem = (group_msg_t *)malloc(sizeof(group_msg_t));
            if(pItem == NULL){
                //内存不足
                return -1;
            }
            pItem->devid = pDpCtrl->DesDevId;
            pItem->dpid = pDpCtrl->DesDpId;
            pItem->value = atof(pDpCtrl->Data);
            if(queue_push(queue_group, (Q_AnyValue)pItem)==0){
                sem_post(&sem_group);
            }
            break;
        }
            
        default:
            break;
    }
    return 0;
}