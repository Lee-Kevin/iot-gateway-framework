#ifndef LDS_LIGHT_H
#define LDS_LIGHT_H
#include "lds_base_define.h"

#define DP_MAX_NUM 256
#define PRESET_MAX_NUM 128
struct lds_dp
{
    int id;
    unsigned char value;
    int  dptype;
    int  code;
};
struct lds_preset
{
    char presetid;
    struct lds_dp dps[DP_MAX_NUM];
};

typedef struct lds_light_t
{
    struct lds_base base;
    char dp_num;
    struct lds_dp  *lds_dp;
    char preset_num;
    struct lds_preset *lds_preset;
    char current_preset;
}lds_light_t;

lds_light_t *lds_light_new(unsigned int devid,unsigned char dpnum,int presetnum,int default_preset);
int lds_light_dp_init(struct lds_base *lds_dev,int dpid,int dptype,int dpvalue,int code);
int lds_light_preset_init(struct lds_base *lds_dev,int presetid,int dpid,int dpvalue);
int lds_light_delete(struct lds_base *lds_dev);
int lds_light_getvalue(struct lds_base *lds_dev,int dpid);
int lds_light_get_dp_type(struct lds_base *lds_dev,int dpid);
int lds_light_set_dp_value(struct lds_base *lds_dev,int dpid,unsigned char value);
int lds_light_get_dp_code(struct lds_base *lds_dev,int dpid);
int lds_light_save_preset(struct lds_base *lds_dev,int presetid);
int lds_light_control_preset(struct lds_base *lds_dev,int presetid);
int lds_light_read(struct lds_base *lds_dev);
int lds_light_write(struct lds_base *lds_dev,int dpid,int value);
#endif