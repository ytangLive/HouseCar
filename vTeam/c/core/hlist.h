/*
 *  hlist.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-8.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef HLIST_H
#define HLIST_H

#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct _list_t{
        hany * items;
        hint32 count;
    } * hlist_t;
	
	typedef hint32 (*list_compare_t)(hlist_t list, hany item1 ,hany item2, hany param,InvokeTickDeclare);
	
	hlist_t list_alloc(hint32 capacity,hint32 extendSize,InvokeTickDeclare);
	
	void list_dealloc(hlist_t list,InvokeTickDeclare);
	
	void list_add(hlist_t list,hany item,InvokeTickDeclare);
	
	void list_insert(hlist_t list,hany item,hint32 index,InvokeTickDeclare);
	
	void list_add_and_order(hlist_t list,hany item,list_compare_t compare,hany param,InvokeTickDeclare);
	
	hany list_remove_at(hlist_t list,hint32 index,InvokeTickDeclare);
	
	void list_remove(hlist_t list, hany item,InvokeTickDeclare);
	
	hany list_get(hlist_t list, hint32 index,InvokeTickDeclare);
    
    hany list_last(hlist_t list,InvokeTickDeclare);
    
    void list_remove_last(hlist_t list,InvokeTickDeclare);
	
	hint32 list_count(hlist_t list,InvokeTickDeclare);
	
	void list_clear(hlist_t list,InvokeTickDeclare);
    
    hint32 list_index_of(hlist_t list,hany item,InvokeTickDeclare);
	
	void list_split_str(hlist_t list, hcchar * str,hcchar *split_chars,InvokeTickDeclare);
	
	void list_split_free(hlist_t list,InvokeTickDeclare);
    
    hlist_t list_clone(hlist_t list,InvokeTickDeclare);
    
    hany list_pop(hlist_t list,InvokeTickDeclare);
    
    void list_swap(hlist_t list,hint32 index1,hint32 index2,InvokeTickDeclare);
    
#define list_alloc(a,b) list_alloc((a),(b),InvokeTickArg)
	
#define list_dealloc(a) list_dealloc((a),InvokeTickArg)
	
#define list_add(a,b) list_add(a,b,InvokeTickArg)
	
#define list_insert(a,b,c) list_insert((a),(b),(c),InvokeTickArg)
	
#define list_add_and_order(a,b,c,d) list_add_and_order((a),(b),(c),(d),InvokeTickArg)
	
#define list_remove_at(a,b) list_remove_at((a),(b),InvokeTickArg)
	
#define list_remove(a,b) list_remove(a,b,InvokeTickArg)
	
#define list_get(a,b) list_get(a,b,InvokeTickArg)
	
#define list_count(a) list_count(a,InvokeTickArg)
	
#define list_clear(a) list_clear(a,InvokeTickArg)
	
#define list_split_str(a,b,c) list_split_str(a,b,c,InvokeTickArg)
	
#define list_split_free(a) list_split_free(a,InvokeTickArg)
    
#define list_last(a) list_last((a),InvokeTickArg)
    
#define list_remove_last(a) list_remove_last((a),InvokeTickArg)
    
#define list_index_of(a,b) list_index_of((a),(b),InvokeTickArg)
    
#define list_clone(a) list_clone((a),InvokeTickArg)
    
#define list_pop(a) list_pop((a),InvokeTickArg)
  
#define list_swap(a,b,c) list_swap((a),(b),(c),InvokeTickArg)
#ifdef __cplusplus
}
#endif


#endif
