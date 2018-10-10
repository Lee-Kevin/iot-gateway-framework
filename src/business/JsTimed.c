#include <stdio.h>
#include <string.h> 
#include <stdlib.h>  
#include <pthread.h>
#include "JS_TimedTask.h"

static cJSON *pJsTimedRoot = NULL;
static char cJSFile[100] = "";
static pthread_mutex_t gJsTimedLock;

static int mJsTimed_GetNum(void)
{
    cJSON *pJsTask = NULL;

    if(!pJsTimedRoot)
        return 0;
    pJsTask = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!cJSON_IsArray(pJsTask))
        return 0;
    
    return cJSON_GetArraySize(pJsTask);
}
static int mJsTimed_GetVer(void)
{
    cJSON *pJsVer = NULL;

    if(!pJsTimedRoot)
        return 0;
    pJsVer = cJSON_GetObjectItem(pJsTimedRoot,"ver");
    if(!cJSON_IsNumber(pJsVer))
        return -1;
    
    return pJsVer->valueint;
}

static cJSON *mJsTimed_GetTaskByIndex(int Index)
{
    cJSON *pJsArray ,*pJsItem;

    if(!pJsTimedRoot)  return NULL;
    
    pJsArray = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!cJSON_IsArray(pJsArray))   return NULL;

    pJsItem = cJSON_GetArrayItem(pJsArray,Index);
    
    return pJsItem;
} 

static cJSON *mJsTimed_GetTaskById(int Id)
{
    cJSON *pJsArray ,*pJsObject ,*pJsItem;
    int i,num;

    if(!pJsTimedRoot)  return NULL;
    
    pJsArray = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!cJSON_IsArray(pJsArray))  return NULL;

    pJsObject = pJsArray->child;
    while(pJsObject){
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(!cJSON_IsNumber(pJsItem)){
            pJsObject = pJsObject->next;
            continue;
        }
        if(pJsItem->valueint == Id){
            return pJsObject;
        }
    }
    return NULL;
} 

static int mJsTimed_SaveFile(void)
{
    char *cJsStr = NULL;
    FILE *f;
    cJSON *pJsVer;
    int ver=0;
    
    if(!pJsTimedRoot) {
        return -1;
    }
    pJsVer = cJSON_GetObjectItem(pJsTimedRoot,"ver");
    if(cJSON_IsNumber(pJsVer)){
        ver = pJsVer->valueint;
        cJSON_SetIntValue(pJsVer,ver+1);
    }
    
   f=fopen("/tmp/TimedTask.tmp","w");
    if(!f) {
        return -1;
    }
    cJsStr = cJSON_Print(pJsTimedRoot);
    fseek(f,0,SEEK_SET);
    fwrite(cJsStr, 1, strlen(cJsStr), f);
    fclose(f);
    rename("/tmp/TimedTask.tmp",cJSFile);
    sync();
    free(cJsStr);
    
    return 0;
}
static int mJsTimed_AddTask(cJSON* pTask)
{
    cJSON *pNewTask, *pJsArray ,*pJsObject ,*pJsItem,*pJsTemp;

    if(!pTask)  return -1;
    if(!pJsTimedRoot)
    {
        pJsTimedRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsTimedRoot,"ver",pJsItem);
    }

    pJsArray = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!cJSON_IsArray(pJsArray))
    {
        pJsArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pJsTimedRoot,"timedtask",pJsArray);
    }
    
    pNewTask = cJSON_Duplicate(pTask,1);
    if(!pNewTask)  return -1;

    pJsItem = cJSON_GetObjectItem(pNewTask,"id");
    if(!cJSON_IsNumber(pJsItem))
    {
        cJSON_Delete(pNewTask);
        return -1;
    }

    int num = cJSON_GetArraySize(pJsArray);
    int i;
    if(pJsItem->valueint == 0){
        int maxid = 0;
        for(i = 0; i < num; i++){
            pJsObject = cJSON_GetArrayItem(pJsArray,i);
            if(!pJsObject) 
                break;
            pJsItem = cJSON_GetObjectItem(pJsObject,"id");
            if(!pJsItem) 
                break;
            if(pJsItem->valueint > maxid)
                maxid = pJsItem->valueint;
        }
        pJsItem = cJSON_GetObjectItem(pNewTask,"id");
        cJSON_SetIntValue(pJsItem,maxid+1);
    }else{
        for(i = 0; i < num; i++){
            pJsObject = cJSON_GetArrayItem(pJsArray,i);
            if(!pJsObject) 
                break;
            pJsTemp = cJSON_GetObjectItem(pJsObject,"id");
            if(!pJsTemp) 
                break;
            if(pJsItem->valueint == pJsTemp->valueint){
                cJSON_DeleteItemFromArray(pJsArray,i);
                break;
            }
        }
    }
    cJSON_AddItemToArray(pJsArray,pNewTask);
    
    return pJsItem->valueint;
}

