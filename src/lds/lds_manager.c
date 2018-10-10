#include "lds_manager.h"

#include "common.h"
#include "debug.h"
#include "queue.h"
#include "uthash.h"
#include "manager.h"
#include "uart.h"

#include "lds_light.h"
//host<<24 + mod << 16 + (short)devid
//modbus_base_t *pMdbBase;

static int stopping = 0;
pthread_t lds_send_threadid;
pthread_t lds_loop_threadid;

//static struct com_t mdb_com;


struct ldsdev_hash{
    int devid;
    lds_base_t *pdev;
    UT_hash_handle hh;
};
static struct ldsdev_hash *g_ldsdev = NULL;


static struct ldsdev_hash *ldshash_find_device(unsigned int devid)
{
    struct ldsdev_hash *s;
    HASH_FIND_INT(g_ldsdev,&devid,s);
    return s;
}

static void ldshash_add_device(unsigned int devid,struct ldsdev_hash *pldsdev)
{
    struct ldsdev_hash *s;
    HASH_FIND_INT(g_ldsdev,&devid,s);
    if(s == NULL){
        s = pldsdev;
        HASH_ADD_INT(g_ldsdev,devid,s);
    }else{
        ;/*节点已存在*/
    }
}
static int ldshash_delete_device(unsigned int devid)
{
    struct ldsdev_hash *s = NULL;
    HASH_FIND_INT(g_ldsdev,&devid,s);
    if( s != NULL){
        HASH_DEL(g_ldsdev,s);
        free(s);
        return 0;
    }
    return -1;//设备不存在
}
static unsigned int ldshash_count_devices()
{
	return HASH_COUNT(g_ldsdev);
}

static void ldshash_delete_devices_all()
{
    struct ldsdev_hash *current_device,*tmp;
    HASH_ITER(hh,g_ldsdev,current_device,tmp){
        HASH_DEL(g_ldsdev,current_device);
        free(current_device);
    }
}

static void ldshash_print_devices()
{
	struct ldsdev_hash *s = NULL;
	for(s=g_ldsdev;s != NULL;s=(struct ldsdev_hash *)(s->hh.next)){
		printf("devid:%d\n",s->devid);
	}
}

static int lds_data_analysis(unsigned char *buf)
{
    int devid;
    unsigned char head,area,cmd;
    struct ldsdev_hash *pldsdev;
    head = buf[0];
    area = buf[1];
    cmd = buf[3];
    devid = (MOD_LDS_ID <<16) + area;
    pldsdev = ldshash_find_device(devid);
    if(pldsdev==NULL)
        return -1;
    if(head == 0xF5){
        switch(cmd){
            case 0xFE:{
                pldsdev->pdev->setdpvalue(pldsdev->pdev,0,buf[4]+1);
                pldsdev->pdev->control_preset(pldsdev->pdev,buf[4]+1);
                //disk update
                //send2net
                break;
            }
            case 0xD6:{
                pldsdev->pdev->setdpvalue(pldsdev->pdev,buf[5]+1,255);
                //disk update
                //send2net
                break;
            }
            case 0xDC:{
                pldsdev->pdev->setdpvalue(pldsdev->pdev,buf[5]+1,0);
                //disk update
                //send2net
                break;
            }
            case 0xD3:{
                pldsdev->pdev->setdpvalue(pldsdev->pdev,buf[5]+1,0xFF-buf[6]);
                //disk update
                //send2net
                break;
            }
            default :
                break;
        }
    
    }
}

