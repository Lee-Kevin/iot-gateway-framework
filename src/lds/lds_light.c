#include "lds_light.h"
#include "dev_common.h"
#include "debug.h"
#include "uart.h"
static unsigned char lds_check_sum(unsigned char *buffer,unsigned short num)
{
    unsigned char sum=0;
    unsigned short i;
    for(i=num;i!=0;i--){
        sum+=*buffer++;
        sum = sum%256;
    }
    sum = 256-sum;
    return sum;
}

lds_light_t *lds_light_new(unsigned int devid,unsigned char dpnum,int presetnum,int default_preset)
{
    lds_light_t *lds_light;
    lds_light = (lds_light_t*)malloc(sizeof(lds_light_t));
    if(lds_light == NULL)
        return NULL;
    lds_light->base.dev_type = DEVTYPE_LDS_LIGHT_ONOFF;
    lds_light->base.devid = devid;
    lds_light->dp_num = dpnum > DP_MAX_NUM ? DP_MAX_NUM:dpnum;
    lds_light->lds_dp = (struct lds_dp*)malloc(sizeof(struct lds_dp)*(lds_light->dp_num));
    if(lds_light->lds_dp == NULL){
        free(lds_light);
        return NULL;
    }
    
    lds_light->preset_num = presetnum > PRESET_MAX_NUM ? PRESET_MAX_NUM:presetnum;
    lds_light->lds_preset = (struct lds_preset*)malloc(sizeof(struct lds_preset)*presetnum);
    if(lds_light->lds_preset == NULL){
        free(lds_light->lds_dp);
        free(lds_light);
        return NULL;
    }
    if(default_preset != 0)
        lds_light->current_preset = default_preset;
    struct lds_base lds_base_light = {lds_light->base.dev_type,lds_light->base.devid,lds_light_dp_init,lds_light_preset_init,lds_light_delete,lds_light_getvalue,lds_light_get_dp_type,lds_light_get_dp_code,lds_light_set_dp_value,lds_light_save_preset,lds_light_control_preset,lds_light_read,lds_light_write};
    lds_light->base = lds_base_light;
    //ANNA_LOG(LOG_DBG,"devid = %d\n",lds_light->base.devid);
    return lds_light;
}

int lds_light_dp_init(struct lds_base *lds_dev,int dpid,int dptype,int dpvalue,int code)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;  
    int i;
    memset(lds_light->lds_dp,0,sizeof(struct lds_dp)*(lds_light->dp_num));
    for(i=0;i<lds_light->dp_num;i++){
        if(lds_light->lds_dp[i].id != 0)
            continue;
        lds_light->lds_dp[i].id = dpid;
        lds_light->lds_dp[i].dptype = dptype;
        lds_light->lds_dp[i].value = dpvalue;
        lds_light->lds_dp[i].code = code;
        break;
    }
    return 0;
}

int lds_light_preset_init(struct lds_base *lds_dev,int presetid,int dpid,int dpvalue)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1; 
    int i,j;
    memset(lds_light->lds_preset,0,sizeof(struct lds_preset)*(lds_light->preset_num));
    for(i=0;i<lds_light->preset_num;i++){
        if(lds_light->lds_preset[i].presetid != 0)
            continue;
        lds_light->lds_preset[i].presetid = presetid;
        for(j=0;j<lds_light->dp_num;j++){
            if(lds_light->lds_preset[i].dps[j].id != 0)
                continue;
            lds_light->lds_preset[i].dps[j].id = dpid;
            lds_light->lds_preset[i].dps[j].value = dpvalue;
            break;
        }
        break;
    }
    return 0;
}

int lds_light_delete(struct lds_base *lds_dev)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1; 
    
    if(lds_light->lds_preset != NULL)
        free(lds_light->lds_preset);
    if(lds_light->lds_dp != NULL)
        free(lds_light->lds_dp);
    free(lds_light);
    return 0;
}

int lds_light_getvalue(struct lds_base *lds_dev,int dpid)
{
    if(lds_dev == NULL )
        return -1;
	lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1; 
    int i,value=-1;
    if(dpid == 0)
        value = lds_light->current_preset;
    else if(dpid > 0){
        for(i=0;i<lds_light->dp_num;i++){
            if(dpid != lds_light->lds_dp[i].id)
                continue;
            value = lds_light->lds_dp[i].value;
        }
    }
    return value;
}

