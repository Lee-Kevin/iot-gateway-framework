#ifndef USER_COMMON_H
#define USER_COMMON_H

#include "uthash.h"
typedef struct user_item
{
    char name[16];
    char passwd[16];
    int  auth;
    UT_hash_handle hh;
} user_item_t;

typedef struct session_item
{
    char session[40];
    char name[16];
    int  timelife;
    UT_hash_handle hh;
}session_item_t;

struct user_item *g_user;
struct session_item *g_session;

#endif

