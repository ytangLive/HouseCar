/*
 *  hrecursive.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-5.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HRECURSIVE_H
#define HRECURSIVE_H

#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif

	HANDLER(hrecursive_t)
	
	
	typedef void (*recursive_callback_t)(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,InvokeTickDeclare);
	
	hrecursive_t recursive_alloc(InvokeTickDeclare);
	
	void recursive_dealloc(hrecursive_t recursive,InvokeTickDeclare);
	
	void recursive_add_caller(hrecursive_t recursive,hany caller,InvokeTickDeclare);

	void recursive_call(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3, hany * result,recursive_callback_t callback,InvokeTickDeclare);
	
#define recursive_alloc() recursive_alloc(InvokeTickArg)
	
#define recursive_dealloc(a) recursive_dealloc((a),InvokeTickArg)
	
#define recursive_add_caller(a,b) recursive_add_caller((a),(b),InvokeTickArg)
    
#define recursive_call(a,b,c,d,e,f,g,h) recursive_call((a),(b),(c),(d),(e),(f),(g),(h),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif

#endif

