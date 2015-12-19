/*
 *  hdata_set.c
 *  C Library
 *
 *  Created by Zhang Hailong on 10-11-14.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hdata_set.h"
#include "hrecursive.h"

#undef hdata_set_alloc

#undef hdata_set_data

#undef hdata_set_sort

#undef hdata_set_sort_result

#undef hdata_set_tree

#undef hdata_set_tree_level

#undef hdata_set_tree_get

typedef struct{
	hext_data_t data;
	
	data_set_tree_item_t * tree_root;
	hmap_t tree_cache;
	
	hlist_t items;
	hint32 tree_level;
}data_set_t;

typedef struct{
	list_compare_t compare;
	hany param;
}data_set_tree_item_compare_param_t;
	
static hint32 data_set_tree_item_compare(hlist_t list,hany item1,hany item2,hany param,InvokeTickDeclare){
	data_set_tree_item_compare_param_t * compare_param = (data_set_tree_item_compare_param_t *)param;
	data_set_tree_item_t *tree_item1 = item1;
	data_set_tree_item_t *tree_item2 = item2;
	return (*compare_param->compare)(list,tree_item1->data,tree_item2->data,compare_param->param,InvokeTickArg);
}

static data_set_tree_item_t * data_set_tree_item_alloc(hany key,hdata_t data,data_set_tree_item_t *parent_item,list_compare_t comparator,hany param,InvokeTickDeclare){
	data_set_tree_item_t * item = (data_set_tree_item_t *)mem_malloc( sizeof(data_set_tree_item_t));
	data_set_tree_item_compare_param_t compare_param;
	memset(item,0,sizeof(data_set_tree_item_t));
	item->key = key;
	item->data = data;
	if(parent_item){
		if(parent_item->childs ==NULL){
			parent_item->childs = list_alloc( 20, 20);
		}
		if(comparator){
			compare_param.compare = comparator;
			compare_param.param = param;
			list_add_and_order(parent_item->childs, item, data_set_tree_item_compare, &compare_param);
		}
		else{
			
			list_add(parent_item->childs, item);
		}
		item->level = parent_item->level +1;
	}
	return item;
}

static void data_set_tree_item_dealloc_recursive_callback(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,InvokeTickDeclare){
	data_set_tree_item_t *item = (data_set_tree_item_t *)caller;
	hint32 i,c;
	c = list_count(item->childs);
	for(i=0;i<c;i++){
		recursive_add_caller(recursive, list_get(item->childs, i));
	}
	mem_free(item);
}

static void data_set_tree_item_dealloc(data_set_tree_item_t * item,InvokeTickDeclare){
	if(item){
		hrecursive_t recursive = recursive_alloc();
	
		recursive_call(recursive, item, NULL, NULL, NULL, NULL, NULL, data_set_tree_item_dealloc_recursive_callback);
	
		recursive_dealloc(recursive);
	}
}

static void ext_obj_data_set_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	if(arg){
		data_set_t * s = (data_set_t *)obj;
		s->data = (hext_data_t)ext_data_retain(arg);
	}
}

static void ext_obj_data_set_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	data_set_t * s = (data_set_t *)obj;
	ext_data_release(s->data);
	data_set_tree_item_dealloc(s->tree_root,InvokeTickArg);
	list_dealloc(s->items);
	map_dealloc(s->tree_cache);
}

ext_class_t hdata_set_class = {ext_obj_data_set_init,ext_obj_data_set_destroy,sizeof(data_set_t)};


hdata_set_t hdata_set_alloc(hext_data_t data,InvokeTickDeclare){
	if(ext_data && hdata_type(ext_data_class(data),ext_data(data)) == HDATA_TYPE_ARRAY){
		return (hdata_set_t)ext_obj_alloc( data, &hdata_set_class);
	}
	return NULL;
}

/**
 return item count
 */
hlist_t hdata_set_sort(hdata_set_t data_set, list_compare_t comparator ,hany param,InvokeTickDeclare){
	data_set_t *ds = (data_set_t *)data_set;
	if(ds && comparator){
		hdata_t items = ext_data(ds->data);
		hdata_t item;
		hint32 i,c;
		c = hdata_array_size(ext_data_class(ds->data), items);
		if(c >0){
			if(ds->items==NULL){
				ds->items = list_alloc( c, 20);
			}
			else{
				list_clear(ds->items);
			}
			for(i=0;i<c;i++){
				item = hdata_array(ext_data_class(ds->data),items,i);
				if(comparator){
					list_add_and_order(ds->items, item, comparator, param);
				}
				else{
					list_add(ds->items,item);
				}
			}
		}
		
		return ds->items;
	}
	return NULL;
}

