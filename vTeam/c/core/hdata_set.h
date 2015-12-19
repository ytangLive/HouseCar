/*
 *  hdata_set.h
 *  C Library
 *
 *  Created by Zhang Hailong on 10-11-14.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef HDATA_SET_H
#define HDATA_SET_H

#include "hdata.h"
#include "hext_obj.h"
#include "hlist.h"
#include "hdata.h"
#include "hmap.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hdata_set_t);
	
	extern ext_class_t hdata_set_class;
	
	/*
	 
	 param mem hmem_t
	 param data ext_data array and  not null
	 */
	hdata_set_t hdata_set_alloc(hext_data_t ext_data,InvokeTickDeclare);
	
#define hdata_set_retain(data_set)		(hdata_set_t)ext_obj_retain((hext_obj_t)(data_set))
#define hdata_set_release(data_set)		ext_obj_release((hext_obj_t)(data_set))
	
	hext_data_t hdata_set_data(hdata_set_t ,InvokeTickDeclare);

	hlist_t hdata_set_sort(hdata_set_t , list_compare_t comparator ,hany param,InvokeTickDeclare);
	
	hlist_t hdata_set_sort_result(hdata_set_t ,InvokeTickDeclare);

	typedef hany (*hdata_set_tree_key_t)(hdata_set_t data_set, hdata_t item,hany param,InvokeTickDeclare);
	
	/*
	 
	 return item tree level count
	 */
	
	typedef struct{
		hlist_t childs;
		hdata_t data;
		hany key;
		hint32 level;
	}data_set_tree_item_t;
	
	hint32 hdata_set_tree(hdata_set_t , hdata_set_tree_key_t key,hdata_set_tree_key_t parent_key,hash_code_t key_hash,equal_t key_equal,list_compare_t comparator,hany param,InvokeTickDeclare);

	hint32 hdata_set_tree_level(hdata_set_t ,InvokeTickDeclare);
	
	
	/*
	 param key == NULL return root
	 */
	const data_set_tree_item_t * hdata_set_tree_get(hdata_set_t ,hany key,InvokeTickDeclare); 
    
    
    
#define hdata_set_alloc(a) hdata_set_alloc((a),InvokeTickArg)
	
#define hdata_set_data(a) hdata_set_data((a),InvokeTickArg)
    
#define hdata_set_sort(a,b,c) hdata_set_sort((a),(b),(c),InvokeTickArg)
	
#define hdata_set_sort_result(a) hdata_set_sort_result((a),InvokeTickArg)

#define hdata_set_tree(a,b,c,d,e,f,g) hdata_set_tree((a),(b),(c),(d),(e),(f),(g),InvokeTickArg)
    
#define hdata_set_tree_level(a) hdata_set_tree_level((a),InvokeTickArg)
	
#define hdata_set_tree_get(a,b) hdata_set_tree_get((a),(b),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif


#endif

