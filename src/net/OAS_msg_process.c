/*Office Automation System*/
/*Home Automation System*/
/*Public Places Automation System*/

/*家庭自动化与办公自动化通常是一致的,
公共场所的用户管理方面相对开发（主要是人为设备触发，智能终端较少）*/

/*协议中指定版本，如果是ver=1234,则版本是(ver/1000)*/
#include "OAS_msg_process.h"
#include "cJSON.h"
#include "common.h"
#include "manager.h"
#include "debug.h"
#include "version.h"
#include "disk.h"
#include "curl/curl.h"
#include "openssl/md5.h"
#define VER_OAS_ID 1    /*办公版本*/

#define R_ROOT       1
#define R_ADMIN     2
#define R_USER       3
#define OUTPUT_BUF_MAX 8196
typedef struct __OAS_MsgCmd{
    char    *pCmd;
    char    rights;
    int    (*net2gw)(struct msg_head *h,unsigned char mod,char *,char *,int );
}OAS_MsgCmd;


static int proc_0100_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0101_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0102_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0103_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0104_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0105_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0106_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0107_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0108_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_0109_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_010A_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_010B_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);


static int proc_1000_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1001_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1002_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1003_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1004_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1005_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1006_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1007_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1008_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1009_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_100A_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_100B_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_100C_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);

static int proc_1100_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1101_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1102_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1103_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1104_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1105_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1106_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);

static int proc_1180_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1181_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1182_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1183_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1184_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1185_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);

static int proc_1200_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1201_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1202_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1203_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1204_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1205_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1206_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1207_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);

static int proc_1300_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1301_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1302_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1303_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1304_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);

static int proc_1400_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1401_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1402_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1403_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);
static int proc_1404_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen);


