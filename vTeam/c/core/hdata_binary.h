/*
 *  hdata_binary.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-11.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */



#ifndef HDATA_BINARY_H
#define HDATA_BINARY_H

#include "hmem.h"
#include "hdata.h"
#include "hbuffer.h"
#include "hext_obj.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	extern hdata_class_t hdata_binary_class;
	 
	hdata_t hdata_binary_alloc(hext_obj_t ext_bytes,InvokeTickDeclare);
	
	void hdata_binary_dealloc(hdata_t data,InvokeTickDeclare);
	
	hext_obj_t hdata_binary_bytes(hdata_t data,InvokeTickDeclare);
    
    hbool hdata_binary_check(hext_obj_t ext_bytes,hint32 length,InvokeTickDeclare);
    
    
    
#define hdata_binary_alloc(a) hdata_binary_alloc((a),InvokeTickArg)
	
#define hdata_binary_dealloc(a) hdata_binary_dealloc((a),InvokeTickArg)
	
#define hdata_binary_bytes(a) hdata_binary_bytes((a),InvokeTickArg)
    
#define hdata_binary_check(a,b) hdata_binary_check((a),(b),InvokeTickArg)

#ifdef __cplusplus
}
#endif


#endif