static int mJsTimed_DelTask(int TaskId)
{
    cJSON *pJsObject,*pJsArray,*pJsItem;
    int  i;
    
    if(!pJsTimedRoot)
        return -1;
    
    if(!TaskId)
    {
        cJSON_DeleteItemFromObject(pJsTimedRoot,"timedtask");
        return 0;
    }
    
    pJsArray = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!pJsArray)
        return -1;

    for(pJsObject = pJsArray->child,i = 0; pJsObject; pJsObject = pJsObject->next, i++)
    {
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(!cJSON_IsNumber(pJsItem))
            continue;
        if(pJsItem->valueint == TaskId)
            break;
    }
    if(!pJsObject)
        return -1;

    cJSON_DeleteItemFromArray(pJsArray,i);
    
    return 0;
}

static int mJsTimed_SetEnable(int TaskId,int Enable)
{
    cJSON *pJsObject,*pJsArray,*pJsItem,*pJsEnable;
    int  i,num;

    if(!pJsTimedRoot)
        return -1;

    pJsArray = cJSON_GetObjectItem(pJsTimedRoot,"timedtask");
    if(!cJSON_IsArray(pJsArray)) return -1;
    
    for(pJsObject = pJsArray->child,i = 0; pJsObject; pJsObject = pJsObject->next, i++){
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(!pJsItem)
            continue;
        if(TaskId == 0){
            pJsEnable = cJSON_GetObjectItem(pJsObject,"enable");
            if(cJSON_IsNumber(pJsEnable)){
                cJSON_SetIntValue(pJsEnable, Enable);
            }
            continue;
        }
        if(pJsItem->valueint == TaskId){
            pJsEnable = cJSON_GetObjectItem(pJsObject,"enable");
            if(cJSON_IsNumber(pJsEnable)){
                cJSON_SetIntValue(pJsEnable, Enable);
            }else{
                return -1;
            }
            break;
         }
    }
    
    if(!pJsObject)
        return -1;
    return 0;
}


int JsTimed_GetNum(void)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return -1;
    }
    ret = mJsTimed_GetNum();
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}

cJSON *JsTimed_GetTaskByIndex(int Index)
{
    cJSON *ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return NULL;
    }
    ret = mJsTimed_GetTaskByIndex(Index);
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}

cJSON *JsTimed_GetTaskById(int Id)
{
    cJSON *ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return NULL;
    }
    ret = mJsTimed_GetTaskById(Id);
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}

int JsTimed_AddTask(cJSON* pJsTask)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return -1;
    }
    ret = mJsTimed_AddTask(pJsTask);
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}
int JsTimed_DelTask(int TaskId)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return -1;
    }
    ret = mJsTimed_DelTask(TaskId);
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}

int JsTimed_SetEnable(int TaskId,int Enable)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return -1;
    }
    ret = mJsTimed_SetEnable(TaskId,Enable);
    pthread_mutex_unlock(&gJsTimedLock);
    return ret;
}

int JsTimed_SaveFile(void)
{
    int ret;

    if(0 != pthread_mutex_lock(&gJsTimedLock)) {
        return -1;
    }
    ret = mJsTimed_SaveFile();
    pthread_mutex_unlock(&gJsTimedLock);

    return ret;
}
int JsTimed_Open(char* sFile)
{
    FILE *f;
    int len;
    char *data;
    
    if(NULL == sFile) {
        return -1;
    }
    if(pJsTimedRoot) {
        return 0;
    }
    f=fopen(sFile,"rb");
    if(!f) {
        return -1;
    }
    sprintf(cJSFile,"%s",sFile);
    fseek(f,0,SEEK_END);
    len=ftell(f);
    fseek(f,0,SEEK_SET);
    data=(char*)malloc(len+1);
    fread(data,1,len,f);
    fclose(f);
    pJsTimedRoot=cJSON_Parse(data);
    free(data);
    if (!pJsTimedRoot) {
        cJSON *pJsItem;
        pJsTimedRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsTimedRoot,"ver",pJsItem);
    }
    
    if(pthread_mutex_init(&gJsTimedLock, NULL) != 0)
    {
       //printf("pthread_mutex_init:%s\n",strerror(errno));
        cJSON_Delete(pJsTimedRoot);
        return -1;
    }
    
    return 0;
}

int JsTimed_Close(void)
{
    if(pJsTimedRoot) {
        JsTimed_SaveFile();
        cJSON_Delete(pJsTimedRoot);
        pJsTimedRoot = NULL;
        pthread_mutex_destroy(&gJsTimedLock);
    }
    return 0;
}

