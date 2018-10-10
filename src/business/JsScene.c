#include <stdio.h>
#include <string.h> 
#include <stdlib.h>  
#include <pthread.h>
#include <errno.h>

#include "JsScene.h"
static cJSON *pJsSceneRoot = NULL;
static pthread_mutex_t gJsSceneLock;
static char cJsFile[100] = "";

static int mJsScene_GetNum(void)
{
    cJSON *pJsArray;
    
    if(!pJsSceneRoot){
        return 0;
    }
    pJsArray= cJSON_GetObjectItem(pJsSceneRoot, "scenetask") ;
    if(!cJSON_IsArray(pJsArray)){
        return 0;
    }
    return cJSON_GetArraySize(pJsArray);
}
static cJSON *mJsScene_GetTaskByIndex(int Index)
{
    cJSON *pJsArray;

    if(!pJsSceneRoot){
        return NULL;
    }
    pJsArray= cJSON_GetObjectItem(pJsSceneRoot, "scenetask") ;
    if(!cJSON_IsArray(pJsArray)){
        return NULL;
    }
    return cJSON_GetArrayItem(pJsArray,Index);
} 

static cJSON *mJsScene_GetTaskById(int Id)
{
    cJSON *pJsArray,*pJsObject,*pJSItem;

    if(!pJsSceneRoot)
    {
        return NULL;
    }
    pJsArray= cJSON_GetObjectItem(pJsSceneRoot, "scenetask") ;
    if(!cJSON_IsArray(pJsArray)){
        return NULL;
    }
    for(pJsObject = pJsArray->child; pJsObject; pJsObject = pJsObject->next){
        pJSItem =  cJSON_GetObjectItem(pJsObject, "id") ;
        if(pJSItem && pJSItem->valueint == Id){
            return pJsObject;
        }
    }
    return NULL;
} 
static int  mJsScene_Save()
{
    cJSON *pJsVer;
    char *cJsStr=NULL;
    FILE *f;
    int ver;
    if(!pJsSceneRoot)
        return -1;
    pJsVer = cJSON_GetObjectItem(pJsSceneRoot,"ver");
    if(cJSON_IsNumber(pJsVer)){
        ver = pJsVer->valueint;
        cJSON_SetIntValue(pJsVer,ver+1);
    }
    f=fopen("/tmp/Scene.tmp","w");
    if(!f) {
        return -1;
    }
    cJsStr = cJSON_PrintUnformatted(pJsSceneRoot);
    fseek(f,0,SEEK_SET);
    fwrite(cJsStr, 1, strlen(cJsStr), f);
    fclose(f);
    free(cJsStr);
    rename("/tmp/Scene.tmp",cJsFile);
    sync();

    return 0;
}

static int mJsScene_DelScene(int SceneId)
{
    cJSON *pJsObject,*pJsArray,*pJsItem,*pJsTimedTask,*pTemp,*pJsTimedItem;
    int  i,j;
      
    if(!pJsSceneRoot)
        return -1;
    if(!SceneId){
        cJSON_DeleteItemFromObject(pJsSceneRoot,"scenetask");
        return 0;
    }
    
    pJsArray = cJSON_GetObjectItem(pJsSceneRoot,"scenetask");
    if(!pJsArray)
        return -1;
    for(pJsObject = pJsArray->child,i = 0; pJsObject; pJsObject = pJsObject->next, i++){
        pJsItem = cJSON_GetObjectItem(pJsObject,"id");
        if(pJsItem && (pJsItem->valueint == SceneId)){        
                break;
        }
    }
    if(!pJsObject)
        return -1;

    cJSON_DeleteItemFromArray(pJsArray,i);
    
    return 0;
}
static int mJsScene_AddScene(cJSON* pScene)
{
    cJSON *pNewScene;
    cJSON *pJsObject,*pJsArray,*pJsItem,*pJsEnable;
    
    if(!pJsSceneRoot){
        pJsSceneRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsSceneRoot,"ver",pJsItem);
    }

    pJsArray = cJSON_GetObjectItem(pJsSceneRoot,"scenetask");
    if(!pJsArray){
        pJsArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pJsSceneRoot,"scenetask",pJsArray);
    }
    pNewScene = cJSON_Duplicate(pScene,1);
    if(!pNewScene){
        return -1;
    }
    pJsItem = cJSON_GetObjectItem(pNewScene,"id");
    if(!pJsItem)  {
        cJSON_Delete(pNewScene);
        return -1;
    }
    if(pJsItem->valueint == 0){
        int maxid = 0;
        pJsObject = pJsArray->child;
        while(pJsObject){
            pJsItem = cJSON_GetObjectItem(pJsObject,"id");
            if(pJsItem && (pJsItem->valueint > maxid)){
                maxid = pJsItem->valueint;
            }
            pJsObject = pJsObject->next;
        }
        pJsItem = cJSON_GetObjectItem(pNewScene,"id");
        cJSON_SetIntValue(pJsItem,maxid+1);
    }else{
        mJsScene_DelScene(pJsItem->valueint);
    }
  
    cJSON_AddItemToArray(pJsArray,pNewScene);
    
    pJsItem = cJSON_GetObjectItem(pNewScene,"id") ;
    return cJSON_IsNumber(pJsItem) ? pJsItem->valueint : -1;
}


