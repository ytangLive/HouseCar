/*
 *  hstruct_list.h
 *  C Library
 *
 *  Created by hailong zhang on 11-2-5.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */


#ifndef HSTRUCT_LIST_H
#define HSTRUCT_LIST_H

#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hstruct_list_t)
	
	typedef hint32 (*struct_list_compare_t)(hstruct_list_t list, hany item1 ,hany item2, hany param,InvokeTickDeclare);
	
	hstruct_list_t struct_list_alloc(hint32 item_size,hint32 capacity,hint32 extendSize,InvokeTickDeclare);
	
	void struct_list_dealloc(hstruct_list_t list,InvokeTickDeclare);
	
	void struct_list_add(hstruct_list_t list,hany item,InvokeTickDeclare);
	
	void struct_list_insert(hstruct_list_t list,hany item,hint32 index,InvokeTickDeclare);
	
	void struct_list_add_and_order(hstruct_list_t list,hany item,struct_list_compare_t compare,hany param,InvokeTickDeclare);
	
	void struct_list_remove_at(hstruct_list_t list,hint32 index,InvokeTickDeclare);
	
	hany struct_list_get(hstruct_list_t list, hint32 index,InvokeTickDeclare);
	
	hint32 struct_list_count(hstruct_list_t list,InvokeTickDeclare);
	
	void struct_list_clear(hstruct_list_t list,InvokeTickDeclare);
    
    
    
    
#define struct_list_alloc(a,b,c) struct_list_alloc((a),(b),(c),InvokeTickArg)
	
#define struct_list_dealloc(a) struct_list_dealloc((a),InvokeTickArg)
	
#define struct_list_add(a,b) struct_list_add((a),(b),InvokeTickArg)
	
#define struct_list_insert(a,b,c) struct_list_insert((a),(b),(c),InvokeTickArg)
	
#define struct_list_add_and_order(a,b,c,d) struct_list_add_and_order((a),(b),(c),(d),InvokeTickArg)
	
#define struct_list_remove_at(a,b) struct_list_remove_at((a),(b),InvokeTickArg)
	
#define struct_list_get(a,b) struct_list_get((a),(b),InvokeTickArg)
	
#define struct_list_count(a) struct_list_count((a),InvokeTickArg)
	
#define struct_list_clear(a) struct_list_clear((a),InvokeTickArg)
	
	
#ifdef __cplusplus
}
#endif


#endif
