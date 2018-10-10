#include <stdio.h>
#include <string.h> 
#include <stdlib.h>  
#include <pthread.h>
#include <errno.h>
#include "cJSON.h"
#include "JS_Group.h"


static cJSON *pJSGroupAddr = NULL;
static char cJsFile[100] = "";
static pthread_mutex_t gJsGroupAddrLock;

/*获取组地址的数量*/
static int mJsGroup_GetNum(void)
{
    cJSON *pJsArray;
    
    if(!pJSGroupAddr) return -1;
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return -1;
    
    return cJSON_GetArraySize(pJsArray);
}

static int mJsGroup_GetAddrByIndex(int Index)
{
    cJSON *pJsArray,*pJsObject,*pJsItem;
    
    if(!pJSGroupAddr) return -1;
    
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return -1;
    
    pJsObject = cJSON_GetArrayItem(pJsArray,Index);
    if(!cJSON_IsObject(pJsObject)) return -1;
    
    pJsItem = cJSON_GetObjectItem(pJsObject,"id");
    if(!cJSON_IsNumber(pJsItem)) return -1;

    return pJsItem->valueint;
}

static cJSON* mJsGroup_GetDpList(int Id)
{
    cJSON *pJsArray,*pJsObject,*pJsItem;
    
    if(!pJSGroupAddr) return NULL;
    
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return NULL;
    pJsObject = pJsArray->child;
    while(pJsObject)
    {
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(cJSON_IsNumber(pJsItem) && (pJsItem->valueint == Id))
        {
            return cJSON_GetObjectItem(pJsObject,"dps");
        }
        pJsObject = pJsObject->next;
    }

    return NULL;
}
static int mJsGroup_DelAddr(int Id)
{
    cJSON *pJsArray,*pJsObject,*pJsItem;
    int cnt=0;
    
    if(!pJSGroupAddr) return 0;
    if(!Id)
    {
        cJSON_DeleteItemFromObject(pJSGroupAddr,"group");
        return 0;
    }
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return 0;
    
    pJsObject = pJsArray->child;
    while(pJsObject)
    {
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(cJSON_IsNumber(pJsItem) && (pJsItem->valueint == Id))
        {
            cJSON_DeleteItemFromArray(pJsArray,cnt);
            return 0;
        }
        pJsObject = pJsObject->next;
        cnt++;
    }

    return 0;
}
static int mJsGroup_DelGroupDp(int Id,int DevId, int DpId)
{
    cJSON *pJsArray,*pJsObject,*pJsItem,*pJsDpArray,*pJsDpObject,*pJsDpItem;
    
    if(!pJSGroupAddr) return 0;
    
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return 0;
    
    pJsObject = pJsArray->child;
    while(pJsObject)
    {
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(cJSON_IsNumber(pJsItem) && (pJsItem->valueint == Id))
        {
            int cnt = 0;

            if(!DevId)
            {
                cJSON_DeleteItemFromObject(pJsObject,"dps");
                return 0;
            }
            pJsDpArray = cJSON_GetObjectItem(pJsObject,"dps");
            if(!cJSON_IsArray(pJsDpArray)) return 0;
            
            pJsDpObject = pJsDpArray->child;
            while(pJsDpObject)
            {
                pJsDpItem = cJSON_GetObjectItem(pJsDpObject,"devid");
                if( cJSON_IsNumber(pJsDpItem) && (pJsDpItem->valueint == DevId))
                {
                    pJsDpItem = cJSON_GetObjectItem(pJsDpObject,"dpid");
                    if( pJsDpItem && (pJsDpItem->valueint == DpId))
                    {
                        cJSON_DeleteItemFromArray(pJsDpArray,cnt);
                        return 0;
                    }
                }
                pJsDpObject = pJsDpObject->next;
                cnt++;
            }
            return 0;
        }
        pJsObject = pJsObject->next;
    }

    return 0;
}

static int mJsGroup_DelDev(int DevId)
{
    int cnt = 0;
    cJSON *pJsGroupArray,*pJsGroupObject;
    cJSON *pJsArray,*pJsObject,*pJsItem;
    
    if(!pJSGroupAddr) return 0;
    
    pJsGroupArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsGroupArray)) return 0;
    
    pJsGroupObject = pJsGroupArray->child;
    for(;pJsGroupObject;pJsGroupObject = pJsGroupObject->next)
    {
        
        pJsArray = cJSON_GetObjectItem(pJsGroupObject,"dps");
        if(!cJSON_IsArray(pJsArray))
            continue;
        pJsObject = pJsArray->child;
        cnt = 0;
        while(pJsObject)
        {
            pJsItem = cJSON_GetObjectItem(pJsObject,"devid");
            if( cJSON_IsNumber(pJsItem) && (pJsItem->valueint == DevId))
            {
                cJSON_DeleteItemFromArray(pJsArray,cnt);
                pJsObject = pJsArray->child;
                cnt = 0;
                continue;
            }
            pJsObject = pJsObject->next;
            cnt++;
        }
    }

    return 0;
}

