/*
 *  hrecursive.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-5.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hrecursive.h"
#include "hserial_list.h"

#undef recursive_alloc

#undef recursive_dealloc

#undef recursive_add_caller

#undef recursive_call

hrecursive_t recursive_alloc(InvokeTickDeclare){
	return (hrecursive_t)serial_list_alloc(sizeof(hany), 0);
}

void recursive_dealloc(hrecursive_t recursive,InvokeTickDeclare){
    serial_list_dealloc((hserial_list_t)recursive);
}

void recursive_add_caller(hrecursive_t recursive,hany caller,InvokeTickDeclare){
    serial_list_item_add((hserial_list_t)recursive, &caller);
}

void recursive_call(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,recursive_callback_t callback,InvokeTickDeclare){
	if(recursive && callback){
		hserial_list_t list = (hserial_list_t)recursive;
		hany * c;
        hany cc;
        serial_list_clear(list);
        serial_list_item_add(list, &caller);
		while((c = (hany *)serial_list_item_last(list))){
            cc =  * c;
            serial_list_remove_last(list);
			(*callback)(recursive,cc,arg0,arg1,arg2,arg3,result,InvokeTickArg);
		}
	}
}

