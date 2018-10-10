#ifndef _SERILPUBLIC_H_
#define _SERILPUBLIC_H_


int SetSerialSpeed(int fd, int speed);
int SetSerialAttribute(int fd, int databits, int stopbits, int parity);

#endif

