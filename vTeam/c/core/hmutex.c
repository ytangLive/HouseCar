/*
 * hmutex.c
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#include "hconfig.h"
#include "hmutex.h"

#ifdef HAS_PTHREAD

typedef struct{
	pthread_mutex_t pmutex;
}mutex_t;

hmutex_t hmutex_alloc(void){
	mutex_t * mutex = (mutex_t *)malloc(sizeof(mutex_t));
	pthread_mutex_init(&mutex->pmutex, NULL);
	return (hmutex_t)mutex;
}
void hmutex_dealloc(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex ;
		pthread_mutex_destroy(&mutex->pmutex);
		free(mutex);
	}
}

void hmutex_lock(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex;
		pthread_mutex_lock(&mutex->pmutex);
	}
}

void hmutex_unlock(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex;
		pthread_mutex_unlock(&mutex->pmutex);
	}
}

#endif

#ifdef __WIN32__

typedef struct{
	HANDLE mutex;
}mutex_t;

hmutex_t hmutex_alloc(){
	mutex_t *mutex = (mutex_t *)malloc(sizeof(mutex_t));
	mutex->mutex = CreateMutex(NULL,0,NULL);
	return (hmutex_t)mutex;
}

void hmutex_dealloc(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex;
		CloseHandle(mutex->mutex);
		free(mutex);
	}
}

void hmutex_lock(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex;
		WaitForSingleObject(mutex->mutex,INFINITE);
	}
}

void hmutex_unlock(hmutex_t hmutex){
	if(hmutex){
		mutex_t * mutex = (mutex_t *)hmutex;
		ReleaseMutex(mutex->mutex);
	}
}



#endif

