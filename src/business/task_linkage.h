#ifndef  TASK_LINKAGE_H
#define TASK_LINKAGE_H

int LinkageModuleInit(void *);
int LinkageModuleExit(void *);
int Send2LinkageModule(int type,void *msg);

#endif