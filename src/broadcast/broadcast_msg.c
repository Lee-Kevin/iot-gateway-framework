#include "broadcast_msg.h"
#include "cJSON.h"
#include "common.h"
#include "debug.h"

int broadcast_msg_proc(struct broadcast_msg_head *h,char *msg,int length)
{
    if(h == NULL || msg == NULL || length <0)
        return ERR_PARAMTER;
    cJSON *json,*pItem,*pObj;
    json = cJSON_Parse(msg);
    if(json){
       pObj = cJSON_GetObjectItem(json,"head");
        if(!pObj){
            //DALITEK_LOG(LOG_ERR,"head error\n");
            return ERR_HEAD;
        }
        pItem = cJSON_GetObjectItem(pObj,"ukey");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->ukey,pItem->valuestring,sizeof(h->ukey)-1);
        }else{
            ANNA_LOG(LOG_ERR,"head ukey error\n");
            cJSON_Delete(json);
            return ERR_METHOD;
        }
        pItem = cJSON_GetObjectItem(pObj,"method");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->method,pItem->valuestring,sizeof(h->method)-1);
        }else{
            ANNA_LOG(LOG_ERR,"head method error\n");
            cJSON_Delete(json);
            return ERR_METHOD;
        }

        pItem = cJSON_GetObjectItem(pObj,"topic");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->topic,pItem->valuestring,sizeof(h->topic)-1);
        }else{
            ANNA_LOG(LOG_ERR,"head topic error\n");
            cJSON_Delete(json);
            return ERR_TOPIC;
        }

        pItem = cJSON_GetObjectItem(pObj,"session");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->session,pItem->valuestring,sizeof(h->session)-1);
        }else{
            ANNA_LOG(LOG_ERR,"head session error\n");
            cJSON_Delete(json);
            return ERR_FROMTOPIC;
        }

        pItem = cJSON_GetObjectItem(pObj,"ts");
        if(pItem && pItem->type == cJSON_String){
            memcpy(h->ts,pItem->valuestring,sizeof(h->session)-1);
        }else{
           ANNA_LOG(LOG_ERR,"head ts error\n");
           cJSON_Delete(json);
           return ERR_TS;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"ver");
        if(pItem && pItem->type == cJSON_String){
            memcpy(h->ver,pItem->valuestring,sizeof(h->ver)-1);
        }else{
            ANNA_LOG(LOG_ERR,"head ver error\n");
            cJSON_Delete(json);
            return ERR_ERRORCODE;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"code");
        if(pItem && pItem->type == cJSON_Number){
            h->code= pItem->valueint;
        }else{
            ANNA_LOG(LOG_ERR,"head ErrorCode error\n");
            cJSON_Delete(json);
            return ERR_ERRORCODE;
        }

        cJSON_Delete(json);
        return ERR_SUCCESS;
    }else{
        ANNA_LOG(LOG_DBG,"json is error\n");
    }
    return ERR_UNKNOW;
}