static OAS_MsgCmd RegOASMsgCmd[]=
{
    /*系统接口*/
    {"0100",(R_ROOT | R_ADMIN),			proc_0100_msg},	/*重启主机*/
    {"0101",(R_ROOT | R_ADMIN),			proc_0101_msg},	/*重启主机应用程序*/
    {"0102",(R_ROOT | R_ADMIN | R_USER),proc_0102_msg},	/*获取主机版本信息*/
    {"0103",(R_ROOT | R_ADMIN | R_USER),proc_0103_msg},	/*获取主机更新信息*/
    {"0104",(R_ROOT | R_ADMIN | R_USER),proc_0104_msg},	/*更新主机系统时间*/
    {"0105",(R_ROOT | R_ADMIN),			proc_0105_msg},	/*更新主机应用程序*/
    {"0106",(R_ROOT | R_ADMIN | R_USER),proc_0106_msg},	/*获取应用程序配置文件版本*/
    {"0107",(R_ROOT | R_ADMIN | R_USER),proc_0107_msg},	/*更新应用程序配置文件*/
    {"0108",(R_ROOT | R_ADMIN),			proc_0108_msg},	/*设置主机网络*/
    {"0109",(R_ROOT | R_ADMIN),			proc_0109_msg},	/*切换主机工作模式：正常工作模式<-->调试工作模式*/
	{"010A",(R_ROOT | R_ADMIN),			proc_010A_msg},	/*广播接口*/
	{"010B",(R_ROOT | R_ADMIN),			proc_010B_msg},	/*修改主机名称*/
	
    /*用户接口*/
    {"1000",(R_ROOT | R_ADMIN | R_USER),proc_1000_msg},	/*用户注册*/
    {"1001",(R_ROOT | R_ADMIN | R_USER),proc_1001_msg}, /*用户注销*/
	{"1002",(R_ROOT | R_ADMIN | R_USER),proc_1002_msg},	/*用户绑定主机*/
    {"1003",(R_ROOT | R_ADMIN | R_USER),proc_1003_msg}, /*用户解绑主机*/
	{"1004",(R_ROOT | R_ADMIN | R_USER),proc_1004_msg},	/*登陆后获取绑定主机列表*/
    {"1005",(R_ROOT | R_ADMIN | R_USER),proc_1005_msg},	/*用户登陆*/
    {"1006",(R_ROOT | R_ADMIN | R_USER),proc_1006_msg},	/*用户退出*/
    {"1007",(R_ROOT | R_ADMIN | R_USER),proc_1007_msg},	/*修改密码*/
    {"1008",(R_ROOT | R_ADMIN),			proc_1008_msg},	/*重置密码*/
    {"1009",(R_ROOT | R_ADMIN),			proc_1009_msg},	/*获取用户列表*/
    {"100A",(R_ROOT | R_ADMIN | R_USER),proc_100A_msg},	/*查看用户权限等级*/
	{"100B",(R_ROOT | R_ADMIN),			proc_100B_msg},	/*管理员申请临时用户，填写信息或随机生成用户名及密码，分享用户名密码或二维码*/
    {"100C",(R_ROOT	),					proc_100C_msg},	/*管理员转让权限申请*/

    /*区域及设备接口*/
    {"1100",(R_ROOT | R_ADMIN),			proc_1100_msg},	/*添加区域*/
    {"1101",(R_ROOT | R_ADMIN),			proc_1101_msg},	/*删除区域*/
    {"1102",(R_ROOT | R_ADMIN),			proc_1102_msg},	/*修改区域名*/
    {"1103",(R_ROOT | R_ADMIN),			proc_1103_msg},	/*区域添加用户及权限*/
    {"1104",(R_ROOT | R_ADMIN),			proc_1104_msg},	/*管理员区域区域用户及权限*/
    {"1105",(R_ROOT | R_ADMIN | R_USER),proc_1105_msg},	/*查看区域设备*/
    {"1106",(R_ROOT | R_ADMIN | R_USER),proc_1106_msg},	/*控制区域设备*/

    {"1180",(R_ROOT | R_ADMIN),			proc_1180_msg},	/*添加设备*/
    {"1181",(R_ROOT | R_ADMIN),			proc_1181_msg},	/*删除设备*/
    {"1182",(R_ROOT | R_ADMIN),			proc_1182_msg},	/*修改设备名*/
    {"1183",(R_ROOT | R_ADMIN),			proc_1183_msg},	/*设备添加用户权限*/
    {"1184",(R_ROOT | R_ADMIN | R_USER),proc_1184_msg},	/*查看设备，普通用户需查看权限*/
    {"1185",(R_ROOT | R_ADMIN | R_USER),proc_1185_msg},	/*控制设备，普通用户需控制权限*/

    /*场景接口*/    
    {"1200",(R_ROOT | R_ADMIN),			proc_1200_msg},	/*添加场景*/
    {"1201",(R_ROOT | R_ADMIN),			proc_1201_msg},	/*删除场景*/
    {"1202",(R_ROOT | R_ADMIN),			proc_1202_msg},	/*修改场景名*/
    {"1203",(R_ROOT | R_ADMIN),			proc_1203_msg},	/*添加场景任务*/
    {"1204",(R_ROOT | R_ADMIN),			proc_1204_msg},	/*删除场景任务*/
    {"1205",(R_ROOT | R_ADMIN),			proc_1205_msg},	/*修改场景任务（名称和设备指令等）*/
    {"1206",(R_ROOT | R_ADMIN | R_USER),proc_1206_msg},	/*获取场景信息*/
    {"1207",(R_ROOT | R_ADMIN | R_USER),proc_1207_msg},	/*场景控制（有控制权限）*/

    /*定时接口*/
    {"1300",(R_ROOT | R_ADMIN),			proc_1300_msg},	/*添加定时*/
    {"1301",(R_ROOT | R_ADMIN),			proc_1301_msg},	/*删除定时*/
    {"1302",(R_ROOT | R_ADMIN),			proc_1302_msg},	/*修改定时*/
    {"1303",(R_ROOT | R_ADMIN | R_USER),proc_1303_msg},	/*获取定时*/
    {"1304",(R_ROOT | R_ADMIN | R_USER),proc_1304_msg},	/*定时使能*/

    /*联动接口*/
    {"1400",(R_ROOT | R_ADMIN),			proc_1400_msg},	/*添加联动*/
    {"1401",(R_ROOT | R_ADMIN),			proc_1401_msg},	/*删除联动*/
    {"1402",(R_ROOT | R_ADMIN),			proc_1402_msg},	/*修改联动*/
    {"1403",(R_ROOT | R_ADMIN | R_USER),proc_1403_msg},	/*获取联动*/
    {"1404",(R_ROOT | R_ADMIN | R_USER),proc_1404_msg}	/*联动使能*/
};

static size_t curl_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}
static int curl_progress_func(char *progress_data,  
                     double t, /* dltotal */  
                     double d, /* dlnow */  
                     double ultotal,  
                     double ulnow)  
{  
  printf("%s %g / %g (%g %%)\n", progress_data, d, t, d*100.0/t);  
  return 0;  
}  

