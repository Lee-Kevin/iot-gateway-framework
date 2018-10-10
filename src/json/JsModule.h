#ifndef JS_MODULE_H
#define JS_MODULE_H

int JsModule_Open(const char *File);
int JsModule_Close(void);
int JsGetModuleValue(const char *ClassName,const char *ModuleName);

#endif