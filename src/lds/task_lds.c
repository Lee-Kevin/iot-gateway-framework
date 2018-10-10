#include "task_lds.h"
#include "lds_base_define.h"
#include "manager.h"
#include "debug.h"
#include "lds_manager.h"
#include "JsUart.h"
struct LdsQueue gLdsQueue;
static int running = 1;
pthread_t report_thread_id;
int LDS_UART_ID=-1;

static void lds_msg_report_process(LdsMsg_t *msg)
{
    DEVDPCTRLMSG pDpCtrl;
    if(msg == NULL)
        return ;
    //ANNA_LOG(LOG_DBG,"type=%d\n",msg->type);
    switch(msg->type){
        case LDS_REPORT_TYPE_VALUE:
        {
            /*给pDpCtrl 赋值，并发送到管理器的消息队列*/   
            pDpCtrl.DesDevId = (MOD_MQTT_ID << 16);
            pDpCtrl.DesDpId = 1;//1代表值改变上传 2.代表传感器类上报 3.代表安防类上报
            pDpCtrl.SrcDevId = msg->devid;
            pDpCtrl.SrcDpId = msg->dpid;
            sprintf(pDpCtrl.Data,"%d",msg->value);
            SendMsg2Manager(DEVDPCTRL,(char*)&pDpCtrl,sizeof(pDpCtrl));
            ANNA_LOG(LOG_DBG,"devid=%d,dpid=%d\n",pDpCtrl.SrcDevId,pDpCtrl.SrcDpId);
            break;
        }
        case LDS_REPORT_TYPE_GROUP:
        {
            break;
        }
        case LDS_REPORT_TYPE_LINKAGE:
        {
            break;
        }
        /*还有安防告警的上报*/
        default :{
            ANNA_LOG(LOG_ERR,"%s\n","not handled message!");
            break;
        }
    }
}

static void lds_report_thread(void *Param)
{
    Q_AnyValue *item;
    LdsMsg_t *ldsmsg=NULL;
    while(running){
        //ANNA_LOG(LOG_DBG,"recv message!\n");
        sem_wait(&gLdsQueue.recv_sem);
        //ANNA_LOG(LOG_DBG,"recv message!\n");
        while(queue_pop(gLdsQueue.recv_queue,(Q_AnyValue *)&ldsmsg) == 0){
                /*处理数据上报给网络*/
            if(ldsmsg == NULL)
                break;
            //ANNA_LOG(LOG_DBG,"recv message!\n");
            lds_msg_report_process(ldsmsg);
            free(ldsmsg);
            ldsmsg = NULL; 
       } 
    }
}


