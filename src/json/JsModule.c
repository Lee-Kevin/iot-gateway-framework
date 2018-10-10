#include "cJSON.h"
#include <stdio.h>
#include "JsModule.h"

static cJSON *pJsModuleRoot = NULL;
static char cJsFile[256] = ""; 

int JsModule_Open(const char *File)
{
    FILE *f;
    char *data;
    if(File == NULL)
        return -1;

    sprintf(cJsFile,"%s",File);
    f = fopen(cJsFile,"r");
    if(!f){
        return -1;
    }
    fseek(f,0,SEEK_END);
    int len = ftell(f);
    fseek(f,0,SEEK_SET);
    data = (char *)malloc(len+1);
    if(data == NULL){
	fclose(f);
        return -1;
    }
    fread(data,1,len,f);
    fclose(f);

    pJsModuleRoot = cJSON_Parse(data);
    free(data);
    if(!pJsModuleRoot){
        printf("Module is null\n");
        return -1;
    }
    return 0;
}
int JsModule_Close(void)
{
    if(pJsModuleRoot) {
        cJSON_Delete(pJsModuleRoot);
        pJsModuleRoot = NULL;
    }
    return 0;
}
int JsGetModuleValue(const char *ClassName,const char *ModuleName)
{
    int ret=-1;
    if(!pJsModuleRoot)
        return -1;
    cJSON *pJsClass=NULL,*pJsModule=NULL;
    pJsClass = cJSON_GetObjectItem(pJsModuleRoot,ModuleName);
    if(!cJSON_IsObject(pJsClass))
        return -1;
    pJsModule = cJSON_GetObjectItem(pJsClass,ModuleName);
    if(cJSON_IsNumber(pJsModule)){
         ret = pJsModule->valueint;
    }else{
        ret = 0;
    }
    return ret;
}
