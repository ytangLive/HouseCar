/*
 *  hlist.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-8.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hlist.h"
#include "hbuffer.h"
#include "hstr.h"

#undef list_alloc

#undef list_dealloc

#undef list_add

#undef list_insert

#undef list_add_and_order

#undef list_remove_at

#undef list_remove

#undef list_get

#undef list_count

#undef list_clear

#undef list_split_str

#undef list_split_free

#undef list_last

#undef list_remove_last

#undef list_index_of

#undef list_clone

#undef list_pop

#undef list_swap

typedef struct{
	hany * items;
    hint32 count;
	hint32 extend_size;
	hint32 max_count;
}list_t;

hlist_t list_alloc(hint32 capacity,hint32 extendSize,InvokeTickDeclare){
	list_t *list = (list_t *)mem_malloc( sizeof(list_t));
	memset(list,0,sizeof(list_t));
	list->extend_size = extendSize;
	list->max_count = capacity;
	list->items = (hany *)mem_malloc( sizeof(hany) *capacity);
	return (hlist_t)list;
}

void list_dealloc(hlist_t hlist,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		mem_free( list->items);
		mem_free( list);
	}
}

void list_add(hlist_t hlist,hany item,InvokeTickDeclare){
	hany * items ;
	if(hlist){
		list_t * list = (list_t *)hlist;
		if(list->count +1 > list->max_count){
			list->max_count = list->max_count + list->extend_size;
			items = (hany *)mem_malloc( sizeof(hany) * list->max_count);
			memcpy(items,list->items,sizeof(hany) * list->count);
			mem_free( list->items);
			list->items= items;
		}
		list->items[list->count ++] = item;
	}
}

void list_insert(hlist_t hlist,hany item,hint32 index,InvokeTickDeclare){
	hany * items ;
	if(hlist){
		list_t * list = (list_t *)hlist;
		hint32 i;
		if(list->count +1 > list->max_count){
			list->max_count = list->max_count + list->extend_size;
			items = (hany *)mem_malloc( sizeof(hany) * list->max_count);
			memcpy(items,list->items,sizeof(hany) * list->count);
			mem_free(list->items);
			list->items= items;
		}
		if(index >=0 && index < list->count){
			for(i = list->count; i> index;i--){
				list->items[i] = list->items[i -1];
			}
			list->items[index] = item;
			list->count ++;
		}
		else{
			list->items[list->count ++] = item;
		}
	}
}

void list_add_and_order(hlist_t hlist,hany item,list_compare_t compare,hany param,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		hint32 i,j;
		hany * items ;
		if(list->count +1 > list->max_count){
			list->max_count = list->max_count + list->extend_size;
			items = (hany *)mem_malloc( sizeof(hany) * list->max_count);
			memcpy(items,list->items,sizeof(hany) * list->count);
			mem_free(list->items);
			list->items= items;
		}
		
		for(i=0;i<list->count;i++){
			if((*compare)(hlist,list->items[i],item,param,InvokeTickArg) >0){
				break;
			}
		}
		
		for(j=list->count;j> i;j--){
			list->items[j ] = list->items[j-1];
		}
		
		list->items[i] = item;
		list->count ++;
	}
}

hany list_remove_at(hlist_t hlist,hint32 index,InvokeTickDeclare){
	hany r =NULL;
	hint32 i;
	if(hlist){
		list_t * list = (list_t *)hlist;
		if(index >=0 && index < list->count){
			r = list->items[index];
			list->count --;
			for(i=index;i<list->count;i++){
				list->items[i] = list->items[i +1];
			}
		}
	}
	return r;
}

void list_remove(hlist_t hlist, hany item,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		hint32 i;
		for(i=0;i<list->count;i++){
			if(item == list->items[i]){
				list->count --;
				for(;i<list->count;i++){
					list->items[i] = list->items[i+1];
				}
				break;
			}
		}
		
	}
}

hany list_get(hlist_t hlist, hint32 index,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		if(index >=0 && index < list->count){
			return list->items[index];
		}
	}
	return NULL;
}

hany list_last(hlist_t list,InvokeTickDeclare){
    return list_get(list,list_count(list,InvokeTickArg) -1,InvokeTickArg);
}

void list_remove_last(hlist_t list,InvokeTickDeclare){
    list_remove_at(list, list_count(list,InvokeTickArg) -1, InvokeTickArg);
}

hany list_pop(hlist_t list,InvokeTickDeclare){
    hany v = list_last(list, InvokeTickArg);
    list_remove_last(list, InvokeTickArg);
    return v;
}

hint32 list_count(hlist_t list,InvokeTickDeclare){
	return list ?((list_t *)list)->count:0;
}

void list_clear(hlist_t hlist,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		list->count = 0;
	}
}

hint32 list_index_of(hlist_t hlist,hany item,InvokeTickDeclare){
    if(hlist){
        hint32 i,c;
        c = list_count(hlist, InvokeTickArg);
        for(i=0;i<c;i++){
            if(list_get(hlist, i, InvokeTickArg) == item){
                return i;
            }
        }
    }
    return -1;
}

static hbool list_split_str_exist_char(hcchar *split_chars ,hchar *c,InvokeTickDeclare){
	hchar * p = (hchar *)split_chars;
	while(*p !='\0'){
		if(*p == *c){
			return hbool_true;
		}
		p++;
	}
	return hbool_false;
}

void list_split_str(hlist_t hlist, hcchar * str,hcchar *split_chars,InvokeTickDeclare){
	if(hlist && str && split_chars){
		hchar * p = (hchar *)str;
		hchar *s = NULL;
		hbuffer_t buffer = buffer_alloc(1024, 1024);
		
		while(*p !='\0'){
			if(list_split_str_exist_char(split_chars,p,InvokeTickArg) ){
				if(buffer_length(buffer) > 0){
					s = NULL;
					str_cpy(&s, buffer_to_str(buffer));
					list_add(hlist, s,InvokeTickArg);
					buffer_clear(buffer);
				}
			}
			else{
				buffer_append(buffer, p, 1);
			}
			p++;
		}
		
		if(buffer_length(buffer) >0){
			s = NULL;
			str_cpy(&s, buffer_to_str(buffer));
			list_add(hlist, s,InvokeTickArg);
		}
		
		buffer_dealloc(buffer);
	}
}

void list_split_free(hlist_t hlist,InvokeTickDeclare){
	if(hlist){
		list_t * list = (list_t *)hlist;
		hint32 i;
		for(i=0;i<list->count;i++){
			mem_free(list->items[i]);
		}
		list->count = 0;
	}
}

hlist_t list_clone(hlist_t list,InvokeTickDeclare){
    list_t * l = (list_t *)list;
    list_t *r = (list_t *)mem_malloc( sizeof(list_t));
	memset(r,0,sizeof(list_t));
	r->extend_size = l->extend_size;
	r->max_count = l->count > l->extend_size ? l->count : l->extend_size;
    r->count = l->count;
    r->items = (hany *)mem_malloc( sizeof(hany) *r->max_count);
	mem_memcpy(r->items, l->items,  sizeof(hany) *r->count);
	return (hlist_t)r;

}

void list_swap(hlist_t hlist,hint32 index1,hint32 index2,InvokeTickDeclare){
    list_t * list = (list_t *)hlist;
    hany t;
    if(list && index1 >=0 && index2 >=0 && index1 < list->count && index2 < list->count){
        t = list->items[index1];
        list->items[index1] = list->items[index2];
        list->items[index2] = t;
    }
}
