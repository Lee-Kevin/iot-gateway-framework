/*******************************************************************************
 * Author			: liang | work at ym.
 * Email			: 944922198@qq.com
 * Last modified	: 2017-01-23 15:18
 * Filename			: disk.c
 * Description		: 
 * *****************************************************************************/
#include "common.h"
#include "task_base.h"
#include "uart.h"
#include "disk.h"
//#include "JsModule.h"
int BaseModuleInit(void *param)
{
    int ret;
    ret = UartInit(NULL);
    if(ret < 0)
        return -1;
  //  ret = JsProject_Open("./json/project.json");
  //  if(ret < 0)
   //     return -1;
    ret = disk_init("db");
    if(ret < 0)
        return -1;
    return ret;
}
int BaseModuleExit(void *param)
{
    disk_exit();
   // JsProject_Close();
    UartUninit();
    return 0;
}