static void lds_send_thread(void *Param)
{ 
    LdsMsg_t *pldsmsg=NULL;
    struct ldsdev_hash *plds;
    while(!stopping){
        if(sem_trywait(&gLdsQueue.send_sem)){
            //ANNA_LOG(LOG_DBG,"count=%d\n",queue_getcount(gLdsQueue.send_queue));
            while(queue_pop(gLdsQueue.send_queue,(Q_AnyValue *)&pldsmsg) == 0){
                /*处理数据*/
                if(pldsmsg == NULL)
                    continue;
                if(pldsmsg->type == LDS_CONTROL_TYPE_WRITE){
                    plds = ldshash_find_device(pldsmsg->devid);
                    if(plds != NULL){
                        if(plds->pdev->write != NULL){
                            plds->pdev->write(plds->pdev,pldsmsg->dpid,pldsmsg->value);
                            //数据库同步
                        }
                        ANNA_LOG(LOG_DBG,"devid=%d,devid=%d,type=%d\n",plds->devid,plds->pdev->devid,plds->pdev->dev_type);
                    }
                }else if(pldsmsg->type == LDS_CONTROL_TYPE_READ){//主动读设备
                    plds = ldshash_find_device(pldsmsg->devid);
                    if(plds != NULL){
                        if(plds->pdev->read != NULL){
                            plds->pdev->read(plds->pdev);
                        }
                    }
                }
                free(pldsmsg); 
                //ANNA_LOG(LOG_DBG,"count=%d\n",queue_getcount(gLdsQueue.send_queue));
                pldsmsg = NULL;
            }
        }else{
            /*轮询，轮询读设备*/
        }
        usleep(20000);
    }
}

static void lds_loop_thread(void *Param)
{
    unsigned char buf[RB_UART_TX_POOL_SIZE];
    while(!stopping){
      //  if(sem_wait(&g_uart[LDS_UART_ID].rb_uart_rx) == 0){
           /*解析来自串口的数据*/
      //     rt_ringbuffer_get(&g_uart[LDS_UART_ID].rb_uart_rx, buf, g_uart[LDS_UART_ID].rb_uart_rx.buffer_ptr[g_uart[LDS_UART_ID].rb_uart_rx.read_index]); 
      //     lds_data_analysis(buf);      
      // }
        
        LdsMsg_t *pldsmsg = malloc(sizeof(LdsMsg_t));
        if(pldsmsg == NULL){
                //内存不够
                return -1;
        }
        pldsmsg->type = LDS_REPORT_TYPE_VALUE;
        pldsmsg->devid = 123456;
        pldsmsg->dpid = 1;
        pldsmsg->value = 15;
        if(queue_push(gLdsQueue.recv_queue,(Q_AnyValue)pldsmsg)==0){
                sem_post(&gLdsQueue.recv_sem);
                //ANNA_LOG(LOG_DBG,"sem_post success\n");
        }else{
                ANNA_LOG(LOG_ERR,"push queue failed\n");
                free(pldsmsg);
        }
        //queue_printOccupy(gLdsQueue.recv_queue);
        usleep(10000);
    }

}
int lds_get_device_dp_value(unsigned int devid,int dpid)
{
    struct ldsdev_hash *s = NULL;
    struct lds_base *pldsdev;
    s = ldshash_find_device(devid);
    if(s == NULL)
        return -1;
    pldsdev = s->pdev;
    if(pldsdev == NULL)
       return -1;
    switch(pldsdev->dev_type){
        case DEVTYPE_LDS_LIGHT_ONOFF:{
            lds_light_t *pldslight = (lds_light_t *)pldsdev;
            if(pldslight!=NULL){
                int dpvalue=-1;
                dpvalue = pldslight->base.getdpvalue(&(pldslight->base),dpid);
                if(dpvalue != -1){
                        DEVDPCTRLMSG devctrlmsg={0};
                        devctrlmsg.SrcDevId = pldslight->base.devid;
                        devctrlmsg.SrcDpId = dpid;
                        sprintf(devctrlmsg.Data,"%d",dpvalue);
                        devctrlmsg.DesDevId = (MOD_MQTT_ID<<16) + 1;
                        devctrlmsg.DesDpId = 1;
                        ANNA_LOG(LOG_DBG,"DesDevId=%d,DesDpId=%d,SrcDevId=%d,SrcDpId=%d\n",devctrlmsg.DesDevId,devctrlmsg.DesDpId,devctrlmsg.SrcDevId,devctrlmsg.SrcDpId);
                        SendMsg2Manager(DEVDPCTRL,(char *)&devctrlmsg,sizeof(DEVDPCTRLMSG));
                }
            }
        }
        default :
            break;
   }
    return 0;
}

