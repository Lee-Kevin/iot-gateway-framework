#ifndef _JS_LINKAGE_H_
#define _JS_LINKAGE_H_
#include "cJSON.h"

int JsLinkage_GetTaskNum(void);
cJSON *JsLinkage_GetTaskByIndex(int Index);
cJSON *JsLinkage_GetTaskById(int Id); 

int JsLinkage_AddLinkageTask(cJSON* pScene);
int JsLinkage_DelLinkageTask(int SceneId); 

int JsLinkage_Save(void);
int JsLinkage_Open(char *sFile);
int JsLinkage_Close(void);

#endif
