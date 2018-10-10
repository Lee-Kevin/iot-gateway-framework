/*******************************************************************************
 * Author			: liang | work at ym.
 * Email			: 944922198@qq.com
 * Last modified	: 2017-01-23 15:18
 * Filename			: sh-queue.c
 * Description		: 
 * *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "queue.h"
#include "debug.h"
queue_t* queue_new(int size){
	queue_t *Q;
	Q = (queue_t *)malloc(sizeof(queue_t));
	if(Q == NULL)
		return NULL;
	Q->data = (Q_AnyValue*)calloc(size, sizeof(Q_AnyValue));
	if(Q->data == NULL){
		free(Q);
		return NULL;
	}
	Q->head = 0;
	Q->tail = 0;
	Q->size = size;
	Q->count = 0;
	pthread_mutex_init(&Q->locker, NULL);
	return Q;
}

void queue_delete(queue_t *Q){
	free(Q->data);
	pthread_mutex_destroy(&Q->locker);
	free(Q);
}

int queue_resize(queue_t *Q, int newsize){
	Q_AnyValue* data;
	int newtail,itail;
	
	if(Q->size >= newsize) //at least grow one item
		return -1;
	pthread_mutex_lock(&Q->locker);
	if(Q->size >= newsize){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	data = (Q_AnyValue*)realloc(Q->data,newsize*sizeof(Q_AnyValue));
	if(data == NULL){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	Q->data = data;
	//grow one or more space,maybe need move items.
	if(((Q->count > 0) && (Q->tail == Q->head)) || (Q->head > Q->tail)){
		for(newtail = Q->size, itail = 0; itail < Q->tail;){
			Q->data[newtail++] = Q->data[itail++];
			if(newtail >= newsize){
				newtail = 0;
			}
		}
		Q->tail = newtail;
	}
	Q->size = newsize;
	pthread_mutex_unlock(&Q->locker);

	return 0;
}

int queue_pop(queue_t *Q, Q_AnyValue*item){
	if(Q->count == 0)
		return -1;
	pthread_mutex_lock(&Q->locker);
	if(Q->count == 0){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	*item = Q->data[Q->head];
	Q->head ++;
	if(Q->head >= Q->size)
		Q->head = 0;
	Q->count --;
	pthread_mutex_unlock(&Q->locker);

	return 0;
}

int queue_push(queue_t *Q, Q_AnyValue item){
	if(Q->count == Q->size)
		return -1;
	pthread_mutex_lock(&Q->locker);
	if(Q->count == Q->size){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	Q->data[Q->tail] = item;
	Q->tail ++;
	if(Q->tail >= Q->size)
		Q->tail = 0;
	Q->count ++;
	pthread_mutex_unlock(&Q->locker);
	return 0;
}

int queue_head(queue_t *Q, Q_AnyValue*item){
	if(Q->count == 0)
		return -1;

	pthread_mutex_lock(&Q->locker);
	if(Q->count == 0){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	*item = Q->data[Q->head];
	pthread_mutex_unlock(&Q->locker);

	return 0;
}

int queue_tail(queue_t *Q, Q_AnyValue*item){
	if(Q->count == 0)
		return -1;
	int tail;
	pthread_mutex_lock(&Q->locker);
	if(Q->count == 0){
		pthread_mutex_unlock(&Q->locker);
		return -1;
	}
	tail = Q->tail;
	if(tail == 0)
		tail = Q->size;
	tail --;
	*item = Q->data[tail];
	pthread_mutex_unlock(&Q->locker);

	return 0;
}

int queue_getcount(queue_t *Q){
	int count;
	pthread_mutex_lock(&Q->locker);
	count = Q->count;
	pthread_mutex_unlock(&Q->locker);

	return count;
}

int queue_getsize(queue_t *Q){
	int size;
	pthread_mutex_lock(&Q->locker);
	size = Q->size;
	pthread_mutex_unlock(&Q->locker);

	return size;
}

int queue_gethead(queue_t *Q){
	int head;
	pthread_mutex_lock(&Q->locker);
	head = Q->head;
	pthread_mutex_unlock(&Q->locker);

	return head;
}

int queue_gettail(queue_t *Q){
	int tail;
	pthread_mutex_lock(&Q->locker);
	tail = Q->tail;
	pthread_mutex_unlock(&Q->locker);

	return tail;
}

void queue_print(queue_t *Q){
	int ihead,icount;
	pthread_mutex_lock(&Q->locker);
	printf("{\nhead:%d,tail=%d,cs=%d/%d\nQ:", Q->head,Q->tail,Q->count,Q->size);
	for(ihead=Q->head,icount=Q->count; icount; icount--){
		//printf("[%d:%X]",ihead, (int)Q->data[ihead]);
		ihead = (ihead+1) % Q->size;
	}
	//STDPRINT("\n}\n");
	pthread_mutex_unlock(&Q->locker);
}

void queue_printOccupy(queue_t *Q){
	pthread_mutex_lock(&Q->locker);
	//printf("[Queue load:%.1f%%]head:%d,tail=%d,cs=%d/%d.\n", Q->count*100.0/Q->size,Q->head,Q->tail,Q->count,Q->size);
    ANNA_LOG(LOG_DBG,"[Queue load:%.1f%%]head:%d,tail=%d,cs=%d/%d.\n", Q->count*100.0/Q->size,Q->head,Q->tail,Q->count,Q->size);
    pthread_mutex_unlock(&Q->locker);
}
