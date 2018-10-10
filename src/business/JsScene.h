#ifndef DALITEK_JS_SCENE_H_
#define DALITEK_JS_SCENE_H_

#include "cJSON.h"


int JsScene_GetNum(void);                           // ��ȡ��������
cJSON *JsScene_GetTaskByIndex(int Index);           // ������Ŷ�����
cJSON *JsScene_GetTaskById(int Id);                 // ����ID������

int JsScene_AddScene(cJSON* pScene);                // ��ӳ���

int JsScene_DelScene(int SceneId);                  // ɾ������
int JsScene_Open(char *sFile);
int JsScene_Close(void);
#endif