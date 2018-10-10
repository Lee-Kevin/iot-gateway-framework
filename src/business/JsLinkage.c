#include "JsLinkage.h"
#include "debug.h"
#include "common.h"

static cJSON *pJsLinkageRoot = NULL;
static pthread_mutex_t gJsLinkageLock;
static char cJsFile[100]="";

static int mJsLinkage_GetTaskNum(void)
{
    cJSON *pJsArray;
    
    if(!pJsLinkageRoot) return -1;
    pJsArray = cJSON_GetObjectItem(pJsLinkageRoot,"linkage");
    if(!cJSON_IsArray(pJsArray)) return 0;
    return cJSON_GetArraySize(pJsArray);
}

int JsLinkage_GetTaskNum(void)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return 0;
    }
    ret = mJsLinkage_GetTaskNum();
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;
}
int mJsLinkage_Save(void)
{
    char *cJsStr = NULL;
    cJSON *pJsVer;
    int ver;
    if(!pJsLinkageRoot) return -1;
    pJsVer = cJSON_GetObjectItem(pJsLinkageRoot,"ver");
    if(cJSON_IsNumber(pJsVer)){
        ver = pJsVer->valueint;
        cJSON_SetIntValue(pJsVer, ver+1);
    }
    
    f=fopen("/tmp/Linkage.tmp","w");
    if(!f) {
        return -1;
    }
    cJsStr = cJSON_PrintUnformatted(pJsLinkageRoot);
    fseek(f,0,SEEK_SET);
    fwrite(cJsStr, 1, strlen(cJsStr), f);
    fclose(f);
    free(cJsStr);
    rename("/tmp/Linkage.tmp",cJsFile);
    sync();

    return 0;
    
}
int JsLinkage_Save(void)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return NULL;
    }
    ret = mJsLinkage_Save();
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;
}
static cJSON *mJsLinkage_GetTaskByIndex(int Index)
{
    cJSON *pJsArray;
    pJsArray = cJSON_GetObjectItem(pJsLinkageRoot,"linkage");
    if(!cJSON_IsArray(pJsArray)) return NULL;
    return cJSON_GetArrayItem(pJsArray,Index);
}
cJSON *JsLinkage_GetTaskByIndex(int Index)
{
    cJSON *ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return NULL;
    }
    ret = mJsLinkage_GetTaskByIndex(Index);
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;
}
static cJSON *mJsLinkage_GetTaskById(int Id)
{
    cJSON *pJsArray,*pJsObject,*pJsItem;
    int i,num;
    pJsArray = cJSON_GetObjectItem(pJsLinkageRoot,"linkage");
    if(!cJSON_IsArray(pJsArray)) return NULL;
    num = cJSON_GetArraySize(pJsArray);
    for(i = 0;i < num;i++){
        pJsObject = cJSON_GetArrayItem(pJsArray,i);
        if(!pJsObject) return NULL;
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(!cJSON_IsNumber(pJsItem)) continue;
        if(pJsItem->valueint == Id) 
            return pJsObject;
    }
    return NULL;
}
cJSON *JsLinkage_GetTaskById(int Id)
{
    cJSON *ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return NULL;
    }
    ret = mJsLinkage_GetTaskById(Id);
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;	
}

static int mJsLinkage_DelLinkageTask(int LinkageTaskId)
{
    cJSON *pJsArray,*pJsItem,*pJsObject;
    int i,num;
    pJsArray = cJSON_GetObjectItem(pJsLinkageRoot,"linkage");
    if(!cJSON_IsArray(pJsArray))
        return -1;
    
    if(!LinkageTaskId){
        cJSON_DeleteItemFromObject(pJsLinkageRoot,"linkage");
        return 0;
    }
    num = mJsLinkage_GetTaskNum();
    for(i = 0;i < num;i++){
        pJsObject = cJSON_GetArrayItem(pJsArray,i);
        if(!cJSON_IsObject(pJsObject))
            continue;
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(!cJSON_IsNumber(pJsItem))
            continue;
        if(pJsItem->valueint == LinkageTaskId){
            break;
        }
    }
    if(i < num)
        cJSON_DeleteItemFromArray(pJsArray,i);
    else
        return -1;
    return 0;
}

