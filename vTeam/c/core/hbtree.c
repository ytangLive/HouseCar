/*
 *  hbtree.c
 *  C Library
 *
 *  Created by hailong zhang on 10-11-16.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hbtree.h"
#include "hrecursive.h"
#include "hlist.h"

#undef btree_alloc

#undef btree_dealloc

#undef btree_cleanup

#undef btree_node_alloc

#undef btree_node_before

#undef btree_node_before_set

#undef btree_node_after

#undef btree_node_after_set

#undef btree_node_parent

#undef btree_node_root

#undef btree_node_root_set

#undef btree_node_remove

#undef btree_each_before_parent

#undef btree_each_center_parent

#undef btree_each_after_parent

typedef struct _btree_node_t{
	struct _btree_node_t * before;
	struct _btree_node_t * after;
	struct _btree_node_t * parent;
	hint32 size;
	hintptr verify;
}btree_node_t;

typedef struct{
	hlist_t alloc_nodes;
	btree_node_t * root_node;
	btree_node_destroy_t destroy;
}btree_t;

#define BTREE_NODE_GET(node)				( (node) ? ( btree_node_t * )( (hbyte *)(node) - sizeof(btree_node_t) ) :NULL)
#define BTREE_NODE_VERIFY(btree,node)		assert((node)->verify == ((hintptr)(node) ^ (node)->size ^ (hintptr)(btree)))
#define BTREE_NODE_IS_DISPLACED(btree,node)	( (node)->parent == NULL && (node) != (btree)->root_node)
#define BTREE_NODE_TO_PTR(node)				( (node) ? ( (hbyte *)(node) + sizeof(btree_node_t) ) :NULL)

hbtree_t btree_alloc(btree_node_destroy_t destroy,InvokeTickDeclare){
	btree_t * btree = mem_malloc( sizeof(btree_t));
	memset(btree,0,sizeof(btree_t));
	btree->alloc_nodes = list_alloc( 20,20);
	btree->destroy = destroy;
	return (hbtree_t)btree;
}

static void btree_node_dealloc(btree_t * btree,btree_node_t *node,InvokeTickDeclare){
	if(btree && node){
	
		BTREE_NODE_VERIFY(btree,node);
		if(btree->destroy){
			(*btree->destroy)((hbtree_t)btree,(hbyte *)node + sizeof(btree_node_t),node->size,InvokeTickArg);
		}
	
		mem_free( node);
	}
}

void btree_dealloc(hbtree_t hbtree,InvokeTickDeclare){
	if(hbtree){
		btree_t * btree = (btree_t *)hbtree;

		hint32 i,c;
		c = list_count(btree->alloc_nodes);
		for(i=0;i<c;i++){
			btree_node_dealloc(btree,list_get(btree->alloc_nodes, i),InvokeTickArg);
		}
		mem_free( btree);
	}
}

hany btree_node_alloc(hbtree_t hbtree,hint32 size,InvokeTickDeclare){
	if(hbtree && size >0){
		btree_t * btree = (btree_t *)hbtree;
		btree_node_t * node = (btree_node_t *)mem_malloc( sizeof(btree_node_t) + size);
		memset(node,0,sizeof(btree_node_t));
		node->size = size;
		node->verify = (hintptr)node ^ node->size ^ (hintptr)btree;
		list_add(btree->alloc_nodes, node);
		return BTREE_NODE_TO_PTR(node);
	}
	return NULL;
}

void btree_cleanup(hbtree_t hbtree,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t *node;
	if(btree){
		hint32 i,c;
		c = list_count(btree->alloc_nodes);
		for(i=0;i<c;i++){
			node = list_get(btree->alloc_nodes, i);
			if(BTREE_NODE_IS_DISPLACED(btree,node)){
				btree_node_dealloc(btree,list_remove_at(btree->alloc_nodes, i--),InvokeTickArg);
			}
		}		
	}
}


hany btree_node_before(hbtree_t hbtree,hany node_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		return BTREE_NODE_TO_PTR(node->before);
	}
	
	return NULL;
}

hany btree_node_before_set(hbtree_t hbtree,hany node_ptr,hany before_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	btree_node_t * before = BTREE_NODE_GET(before_ptr);
	btree_node_t * r = NULL;
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		if(before){
			BTREE_NODE_VERIFY(btree,before);
			assert(BTREE_NODE_IS_DISPLACED(btree,before));
		}
		r = node->before;
		if(r){
			r->parent = NULL;
		}
		
		node->before = before;
		
		if(before){
			before->parent = node;
		}
		
		return r;
	}
	
	return NULL;
}

hany btree_node_after(hbtree_t hbtree,hany node_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		return BTREE_NODE_TO_PTR(node->after);
	}
	return NULL;
}

hany btree_node_after_set(hbtree_t hbtree,hany node_ptr,hany after_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	btree_node_t * after = BTREE_NODE_GET(after_ptr);
	btree_node_t * r = NULL;
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		if(after){
			BTREE_NODE_VERIFY(btree,after);
			assert(BTREE_NODE_IS_DISPLACED(btree,after));
		}
		
		r = node->after;
		if(r){
			r->parent = NULL;
		}
		
		node->after = after;
		
		if(after){
			after->parent = node;
		}
		
		return r;
	}
	
	return NULL;
}

hany btree_node_parent(hbtree_t hbtree,hany node_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		return BTREE_NODE_TO_PTR(node->parent);
	}
	return NULL;
}


hany btree_node_root(hbtree_t hbtree,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	if(btree){
		return BTREE_NODE_TO_PTR(btree->root_node);
	}
	
	return NULL;
}

hany btree_node_root_set(hbtree_t hbtree,hany root_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * root = BTREE_NODE_GET(root_ptr);
	btree_node_t *r = NULL;
	if(btree){
		if(root){
			BTREE_NODE_VERIFY(btree,root);
			assert(BTREE_NODE_IS_DISPLACED(btree,root));
		}
		r = btree->root_node;
		r->parent = NULL;
		
		btree->root_node = root;
		root->parent = NULL;
		
		return r;
	}
	
	return NULL;
}

hany btree_node_remove(hbtree_t hbtree,hany node_ptr,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	btree_node_t * node = BTREE_NODE_GET(node_ptr);
	if(btree && node){
		BTREE_NODE_VERIFY(btree,node);
		if(!BTREE_NODE_IS_DISPLACED(btree,node)){
		
			if(node->parent){
				if(node->parent->before == node){
					node->parent->before =NULL;
				}
				else if(node->parent->after == node){
					node->parent->after = NULL;
				}
				
				node->parent = NULL;
			}
			else{
				btree->root_node = NULL;
			}
			
		}
		
		return node;
	}
	
	return NULL;
}

static void btree_each_node_before_parent(hbtree_t hbtree,btree_node_t * node,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	(*callback)(hbtree,BTREE_NODE_TO_PTR(node),param,InvokeTickArg);
	if(node->before){
		btree_each_node_before_parent(hbtree,node->before,callback,param,InvokeTickArg);
	}
	if(node->after){
		btree_each_node_before_parent(hbtree,node->after,callback,param,InvokeTickArg);
	}
}

void btree_each_before_parent(hbtree_t hbtree,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	if(btree && btree->root_node && callback){
		btree_each_node_before_parent(hbtree,btree->root_node,callback,param,InvokeTickArg);
	}
}

static void btree_each_node_center_parent(hbtree_t hbtree,btree_node_t * node,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	if(node->before){
		btree_each_node_before_parent(hbtree,node->before,callback,param,InvokeTickArg);
	}
	(*callback)(hbtree,BTREE_NODE_TO_PTR(node),param,InvokeTickArg);
	if(node->after){
		btree_each_node_before_parent(hbtree,node->after,callback,param,InvokeTickArg);
	}
}

void btree_each_center_parent(hbtree_t hbtree,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	if(btree && btree->root_node && callback){
		btree_each_node_center_parent(hbtree,btree->root_node,callback,param,InvokeTickArg);
	}
}

static void btree_each_node_after_parent(hbtree_t hbtree,btree_node_t * node,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	if(node->before){
		btree_each_node_before_parent(hbtree,node->before,callback,param,InvokeTickArg);
	}
	if(node->after){
		btree_each_node_before_parent(hbtree,node->after,callback,param,InvokeTickArg);
	}
	(*callback)(hbtree,BTREE_NODE_TO_PTR(node),param,InvokeTickArg);
}

void btree_each_after_parent(hbtree_t hbtree,btree_each_callback_t callback,hany param,InvokeTickDeclare){
	btree_t * btree = (btree_t *)hbtree;
	if(btree && btree->root_node && callback){
		btree_each_node_after_parent(hbtree,btree->root_node,callback,param,InvokeTickArg);
	}
}