int lds_get_device_value(unsigned int devid)
{
    struct ldsdev_hash *s = NULL;
    struct lds_base *pldsdev;
    s = ldshash_find_device(devid);
    if(s == NULL)
        return -1;
    pldsdev = s->pdev;
    if(pldsdev == NULL)
       return -1;
	switch(pldsdev->dev_type){
        case DEVTYPE_LDS_LIGHT_ONOFF:{
            lds_light_t *pldslight = (lds_light_t *)pldsdev;
            if(pldslight!=NULL){
                int i;
                DEVDPCTRLMSG devctrlmsg={0};
                devctrlmsg.SrcDevId = pldslight->base.devid;
                devctrlmsg.DesDevId = (MOD_MQTT_ID<<16) + 1;
                devctrlmsg.DesDpId = 1;
                for(i=0;i<pldslight->dp_num;i++){
                    devctrlmsg.SrcDpId = pldslight->lds_dp[i].id;
                    sprintf(devctrlmsg.Data,"%d",pldslight->lds_dp[i].value);
                    ANNA_LOG(LOG_DBG,"DesDevId=%d,DesDpId=%d,SrcDevId=%d,SrcDpId=%d\n",devctrlmsg.DesDevId,devctrlmsg.DesDpId,devctrlmsg.SrcDevId,devctrlmsg.SrcDpId);
                    SendMsg2Manager(DEVDPCTRL,(char *)&devctrlmsg,sizeof(DEVDPCTRLMSG));
                }
                devctrlmsg.SrcDpId = 0;
                sprintf(devctrlmsg.Data,"%d",pldslight->current_preset);
                ANNA_LOG(LOG_DBG,"DesDevId=%d,DesDpId=%d,SrcDevId=%d,SrcDpId=%d\n",devctrlmsg.DesDevId,devctrlmsg.DesDpId,devctrlmsg.SrcDevId,devctrlmsg.SrcDpId);
                SendMsg2Manager(DEVDPCTRL,(char *)&devctrlmsg,sizeof(DEVDPCTRLMSG));
            }
        }
        default:
            break;
   }
    return 0;
}

void lds_get_device_all_value(void)
{
    struct ldsdev_hash *s = NULL;
	for(s=g_ldsdev;s != NULL;s=(struct ldsdev_hash *)(s->hh.next)){
		lds_get_device_value(s->devid);
	}
}

#if 0
int lds_device_add(struct lds_base * plds_dev)
{
    int ret=0; 
    struct lds_base *pdev=NULL;
    switch(devtype){
        case MODBUS_DEVTYPE_LIGHT){
            lds_light_t *plight = (lds_light_t*)malloc(sizeof(lds_light_t));
            plight->base = {devtype,devid,lds_light_init,lds_light_uninit,lds_light_getvalue,modbus_light_read,modbus_light_control,NULL,NULL};
            plight->base->init(plight,devid);
            pdev = (lds_base *)plight;
            if(pdev == NULL)
                break;
            ldshash_add_device(pdev->devid,pdev);
            break;
        }
        case 
            default:
                ret = -1;
                break;
        }
    return ret;
}
    
int lds_device_delete(int devid)
{
    return ldshash_delete_device(devid);
}

void lds_device_clear_all()
{
    ldshash_delete_devices_all();
}



