#ifndef  TASK_GROUP_H
#define TASK_GROUP_H
#include "JsGroup.h"

int GroupModuleInit(void *);
int GroupModuleExit(void *);
int Send2GroupModule(int type,void *msg);

#endif