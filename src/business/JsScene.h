#ifndef DALITEK_JS_SCENE_H_
#define DALITEK_JS_SCENE_H_

#include "cJSON.h"


int JsScene_GetNum(void);                           // 获取场景总数
cJSON *JsScene_GetTaskByIndex(int Index);           // 根据序号读场景
cJSON *JsScene_GetTaskById(int Id);                 // 根据ID读场景

int JsScene_AddScene(cJSON* pScene);                // 添加场景

int JsScene_DelScene(int SceneId);                  // 删除场景
int JsScene_Open(char *sFile);
int JsScene_Close(void);
#endif