/*
 *  hbtree.h
 *  C Library
 *
 *  Created by hailong zhang on 10-11-16.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */


#ifndef HBTREE_H
#define HBTREE_H

#include "hmem.h"
#include "hbuffer.h"



#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hbtree_t)
	
	
	typedef void (*btree_node_destroy_t)(hbtree_t,hany node,hint32 size,InvokeTickDeclare);
	
	hbtree_t btree_alloc(btree_node_destroy_t destroy,InvokeTickDeclare);
	
	void btree_dealloc(hbtree_t,InvokeTickDeclare);
	
	void btree_cleanup(hbtree_t ,InvokeTickDeclare);
	
	hany btree_node_alloc(hbtree_t,hint32 size,InvokeTickDeclare);
	
	hany btree_node_before(hbtree_t,hany node,InvokeTickDeclare);
	
	hany btree_node_before_set(hbtree_t,hany node,hany before,InvokeTickDeclare);
	
	hany btree_node_after(hbtree_t,hany node,InvokeTickDeclare);
	
	hany btree_node_after_set(hbtree_t,hany node,hany after,InvokeTickDeclare);
	
	hany btree_node_parent(hbtree_t ,hany node,InvokeTickDeclare);
	
	hany btree_node_root(hbtree_t,InvokeTickDeclare);
	
	hany btree_node_root_set(hbtree_t,hany root,InvokeTickDeclare);
	
	hany btree_node_remove(hbtree_t,hany node,InvokeTickDeclare);
	
	typedef hbool (*btree_each_callback_t)(hbtree_t,hany node,hany param,InvokeTickDeclare);
	
	void btree_each_before_parent(hbtree_t,btree_each_callback_t callback,hany param,InvokeTickDeclare);
	
	void btree_each_center_parent(hbtree_t,btree_each_callback_t callback,hany param,InvokeTickDeclare);
	
	void btree_each_after_parent(hbtree_t,btree_each_callback_t callback,hany param,InvokeTickDeclare);
    
   
    
#define btree_alloc(a) btree_alloc((a),InvokeTickArg)
	
#define btree_dealloc(a) btree_dealloc((a),InvokeTickArg)
	
#define btree_cleanup(a) btree_cleanup((a),InvokeTickArg)
	
#define btree_node_alloc(a,b) btree_node_alloc((a),(b),InvokeTickArg)
	
#define btree_node_before(a,b) btree_node_before((a),(b),InvokeTickArg)
	
#define btree_node_before_set(a,b,c) btree_node_before_set((a),(b),(c),InvokeTickArg)
	
#define btree_node_after(a,b) btree_node_after((a),(b),InvokeTickArg)
	
#define btree_node_after_set(a,b,c) btree_node_after_set((a),(b),(c),InvokeTickArg)
	
#define btree_node_parent(a,b) btree_node_parent((a),(b),InvokeTickArg)
	
#define btree_node_root(a) btree_node_root((a),InvokeTickArg) 
	
#define btree_node_root_set(a,b) btree_node_root_set((a),(b),InvokeTickArg)
	
#define btree_node_remove(a,b) btree_node_remove((a),(b),InvokeTickArg)

#define btree_each_before_parent(a,b,c) btree_each_before_parent((a),(b),(c),InvokeTickArg)
	
#define btree_each_center_parent(a,b,c) btree_each_center_parent((a),(b),(c),InvokeTickArg)
	
#define btree_each_after_parent(a,b,c) btree_each_after_parent((a),(b),(c),InvokeTickArg)

#ifdef __cplusplus
}
#endif


#endif
