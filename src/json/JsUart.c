#include "cJSON.h"
#include <stdio.h>
#include "JsUart.h"

static cJSON *pJsUartRoot = NULL;
static char cJsFile[256] = ""; 

int JsUart_Open(const char *File)
{
    FILE *f;
    char *data;
    if(File == NULL)
        return -1;

    sprintf(cJsFile,"%s",File);
    f = fopen(cJsFile,"r");
    if(!f){
        return -1;
    }
    fseek(f,0,SEEK_END);
    int len = ftell(f);
    fseek(f,0,SEEK_SET);
    data = (char *)malloc(len+1);
    if(data == NULL){
    	fclose(f);
        return -1;
    }
    fread(data,1,len,f);
    fclose(f);

    pJsUartRoot = cJSON_Parse(data);
    free(data);
    if(!cJSON_IsArray(pJsUartRoot)){
        printf("pJsUartRoot is null\n");
        return -1;
    }
    return 0;
}
int JsUart_Close(void)
{
    if(pJsUartRoot) {
        cJSON_Delete(pJsUartRoot);
        pJsUartRoot = NULL;
    }
    return 0;
}
int JsUart_Get_UartConfig_By_Id(int uartid,struct uart_config *uc)
{
    if(!pJsUartRoot)
        return -1;
    cJSON *pJsUartItem,*pJsConfig,*pJsUartId,*pJsItem;
    pJsUartItem = pJsUartRoot->child;
    while(pJsUartItem){
        pJsUartId = cJSON_GetObjectItem(pJsUartItem,"Uartid");
        if(!cJSON_IsNumber(pJsUartId)){
            pJsUartItem = pJsUartItem->next;
            continue;
        }
        if(pJsUartId->valueint != uartid){
            pJsUartItem = pJsUartItem->next;
            continue;
        }    
        pJsConfig = cJSON_GetObjectItem(pJsUartItem,"Config");
        if(!cJSON_IsObject(pJsConfig)){
            break;
        }
        pJsItem = cJSON_GetObjectItem(pJsConfig,"Uartstr");
        if(!cJSON_IsString(pJsItem)){
            break;
        }
        snprintf(uc->uartstr,sizeof(uc->uartstr)-1,pJsItem->valuestring);
        pJsItem = cJSON_GetObjectItem(pJsConfig,"Baudrate");
        if(!cJSON_IsNumber(pJsItem)){
            break;
        }
        uc->baudrate = pJsItem->valueint;
        pJsItem = cJSON_GetObjectItem(pJsConfig,"Databits");
        if(!cJSON_IsNumber(pJsItem)){
            break;
        }
        uc->databits = pJsItem->valueint;
        pJsItem = cJSON_GetObjectItem(pJsConfig,"Parity");
        if(!cJSON_IsString(pJsItem)){
            break;
        }
        uc->parity[0] = (char)atoi(pJsItem->valuestring);
        pJsItem = cJSON_GetObjectItem(pJsConfig,"Stopbits");
        if(!cJSON_IsNumber(pJsItem)){
            break;
        }
        uc->stopbits = pJsItem->valueint;
        break;
    }
    return 0;
}

int JsUart_Get_UartConfig_By_Index(int index,int *uartid,struct uart_config *uc)
{
    if(!pJsUartRoot)
        return -1;
    cJSON *pJsUartItem,*pJsConfig,*pJsUartId,*pJsItem;
    pJsUartItem = pJsUartRoot->child;
    pJsUartItem = cJSON_GetArrayItem(pJsUartRoot,index);
    if(!cJSON_IsObject(pJsUartItem))
        return -1;
    
    pJsUartId = cJSON_GetObjectItem(pJsUartItem,"Uartid");
    if(!cJSON_IsNumber(pJsUartId)){
        return -1;
    }
    *uartid = pJsUartId->valueint;

    pJsConfig = cJSON_GetObjectItem(pJsUartItem,"Config");
    if(!cJSON_IsObject(pJsConfig)){
        return -1;
    }
    pJsItem = cJSON_GetObjectItem(pJsConfig,"Uartstr");
    if(!cJSON_IsString(pJsItem)){
       return -1;
    }
    snprintf(uc->uartstr,sizeof(uc->uartstr)-1,pJsItem->valuestring);
    pJsItem = cJSON_GetObjectItem(pJsConfig,"Baudrate");
    if(!cJSON_IsNumber(pJsItem)){
        return -1;
    }
    uc->baudrate = pJsItem->valueint;
    pJsItem = cJSON_GetObjectItem(pJsConfig,"Databits");
    if(!cJSON_IsNumber(pJsItem)){
        return -1;
    }
    uc->databits = pJsItem->valueint;
    pJsItem = cJSON_GetObjectItem(pJsConfig,"Parity");
    if(!cJSON_IsString(pJsItem)){
        return -1;
    }
    uc->parity[0] = (char)atoi(pJsItem->valuestring);
    pJsItem = cJSON_GetObjectItem(pJsConfig,"Stopbits");
    if(!cJSON_IsNumber(pJsItem)){
        return -1;
    }
    uc->stopbits = pJsItem->valueint;
    return 0;
}



int JsUart_Get_Uart_Count(void)
{
    if(!pJsUartRoot)
        return -1;
    return cJSON_GetArraySize(pJsUartRoot);
}
int JsUart_Get_Uartid_By_Param(char *module,int *uartid)
{
    if(!pJsUartRoot)
        return -1;
    
    cJSON *pJsUartItem,*pJsUartId=NULL,*pJsModule;
    pJsUartItem = pJsUartRoot->child;
    while(pJsUartItem){
        pJsModule = cJSON_GetObjectItem(pJsUartItem,"Module");
        if(!cJSON_IsString(pJsModule)){
            continue;
        }
        if(strcmp(module,pJsModule->valuestring) != 0)
            continue;
        
        pJsUartId = cJSON_GetObjectItem(pJsUartItem,"Uartid");
        if(!cJSON_IsNumber(pJsUartId)){
            return -1;
        }
        *uartid = pJsUartId->valueint;
        break;
    }
    if(pJsUartId == NULL)
        return -1;
    return 0;
}