static int mJsGroup_AddAddr(int Id)
{
    cJSON *pJsArray,*pJsObject,*pJsItem;
    
    if(!pJSGroupAddr) 
    {
        pJSGroupAddr = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJSGroupAddr,"ver",pJsItem);
    }
    
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray))
    {
        pJsArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pJSGroupAddr,"group",pJsArray);
    }
    if(0 == Id)
    {
        int maxid = 0;
        pJsObject = pJsArray->child;
        while(pJsObject)
        {
            pJsItem = cJSON_GetObjectItem(pJsObject,"id");
            if(cJSON_IsNumber(pJsItem) && (pJsItem->valueint > maxid))
            {
                maxid = pJsItem->valueint;
            }
            pJsObject = pJsObject->next;
        }
        Id = maxid + 1;
    }
    else
    {
        mJsGroup_DelAddr(Id);
    }
    
    pJsObject = cJSON_CreateObject();
    pJsItem   = cJSON_CreateNumber(Id);
    cJSON_AddItemToObject(pJSGroupAddr,"id",pJsItem);
    cJSON_AddItemToArray(pJsArray,pJsObject);
    
    return Id;
}

static int mJsGroup_AddDp(int Id,SGroup_Dp* pDp)
{
    cJSON *pJsArray,*pJsObject,*pJsItem,*pJsDpArray,*pJsDpObject;
    
    if(!pJSGroupAddr) return -1;
    if(!(pDp->DevId && pDp->DpId)) return -1;
    
    mJsGroup_DelGroupDp(Id,pDp->DevId,pDp->DpId);
    
    pJsArray = cJSON_GetObjectItem(pJSGroupAddr,"group");
    if(!cJSON_IsArray(pJsArray)) return -1;
    pJsObject = pJsArray->child;
    while(pJsObject)
    {
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(cJSON_IsNumber(pJsItem) && (pJsItem->valueint == Id))
        {
            pJsDpArray = cJSON_GetObjectItem(pJsObject,"dps");
            if(!cJSON_IsArray(pJsDpArray))
            {
                pJsDpArray = cJSON_CreateArray();
                cJSON_AddItemToObject(pJsObject,"dps",pJsDpArray);
            }
            pJsDpObject = cJSON_CreateObject();
            pJsItem = cJSON_CreateNumber(pDp->DevId);
            cJSON_AddItemToObject(pJsDpObject,"devid",pJsItem);
            pJsItem = cJSON_CreateNumber(pDp->DpId);
            cJSON_AddItemToObject(pJsDpObject,"dpid",pJsItem);
            pJsItem = cJSON_CreateString(pDp->Name);
            cJSON_AddItemToObject(pJsDpObject,"name",pJsItem);
            cJSON_AddItemToArray(pJsDpArray,pJsDpObject);
            
            return 0;
        }
        pJsObject = pJsObject->next;
    }

    return -1;
}

static int mJSGroup_SaveFile(void)
{
    char *cJsStr = NULL;
    FILE *f;
    int ver;
    cJSON *pJsVer;
    
    if(!pJSGroupAddr) {
        return -1;
    }

    pJsVer = cJSON_GetObjectItem(pJSGroupAddr,"ver");
    if(cJSON_IsNumber(pJsVer)){
        ver = pJsVer->valueint;
        cJSON_SetIntValue(pJsVer,ver+1);
    }
    
    f=fopen("/tmp/Group.tmp","w");
    if(!f) {
        return -1;
    }
    cJsStr = cJSON_PrintUnformatted(pJSGroupAddr);
    fseek(f,0,SEEK_SET);
    fwrite(cJsStr, 1, strlen(cJsStr), f);
    fclose(f);
    free(cJsStr);
    rename("/tmp/Group.tmp",cJsFile);
    sync();
    
    return 0;
}

int JsGroup_GetNum(void)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_GetNum();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_GetAddrByIndex(int Index)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_GetAddrByIndex(Index);
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}


cJSON* JsGroup_GetDpList(int id)
{
    cJSON *ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return NULL;
    }
    ret = mJsGroup_GetDpList(id);
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_AddAddr(int Id)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_AddAddr(Id);
    mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_AddDp(int Id,SGroup_Dp* pDp)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_AddDp(Id,pDp);
    mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_DelAddr(int id)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_DelAddr(id);
    mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_DelGroupDp(int Id,int DevId, int DpId)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_DelGroupDp(Id,DevId,DpId);
    mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_DelDev(int DevId)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJsGroup_DelDev(DevId);
    mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JSGroup_SaveFile(void)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsGroupAddrLock)) {
        return -1;
    }
    ret = mJSGroup_SaveFile();
    pthread_mutex_unlock(&gJsGroupAddrLock);

    return ret;
}

int JsGroup_Open(char* sFile)
{
	FILE *f;
    int len;
    char *data;
    
    if(NULL == sFile) {
        return -1;
    }
    if(pJSGroupAddr) {
        return 0;
    }
    f=fopen(sFile,"rb+");
    if(!f) {
        return -1;
    }
    sprintf(cJsFile,"%s",sFile);
    fseek(f,0,SEEK_END);
    len=ftell(f);
    fseek(f,0,SEEK_SET);
	data=(char*)malloc(len+1);
    fread(data,1,len,f);
    fclose(f);
	pJSGroupAddr=cJSON_Parse(data);
	free(data);
	if (!pJSGroupAddr) {
        cJSON *pJsItem;
        pJSGroupAddr = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJSGroupAddr,"ver",pJsItem);
    }
    
    if(pthread_mutex_init(&gJsGroupAddrLock, NULL) != 0)
    {
        printf("pthread_mutex_init:%s\n",strerror(errno));
        cJSON_Delete(pJSGroupAddr);
        return -1;
    }
    
    return 0;
}

int JsGroup_Close(void)
{
    if(pJSGroupAddr) {
        cJSON_Delete(pJSGroupAddr);
        pJSGroupAddr = NULL;
        pthread_mutex_destroy(&gJsGroupAddrLock);
    }
    return 0;
}


