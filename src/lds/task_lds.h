#ifndef _TASK_LDS_H
#define _TASK_LDS_H

int LdsModuleInit(void *param);
int LdsModuleExit(void *param);
int LdsModuleTick(void *);
int Send2LdsModule(int type,void *msg);

#endif