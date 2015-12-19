/*
 *  queue.c
 *  C Library
 *
 *  Created by hailong zhang on 10-8-18.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hqueue.h"
#include "hmutex.h"

#undef queue_alloc
#undef queue_dealloc
#undef queue_enter
#undef queue_out
#undef queue_peek
#undef queue_number
#undef queue_remove

typedef struct{
	void *item;
	hint32 priority;
	void *next;
}QueueItem;

typedef struct{
	QueueItem *begin;
	hint32 number;
	hmutex_t locker;
}Queue ;

hqueue_t queue_alloc(InvokeTickDeclare){
	Queue *queue = (Queue *)mem_malloc( sizeof(Queue));
	queue->begin = NULL;
	queue->number = 0;
	queue->locker = hmutex_alloc();
	return (hqueue_t)queue;
}


void queue_dealloc(hqueue_t hQueue,InvokeTickDeclare){
	if(hQueue){
		Queue *queue = (Queue *)hQueue;
		QueueItem *item  ,*n = NULL;
		
		hmutex_lock(queue->locker);
		item = queue->begin;
		while(item){
			n = (QueueItem *)item->next;
			mem_free(item);
			item = n;
		}
		hmutex_unlock(queue->locker);
		hmutex_dealloc(queue->locker);
		mem_free(queue);
	}
}

void queue_enter(hqueue_t hQueue,void *item,hint32 priority,InvokeTickDeclare){
	if(hQueue && item){
		Queue *queue = (Queue *)hQueue;
		QueueItem *queueItem;
		QueueItem * i,*p;
		hmutex_lock(queue->locker);
		queueItem = (QueueItem *)mem_malloc( sizeof(QueueItem));
		queueItem->next = NULL;
		queueItem->item = item;
		queueItem->priority = priority;
		
		p = NULL;
		i = queue->begin;
		while(i && i->priority >= priority){
			p = i;
			i = i->next;
		}
		
		if(p){
			queueItem->next = p->next;
			p->next = queueItem;
		}
		else{
			queueItem->next = queue->begin;
			queue->begin = queueItem;
		}
		
		queue->number ++;
		hmutex_unlock(queue->locker);
	}
}

void * queue_out(hqueue_t hQueue,InvokeTickDeclare){
	void *item = NULL;
	if(hQueue){
		Queue *queue = (Queue *)hQueue;
		hmutex_lock(queue->locker);
		if(queue->begin){
			QueueItem *queueItem = queue->begin;
			queue->begin = queueItem->next;
			queue->number --;
			item = queueItem?queueItem->item:NULL;
			mem_free(queueItem);
		}
		hmutex_unlock(queue->locker);
	}
	return item;
}

void * queue_peek(hqueue_t hQueue,InvokeTickDeclare){
	void *item = NULL;
	if(hQueue){
		Queue *queue = (Queue *)hQueue;
		hmutex_lock(queue->locker);
		if(queue->begin){
			QueueItem *queueItem = queue->begin;
			item = queueItem?queueItem->item:NULL;
		}
		hmutex_unlock(queue->locker);
	}
	return item;	
}

hint32 queue_number(hqueue_t hQueue,InvokeTickDeclare){
	hint32 number = 0;
	if(hQueue){
		Queue *queue = (Queue *)hQueue;
		hmutex_lock(queue->locker);
		number = queue->number;
		hmutex_unlock(queue->locker);
	}
	return number;
}

void queue_remove(hqueue_t hqueue,queue_filter_t filter,hany dest,InvokeTickDeclare){
	if(hqueue && filter){
		Queue *queue = (Queue *)hqueue;
		QueueItem *item ,*p = NULL;
		
		hmutex_lock(queue->locker);
		item = queue->begin;
		while(item){
			if((*filter)(hqueue,item->item,dest,InvokeTickArg)){
				if(p){
					p->next = item->next;
					mem_free( item);
					item = p->next;
				}
				else{
					queue->begin = item->next;
					mem_free( item);
					item = queue->begin;
					p = NULL;
				}
				continue;
			}
			p = item;
			item = item->next;
		}
		hmutex_unlock(queue->locker);
	}
}
