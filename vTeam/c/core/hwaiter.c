/*
 * hwaiter.c
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#include "hconfig.h"
#include "hwaiter.h"

#ifdef HAS_PTHREAD

typedef struct{
	pthread_mutex_t pmutex;
	pthread_cond_t pcond;
}waiter_t;

hwaiter_t hwaiter_alloc(void){
	waiter_t *waiter = (waiter_t *)malloc(sizeof(waiter_t));
	pthread_mutex_init(&waiter->pmutex, NULL);
	pthread_cond_init(&waiter->pcond, NULL);
	return (hwaiter_t)waiter;
}

void hwaiter_dealloc(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t *waiter = (waiter_t *)hwaiter;
		pthread_cond_destroy(&waiter->pcond);
		pthread_mutex_destroy(&waiter->pmutex);
		free(waiter);
	}
}
void hwaiter_lock(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t *waiter = (waiter_t *)hwaiter;
		pthread_mutex_lock(&waiter->pmutex);
	}
}
void hwaiter_unlock(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t *waiter = (waiter_t *)hwaiter;
		pthread_mutex_unlock(&waiter->pmutex);
	}
}
void hwaiter_wait(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t *waiter = (waiter_t *)hwaiter;
		pthread_cond_wait(&waiter->pcond, &waiter->pmutex);
	}
}
void hwaiter_join(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t *waiter = (waiter_t *)hwaiter;
		pthread_cond_broadcast(&waiter->pcond);
	}
}
#endif

#ifdef __WIN32__

typedef struct{
	HANDLE mutex;
	HANDLE event;
}waiter_t;

hwaiter_t hwaiter_alloc(){
	waiter_t * waiter = (waiter_t *)malloc(sizeof(waiter_t));
	waiter->mutex = CreateMutex(NULL,0,NULL);
	waiter->event = CreateEvent(NULL,0,0,NULL);
	return (hwaiter_t)waiter;
}

void hwaiter_dealloc(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t * waiter = (waiter_t *)hwaiter;
		CloseHandle(waiter->mutex);
		CloseHandle(waiter->event);
		free(waiter);
	}
}

void hwaiter_lock(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t * waiter = (waiter_t *)hwaiter;
		WaitForSingleObject(waiter->mutex,INFINITE);
	}
}

void hwaiter_unlock(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t * waiter = (waiter_t *)hwaiter;
		ReleaseMutex(waiter->mutex);
	}
}

void hwaiter_wait(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t * waiter = (waiter_t *)hwaiter;
		ReleaseMutex(waiter->mutex);
		ResetEvent(waiter->event);
		WaitForSingleObject(waiter->mutex,INFINITE);
	}
}
void hwaiter_join(hwaiter_t hwaiter){
	if(hwaiter){
		waiter_t * waiter = (waiter_t *)hwaiter;
		ReleaseMutex(waiter->mutex);
		SetEvent(waiter->event);
		WaitForSingleObject(waiter->mutex,INFINITE);
	}
}

#endif

