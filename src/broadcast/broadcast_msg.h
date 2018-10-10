#ifndef BROADCAST_MSG_H
#define BROADCAST_MSG_H

struct broadcast_msg_head
{
    char ukey[64];
    char method[10];
    char topic[64];
    char session[64];
    char ts[20];
    int  ver[20];
    int  code;
};
int broadcast_msg_proc(struct broadcast_msg_head *h,char *msg,int length);

#endif