static int mJsLinkage_AddLinkageTask(cJSON* pLinkageTask)
{
    cJSON *pNewTask,*pJsArray,*pJsObject,*pJsItem,*pJsTmp;
    if(!pLinkageTask) return -1;
    if(!pJsLinkageRoot){
        pJsLinkageRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsLinkageRoot,"ver",pJsItem);
    }
    pJsArray = cJSON_GetObjectItem(pJsLinkageRoot,"linkage");
    if(!cJSON_IsArray(pJsArray)) return -1;

    pNewTask = cJSON_Duplicate(pLinkageTask,1);
    if(!pNewTask) return -1;

    pJsItem = cJSON_GetObjectItem(pNewTask,"id");
    if(!cJSON_IsNumber(pJsItem)){
        cJSON_Delete(pNewTask);
        return -1;
    }
    int num = cJSON_GetArraySize(pJsArray);
    int i;
    if(pJsItem->valueint == 0){
        int maxid=0;
        for(i = 0;i < num;i++){
            pJsObject = cJSON_GetArrayItem(pJsArray,i);
            if(!pJsObject) continue;
            pJsItem = cJSON_GetObjectItem(pJsObject,"id");
            if(!cJSON_IsNumber(pJsItem)) continue;
            if(pJsItem->valueint > maxid)
                maxid = pJsItem->valueint;
        }
        pJsItem = cJSON_GetObjectItem(pNewTask,"id");
        cJSON_SetIntValue(pJsItem,maxid+1);
    }else{
        for(i = 0;i<num;i++){
            pJsObject = cJSON_GetArrayItem(pJsArray,i);
            if(!pJsObject) continue;
            pJsTmp = cJSON_GetObjectItem(pJsObject,"id");
            if(!cJSON_IsNumber(pJsTmp)) continue;
            if(pJsItem->valueint == pJsTmp->valueint){
                cJSON_DeleteItemFromArray(pJsArray,i);
                break;
            }
        }
    }
    cJSON_AddItemToArray(pJsArray,pNewTask);
    return pJsItem->valueint;
}

int JsLinkage_DelLinkageTask(int LinkageTaskId)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return NULL;
    }
    ret = mJsLinkage_DelLinkageTask(LinkageTaskId);
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;
}
int JsLinkage_AddLinkageTask(cJSON* pLinkageTask)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsLinkageLock)) {
        return NULL;
    }
    ret = mJsLinkage_AddLinkageTask(pLinkageTask);
    pthread_mutex_unlock(&gJsLinkageLock);
    return ret;
}
 

int JsLinkage_Open(char *sFile)
{
    FILE *f;
    int len;
    char *data;
    
    if(NULL == sFile) {
        return -1;
    }
    if(pJsLinkageRoot) {
        return 0;
    }
    f=fopen(sFile,"rb");
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
    pJsLinkageRoot=cJSON_Parse(data);
    free(data);
    if (!pJsLinkageRoot) {
        cJSON *pJsItem;
        pJsLinkageRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsLinkageRoot,"ver",pJsItem);
    }
    
    if(pthread_mutex_init(&gJsLinkageLock, NULL) != 0)
    {
       //printf("pthread_mutex_init:%s\n",strerror(errno));
        cJSON_Delete(pJsLinkageRoot);
        return -1;
    }
    
	return 0;
}
int JsLinkage_Close(void)
{
    if(pJsLinkageRoot){
        cJSON_Delete(pJsLinkageRoot);
        pJsLinkageRoot = NULL;
        pthread_mutex_destroy(&gJsLinkageLock);
    }
    return 0;
}