hlist_t hdata_set_sort_result(hdata_set_t data_set,InvokeTickDeclare){
	data_set_t * ds = (data_set_t *)data_set;
	return ds?ds->items:NULL;
}

/**
 
 return item tree level count
 */

hint32 hdata_set_tree(hdata_set_t data_set, hdata_set_tree_key_t key,hdata_set_tree_key_t parent_key,hash_code_t key_hash,equal_t key_equal,list_compare_t comparator,hany param,InvokeTickDeclare){
	data_set_t * ds = (data_set_t *)data_set;
	hany k;
	hany pk;
	hint32 i,c;
	hdata_t items,item;
	data_set_tree_item_t * tree_item,*tree_pitem;
	data_set_tree_item_compare_param_t compare_param;
	if(ds && key && parent_key){
		data_set_tree_item_dealloc(ds->tree_root,InvokeTickArg);
		ds->tree_root = data_set_tree_item_alloc( NULL, ext_data(ds->data),NULL,NULL,NULL,InvokeTickArg);
		ds->tree_level = 0;
		if(ds->tree_cache == NULL){
			ds->tree_cache = map_alloc( key_hash, key_equal);
		}
		else{
			map_clear(ds->tree_cache);
		}
		items = ext_data(ds->data);
		c = hdata_array_size(ext_data_class(ds->data), items);
		for(i=0;i<c;i++){
			item = hdata_array(ext_data_class(ds->data),items,i);
			k = (*key)(data_set,item,param,InvokeTickArg);
			pk = (*parent_key)(data_set,item,param,InvokeTickArg);
			tree_pitem = map_get(ds->tree_cache, pk);
			tree_item = data_set_tree_item_alloc( k, item, tree_pitem == NULL?ds->tree_root:tree_pitem,comparator,param,InvokeTickArg);
			data_set_tree_item_dealloc( map_put(ds->tree_cache, k, tree_item),InvokeTickArg);
			if(ds->tree_level < tree_item->level){
				ds->tree_level = tree_item->level;
			}
		}
		c = list_count(ds->tree_root->childs);
		for(i=0;i<c;i++){
			tree_item = list_get(ds->tree_root->childs, i);
			pk = (*parent_key)(data_set,tree_item->data,param,InvokeTickArg);
			tree_pitem = map_get(ds->tree_cache, pk);
			if(tree_pitem){
				list_remove_at(ds->tree_root->childs, i--);
				c = list_count(ds->tree_root->childs);
				if(tree_pitem->childs==NULL){
					tree_pitem->childs = list_alloc(20, 20);
				}
				if(comparator){
					compare_param.compare = comparator;
					compare_param.param = param;
					list_add_and_order(tree_pitem->childs, tree_item, data_set_tree_item_compare, &compare_param);
				}
				else{
					list_add(tree_pitem->childs,tree_item);
				}
				tree_item->level = tree_pitem->level +1;
				if(ds->tree_level < tree_item->level){
					ds->tree_level = tree_item->level;
				}
			}	
		}
		return ds->tree_level;
	}
	return 0;
}

const data_set_tree_item_t * hdata_set_tree_get(hdata_set_t data_set,hany key,InvokeTickDeclare){
	data_set_t * ds = (data_set_t *)data_set;
	if(ds){
		if(key){
			return (const data_set_tree_item_t *)map_get(ds->tree_cache, key);
		}
		else{
			return (const data_set_tree_item_t *)ds->tree_root;
		}
	}
	return NULL;
}

hint32 hdata_set_tree_level(hdata_set_t data_set ,InvokeTickDeclare){
	data_set_t * ds = (data_set_t *)data_set;
	return ds?ds->tree_level:0;
}

hext_data_t hdata_set_data(hdata_set_t data_set ,InvokeTickDeclare){
	data_set_t * ds = (data_set_t *)data_set;
	return ds?ds->data:NULL;
}