int lds_device_init()
{
    //从数据库读数据
    //创建hash表
    //创建线程
    struct dev_base *pdev=NULL;
    struct dev_base *ptmp;
    int lds_dev_number=0;
    
    
    /*指定已初始化的串口*/
    int i;
    mdb_com.protocol_id = 0;
    for(i=0;i<UART_MAX;i++){
        if(m_com[i].protocol_id == MOD_LDS_ID){
            mdb_com = (struct com_t)m_com[i];
            break;
        }
    }
    if(mdb_com.protocol_id == 0){
        ANNA_LOG(LOG_ERR,"modbus load com failed!\n");
        return -1;
    }

    mdb_dev_number = disk_get_dev_number_by_com(mdb_com);
    if(mdb_dev_number <= 0){
        return -1;
    }

    pdev = (struct dev_base *)malloc(sizeof(struct dev_base));
    if(pdev == NULL){
        //内存已满
        return -1;
    } 
    memset(pdev,0,sizeof(struct dev_base)*mdb_dev_number);
    
    for(i=0;i<mdb_dev_number;i++){
        struct modbus_base *pmdbdev=NULL;
        ptmp = pdev[i];
        if(ptmp->devtype < 1)
            continue;
        switch(ptmp->devtype){
            case MODBUS_DEVTYPE_LIGHT:{
                mosbus_light_t *plight = (mosbus_light_t*)malloc(sizeof(mosbus_light_t));
                if(plight == NULL){
                    //内存不够
                    return -1;
                }
                plight={ptmp->devtype,ptmp->devid,modbus_light_init,modbus_light_uninit,modbus_light_getvalue,modbus_light_read,modbus_light_control,NULL,NULL};
                plight->base->init(plight,ptmp->devid);
                pdev = (modbus_base *)plight;
                if(pdev == NULL)
                    break;
                mdb_add_device(pdev->devid,pdev);
                }
            default:
                break;
        }
    }
    
    
    
}
#endif
int lds_manager_init(void)
{
    /*从数据库获取n个设备，加载对象到内存中*/
    int i,j,k;
    lds_base_t *pdev_lds;
    
    for(i=0;i<100;i++){
        int devid= (MOD_LDS_ID<<16) +i+1;
        int devtype = DEVTYPE_LDS_LIGHT_ONOFF;
        int devdpnum = (i+1)%5;
        int devpresetnum= 3;
        //ANNA_LOG(LOG_DBG,"devid = %d\n",devid);
        lds_light_t *lds_light =  lds_light_new(devid,devdpnum,devpresetnum,1);
        if(lds_light != NULL){
            for(j=0;j<devdpnum;j++){
                lds_light_dp_init(lds_light, j, 1, 0, LDS_REPORT_TYPE_VALUE);
            }
            for(k=1;k<=devpresetnum;k++){
                for(j=0;j<devdpnum;j++){
                    //lds_light_dp_init(lds_light, j, 1, 0, LDS_REPORT_TYPE_VALUE);
                    lds_light_preset_init(lds_light,j, lds_light->lds_dp[j].id, 254);
                }
                
            }
          //lds_light->base = {devtype,devid,lds_light_dp_init,lds_light_preset_init,lds_light_delete,lds_light_getvalue,lds_light_get_dp_type,lds_light_get_dp_code,lds_light_set_dp_value,lds_light_save_preset,lds_light_read,lds_light_write};
            pdev_lds = (lds_base_t *)lds_light;
            struct ldsdev_hash *pldshash=NULL;
            pldshash = malloc(sizeof(struct ldsdev_hash));
            if(pldshash == NULL)
                break;
            pldshash->devid = devid; 
            pldshash->pdev = pdev_lds;
            ldshash_add_device(devid,pldshash);
        }
    }
    #if 0
    ldshash_print_devices();
    ANNA_LOG(LOG_DBG,"count = %d\n",ldshash_count_devices());
    ldshash_delete_devices_all();
    ldshash_print_devices();
    ANNA_LOG(LOG_DBG,"count = %d\n",ldshash_count_devices());
    #endif
    if(pthread_create(&lds_send_threadid, NULL, (void *)lds_send_thread, 0) != 0){
        return -1;
    }
    if(pthread_create(&lds_loop_threadid, NULL, (void *)lds_loop_thread, 0) != 0){
        return -1;
    }
    return 0;
}

void lds_manager_uninit(void)
{
    stopping = 1;
    pthread_cancel(lds_send_threadid);
    pthread_join(lds_send_threadid,NULL);
    pthread_cancel(lds_loop_threadid);
    pthread_join(lds_loop_threadid,NULL);
}


