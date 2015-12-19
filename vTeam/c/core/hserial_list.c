/*
 *  hserial_list.c
 *  C_Library
 *
 *  Created by hailong zhang on 11-3-8.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hserial_list.h"

#define ITEM_EXTEND_MIN 10

#undef serial_list_alloc

#undef serial_list_dealloc

#undef serial_list_extra

#undef serial_list_extra_bytes

#undef serial_list_count

#undef serial_list_item_at

#undef serial_list_item_add

#undef serial_list_item_insert

#undef serial_list_remove_at

#undef serial_list_clear

#undef serial_list_item_last

#undef serial_list_remove_last

#undef serial_list_index_of

typedef struct{
    hbyte * data;
    hint32 length;
    hint32 item_length;
	hint32 item_bytes;
	hint32 extra_bytes;
}serial_list_t;

static void _serial_list_data_extend(serial_list_t * list,hint32 length,InvokeTickDeclare){
	if(length > list->length){
		list->length = length < ITEM_EXTEND_MIN * list->item_bytes ?  ITEM_EXTEND_MIN * list->item_bytes : length;
		if(list->data){
			list->data = mem_realloc( list->data, list->length);
		}
		else{
			list->data = mem_malloc(list->length);
		}
	}
}

hserial_list_t serial_list_alloc( hint32 item_bytes,hint32 extra_bytes,InvokeTickDeclare){
	if(item_bytes >0 && extra_bytes >=0){
		serial_list_t * list = (serial_list_t *)mem_malloc( sizeof(serial_list_t) + extra_bytes);
		memset(list, 0,sizeof(serial_list_t) + extra_bytes);
		list->item_bytes = item_bytes;
		list->extra_bytes = extra_bytes;
	
		return (hserial_list_t) list;
	}
	return NULL;
}

void serial_list_dealloc(hserial_list_t hList,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list){
        if(list->data){
            mem_free(list->data);
        }
		mem_free(list);
	}
}

hany serial_list_extra(hserial_list_t hList,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list && list->extra_bytes >0){
		return (hbyte *)list  + sizeof(serial_list_t);
	}
	return NULL;
}

hint32 serial_list_extra_bytes(hserial_list_t hList,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list){
		return list->extra_bytes;
	}
	return 0;
}

hint32 serial_list_count(hserial_list_t hList,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list){
		return list->item_length;
	}
	return 0;
}

#define SERIAL_LIST_ITEM(list,i)	(hany)((list)->data? (list)->data + (i) * (list)->item_bytes : NULL)

hany serial_list_item_at(hserial_list_t hList,hint32 i,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list && i >=0 && i<list->item_length){
		return SERIAL_LIST_ITEM(list,i);
	}
	return NULL;
}

hany serial_list_item_add(hserial_list_t hList,hany value,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	hany item = NULL;
	if(list){
		_serial_list_data_extend(list,(list->item_length + 1) * list->item_bytes,InvokeTickArg);
		item = SERIAL_LIST_ITEM(list,list->item_length);
		if(value){
			memcpy(item, value, list->item_bytes);
		}
		else{
			memset(item, 0,list->item_bytes);
		}
		list->item_length ++;
		return item;
	}
	return NULL;
}

hany serial_list_item_insert(hserial_list_t hList,hany value,hint32 i,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	hany item = NULL;
	hint32 n;
	if(list && i>=0 && i<=list->item_length){
		_serial_list_data_extend(list,(list->item_length + 1) * list->item_bytes,InvokeTickArg);
		for(n = list->item_length;n > i;n--){
			memcpy(SERIAL_LIST_ITEM(list,n), SERIAL_LIST_ITEM(list,n-1), list->item_bytes);
		}
		item = SERIAL_LIST_ITEM(list,i);
		if(value){
			memcpy(item, value, list->item_bytes);
		}
		else{
			memset(item,0, list->item_bytes);
		}
		list->item_length ++;
		return item;
	}
	return NULL;
}

hbool serial_list_remove_at(hserial_list_t hList,hint32 i,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	hint32 n;
	if(list && i>=0 && i<list->item_length){
		
		for(n=i;n<list->item_length -1;n++){
			memcpy(SERIAL_LIST_ITEM(list,n), SERIAL_LIST_ITEM(list,n+1), list->item_bytes);
		}
		list->item_length --;
		
		return hbool_true;
	}
	return hbool_false;
}

hbool serial_list_clear(hserial_list_t hList,InvokeTickDeclare){
	serial_list_t * list = (serial_list_t *)hList;
	if(list ){
		list->item_length = 0;
		
		return hbool_true;
	}
	return hbool_false;
}

hany serial_list_item_last(hserial_list_t hList,InvokeTickDeclare){
    serial_list_t * list = (serial_list_t *)hList;
	if(list && list->item_length >0){
		return SERIAL_LIST_ITEM(list,list->item_length-1);
	}
	return NULL;
}

hbool serial_list_remove_last(hserial_list_t hList,InvokeTickDeclare){
    serial_list_t * list = (serial_list_t *)hList;
	if(list && list->item_length >0 ){
		list->item_length --;
		return hbool_true;
	}
	return hbool_false;
}

hint32 serial_list_index_of(hserial_list_t hList,hany item,serial_list_equal_t equal,InvokeTickDeclare){
    hint32 i;
    serial_list_t * list = (serial_list_t *)hList;
    for(i=0;i<list->item_length;i++){
        if((*equal)(hList,item, SERIAL_LIST_ITEM(list,i),InvokeTickArg)){
            return i;
        }
    }
    
    return -1;
}


