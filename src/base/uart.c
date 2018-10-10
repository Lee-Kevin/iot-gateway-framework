#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "uart.h"
#include "JsUart.h"
uart_t g_uart[UART_MAX]={0};


static int SerialInit(uart_t *u)
{
    int ret=-1;
    u->uart_fd= open(u->config.uartstr, O_RDWR|O_NOCTTY);
    if (u->uart_fd < 0){
        printf("\nOpen Serial:%s failed\n",u->config.uartstr);
        return -1;
    }
    ret = SetSerialSpeed(u->uart_fd, u->config.baudrate);
    if(ret != 0){
        close(u->uart_fd);
        return ret;
    }
    ret = SetSerialAttribute(u->uart_fd, u->config.databits, u->config.stopbits, u->config.parity[0]);
    if(ret != 0){
        close(u->uart_fd);
        return ret;
    }
    fcntl(u->uart_fd,0);
    return 0;
}


void uart1_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=0;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}

void uart2_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=1;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}
void uart3_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=2;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}
void uart4_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=3;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}
void uart5_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=4;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}
void uart6_thread(void *Param)
{
    unsigned char buf[RB_UART_LEN_MAX];
    int i,len,flag,result;
    int send_trigger;

    const int uid=5;
    int ret = 0;
    send_trigger = 0;
    
    while(g_uart[uid].running){
        len = 0;
        if(g_uart[uid].uart_fd < 0){
            SerialInit(&g_uart[uid]);
            usleep(2000);
        }
        ret = read(g_uart[0].uart_fd, buf, 0x01);
        if(ret == 1){
            do{
                usleep(2000);
                len++;
                if(len < RB_UART_LEN_MAX){
                    ret = read(g_uart[uid].uart_fd, &buf[len], 0x01);
                }else{
                    ret = read(g_uart[uid].uart_fd, &buf[RB_UART_LEN_MAX-1], 0x01);
                }
            }while(ret == 1);

            if(len <= RB_UART_LEN_MAX){
                rt_ringbuffer_putchar(&g_uart[uid].rb_uart_rx,len+1);
                rt_ringbuffer_put(&g_uart[uid].rb_uart_rx,&buf[0],len);
                sem_post(&g_uart[uid].sem_uart_rx);
            }
            send_trigger = 10;//20ms 
        }else{
            if(send_trigger != 0){
                send_trigger--;
            }else{
                result = sem_trywait(&g_uart[uid].sem_uart_tx);
                if(result == 0){
                    rt_ringbuffer_get(&g_uart[uid].sem_uart_tx, buf, g_uart[uid].rb_uart_tx.buffer_ptr[g_uart[uid].rb_uart_tx.read_index]);
                    len = buf[0] - 1;
                    write(g_uart[uid].uart_fd, &buf[1], len);
                    send_trigger = 50;	//100ms
                }
            }
        }
        usleep(2000);
    }
    return ;
}




static int uart_x_init(int id,struct uart_config *uc)
{    
    g_uart[id].running = 0;
    g_uart[id].uart_fd = -1;
    sem_init(&g_uart[id].sem_uart_tx,0,0);
    sem_init(&g_uart[id].sem_uart_rx,0,0);

    rt_ringbuffer_init(&g_uart[id].rb_uart_tx,g_uart[id].rb_uart_tx_pool,RB_UART_TX_POOL_SIZE);
    rt_ringbuffer_init(&g_uart[id].rb_uart_rx,g_uart[id].rb_uart_rx_pool,RB_UART_RX_POOL_SIZE);

    snprintf(g_uart[id].config.uartstr,sizeof(g_uart[id].config.uartstr)-1,uc->uartstr);
    printf("######%s\n",g_uart[id].config.uartstr);
    g_uart[id].config.baudrate = uc->baudrate;
    g_uart[id].config.databits = uc->databits;
    g_uart[id].config.stopbits = uc->stopbits;
    g_uart[id].config.parity[0] = uc->parity[0];
    if(SerialInit(&g_uart[id]) < 0){
        sem_destroy(&g_uart[id].sem_uart_tx);
        sem_destroy(&g_uart[id].sem_uart_rx);
        return -1;
    }
    g_uart[id].running = 1;

    if(id = 0){
        g_uart[id].uart_thread_fun = uart1_thread;
    }else if(id = 1 && id < UART_MAX){
        g_uart[id].uart_thread_fun = uart2_thread;
    }else if(id = 2 && id < UART_MAX){
        g_uart[id].uart_thread_fun = uart3_thread;
    }else if(id = 3 && id < UART_MAX){
        g_uart[id].uart_thread_fun = uart4_thread;
    }else if(id = 4 && id < UART_MAX){
        g_uart[id].uart_thread_fun = uart5_thread;
    }else if(id = 5 && id < UART_MAX){
        g_uart[id].uart_thread_fun = uart6_thread;
    }
    if(g_uart[id].uart_thread_fun == NULL){
        sem_destroy(&g_uart[id].sem_uart_tx);
        sem_destroy(&g_uart[id].sem_uart_rx);
        g_uart[id].running = 0;
        return -1;
    }
    if(pthread_create(&g_uart[id].thread_id, NULL,(void *)g_uart[id].uart_thread_fun, 0) != 0){
        sem_destroy(&g_uart[id].sem_uart_tx);
        sem_destroy(&g_uart[id].sem_uart_rx);
        g_uart[id].running = 0;
        return -1;
    }
    return 0;
}




int UartInit(void *param)
{
    struct uart_config uc;
    int i;
    if(JsUart_Open(UART_CONFIGURE_FILE)!=0)
        return -1;
    int cnt = JsUart_Get_Uart_Count();
    if(cnt <= 0 || cnt >= UART_MAX){
        JsUart_Close();
        return -1;
    }
    for(i = 0;i<cnt;i++){
       int uartid=-1;
       if(JsUart_Get_UartConfig_By_Index(i,&uartid,&uc)==0){
            if(uartid <= 0 ||uartid > UART_MAX)
                continue;
            uart_x_init(uartid-1,&uc);
       }
    }
    JsUart_Close();
    return 0;
}

void UartUninit(void)
{
    int i;
    for(i = 0;i<UART_MAX;i++){
        if(g_uart[i].running != 0){
            sem_destroy(&g_uart[i].sem_uart_tx);
            sem_destroy(&g_uart[i].sem_uart_tx);
            g_uart[i].running = 0;
            pthread_cancel(g_uart[i].thread_id);
            pthread_join(g_uart[i].thread_id,NULL);
            if(g_uart[i].uart_fd != -1)
                close(g_uart[i].uart_fd);
        }
    }
}