static int curl_download_application(const char *url,char *savefile,int report_progress)
{
	CURL *curl;
	CURLcode res;
	FILE *outfile;
	char *progress_data="#";
	
	curl = curl_easy_init();
	if(curl){
		outfile = fopen(savefile,"wb");
		curl_easy_setopt(curl,CURLOPT_URL,url);
		curl_easy_setopt(curl,CURLOPT_WRITEDATA,outfile);
		curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,curl_write_func);
		curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
		if(report_progress){
			curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,curl_progress_func);
			curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,progress_data);
		}
		res = curl_easy_perform(curl);
		fclose(outfile);
		curl_easy_cleanup(curl);
	}else
		return -1;
	return 0;
}

static int file_md5sum(const char *filename,unsigned char out_md5[],int len)
{
	MD5_CTX ctx;
	unsigned char outmd[16]={0};
	unsigned char md5str[33]={'\0'};
	unsigned char tmpstr[3]={0};
	char buffer[1024];
	int i;
	int buf_len;
	FILE *fp=NULL;
	fp = fopen(filename,"rb");
	if(fp == NULL){
		return -1;
	}
	MD5_Init(&ctx);
	while((buf_len=fread(buffer,1,1024,fp))>0){
		MD5_Update(&ctx,buffer,buf_len);
		memset(buffer,0,sizeof(buffer));
	}
	MD5_Final(outmd,&ctx);
	for(i = 0;i < 16;i++){
		sprintf(tmpstr,"%02X",outmd[i]);
		strncat(md5str,tmpstr,2);
	}
	strncpy(out_md5,md5str,33);
	return 0;
}


static int proc_0100_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char ts[20]={0};
    struct timeval current;
    gettimeofday(&current, NULL);
    sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
    if(mod == MOD_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
    else if(mod == MOD_LOCAL_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
    SendMsg2Manager(APPEXIT,NULL,0);
    return err_code;	
}
static int proc_0101_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char ts[20]={0};
    struct timeval current;
    gettimeofday(&current, NULL);
    sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
    if(mod == MOD_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
    else if(mod == MOD_LOCAL_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
    SendMsg2Manager(SYSREBOOT,NULL,0);
    return err_code;	
}

static int proc_0102_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
	if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char ts[20]={0};
    struct timeval current;
    gettimeofday(&current, NULL);
    sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
    if(mod == MOD_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"software\":\"%s\"}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,SOFTWARE_VERSION);
    else if(mod == MOD_LOCAL_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"software\":\"%s\"}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,SOFTWARE_VERSION);
    return err_code;
}

static int proc_0103_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
	if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char ts[20]={0};
    struct timeval current;
    gettimeofday(&current, NULL);
    sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
    if(mod == MOD_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"update_info\":\"%s\"}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,UPDATE_INFO);
    else if(mod == MOD_LOCAL_MQTT_ID)
        snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"update_info\":\"%s\"}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,UPDATE_INFO);
    return err_code;
}

static int proc_0104_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
	if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char cmd[64] = {0};

	cJSON *pRoot,*pDateTime,*pBody;
    pRoot = cJSON_Parse(msg);
    if(pRoot == NULL){
        return ERR_JSON;
    }
	do{
		pBody = cJSON_GetObjectItem(pRoot,"body");
		if(!cJSON_IsObject(pBody)){
			err_code = ERR_BODY;
			break;
		}
		pDateTime = cJSON_GetObjectItem(pBody,"datetime");
		if(!cJSON_IsString(pDateTime)){
			err_code = ERR_FIELD;
			break;
		}
		snprintf(cmd,sizeof(cmd)-1,"echo 1 | sudo -S date -s \"%s\"",pDateTime->valuestring);
		system(cmd);
		char ts[20]={0};
    	struct timeval current;
    	gettimeofday(&current, NULL);
    	sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
    	if(mod == MOD_MQTT_ID)
        	snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
    	else if(mod == MOD_LOCAL_MQTT_ID)
        	snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS);
	}while(0);
	cJSON_Delete(pRoot);
	return err_code;
}

/*application update*/
static int proc_0105_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
	if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
	int progress=0;
	char url[512] = {0};
	unsigned char p_md5[64] = {0};
	unsigned char c_package_md5[33]={'\0'};
	cJSON *pRoot,*pBody,*pItem;
    pRoot = cJSON_Parse(msg);
    if(pRoot == NULL){
        return ERR_JSON;
    }
	do{
		pBody = cJSON_GetObjectItem(pRoot,"body");
		if(!cJSON_IsObject(pBody)){
			err_code = ERR_BODY;
			break;
		}
		pItem = cJSON_GetObjectItem(pBody,"ver");
		if(!cJSON_IsString(pItem)){
			err_code = ERR_FIELD;
			break;
		}
		if(!strcmp(SOFTWARE_VERSION,pItem->valuestring)){
			err_code = ERR_VERSION;
			break;
		}
		pItem = cJSON_GetObjectItem(pBody,"url");
		if(!cJSON_IsString(pItem)){
			err_code = ERR_FIELD;
			break;
		}
		snprintf(url,511,"%s",pItem->valuestring);
		pItem = cJSON_GetObjectItem(pBody,"package_md5");
		if(!cJSON_IsString(pItem)){
			err_code = ERR_FIELD;
			break;
		}
		snprintf(p_md5,63,"%s",pItem->valuestring);
		pItem = cJSON_GetObjectItem(pBody,"app_md5");
		if(!cJSON_IsString(pItem)){
			err_code = ERR_FIELD;
			break;
		}
		pItem = cJSON_GetObjectItem(pBody,"progress");
		if(cJSON_IsNumber(pItem)){
			progress = pItem->valueint;
		}
		if(curl_download_application(url,"bin.tar.gz",progress)<0){
			err_code = ERR_DOWNLOAD;
			break;
		}
		file_md5sum("bin.tar.gz",c_package_md5,33);
		system("rm /tmp/bin -rf");
		system("tar -zxvf bin.tar.gz -C /tmp");
		
	}while(0);
	cJSON_Delete(pRoot);
    return err_code;
}

