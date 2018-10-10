
#include <stdarg.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <sys/socket.h>    
#include <netdb.h>    
#include <arpa/inet.h>    
#include <net/if.h>      
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>  
#include <net/route.h>  

#include "debug.h"
#include "task_broadcast.h"
#include "broadcast_msg.h"
#include "common.h"

#define BC_BUF_MAX_LEN (8192)
#define BROADCAST_SERVER_PORT (4930)
static int running = true;
static pthread_t  looping;
static int  sockfd = -1 ;
static char uuid[512] ;

static int broadcast_srv_init(void)
{
    int ret = -1 ;   
    struct sockaddr_in addr;
    int opt = 1;    
        
    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;        
    addr.sin_port = htons(BROADCAST_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    
    if ( sockfd < 0 ){
        ANNA_LOG(LOG_ERR," broadcast socket create fail!\n");
        goto exit;
    }

    if (bind (sockfd, (struct sockaddr*)&addr, sizeof (addr)) == -1){            
        ANNA_LOG(LOG_ERR,"bind");            
        goto exit;    
    }    

    ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));      
    if ( ret == -1){
        ANNA_LOG(LOG_ERR, "setsockopt error !\n" ) ;
        goto  exit ; 
    }
       // ANNA_LOG(LOG_DBG,"tinitizlize broadcast OK!\n");        
    return ret; 
exit:
    if ( sockfd != -1 ){
        close( sockfd ) ; 
    }

    return ret; 
}
static void* Broadcast_looping(void* param)
{
    struct sockaddr_in addrcli = {0};    
    socklen_t addrlen = sizeof(addrcli);    
    ssize_t size = 0 ;
    char buf[BC_BUF_MAX_LEN] = {0};
    fd_set watchset;

    addrcli.sin_family = AF_INET;
    addrcli.sin_addr.s_addr = htonl(INADDR_ANY);
    addrcli.sin_port = htons(BROADCAST_SERVER_PORT);

    char response[512]={0};
    while(running){
        FD_ZERO(&watchset);
        FD_SET(sockfd,&watchset);     
		switch(select(sockfd+1,&watchset,NULL,NULL,NULL)){
            case -1:
            	ANNA_LOG(LOG_ERR,"select error\n");
                break;
            case 0:
                break;
            default:{
                struct broadcast_msg_head msg_head={0};
                memset( buf,0,BC_BUF_MAX_LEN);
                size  = recvfrom(sockfd, buf ,BC_BUF_MAX_LEN, 0,(struct sockaddr*)&addrcli, &addrlen); 
                if( size < 1 ){
                    continue;
                }
                ANNA_LOG(LOG_DBG,"[recv broadcast msg]:%s\n",buf);
                if (broadcast_msg_proc(&msg_head,buf,size) != 0)
                    continue;
                if(strcmp(msg_head.method,"1400") == 0){
                    struct ifreq ifr;
                    strcpy(ifr.ifr_name,"eth0");
                    if(ioctl(sockfd,SIOCGIFADDR,&ifr) != 0)
                    perror("ioctl");
                    //ANNA_LOG(LOG_DBG,"%s\n",(char *)inet_ntoa( ((struct sockaddr_in *)&(addrcli))->sin_addr));                 
                    sprintf(response,"{\"head\":{\"ukey\":\"%s\",\"method\":\"1400\",\"topic\":\"%s\",\"session\":\"%s\",\"ts\":\"%d\",\"ver\":\"%d\",\"code\":0},\"body\":{\"gateway\":\"%s\",\"host\":\"%s\"}}",msg_head.ukey,uuid,msg_head.session,time(0),1,uuid,(char *)inet_ntoa( ((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));
                    sendto (sockfd, response,strlen(response), 0,(struct sockaddr*)&addrcli,addrlen);
                    ANNA_LOG(LOG_DBG, "[send broadcast msg]:%s\n",response);
                    //memset(&msg_head.method[0],0,sizeof(msg_head));
                }
                memset(response,0,512);
                break;
            }
            }
        }
    return NULL;
}
int BroadcastModuleInit(void *param)
{
/*
    char *product_key=NULL,*device_name=NULL;
    char ipmode[20]={0};
    char ipaddr[20]={0};
    char mask[20]={0};
    char gateway[20]={0};
	char dns[20]={0};
    disk_get_system_paramter("ipmode",ipmode,19);
    disk_get_system_paramter("ipaddr",ipaddr,19);
    disk_get_system_paramter("mask",mask,19);
    disk_get_system_paramter("gateway",gateway,19);
    disk_get_system_paramter("dns",dns,19);
	
	if(strlen(dns) > 1 && strcmp(ipmode,"static") == 0){
		system("echo \"\" > /etc/resolv.conf");
		char cmdstr[100]={0};
		sprintf(cmdstr,"echo \"nameserver %s\" > /etc/resolv.conf",dns);
		system(cmdstr);
	}

    ipaddr_set("eth0",ipmode,ipaddr,mask,gateway);
    
    product_key = disk_get_product_key();
    device_name = disk_get_device_name();
    if(product_key == NULL && device_name== NULL ){
         DALITEK_PRINT("paramter error\n");
         return -1;
    }
    */
    //sprintf(uuid,"/%s/%s",product_key,device_name);
    sprintf(uuid,"%s",disk_get_uuid());
    broadcast_srv_init();
    running = true;
    if(pthread_create(&looping, NULL, Broadcast_looping, 0) != 0){
        ANNA_LOG(LOG_ERR,"mqtt thread create failed\n");
        return -1;
    }
    ANNA_LOG(LOG_INFO,"[ BroadcastModuleInit success! ]\n");
    return 0;
}
int BroadcastModuleExit(void *param)
{
    running = false;
    pthread_cancel(looping);
    pthread_join(looping,NULL);
    if(sockfd != -1){
    close(sockfd);
        sockfd = -1;
    }
    ANNA_LOG(LOG_INFO,"[ Broadcast exited! ]\n");	
    return 0;
}
