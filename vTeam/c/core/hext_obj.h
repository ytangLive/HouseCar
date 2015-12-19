/*
 *  extern_obj.h
 *  C_Library
 *
 *  Created by hailong zhang on 10-9-16.
 *  Copyright 2010 hailong. All rights reserved.
 *
 */

#ifndef HEXT_OBJ_H
#define HEXT_OBJ_H

#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif
	

	HANDLER(hext_obj_t);

	
	typedef void (*ext_init_t)(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare);
	
	typedef void (*ext_destroy_t)(hext_obj_t obj,hint32 size,InvokeTickDeclare);
	
	typedef struct _extern_class{
		ext_init_t init;
		ext_destroy_t destroy;
		hint32 size;
	}ext_class_t;
	

		
	hext_obj_t ext_obj_alloc(hany arg,const ext_class_t * ext_class,InvokeTickDeclare);
    
    hext_obj_t ext_obj_alloc_with_extra(hint32 extra,hany arg,const ext_class_t * ext_class,InvokeTickDeclare);
	
	const ext_class_t * ext_obj_class(hext_obj_t ext_obj,InvokeTickDeclare);
	
	hint32 ext_obj_retain_count(hext_obj_t ext_obj,InvokeTickDeclare);
	
	hext_obj_t ext_obj_retain(hext_obj_t ext_obj,InvokeTickDeclare);
	
	void ext_obj_release(hext_obj_t ext_obj,InvokeTickDeclare);
	
	extern ext_class_t ext_bytes_class;
	
	hext_obj_t ext_bytes_obj_alloc(hint32 size,InvokeTickDeclare);
	
	hint32 ext_bytes_obj_length(hext_obj_t data_obj_t,InvokeTickDeclare);
	
	hbyte * ext_bytes_obj_data(hext_obj_t data_obj_t,InvokeTickDeclare);
	
	hext_obj_t ext_bytes_from_data(hbyte * data,InvokeTickDeclare);
    
    
#define ext_obj_alloc(a,b) ext_obj_alloc((a),(b),InvokeTickArg)
    
#define ext_obj_alloc_with_extra(a,b,c) ext_obj_alloc_with_extra((a),(b),(c),InvokeTickArg)
	
#define ext_obj_class(a) ext_obj_class((a),InvokeTickArg)
	
#define ext_obj_retain_count(a) ext_obj_retain_count((a),InvokeTickArg)
	
#define ext_obj_retain(a) ext_obj_retain((a),InvokeTickArg)
	
#define ext_obj_release(a) ext_obj_release((a),InvokeTickArg)
	
#define ext_bytes_obj_alloc(a) ext_bytes_obj_alloc((a),InvokeTickArg)
	
#define ext_bytes_obj_length(a) ext_bytes_obj_length((a),InvokeTickArg)
	
#define ext_bytes_obj_data(a) ext_bytes_obj_data((a),InvokeTickArg)
	
#define ext_bytes_from_data(a) ext_bytes_from_data((a),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif
		
#endif