static int proc_0106_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
	int ver=-1;
    char ts[20]={0};
    struct timeval current;
    gettimeofday(&current, NULL);
    sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
	do{
		//ver = JsProjectGetVersion();
		ver = 1;
		if(ver < 0){
			err_code = ERR_VERSION;
			break;
		}
		if(mod == MOD_MQTT_ID)
        	snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"project_version\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,ver);
    	else if(mod == MOD_LOCAL_MQTT_ID)
        	snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"project_version\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,ver);
	}while(0);
 	return err_code;
}

static int proc_0107_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_0108_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_0109_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_010A_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_010B_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1000_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
   /* {"head":{"ukey":"","ver":"","session":"","method":"1000","topic":"","ts":"","code":0},
   "body":{"username":"","passwd":"","tel":"","email":"","i.d.card":""}}*/
    if(h== NULL || msg== NULL || out == NULL)
        return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    char user_name[33]={0};
    char user_passwd[17]={0};
    char user_tel[12]={0};
    char user_email[64]={0};
    char user_idcard[19]={0};
    cJSON *pRoot,*pBody,*pItem;
    pRoot = cJSON_Parse(msg);
    if(pRoot == NULL){
        return ERR_JSON;
    }
    do{
        pBody = cJSON_GetObjectItem(pRoot,"body");
        if(! cJSON_IsObject(pBody)){
            err_code = ERR_BODY;
            break;
        }
        pItem = cJSON_GetObjectItem(pBody,"username");
        if(!cJSON_IsString(pItem)){
            err_code = ERR_FIELD;
            break;
        }
        strncpy(user_name,pItem->valuestring,sizeof(user_name)-1);
        
        pItem = cJSON_GetObjectItem(pBody,"passwd");
        if(!cJSON_IsString(pItem)){
            err_code = ERR_FIELD;
            break;
        }
        strncpy(user_passwd,pItem->valuestring,sizeof(user_passwd)-1);

        pItem = cJSON_GetObjectItem(pBody,"tel");
        if(!cJSON_IsString(pItem)){
            err_code = ERR_FIELD;
            break;
        }
        strncpy(user_tel,pItem->valuestring,sizeof(user_tel)-1);

        pItem = cJSON_GetObjectItem(pBody,"email");
        if(!cJSON_IsString(pItem)){
            err_code = ERR_FIELD;
            break;
        }
        strncpy(user_email,pItem->valuestring,sizeof(user_email)-1);

        pItem = cJSON_GetObjectItem(pBody,"i.d.card");
        if(!cJSON_IsString(pItem)){
            err_code = ERR_FIELD;
            //break;
        }
        strncpy(user_idcard,pItem->valuestring,sizeof(user_idcard)-1);

        //write info to db
        //load info to user module
    }while(0);

    
    return 0;
}

