/*
 * hthread.c
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#include "hconfig.h"
#include "hthread.h"

#ifdef HAS_PTHREAD

typedef struct{
	pthread_t pthread;
	char cancel;
	hthread_callback_t callback;
	void *userData;
}thread_t;

static void * pthread_callback(void *userData){
	thread_t *thread = (thread_t *)userData;
	if(thread->callback){
		(*thread->callback)(thread->userData,InvokeTickRoot);
	}
	if(thread->cancel){
		free(thread);
	}
	else{
		thread->pthread = 0;
	}
	return NULL;
}

hthread_t hthread_create(hthread_callback_t callback,void *userData,InvokeTickDeclare){
	thread_t * thread = (thread_t *)mem_malloc(sizeof(thread_t));
	thread->pthread = 0;
	thread->cancel = 0;
	thread->callback = callback;
	thread->userData = userData;
	pthread_create(&thread->pthread, NULL, pthread_callback, thread);
	return (hthread_t)thread;
}

hint32 hthread_testcancel(hthread_t hthread){
	return hthread?((thread_t *)hthread)->cancel:0;
}

void hthread_cancel(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		thread->cancel = 1;
		if(thread->pthread == 0){
			free(thread);
		}
	}
}

void hthread_cancel_join(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		thread->cancel = 1;
		if(thread->pthread == 0){
			free(thread);
		}
		else{
			pthread_join(thread->pthread, NULL);
		}	
	}
}

void hthread_join(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		if(thread->pthread){
			pthread_join(thread->pthread, NULL);
			thread->pthread =0;
		}
	}
}

huintptr hthread_current_id(void){
	return (huintptr)pthread_self();
}

void hthread_yield(void){
	usleep(1);
}

hthread_key_t hthread_key_create( void (* dealloc)(void *)){
    pthread_key_t key = 0;
    pthread_key_create(&key, dealloc);
    return (hthread_key_t)(hintptr) key;
}

void * hthread_key_value(hthread_key_t key){
    return pthread_getspecific((hintptr) key);
}

void hthread_key_value_set(hthread_key_t key,void * value){
    pthread_setspecific((hintptr)key, value);
}

#endif


#ifdef __WIN32__


typedef struct{
	HANDLE handle;
	char cancel;
	hthread_callback_t callback;
	void *userData;
}thread_t;


static DWORD WINAPI win_thread_callback(LPVOID userData){
	thread_t * thread = (thread_t *) userData;
	if(thread->callback){
		(*thread->callback)(thread->userData,InvokeTickRoot);
	}
	if(thread->cancel){
		free(thread);
	}
	else{
		thread->handle = 0;
	}
	return 0;
}


hthread_t hthread_create(hthread_callback_t callback,void *userData,InvokeTickDeclare){
	thread_t * thread = mem_malloc(sizeof(thread_t));
	thread->cancel = 0;
	thread->callback = callback;
	thread->userData = userData;
	thread->handle = CreateThread(NULL,0,win_thread_callback,thread,0,0);
	return (hthread_t)thread;
}
	

hint32 hthread_testcancel(hthread_t hthread){
	return hthread?((thread_t *)hthread)->cancel:0;
}

void hthread_cancel(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		thread->cancel = 1;
		if(thread->handle == 0){
			free(thread);
		}
	}
}

void hthread_cancel_join(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		thread->cancel = 1;
		if(thread->handle == 0){
			free(thread);
		}
		else{
			WaitForSingleObject(thread->handle,INFINITE);
		}	
	}
}

void hthread_join(hthread_t hthread){
	if(hthread){
		thread_t * thread = (thread_t *)hthread;
		if(thread->handle){
			WaitForSingleObject(thread->handle,INFINITE);
			thread->handle =0;
		}
	}
}

hint32 hthread_current_id(){
        return GetCurrentThreadId();
}

void hthread_yield(){
	sleep(1);
}

#endif
