/*******************************************************************************
 * Author			: edwin.liang 
 * Email			: 944922198@qq.com
 * Last modified	: 2017-08-43 15:19
 * Filename			: task_mqtt.c
 * Description		: 
 * *****************************************************************************/
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <mosquitto.h>
#include <uuid/uuid.h>

//”Ú√˚Ω‚Œˆ
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "manager.h"
#include "OAS_msg_process.h"
#include "debug.h"
#include "disk.h"

#define BUF_MAX_LEN 8196

static int running = 1;
static struct mosquitto *mosq;
static pthread_t loop_thread_id; 
static int tick = 10;

static char *myuuid=NULL;
static char mytopic[128]={0};
static char mystate[256]={0};
static char myupdate[256]={0};
static int messageid=0;
static void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
    mosquitto_subscribe(mosq, NULL,mytopic, 1);
    int m_id=0;
    ANNA_LOG(LOG_INFO,"subscribe topic %s\n",mytopic);
    mosquitto_publish(mosq, (int *)m_id, mystate, 6,"online", 1, 1);
}
static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	struct msg_head st={0};
    char output[256]={0};
    int ret;

    ANNA_LOG(LOG_DBG,"topic:%s\nmessage:%s\n",message->topic,message->payload);
    if(strcmp(message->topic,mytopic)){
        ANNA_LOG(LOG_DBG,"it's not my message\n");
        return 0;
    }

    if(mqtt2gw_OAS_msg_head_parse(&st,message->payload)!=0){
        ANNA_LOG(LOG_ERR,"message head parse error\n");
        return -1;
    }

	ret = mqtt2gw_OAS_msg_proc(message->mid,&st,MOD_MQTT_ID,(char*)message->payload,output,256);
	if(ret != 0){
		char ts[20]={0};
		struct timeval current;
		gettimeofday(&current, NULL);
		sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
		memset(output,0,256);
		sprintf(output,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",st.ukey,st.session,st.method,st.topic,ts,ret);
	}
	remote_publish(st.topic,output);
    ANNA_LOG(LOG_INFO,"output : %s\n",output);
    
	if(0==strcmp("{\"cmd\":\"open\"}",message->payload)){
        system("echo 1 > /sys/class/leds/mediatek\:green\:lan_1/brightness; echo 1 > /sys/class/leds/mediatek\:green\:lan_2/brightness;echo 1 > /sys/class/leds/mediatek\:green\:system/brightness;echo 1 > /sys/class/leds/mediatek\:green\:wan/brightness;echo 1 > /sys/class/leds/mediatek\:green\:wifi/brightness");
    }else if(0==strcmp("{\"cmd\":\"close\"}",message->payload)){
        system("echo 0 > /sys/class/leds/mediatek\:green\:lan_1/brightness; echo 0 > /sys/class/leds/mediatek\:green\:lan_2/brightness;echo 0 > /sys/class/leds/mediatek\:green\:system/brightness;echo 0 > /sys/class/leds/mediatek\:green\:wan/brightness;echo 0 > /sys/class/leds/mediatek\:green\:wifi/brightness");
    }
}
static void subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    ANNA_LOG(LOG_INFO,"subscribe callback\n");
}
static void log_callback(struct mosquitto *mosq, void *obj, int level, const char *str)
{   
    ANNA_LOG(LOG_INFO,"log:%s\n",str);
}
static void disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    ANNA_LOG(LOG_INFO,"disconnect\n");
}
static void publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
    ANNA_LOG(LOG_INFO,"publish callback\n");
}
void  remote_publish(char *topic,char *str)
{
    int pub_id=0;
    mosquitto_publish(mosq, (int *)&pub_id, topic, strlen(str),str, 0, 0);
}

static void* msg_loop_thread(void *param)
{
    int rc = -1;
    while(running){
        rc = mosquitto_loop(mosq, -1, 1);
        if(running && rc){
            sleep(2);
            rc = mosquitto_reconnect(mosq);
            ANNA_LOG(LOG_INFO,"reconnect\n");
        }
    }
    return NULL;
}

int MqttModuleInit(void *param)
{
    int rt = -1;
    bool clean_session = false;
    
    mosquitto_lib_init();
    myuuid = disk_get_uuid();
    if(myuuid == NULL)
        return -1;
    sprintf(mytopic,"%s/command",myuuid);
    sprintf(mystate,"%s/state",myuuid);
    sprintf(myupdate,"%s/update",myuuid);
    ANNA_LOG(LOG_INFO,"uuid:%s\n",myuuid);
    ANNA_LOG(LOG_INFO,"myupdate:%s\n",myupdate);
    mosq = mosquitto_new((const char*)myuuid, clean_session, NULL);
    if(mosq){
        mosquitto_connect_callback_set(mosq,connect_callback);
        mosquitto_disconnect_callback_set(mosq,disconnect_callback);
        mosquitto_log_callback_set(mosq,log_callback);
        mosquitto_message_callback_set(mosq,message_callback);
        mosquitto_subscribe_callback_set(mosq,subscribe_callback);
        mosquitto_publish_callback_set(mosq,publish_callback);
        mosquitto_will_set(mosq,mystate,7,"offline",1,1);
    	//mosquitto_will_clear(mosq);
    }else{
        ANNA_LOG(LOG_ERR,"mosquitto new failed\n");
        return -1;
    }  	
    //mosquitto_username_pw_set(mosq,"mahao","mahao12345@");
    mosquitto_connect(mosq, "47.97.195.216", 1883, 60);

    //add reconnect thread
    if(pthread_create(&loop_thread_id, NULL, msg_loop_thread, 0) != 0){
        ANNA_LOG(LOG_ERR,"mqtt thread create failed\n");
        return -1;
    }
    ANNA_LOG(LOG_INFO,"[ mqtt init success! ]\n");
    return 0;
}

int MqttModuleExit(void *param)
{
    running = 0;
    pthread_cancel(loop_thread_id);
    pthread_join(loop_thread_id,NULL);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    ANNA_LOG(LOG_INFO,"[ mqtt exit! ]\n");
    return 0;
}
int MqttModuleTick(void *param)
{
    //just testing
    if(tick%60 == 0)
        ANNA_LOG(LOG_INFO,"MqttModuleTick\n");
    if(tick == 0){
        tick = 24*60*60;
    }
    tick--;
    return 0;
}
int Send2MqttModule(int type,void *msg)
{
    ANNA_LOG(LOG_INFO,"Send2MqttModule\n");
    char msgbuf[128]={0};
    sprintf(msgbuf,"messageid = %d\n",messageid++);
    remote_publish(myupdate,msgbuf);
    return 0;
}




