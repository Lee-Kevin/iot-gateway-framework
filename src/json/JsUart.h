#ifndef JS_MODULE_H
#define JS_MODULE_H
#include "uart.h"

#define UART_CONFIGURE_FILE     "./json/uart.json"

int JsUart_Open(const char *File);
int JsUart_Close(void);
int JsUart_Get_UartConfig_By_Id(int uartid,struct uart_config *uc);
int JsUart_Get_UartConfig_By_Index(int index,int *uartid,struct uart_config *uc);
int JsUart_Get_Uartid_By_Param(char *module,int *uartid);
int JsUart_Get_Uart_Count(void);
#endif
