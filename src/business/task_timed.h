#ifndef DALITEK_TASK_TIMED_H
#define DALITEK_TASK_TIMED_H
#include "cJSON.h"

int TimedModuleInit(void *);
int TimedModuleExit(void *);
int TimedModuleTick(void *);
int Send2TimedModule(int type,void *msg);
int TimedTaskDel(int pDelId);
int TimedTaskAdd(cJSON *pmTask);
int TimedTaskSetEnable(int TaskId,int enable);
#endif