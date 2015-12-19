/*
 *  hstruct_list.c
 *  C Library
 *
 *  Created by hailong zhang on 11-2-5.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"

#include "hstruct_list.h"


#undef struct_list_alloc

#undef struct_list_dealloc

#undef struct_list_add

#undef struct_list_insert

#undef struct_list_add_and_order

#undef struct_list_remove_at

#undef struct_list_get

#undef struct_list_count

#undef struct_list_clear

typedef struct{
	hbyte * items;
	hint32 item_size;
	hint32 extend_size;
	hint32 count;
	hint32 max_count;
}struct_list_t;

hstruct_list_t struct_list_alloc(hint32 item_size,hint32 capacity,hint32 extendSize,InvokeTickDeclare){
	struct_list_t *list = (struct_list_t *)mem_malloc(sizeof(struct_list_t));
	memset(list,0,sizeof(struct_list_t));
	list->extend_size = extendSize;
	list->max_count = capacity;
	list->item_size = item_size;
	list->items = mem_malloc( item_size *capacity);
	return (hstruct_list_t)list;
}

void struct_list_dealloc(hstruct_list_t hlist,InvokeTickDeclare){
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		mem_free(list->items);
		mem_free(list);
	}
}

static void struct_list_extend(struct_list_t * list,hint32 count,InvokeTickDeclare){
	if(count > list->max_count){
		list->max_count = list->max_count + list->extend_size;
		if(list->max_count < count){
			list->max_count = count;
		}
		list->items = realloc( list->items, list->item_size * list->max_count);
	}
}

void struct_list_add(hstruct_list_t hlist,hany item,InvokeTickDeclare){
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		struct_list_extend(list,list->count +1,InvokeTickArg);
		if(item){
			memcpy(list->items + list->item_size * list->count ++,item,list->item_size);
		}
		else{
			memset(list->items + list->item_size * list->count ++,0,list->item_size);
		}
	}
}

void struct_list_insert(hstruct_list_t hlist,hany item,hint32 index,InvokeTickDeclare){

	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		hint32 i;
		struct_list_extend(list,list->count +1,InvokeTickArg);
		if(index >=0 && index < list->count){
			for(i = list->count; i> index;i--){
				memcpy(list->items + i * list->item_size,list->items + (i -1) * list->item_size,list->item_size);
			}
			if(item){
				memcpy(list->items + index * list->item_size,item,list->item_size);
			}
			else{
				memset(list->items + index * list->item_size,0,list->item_size);
			}
			list->count ++;
		}
		else{
			if(item){
				memcpy(list->items + list->item_size * list->count ++,item,list->item_size);
			}
			else{
				memset(list->items + list->item_size * list->count ++,0,list->item_size);
			}
		}
	}
}

void struct_list_add_and_order(hstruct_list_t hlist,hany item,struct_list_compare_t compare,hany param,InvokeTickDeclare){
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		hint32 i,j;
		struct_list_extend(list,list->count +1,InvokeTickArg);
		
		for(i=0;i<list->count;i++){
			if((*compare)(hlist,list->items + i* list->item_size,item,param,InvokeTickArg) >0){
				break;
			}
		}
		
		for(j=list->count;j> i;j--){
			memcpy(list->items + j * list->item_size,list->items + (j-1) * list->item_size,list->item_size);
		}
		if(item){
			memcpy(list->items + i * list->item_size,item,list->item_size);
		}
		else{
			memset(list->items + i * list->item_size,0,list->item_size);
		}
		list->count ++;
	}
}

void struct_list_remove_at(hstruct_list_t hlist,hint32 index,InvokeTickDeclare){
	hint32 i;
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		if(index >=0 && index < list->count){
			list->count --;
			for(i=index;i<list->count;i++){
				memcpy(list->items + i * list->item_size,list->items + (i +1) * list->item_size,list->item_size);
			}
		}
	}
}

hany struct_list_get(hstruct_list_t hlist, hint32 index,InvokeTickDeclare){
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		if(index >=0 && index < list->count){
			return list->items + index * list->item_size;
		}
	}
	return NULL;
}

hint32 struct_list_count(hstruct_list_t list,InvokeTickDeclare){
	return list ?((struct_list_t *)list)->count:0;
}

void struct_list_clear(hstruct_list_t hlist,InvokeTickDeclare){
	if(hlist){
		struct_list_t * list = (struct_list_t *)hlist;
		list->count = 0;
	}
}
