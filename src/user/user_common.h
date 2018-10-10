#ifndef USER_COMMON_H
#define USER_COMMON_H
#include "uthash.h"

typedef struct user_item
{
	char name[16];
	char passwd[16];
	UT_hash_handle hh;
}user_item_t;
typedef struct session_item
{
	char session[40];
	int  lifetime;
	UT_hash_handle hh;
}session_item_t;

extern struct user_item *g_users;
extern struct session_item *g_sessions;

#endif