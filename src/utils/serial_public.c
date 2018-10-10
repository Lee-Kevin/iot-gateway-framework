/****************************************************************************
Filename:
Description:
Author:
Date:
Modified History:
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

static int speed_arr[] = {B230400, B115200, B57600, B38400, B19200, \
                          B9600, B4800, B2400, B1800, B1200, B600, B300
                         };
static int name_arr[]  = {230400,  115200,  57600,  38400,  19200,  \
                          9600,  4800,  2400,  1800,  1200,  600,  300
                         };
int SetSerialSpeed(int fd, int speed)
{
    int   i;
    int   status;
    struct termios   Opt;
    
    tcgetattr(fd, &Opt);
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0)
            {
                perror("tcsetattr fd");
                return -1;
            }
            tcflush(fd,TCIOFLUSH);
            return 0;
        }
    }
    return -1;
}

int SetSerialAttribute(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr,"Unsupported data size\n");
        return -1;
    }
    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
    case 1:
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
    case 2:
        options.c_cflag |= PARENB;     /* Enable parity */
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;      /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
    case 0:
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return -1;
    }
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported stop bits\n");
        return -1;
    }
    /* Set input parity option */
    if (parity != 'n')
    {
        options.c_iflag |= INPCK;
    }
    options.c_cflag |= (CLOCAL|CREAD);
    options.c_iflag &=~(IXON | IXOFF | IXANY);
    options.c_iflag &=~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag |= ISIG;
    tcflush(fd,TCIFLUSH);
    options.c_oflag = 0;
    //options.c_lflag = 0;
    //options.c_cc[VTIME] = 15;                        // delay 15 seconds
    //options.c_cc[VMIN] = 0;                       // Update the options and do it NOW
    options.c_cc[VTIME] = 0;                       
    options.c_cc[VMIN] = 0;                      
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        return -1;
    }
    fcntl(fd, F_SETFL, 0);  // ижии??б┴ииии??бъи║?
    return 0;
}

