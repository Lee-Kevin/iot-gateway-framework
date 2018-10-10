#ifndef TASK_SCENE_H
#define TASK_SCENE_H

int SceneModuleInit(void *);
int SceneModuleExit(void *);
int SceneModuleTick(void *);
int Send2SceneModule(int type,void *msg);

#endif
