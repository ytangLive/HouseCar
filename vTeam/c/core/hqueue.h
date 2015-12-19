/*
 *  queue.h
 *  C Library
 *
 *  Created by hailong zhang on 10-8-18.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */


#ifndef _H_QUEUE_H
#define _H_QUEUE_H
#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hqueue_t)

	hqueue_t queue_alloc(InvokeTickDeclare);
    
	void queue_dealloc(hqueue_t hqueue,InvokeTickDeclare);
	
	void queue_enter(hqueue_t hqueue,hany item,hint32 priority,InvokeTickDeclare);
	
	hany queue_out(hqueue_t hqueue,InvokeTickDeclare);
	
	hany queue_peek(hqueue_t hqueue,InvokeTickDeclare);
	
	hint32 queue_number(hqueue_t hqueue,InvokeTickDeclare);

	typedef hbool (*queue_filter_t)(hqueue_t queue,hany item,hany dest,InvokeTickDeclare);
	
	void queue_remove(hqueue_t queue,queue_filter_t filter,hany dest,InvokeTickDeclare);
    
    #define queue_alloc() queue_alloc(InvokeTickArg)
    #define queue_dealloc(a) queue_dealloc((a),InvokeTickArg)
    #define queue_enter(a,b,c) queue_enter((a),(b),(c),InvokeTickArg)
    #define queue_out(a) queue_out((a),InvokeTickArg)
    #define queue_peek(a) queue_peek((a),InvokeTickArg)
    #define queue_number(a) queue_number((a),InvokeTickArg)
    #define queue_remove(a,b,c) queue_remove((a),(b),(c),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif

#endif