int JsScene_GetNum(void)
{
    int ret;
    if(0 != pthread_mutex_lock(&gJsSceneLock)) {
        return -1;
    }
    ret = mJsScene_GetNum();
    pthread_mutex_unlock(&gJsSceneLock);

    return ret;
}
cJSON *JsScene_GetTaskByIndex(int Index)
{
    cJSON *ret;

    if(0 != pthread_mutex_lock(&gJsSceneLock)) {
        return NULL;
    }
    ret = mJsScene_GetTaskByIndex(Index);
    pthread_mutex_unlock(&gJsSceneLock);
    
    return ret;
}
cJSON *JsScene_GetTaskById(int Id)
{
     cJSON *ret;

    if(0 != pthread_mutex_lock(&gJsSceneLock)) {
        return NULL;
    }
    ret = mJsScene_GetTaskById(Id);
    pthread_mutex_unlock(&gJsSceneLock);

    return ret;
}

int JsScene_AddScene(cJSON* pScene)
{
    int ret;
    cJSON *pJsItem;

    if(!pScene){
        DALITEK_PRINT("Input param error\n");
        return -1;
    }
    pJsItem = cJSON_GetObjectItem(pScene, "id");
    if(!cJSON_IsNumber(pJsItem)){
        DALITEK_PRINT("Input param error\n");
        return -1;
    }
    pJsItem = cJSON_GetObjectItem(pScene, "name") ;
    if(!cJSON_IsString(pJsItem)){
        DALITEK_PRINT("Input param error\n");
        return -1;
    }
    if(0 != pthread_mutex_lock(&gJsSceneLock)) {
        return -1;
    }
    ret = mJsScene_AddScene(pScene);
    mJsScene_Save();
    pthread_mutex_unlock(&gJsSceneLock);

    return ret;
}

int JsScene_DelScene(int SceneId)
{
    int ret;
    DALITEK_PRINT("JsScene_DelScene\n");
    if(SceneId < 0) {
        DALITEK_PRINT("Input param error\n");
        return -1;
    }
    if(0 != pthread_mutex_lock(&gJsSceneLock)) {
        return -1;
    }
    ret = mJsScene_DelScene(SceneId);
    mJsScene_Save();
    pthread_mutex_unlock(&gJsSceneLock);

    return ret;
}

/*read json from disk */
int JsScene_Open(char* sFile)
{
    FILE *f;
    int len;
    char *data;
    
    if(NULL == sFile) {
        return -1;
    }
    if(pJsSceneRoot) {
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
    pJsSceneRoot=cJSON_Parse(data);
    free(data);
    if (!pJsSceneRoot) {
        cJSON *pJsItem;
        pJsSceneRoot = cJSON_CreateObject();
        pJsItem = cJSON_CreateNumber(1);
        cJSON_AddItemToObject(pJsSceneRoot,"ver",pJsItem);
    }
    
    if(pthread_mutex_init(&gJsSceneLock, NULL) != 0)
    {
        printf("pthread_mutex_init:%s\n",strerror(errno));
        cJSON_Delete(pJsSceneRoot);
        return -1;
    }

    return 0;
}
int JsScene_Close(void)
{
    if(pJsSceneRoot) {
        cJSON_Delete(pJsSceneRoot);
        pJsSceneRoot = NULL;
        pthread_mutex_destroy(&gJsSceneLock);
    }
    return 0;
}
