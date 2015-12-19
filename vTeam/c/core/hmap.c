/*
 *  btree.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-5.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hmap.h"
#include "hrecursive.h"
#include "hstr.h"

#undef map_alloc

#undef map_dealloc

#undef map_put

#undef map_get

#undef map_remove

#undef map_get_and_exist

#undef map_get_by_defalut

#undef map_each

#undef map_clear

#undef map_count

typedef struct{
	hany key;
	hany value;
	hany next;
}map_node_t;

typedef struct{
	hintptr hash;
	map_node_t * nodes;
	hany before;
	hany after;
}map_nodeset_t;

typedef struct {
    hint32 count;
	map_nodeset_t **nodesets;
	hint32 nodeset_count;
	hrecursive_t recursive;
	equal_t equal;
	hash_code_t hash_code;
}map_t;

#pragma mark -
#pragma mark node interface



map_node_t * map_node_alloc(map_t * map,hany key ,hany value,InvokeTickDeclare);

void map_node_dealloc(map_t * map,map_node_t *node,InvokeTickDeclare);

#pragma mark -
#pragma mark node set interface

map_nodeset_t * map_nodeset_alloc(map_t * map,hintptr hash,InvokeTickDeclare);

void map_nodeset_dealloc(map_t * map,map_nodeset_t * nodeset,InvokeTickDeclare);

map_node_t * map_nodeset_node_get(map_t * map,map_nodeset_t * nodeset,hany key,InvokeTickDeclare);

map_node_t * map_nodeset_node_put(map_t * map,map_nodeset_t * nodeset,map_node_t * node,InvokeTickDeclare);

map_node_t * map_nodeset_node_remove(map_t * map,map_nodeset_t * nodeset,hany key ,InvokeTickDeclare);

#pragma mark -
#pragma mark map private interface

map_nodeset_t * map_nodeset_find(map_t * map,hintptr hash,InvokeTickDeclare);

map_nodeset_t * map_nodeset_get(map_t * map,hintptr hash,InvokeTickDeclare);


#pragma mark -
#pragma mark map implements

hintptr hash_code_str(hany key ,InvokeTickDeclare){
	hintptr hash = 0,size = sizeof(hintptr),i=0,c;
	hchar * p = (hchar *)key;
	while(*p !='\0'){
		c = (hint32)*p;
		c = c << i * 8;
		hash = hash ^ c;
		if(++i >= size){
			i = 0;
		}
		p++;
	}
	return hash;
}

hintptr hash_code_any(hany key,InvokeTickDeclare){
	return (hintptr)key;
}

hbool equal_str(hany key1,hany key2,InvokeTickDeclare){
	return str_cmp((hcchar *)key1, (hcchar *)key2) ==0;
}

hbool equal_any(hany key1,hany key2,InvokeTickDeclare){
	return key1 == key2;
}



hmap_t map_alloc(hint32 capacity,hash_code_t hash_code ,equal_t equal,InvokeTickDeclare){
	map_t *map = (map_t *)mem_malloc( sizeof(map_t));
	memset(map,0,sizeof(map_t));
	map->hash_code = hash_code?hash_code:hash_code_any;
	map->equal = equal?equal:equal_any;
	map->recursive = recursive_alloc();
	map->nodeset_count =  capacity;
	map->nodesets = (map_nodeset_t **)mem_malloc(sizeof(map_nodeset_t *) * capacity);
	memset(map->nodesets,0, sizeof(map_nodeset_t *) * capacity);
	return (hmap_t)map;
}

void map_dealloc(hmap_t hmap,InvokeTickDeclare){
	if(hmap){
		map_t *map = (map_t *)hmap;
		map_clear(hmap,InvokeTickArg);
		recursive_dealloc(map->recursive);
		mem_free(map->nodesets);
		mem_free(map);
	}
}

hany map_put(hmap_t hmap,hany key,hany value,InvokeTickDeclare){
	hany r = NULL;
	if(hmap && key){
		map_t *map = (map_t *)hmap;
		hintptr hash = (*map->hash_code)(key,InvokeTickArg);
		map_node_t * node = map_node_alloc(map, key, value,InvokeTickArg);
		map_nodeset_t * nodeset = map_nodeset_get(map, hash,InvokeTickArg);
		map_node_t * old_node = map_nodeset_node_put(map, nodeset, node,InvokeTickArg);
		if(old_node){
			r = old_node->value;
			map_node_dealloc(map, old_node,InvokeTickArg);
		}
		else{
			map->count ++;
		}
	}
	
	return r;
}

hany map_get(hmap_t hmap,hany key,InvokeTickDeclare){
	hany r = NULL;
	if(hmap && key){
		map_t *map = (map_t *)hmap;
		hintptr hash = (*map->hash_code)(key,InvokeTickArg);
		map_nodeset_t * nodeset = map_nodeset_find(map, hash,InvokeTickArg);
		if(nodeset){
			map_node_t * node = map_nodeset_node_get(map, nodeset, key,InvokeTickArg);
			if(node){
				r = node->value;
			}
		}
	}
	
	return r;
}

hany map_remove(hmap_t hmap,hany key,InvokeTickDeclare){
	hany r = NULL;
	if(hmap && key){
		map_t *map = (map_t *)hmap;
		hintptr hash = (*map->hash_code)(key,InvokeTickArg);
		map_nodeset_t * nodeset = map_nodeset_find(map, hash,InvokeTickArg);
		if(nodeset){
			map_node_t * node = map_nodeset_node_remove(map, nodeset, key,InvokeTickArg);
			if(node){
				r = node->value;
				map_node_dealloc(map, node,InvokeTickArg);
				map->count --;
			}
		}
	}
	
	return r;
}

hany map_get_and_exist(hmap_t hmap,hany key,hbool * exist,InvokeTickDeclare){
	hany r = NULL;
	map_t *map;
	hintptr hash;
	map_nodeset_t * nodeset;
	map_node_t * node;
	*exist = hbool_false;
	if(hmap && key){
		map = (map_t *)hmap;
		hash = (*map->hash_code)(key,InvokeTickArg);
		nodeset = map_nodeset_find(map, hash,InvokeTickArg);
		if(nodeset){
			node = map_nodeset_node_get(map, nodeset, key,InvokeTickArg);
			if(node){
				r = node->value;
				*exist = hbool_true;
			}
		}
	}
	
	return r;
}

hany map_get_by_defalut(hmap_t hmap,hany key,hany defaultValue,InvokeTickDeclare){
	hany r = defaultValue;
	if(hmap && key){
		map_t *map = (map_t *)hmap;
		hintptr hash = (*map->hash_code)(key,InvokeTickArg);
		map_nodeset_t * nodeset = map_nodeset_find(map, hash,InvokeTickArg);
		if(nodeset){
			map_node_t * node = map_nodeset_node_get(map, nodeset, key,InvokeTickArg);
			if(node){
				r = node->value;
			}
		}
	}
	
	return r;
}

static void map_each_callback(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,InvokeTickDeclare){
	map_nodeset_t *nodeset = (map_nodeset_t *)caller;
	map_t *map = (map_t *)arg0;
	map_each_t echo = (map_each_t)arg1;
	map_node_t *node = nodeset->nodes;
	while(node){
		(*echo)((hmap_t)map,node->key,node->value,arg2,arg3,InvokeTickArg);
		node = node->next;
	}
	if(nodeset->after){
		recursive_add_caller(recursive, nodeset->after);
	}
	if(nodeset->before){
		recursive_add_caller(recursive, nodeset->before);
	}
}

void map_each(hmap_t hmap,map_each_t each,hany arg0,hany arg1,InvokeTickDeclare){
	if(hmap && each){
		map_t *map = (map_t *)hmap;
		hint32 i,c = map->nodeset_count;
		map_nodeset_t * nodeset = NULL;
		for(i=0;i<c;i++){
			nodeset = map->nodesets[i];
			if(nodeset){
				recursive_call(map->recursive, nodeset, map,each,arg0,arg1,NULL, map_each_callback);
			}
		}
	}
}

void map_clear(hmap_t hmap,InvokeTickDeclare){
	if(hmap ){
		map_t *map = (map_t *)hmap;
		hint32 i,c = map->nodeset_count;
		map_nodeset_t * nodeset = NULL;
		for(i=0;i<c;i++){
			nodeset = map->nodesets[i];
			if(nodeset){
				map_nodeset_dealloc(map,nodeset,InvokeTickArg);
				map->nodesets[i] = NULL;
			}
		}
		map->count = 0;
	}
}



#pragma mark -
#pragma mark node implements


map_node_t * map_node_alloc(map_t * map,hany key ,hany value,InvokeTickDeclare){
	map_node_t *node = mem_malloc( sizeof(map_node_t));
	node->key = key;
	node->value = value;
	node->next = NULL;
	return node;
}

void map_node_dealloc(map_t * map,map_node_t *node,InvokeTickDeclare){
	mem_free( node);
}


#pragma mark -
#pragma mark node set implements



map_nodeset_t * map_nodeset_alloc(map_t * map,hintptr hash,InvokeTickDeclare){
	map_nodeset_t * nodeset = (map_nodeset_t *)mem_malloc(sizeof(map_nodeset_t));
	nodeset->nodes = NULL;
	nodeset->hash = hash;
	nodeset->before = NULL;
	nodeset->after = NULL;
	return nodeset;
}

static void map_nodeset_dealloc_callback(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,InvokeTickDeclare){
	map_nodeset_t *nodeset = (map_nodeset_t *)caller;
	map_t *map = (map_t *)arg0;
	map_node_t *node = nodeset->nodes,*n;
	while(node){
		n = node->next;
		map_node_dealloc(map, node,InvokeTickArg);
		node = n;
	}
	if(nodeset->after){
		recursive_add_caller(recursive, nodeset->after);
	}
	if(nodeset->before){
		recursive_add_caller(recursive, nodeset->before);
	}
	mem_free( nodeset);
}

void map_nodeset_dealloc(map_t * map,map_nodeset_t * nodeset,InvokeTickDeclare){
	recursive_call(map->recursive, nodeset, map,NULL,NULL,NULL,NULL, map_nodeset_dealloc_callback);
}


map_node_t * map_nodeset_node_get(map_t * map,map_nodeset_t * nodeset,hany key,InvokeTickDeclare){
	map_node_t *node = nodeset->nodes;
	while(node){
		if((*map->equal)(node->key,key,InvokeTickArg)){
			return node;
		}
		node = node->next;
	}
	return NULL;
}

map_node_t * map_nodeset_node_put(map_t * map,map_nodeset_t * nodeset,map_node_t * new_node,InvokeTickDeclare){
	map_node_t *node = nodeset->nodes,*p=NULL;
	while(node){
		if((*map->equal)(node->key,new_node->key,InvokeTickArg)){
			break;
		}
		p = node;
		node = node->next;
	}
	if(node){
		
		if(p){
			p->next = new_node;
			new_node->next = node->next;
		}
		else{
			nodeset->nodes = new_node;
			new_node->next = node->next;
		}
		node->next = NULL;
	}
	else{
		if(p){
			p->next = new_node;
		}
		else{
			nodeset->nodes = new_node;
		}
		new_node->next = NULL;
	}
	
	return node;
}

map_node_t * map_nodeset_node_remove(map_t * map,map_nodeset_t * nodeset,hany key ,InvokeTickDeclare){
	map_node_t *node = nodeset->nodes,*p=NULL;
	
	while(node){
		if((*map->equal)(node->key,key,InvokeTickArg)){
			if(p){
				p->next = node->next;
				node->next = NULL;
			}
			else{
				nodeset->nodes = node->next;
				node->next = NULL;
			}
			break;
		}
		p = node;
		node = node->next;
	}
	return node;
}

#pragma mark -
#pragma mark map private interface

typedef struct{
	map_nodeset_t * parent_nodeset;
	map_nodeset_t * nodeset;
}map_nodeset_find_result_t;

static void map_nodeset_find_callback(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany *result,InvokeTickDeclare){
	hintptr hash = (hintptr)arg1;
	map_nodeset_find_result_t * r = (map_nodeset_find_result_t *)result;
	map_nodeset_t * nodeset =(map_nodeset_t *) caller;
	if(nodeset->hash ==hash){
		r->nodeset = nodeset;
	}
	else if(nodeset->hash < hash){
		if(nodeset->after){
			r->parent_nodeset = nodeset->after;
			recursive_add_caller(recursive, nodeset->after);
		}
	}
	else if(nodeset->hash > hash){
		if(nodeset->before){
			r->parent_nodeset = nodeset->before;
			recursive_add_caller(recursive, nodeset->before);
		}
	}
}

map_nodeset_t * map_nodeset_find(map_t * map,hintptr hash,InvokeTickDeclare){
	map_nodeset_find_result_t result ;
	map_nodeset_t * nodeset = map->nodesets[ abs((hint32)(hash % map->nodeset_count))];
	memset(&result,0,sizeof(map_nodeset_find_result_t));
	if(nodeset){
		result.parent_nodeset = nodeset;
		recursive_call(map->recursive, nodeset, map,(hany)hash,NULL,NULL,(hany *)&result, map_nodeset_find_callback);
	}
	return result.nodeset;
}

map_nodeset_t * map_nodeset_get(map_t * map,hintptr hash,InvokeTickDeclare){
	map_nodeset_find_result_t result ;
	map_nodeset_t * nodeset = map->nodesets[ abs((hint32)(hash % map->nodeset_count))];
	memset(&result,0,sizeof(map_nodeset_find_result_t));
	if(nodeset){
		result.parent_nodeset = nodeset;
		recursive_call(map->recursive, nodeset, map,(hany)hash,NULL,NULL,(hany *)&result, map_nodeset_find_callback);
		if(result.nodeset == NULL){
			result.nodeset = map_nodeset_alloc(map, hash,InvokeTickArg);
			if(hash > result.parent_nodeset->hash){
				result.parent_nodeset->after = result.nodeset;
			}
			else{
				result.parent_nodeset->before = result.nodeset;
			}
		}
	}
	else{
		result.nodeset = map_nodeset_alloc(map, hash,InvokeTickArg);
		map->nodesets[ abs((hint32)(hash % map->nodeset_count))] = result.nodeset;
	}
	
	return result.nodeset;
}

hint32 map_count(hmap_t map,InvokeTickDeclare){
	return map ? ((map_t *)map)->count:0;
}
