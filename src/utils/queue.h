
#ifndef _SH_QUEUE_H_
#define _SH_QUEUE_H_

#include <pthread.h>
#include "stdint.h"

typedef void* Q_AnyValue;
typedef struct _queue_t{
	Q_AnyValue* data;
	int head;
	int tail;
	int size;
	int count;
	pthread_mutex_t locker;
}queue_t;

queue_t* queue_new(int size);
void queue_delete(queue_t *Q);
//need external mutex to avoid mutilthread call queue_resize synchronously
int queue_resize(queue_t *Q, int newsize);
int queue_pop(queue_t *Q, Q_AnyValue*item);
int queue_push(queue_t *Q, Q_AnyValue item);
int queue_head(queue_t *Q, Q_AnyValue*item);
int queue_tail(queue_t *Q, Q_AnyValue*item);

int queue_getcount(queue_t *Q);
int queue_getsize(queue_t *Q);

int queue_gethead(queue_t *Q);
int queue_gettail(queue_t *Q);

void queue_print(queue_t *Q);
void queue_printOccupy(queue_t *Q);
	
#endif