int lds_light_get_dp_type(struct lds_base *lds_dev,int dpid)
{
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1; 
	int i,dptype=-1;
    for(i=0;i<lds_light->dp_num;i++){
        if(lds_light->lds_dp[i].id == dpid){
            dptype = lds_light->lds_dp[i].dptype;
            break;
        }
    }
    return dptype;
}

int lds_light_set_dp_value(struct lds_base *lds_dev,int dpid,unsigned char value)
{
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
	int i;
    for(i=0;i<lds_light->dp_num;i++){
        if(lds_light->lds_dp[i].id == dpid){
            lds_light->lds_dp[i].value = value;;
            break;
        }
    }
    return (i < lds_light->dp_num) ? 0:(-1);
}

int lds_light_get_dp_code(struct lds_base *lds_dev,int dpid)
{
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
	int i,code=-1;
    for(i=0;i<lds_light->dp_num;i++){
        if(lds_light->lds_dp[i].id == dpid){
            code = lds_light->lds_dp[i].code;
            break;
        }
    }
    return code;
}

int lds_light_save_preset(struct lds_base *lds_dev,int presetid)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
    int i,j,k;
    for(i=0;i<lds_light->preset_num;i++){
        if(lds_light->lds_preset[i].presetid == presetid){
            for(j=0;j<lds_light->dp_num;j++){
                for(k=0;k<lds_light->dp_num;k++){
                    if(lds_light->lds_preset[i].dps[j].id == lds_light->lds_dp[k].id){
                        lds_light->lds_preset[i].dps[j].value = lds_light->lds_dp[k].value;
                    }
                }
            }
            break;
        }
    }
    return (i < lds_light->dp_num) ? 0:(-1);
}

int lds_light_control_preset(struct lds_base *lds_dev,int presetid)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
    int i,j,k;
    for(i=0;i<lds_light->preset_num;i++){
        if(lds_light->lds_preset[i].presetid == presetid){
            for(j=0;j<lds_light->dp_num;j++){
                for(k=0;k<lds_light->dp_num;k++){
                    if(lds_light->lds_preset[i].dps[j].id == lds_light->lds_dp[k].id){
                        lds_light->lds_dp[k].value = lds_light->lds_preset[i].dps[j].value;
                    }
                }
            }
            break;
        }
    }
    return (i < lds_light->dp_num) ? 0:(-1);
}

int lds_light_read(struct lds_base *lds_dev)
{
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
    return 0;
}

int lds_light_write(struct lds_base *lds_dev,int dpid,int value)
{
    if(lds_dev == NULL)
        return -1;
    lds_light_t *lds_light=(lds_light_t *)lds_dev;
    if(lds_light == NULL)
        return -1;
    /*dpid=*/
    ANNA_LOG(LOG_DBG,"dpid = %d,value=%d\n",dpid,value);
    if(dpid > 0){
        unsigned char addr = (lds_light->base.devid) & 0x000000FF;
        int dptype = lds_light_get_dp_type(lds_light,dpid);
        char buf[8]={0};
        buf[0] = 0xF5;
        buf[1] = addr;
        buf[2] = 0xFF;
        if(dptype == DP_TYPE_LIGHT_BOOL_ONOFF){
            buf[3] = 0xD3;
            buf[4] = 0x0A;
            buf[5] = dpid - 1;
            buf[6] = (0xFF-(unsigned char)value) == 0 ? 255:1;        
        }else if(dptype == DP_TYPE_LIGHT_CHAR_LEVEL){
            ;
        }
        buf[7] = lds_check_sum(buf,7);
        lds_light_set_dp_value(lds_light,dpid,(unsigned char)value);
        //发送指令到串口
        rt_ringbuffer_putchar(&g_uart[LDS_UART_ID].rb_uart_tx,9);
        rt_ringbuffer_put(&g_uart[LDS_UART_ID].rb_uart_tx,buf,8);
    }else if(dpid == 0){
        /*场景控制*/
        lds_light_control_preset(lds_light,value);
        /*数据库的preset 到 dp状态同步*/
        //发送指令到串口
    }else if(dpid == -1){
        /*场景保存*/
        lds_light_save_preset(lds_light,value);
        /*数据库的dp状态 到 preset表同步*/
        //发送指令到串口
    }
    
    return 0;
}

