#include "task_scene.h"
#include "cJSON.h"
#include "JsScene.h"

static int SceneExecute(int SceneId)
{
    cJSON *pJsScene,*pJsArray,*pJsObject,*pJsItem,*pJsEnble;
    char  *pData;
    
    pJsScene = JsScene_GetTaskById(SceneId);
    if(!pJsScene)
        return -1;

    pJsArray = cJSON_GetObjectItem(pJsScene, "devdptask");
    if(pJsArray){
        DEVDPCTRLMSG mDpmsg;
        mDpmsg.SrcDevId     = SETDEVID(MOD_SCENE_ID,SceneId);
        for(pJsObject = pJsArray->child; pJsObject; pJsObject = pJsObject->next){
            pJsItem = cJSON_GetObjectItem(pJsObject, "id");
            if(!pJsItem || (pJsItem->type != cJSON_Number))
                continue;
            mDpmsg.SrcDpId = pJsItem->valueint;

            pJsItem = cJSON_GetObjectItem(pJsObject, "devid");
            if(!pJsItem || (pJsItem->type != cJSON_Number))
                continue;
            mDpmsg.DesDevId = pJsItem->valueint;
            
            pJsItem = cJSON_GetObjectItem(pJsObject, "dpid");
            if(!pJsItem || (pJsItem->type != cJSON_Number))
                continue;
            mDpmsg.DesDpId = pJsItem->valueint;

            pJsItem = cJSON_GetObjectItem(pJsObject, "data");
            if(!pJsItem  || (pJsItem->type != cJSON_String))
                continue;
            else
                pData = pJsItem->valuestring;
            sprintf(mDpmsg.Data,"%s",pData);
            //DALITEK_LOG(LOG_DBG,"id devid dpid data:%d,%d,%d,%s\n",SceneId,mDpmsg.DesDevID,mDpmsg.DesDpID,mDpmsg.Data);
            SendMsg2Main(DEVDPCTRL, (char*)&mDpmsg, sizeof(mDpmsg));	 
        }
       
    }
    return 0;
}

int SceneModuleInit(void *param)
{
    int scene_num=0;
    int index;
    cJSON *pJsItem,*pJsId,*pJsEnable,*pJsTimedTask;
    if(JsScene_Init()== -1)
        return 0;
    return 0;
}
int SceneModuleExit(void *param)
{
    JsScene_Uninit();
    return 0;
}
int SceneModuleTick(void *param)
{
	return 0;
}
int Send2SceneModule(int type,void *msg)
{
    DALITEK_LOG(LOG_DBG, "Send2SceneModule");
    DEVDPCTRLMSG *pDpCtrl;
    int ret = 0;
    switch(type){
        case DEVDPCTRL:{
            pDpCtrl= (DEVDPCTRLMSG *)msg;
            if(GETMODID(pDpCtrl->DesDevId) != MOD_SCENE_ID) {
                break;
            }
            if(pDpCtrl->DesDpId == 1) {
                if('1' == pDpCtrl->Data[0]) {
                    //DALITEK_PRINT("SceneExecute\n");
                    SceneExecute(GETDEVID(pDpCtrl->DesDevId));
                }
            } 
            break;
        }
        default:
            break;
    }	
    return 0;
}
