#ifndef __JS_TIMEDTASK_H_
#define __JS_TIMEDTASK_H_
#include "cJSON.h"


int JsTimed_GetNum(void);
cJSON *JsTimed_GetTaskByIndex(int Index);
cJSON *JsTimed_GetTaskById(int Id);
int JsTimed_AddTask(cJSON* pJsTask);
int JsTimed_DelTask(int TaskId);
int JsTimed_SetEnable(int TaskId,int Enable);
int JsTimed_SaveFile(void);
int JsTimed_Open(char* sFile);
int JsTimed_Close(void);

#endif