static int proc_1001_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1002_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1003_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1004_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1005_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1006_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1007_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1008_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1009_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_100A_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_100B_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_100C_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


static int proc_1100_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    if(h== NULL || msg== NULL || out == NULL)
            return ERR_PARAMTER;
    int err_code = ERR_SUCCESS;
    int ver=-1;
    char ts[20]={0};
    cJSON *pRoot,*pBody,*pFloor,*pRoom;

    pRoot = cJSON_Parse(msg);
    if(pRoot == NULL){
        return ERR_JSON;
    }
    do{
        pBody = cJSON_GetObjectItem(pRoot,"body");
        if(!cJSON_IsObject(pBody)){
            err_code = ERR_BODY;
            break;
        }
        pFloor = cJSON_GetObjectItem(pBody,"floorid");
        if(!cJSON_IsString(pFloor)){
            err_code = ERR_FIELD;
            break;
        }
        struct timeval current;
        gettimeofday(&current, NULL);
        sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
        if(mod == MOD_MQTT_ID)
            snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"project_version\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,ver);
        else if(mod == MOD_LOCAL_MQTT_ID)
            snprintf(out,outlen,"{\"head\":{\"ukey\":\"%s\",\"ver\":1,\"session\":\"%s\",\"method\":\"%s\",\"topic\":\"%s\",\"ts\":\"%s\",\"code\":%d},\"body\":{\"project_version\":%d}}",h->ukey,h->session,h->method,uuid,ts,ERR_SUCCESS,ver);
    }while(0);
    return err_code;
}

static int proc_1101_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1102_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1103_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1104_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1105_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1106_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


static int proc_1180_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1181_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1182_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1183_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1184_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


static int proc_1185_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    if(h== NULL || msg== NULL || out == NULL || outlen < OUTPUT_BUF_MAX)
        return ERR_PARAMTER;
    
    int err_code = ERR_SUCCESS;
    cJSON *root,*pItem,*pObj;
    DEVDPCTRLMSG devdpctrl={0};
    char *str = NULL;
    root = cJSON_Parse(msg);
    if(root == NULL){
        return ERR_JSON;
    }
    do{
        pObj = cJSON_GetObjectItem(root,"body");
        if(!pObj){
                        err_code = ERR_BODY;
            break;
        }
        pItem = cJSON_GetObjectItem(pObj,"devid");
        if(pItem && pItem->type == cJSON_Number){
                    devdpctrl.DesDevId= pItem->valueint;
        }else{
                   err_code = ERR_FIELD;
                   break;
        }
        pItem = cJSON_GetObjectItem(pObj,"dpid");
        if(pItem && pItem->type == cJSON_Number){
            devdpctrl.DesDpId = pItem->valueint;
        }else{
                     err_code = ERR_FIELD;
                     break;
               }

                pItem = cJSON_GetObjectItem(pObj,"data");
		if(pItem && pItem->type == cJSON_String){
                    if(strlen(pItem->valuestring)>(MAX_DATA_LEN-1)){
                        ANNA_LOG(LOG_ERR,"body data length error\n");
                        err_code =  ERR_FIELD;
                        break;
                    }else{
                        if(strlen(pItem->valuestring) == 0)
                            strcpy(devdpctrl.Data,"\"\"");
                        else
                            strcpy(devdpctrl.Data,pItem->valuestring);
                    }
		}else if(pItem && pItem->type == cJSON_Object){
		    str = cJSON_Print(pItem);
                    if(str){
		        strcpy(devdpctrl.Data,str);
                        free(str);
                    }else{
                        err_code =  ERR_FIELD;
                        break;
                    }
                        
                }else{
		    ANNA_LOG(LOG_ERR,"body data error\n");
                    err_code =  ERR_FIELD;
                    break;
                }
	}while(0);
        devdpctrl.SrcDevId = mod<<24;
        devdpctrl.SrcDpId = 0;
        char ts[20]={0};
        struct timeval current;
        gettimeofday(&current, NULL);
        sprintf(ts,"%d%03d",current.tv_sec,current.tv_usec/1000);
        //strrev(ts);
        if(mod == MOD_MQTT_ID)
            snprintf(out,outlen,"{\"head\":{\"method\":\"%s\",\"topic\":\"%s\",\"fromtopic\":\"\",\"ts\":\"%s\",\"ErrorCode\":%d}}",h->method,h->topic,ts,ERR_SUCCESS);
        else if(mod == MOD_LOCAL_MQTT_ID)
            snprintf(out,outlen,"{\"head\":{\"method\":\"%s\",\"topic\":\"%s\",\"fromtopic\":\"\",\"ts\":\"%s\",\"ErrorCode\":%d}}",h->method,ts,ERR_SUCCESS);
        cJSON_Delete(root);
        ANNA_LOG(LOG_DBG,"SendMsg2Main");
        if(err_code == ERR_SUCCESS)
            SendMsg2Manager(DEVDPCTRL, (char*)&devdpctrl, sizeof(devdpctrl));
        return err_code;
    return ERR_SUCCESS;
}

