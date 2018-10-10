#ifndef DEV_COMMOM_H
#define DEV_COMMOM_H

/*
数据类型定义：
devid unsigned int
devtype unsigned short
dpid int
dptype unsigned short

*/

typedef struct datapoint
{
    int devid;
    int devtype;
    int dpid;
    int dptype;
    double data;//default
}datapoint_t;

#define DP_TYPE_LIGHT_BOOL_ONOFF        1
#define DP_TYPE_LIGHT_CHAR_LEVEL        2
#define DP_TYPE_LIGHT_CHAR_PRESET       3
#endif