int LdsModuleInit(void *param)
{

    int uartid=-1;
    if(JsUart_Open(UART_CONFIGURE_FILE)!=0)
        return -1;
    JsUart_Get_Uartid_By_Param("LDS",&uartid);
    if(uartid != -1)
        LDS_UART_ID = uartid;
    else
        return -1;
    JsUart_Close();
    
    ANNA_LOG(LOG_DBG,"LDS_UART_ID = %d\n",LDS_UART_ID);

    
    running = 0;
    //1.信号量初始化
    if(sem_init(&(gLdsQueue.send_sem),0,0) < 0)
        return -1;
    if(sem_init(&(gLdsQueue.recv_sem),0,0) < 0)
        return -1;
    //2.队列初始化
    gLdsQueue.send_queue = queue_new(LDS_QUEUE_SIZE);
    if(gLdsQueue.send_queue == NULL)
        return -1;
    gLdsQueue.recv_queue = queue_new(LDS_QUEUE_SIZE);
    if(gLdsQueue.recv_queue == NULL)
        return -1;
    //3.创建线程
    running = 1;
    if(pthread_create(&report_thread_id, NULL, (void *)lds_report_thread, 0) != 0){
		running = 0;
        return -1;
    }
    //4.lds管理模块的初始化
    if(lds_manager_init() < 0)
        return -1;
    return 0;
}
int LdsModuleExit(void *param)
{
    //1.modbus管理模块退出
    lds_manager_uninit();
    //2.线程退出
    running = 0;
    pthread_cancel(report_thread_id);
    pthread_join(report_thread_id,NULL);
    //3.信号量销毁
    sem_destroy(&(gLdsQueue.send_sem));
    sem_destroy(&(gLdsQueue.recv_sem));
    //4.队列释放
    queue_delete(gLdsQueue.send_queue);
    queue_delete(gLdsQueue.recv_queue);

    return 0;
}
int LdsModuleTick(void *param)
{
    //检测modbus管理模块的运行状态
    DEVDPCTRLMSG DevDpCtrlMsg={0};
    DevDpCtrlMsg.DesDevId = (MOD_LDS_ID<<16)+99;
    DevDpCtrlMsg.DesDpId = 3;
    DevDpCtrlMsg.SrcDevId = MOD_MQTT_ID<<16;
    DevDpCtrlMsg.SrcDpId = 1;
    sprintf(DevDpCtrlMsg.Data,"%d",1);
    //SendMsg2Manager(DEVDPCTRL,(char *)(&DevDpCtrlMsg),sizeof(DevDpCtrlMsg));
    //ANNA_LOG(LOG_INFO,"modid=%d",GETMODID(DevDpCtrlMsg.DesDevId));
}
int Send2LdsModule(int type,void *msg)
{
    if(type < 1 || msg == NULL)
        return -1; 
    DEVDPCTRLMSG *pDpCtrl;
    LdsMsg_t ldsmsg;
    int data;
    //ANNA_LOG(LOG_ERR,"running...%d\n",running);
    if(running == 0)
        return -1;
    switch(type){
        case  DEVDPCTRL:{
            pDpCtrl = (DEVDPCTRLMSG *)msg;
            int modid = GETMODID(pDpCtrl->DesDevId);
            if(modid != MOD_LDS_ID)
                break;
            unsigned int devid = pDpCtrl->DesDevId;
            int dpid = pDpCtrl->DesDpId;
            int devaddr = GETDEVID(pDpCtrl->DesDevId);
            
            if(pDpCtrl->Data[0]== '\"' && pDpCtrl->Data[1]== '\"'){
                if(GETDEVID(pDpCtrl->DesDevId) == 0 && pDpCtrl->DesDpId == 0){
                    lds_get_device_all_value();
                    break;
                }else if(GETDEVID(pDpCtrl->DesDevId) != 0 && pDpCtrl->DesDpId == 0){
                    lds_get_device_value(devid);
                    break;
                }else if(GETDEVID(pDpCtrl->DesDevId) != 0 && pDpCtrl->DesDpId != 0){
                    lds_get_device_dp_value(devid,dpid);
                    break;
                }
                break;
            }
            
            if(1 != sscanf((const char*)pDpCtrl->Data, "%d", &data)){
                ANNA_LOG(LOG_ERR,"invalid data\n");
                break;
            }
            LdsMsg_t *pldsmsg = malloc(sizeof(LdsMsg_t));
            if(pldsmsg == NULL){
                //内存不够
                return -1;
            }
            pldsmsg->devid = devid;
            pldsmsg->dpid = dpid;
            pldsmsg->value = data;
            if(pldsmsg->value < 0)
                pldsmsg->type = LDS_CONTROL_TYPE_READ;
            else
                pldsmsg->type = LDS_CONTROL_TYPE_WRITE;
            if(queue_push(gLdsQueue.send_queue,(Q_AnyValue)pldsmsg)==0){
                sem_post(&gLdsQueue.send_sem);
            }else{
                ANNA_LOG(LOG_ERR,"push queue failed\n");
                free(pldsmsg);
            }
            //queue_printOccupy(gLdsQueue.send_queue);
            break;
        }
       default:
        break;
    }
    return 0;
}