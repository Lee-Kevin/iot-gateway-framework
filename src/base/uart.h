#ifndef UART_H
#define UART_H
#include <pthread.h>
#include <semaphore.h>
#include "rt_ringbuffer.h"

#define UART_MAX                4

#define RB_UART_TX_POOL_SIZE        1024
#define RB_UART_RX_POOL_SIZE        1024
#define RB_UART_LEN_MAX                 254

struct uart_config
{
    char uartstr[16];
    int baudrate;
    int databits; 
    int stopbits;
    char parity[1];
};

typedef struct _uart
{
    int running;
    //int protocol_id;
    int uart_fd;
    struct uart_config config;
    pthread_t thread_id;
    sem_t sem_uart_tx;
    sem_t sem_uart_rx;
    struct rt_ringbuffer rb_uart_tx;
    struct rt_ringbuffer rb_uart_rx;
    unsigned char rb_uart_tx_pool[RB_UART_TX_POOL_SIZE];
    unsigned char rb_uart_rx_pool[RB_UART_TX_POOL_SIZE];
    void (*uart_thread_fun)(void *);
}uart_t;

extern struct _uart g_uart[UART_MAX];

int UartInit(void *param);
void UartUninit(void);

#endif
