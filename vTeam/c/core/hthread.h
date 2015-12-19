/*
 * hthread.h
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#ifndef HTHREAD_H_
#define HTHREAD_H_

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hthread_t)
    HANDLER(hthread_key_t)
	
	typedef void (*hthread_callback_t)(void *userData,InvokeTickDeclare);

	hthread_t hthread_create(hthread_callback_t callback,void *userData,InvokeTickDeclare);
	
	hint32 hthread_testcancel(hthread_t hthread);
	
	void hthread_cancel(hthread_t hthread);
	
	void hthread_cancel_join(hthread_t hthread);
	
	void hthread_join(hthread_t hthread);
	
	huintptr hthread_current_id(void);
	
	void hthread_yield(void);
    
    hthread_key_t hthread_key_create( void (*)(void *));
    
    void * hthread_key_value(hthread_key_t key);
    
    void hthread_key_value_set(hthread_key_t key,void * value);

#ifdef __cplusplus
}
#endif
	
	
#endif /* HTHREAD_H_ */
