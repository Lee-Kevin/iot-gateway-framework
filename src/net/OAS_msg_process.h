#ifndef _OAS_MSG_PROCESS_H_
#define _OAS_MSG_PROCESS_H_

typedef struct msg_head{
    char ukey[64];
    int   ver;
    char method[16];
    char topic[64];
    char session[64];
    char ts[20];
    int   code;
}msg_head_t;

int mqtt2gw_OAS_msg_head_parse(struct msg_head * h,char *str);
int mqtt2gw_OAS_msg_proc(int mid,struct msg_head *st,int mod,char *msg,char *out,int outlen);
#endif
