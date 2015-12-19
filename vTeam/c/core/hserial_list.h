/*
 *  hserial_list.h
 *  C_Library
 *
 *  Created by hailong zhang on 11-3-8.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */


#ifndef _HSERIAL_LIST_H
#define _HSERIAL_LIST_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "hmem.h"
	
    struct _serial_list_t{
        hbyte * data;
        hint32 length;
        hint32 item_length;
    };
	
    typedef struct _serial_list_t * hserial_list_t;
    
	hserial_list_t serial_list_alloc(hint32 item_bytes,hint32 extra_bytes,InvokeTickDeclare);
	
	void serial_list_dealloc(hserial_list_t list,InvokeTickDeclare);
	
	hany serial_list_extra(hserial_list_t list,InvokeTickDeclare);
	
	hint32 serial_list_extra_bytes(hserial_list_t list,InvokeTickDeclare);
	
	hint32 serial_list_count(hserial_list_t list,InvokeTickDeclare);
	
	hany serial_list_item_at(hserial_list_t list,hint32 i,InvokeTickDeclare);
	
	hany serial_list_item_add(hserial_list_t list,hany value,InvokeTickDeclare);
	
	hany serial_list_item_insert(hserial_list_t list,hany value,hint32 i,InvokeTickDeclare);
	
	hbool serial_list_remove_at(hserial_list_t list,hint32 i,InvokeTickDeclare);
	
	hbool serial_list_clear(hserial_list_t list,InvokeTickDeclare);
	
    hany serial_list_item_last(hserial_list_t list,InvokeTickDeclare);
    
    hbool serial_list_remove_last(hserial_list_t list,InvokeTickDeclare);
	
    typedef hbool (*serial_list_equal_t)(hserial_list_t list,hany item1,hany item2,InvokeTickDeclare);
    
    hint32 serial_list_index_of(hserial_list_t list,hany item, serial_list_equal_t equal,InvokeTickDeclare);
    
#define serial_list_alloc(a,b) serial_list_alloc((a),(b),InvokeTickArg)
	
#define serial_list_dealloc(a) serial_list_dealloc((a),InvokeTickArg)
	
#define serial_list_extra(a) serial_list_extra((a),InvokeTickArg)
	
#define serial_list_extra_bytes(a) serial_list_extra_bytes((a),InvokeTickArg)
	
#define serial_list_count(a) serial_list_count((a),InvokeTickArg)
	
#define serial_list_item_at(a,b) serial_list_item_at((a),(b),InvokeTickArg)
	
#define serial_list_item_add(a,b) serial_list_item_add((a),(b),InvokeTickArg)
	
#define serial_list_item_insert(a,b,c) serial_list_item_insert((a),(b),(c),InvokeTickArg)
	
#define serial_list_remove_at(a,b) serial_list_remove_at((a),(b),InvokeTickArg)
	
#define serial_list_clear(a) serial_list_clear((a),InvokeTickArg)
	
#define serial_list_item_last(a) serial_list_item_last((a),InvokeTickArg)
    
#define serial_list_remove_last(a) serial_list_remove_last((a),InvokeTickArg)
    
#define serial_list_index_of(a,b,c) serial_list_index_of((a),(b),(c),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif


#endif
