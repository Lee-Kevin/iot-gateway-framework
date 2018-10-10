#ifndef LDS_MANAGER_H
#define LDS_MANAGER_H
#include "lds_base_define.h"

#if 0
int lds_device_init();
int lds_device_uninit(struct lds_base *);
int lds_device_read(struct lds_base *);
int lds_device_control(struct lds_base *,int dpid,double value);
double lds_device_get_dp_value(struct lds_base *,int dpid);
int lds_device_set_parameter(struct lds_base *,void *param);
int lds_device_get_parameter(struct lds_base *,void *param);

int lds_device_add(int devtype,int devid);
int lds_device_delete(int devid);
void lds_device_clear_all();
#endif
int lds_manager_init(void);
void lds_manager_uninit(void);

int lds_get_device_dp_value(unsigned int devid,int dpid);
int lds_get_device_value(unsigned int devid);
void lds_get_device_all_value(void);


#endif
