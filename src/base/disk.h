#ifndef _DISK_H_
#define _DISK_H_
#include "user_common.h"
//#include "cJSON.h"
extern char uuid[64];
//#include "device_define.h"



int disk_init(void *param);
int disk_exit(void);
char *disk_get_uuid(void);

//增加用户
int disk_user_add(const char *name,const char *passwd);
//删除用户
int disk_user_delete(const char *name,const char *passwd);
//修改用户密码
int disk_user_modify_passwd(const char *name,const char *old_passwd,const char *new_passwd);
//修改用户昵称
int disk_user_modify_name(const char *name,const char *passwd,const char *new_name);

int disk_user_get_items(struct user_item *p_user);

struct user_item *disk_user_get_item(const char *name,struct user_item *p_user);

#endif