static int proc_1200_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1201_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1202_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1203_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1204_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1205_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1206_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1207_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


static int proc_1300_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1301_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1302_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1303_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1304_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


static int proc_1400_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1401_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1402_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1403_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}

static int proc_1404_msg(struct msg_head *h,int mod,char *msg,char *out,int outlen)
{
    return 0;
}


int mqtt2gw_OAS_msg_head_parse(struct msg_head *h,char *str)
{
    cJSON *json,*pItem,*pObj;
    if(h==NULL || str==NULL)
        return -1;
    json = cJSON_Parse(str);
    if(json){
        pObj = cJSON_GetObjectItem(json,"head");
        if(!pObj){
            ANNA_LOG(LOG_ERR,"head error\n");
            return ERR_HEAD;
        }
        pItem = cJSON_GetObjectItem(pObj,"ukey");
        if(cJSON_IsString(pItem)){
            strncpy(h->ukey,pItem->valuestring,sizeof(h->ukey)-1);
        }else{
            cJSON_Delete(json);
            return ERR_UKEY;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"ver");
        if(cJSON_IsNumber(pItem)){
            h->ver = pItem->valueint;
        }else{
            cJSON_Delete(json);
            return ERR_VERSION;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"method");
        if(cJSON_IsString(pItem)){
            strncpy(h->method,pItem->valuestring,sizeof(h->method)-1);
        }else{
            cJSON_Delete(json);
            return ERR_METHOD;
        }
        pItem = cJSON_GetObjectItem(pObj,"topic");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->topic,pItem->valuestring,sizeof(h->topic)-1);
        }else{
            cJSON_Delete(json);
            return ERR_TOPIC;
        }
        pItem = cJSON_GetObjectItem(pObj,"session");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->session,pItem->valuestring,sizeof(h->session)-1);
        }else{
            cJSON_Delete(json);
            return ERR_SESSION;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"ts");
        if(pItem && pItem->type == cJSON_String){
            strncpy(h->ts,pItem->valuestring,sizeof(h->ts)-1);
        }else{
            cJSON_Delete(json);
            return ERR_TS;
        }
        
        pItem = cJSON_GetObjectItem(pObj,"code");
        if(pItem && pItem->type == cJSON_Number){
            h->code= pItem->valueint;
        }else{
            cJSON_Delete(json);
            return ERR_FIELD;
        }

        cJSON_Delete(json);
        return ERR_SUCCESS;
    }else{
        //print error
    }
    return ERR_JSON;
}



int mqtt2gw_OAS_msg_proc(int mid,struct msg_head *st,int mod,char *msg,char *out,int outlen)
{
    if(st == NULL || msg == NULL)
        return ERR_PARAMTER;
    int i;
    struct msg_head *pHead=st;
	if(pHead == NULL)
		return ERR_PARAMTER;
    int rt = ERR_SUCCESS;
    for(i = 0;i < (sizeof(RegOASMsgCmd)/sizeof(OAS_MsgCmd)); i++) {
        if(strcmp(pHead->method,RegOASMsgCmd[i].pCmd))
            continue;

		/*接口权限过滤*/
		/*if(!(RegOASMsgCmd[i].rights & (user_rights))){
			//反馈权限提示
			rt=ERR_RIGHTS;
			break;
		}*/

        rt = RegOASMsgCmd[i].net2gw(pHead,mod,msg,out,outlen);
        break;
    }
    if(i == (sizeof(RegOASMsgCmd)/sizeof(OAS_MsgCmd))){
        //DALITEK_PRINT("This method is not yet supported\n");
        rt = ERR_METHOD;
    }
        
    return rt;
}



/*上报普通设备消息到网络*/
int report_1A00_msg(int mod,char *msg)
{
    return 0;
}
/*上报安防设备消息到网络*/
int report_1A01_msg(int mod,char *msg)
{
    return 0;
}



