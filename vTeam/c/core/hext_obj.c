/*
 *  ext_obj.c
 *  C_Library
 *
 *  Created by hailong zhang on 10-9-16.
 *  Copyright 2010 hailong. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hext_obj.h"
#include "hmem.h"

#undef ext_obj_alloc

#undef ext_obj_class

#undef ext_obj_retain_count

#undef ext_obj_retain

#undef ext_obj_release

#undef ext_bytes_obj_alloc

#undef ext_bytes_obj_length

#undef ext_bytes_obj_data

#undef ext_bytes_from_data

#undef ext_obj_alloc_with_extra

typedef struct{
	hint32 length;
	hbyte * data;
}ext_bytes_t;

static void _extern_bytes_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	ext_bytes_t * data = (ext_bytes_t *)obj;
	data->length = (hint32)(hintptr) arg;
	data->data = mem_malloc(data->length + sizeof(hext_obj_t));
	mem_memcpy(data->data,& obj, sizeof(hext_obj_t));
}

static void _extern_bytes_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	ext_bytes_t * data = (ext_bytes_t *)obj;
	mem_free(data->data);
}

ext_class_t ext_bytes_class={_extern_bytes_init,_extern_bytes_destroy,(hint32)sizeof(ext_bytes_t)};

hext_obj_t ext_bytes_obj_alloc(hint32 size,InvokeTickDeclare){
    if(size>0){
        return ext_obj_alloc((hany)(hintptr)size, &ext_bytes_class,InvokeTickArg);
    }
    return NULL;
}

hint32 ext_bytes_obj_length(hext_obj_t data_obj_t,InvokeTickDeclare){
	ext_bytes_t * data = (ext_bytes_t *) data_obj_t;
	return data?data->length:0;
}

hchar * ext_bytes_obj_data(hext_obj_t data_obj_t,InvokeTickDeclare){
	ext_bytes_t * data = (ext_bytes_t *) data_obj_t;
	return data?data->data + sizeof(hext_obj_t):NULL;
}

hext_obj_t ext_bytes_from_data(hbyte * data,InvokeTickDeclare){
	return data? *(hext_obj_t *)(data - sizeof(hext_obj_t)):NULL;
}

typedef struct {
	hint32 retain_count;
	const ext_class_t * ext_class;
    hint32 length;
}ext_obj_t;

hext_obj_t ext_obj_alloc(hany arg,const ext_class_t * ext_class,InvokeTickDeclare){
	if(ext_class){
		return ext_obj_alloc_with_extra(0,arg,ext_class,InvokeTickArg);
	}
	return NULL;
}

hext_obj_t ext_obj_alloc_with_extra(hint32 extra,hany arg,const ext_class_t * ext_class,InvokeTickDeclare){
    if(ext_class){
		hint32 obj_length  = sizeof(hint32) + ext_class->size + extra + sizeof(ext_obj_t);
		hbyte * obj_bytes = (hbyte *)mem_malloc( obj_length);
		hext_obj_t hobj = (hext_obj_t)(obj_bytes + sizeof(ext_obj_t));
		ext_obj_t * obj = (ext_obj_t *)(obj_bytes);
		mem_memset(obj_bytes,0,obj_length);
		obj->ext_class = ext_class;
		obj->retain_count = 1;
        obj->length = obj_length;
		
		if(ext_class && ext_class->init){
			(*ext_class->init)(hobj,ext_class->size,arg,InvokeTickArg);
		}
		
		return hobj;
	}
	return NULL;
}

#define EXT_OBJ_GET(obj) (obj ? ((ext_obj_t *) ((hbyte *)obj - sizeof(ext_obj_t)) ):NULL)

const ext_class_t * ext_obj_class(hext_obj_t ext_obj,InvokeTickDeclare){
	ext_obj_t * obj = EXT_OBJ_GET(ext_obj);
	return obj?obj->ext_class:NULL;
}


hint32 ext_obj_retain_count(hext_obj_t ext_obj,InvokeTickDeclare){
	ext_obj_t * obj = EXT_OBJ_GET(ext_obj);
	return obj?obj->retain_count:0;
}


hext_obj_t ext_obj_retain(hext_obj_t ext_obj,InvokeTickDeclare){

	ext_obj_t * obj = EXT_OBJ_GET(ext_obj);
	if(obj){
		obj->retain_count ++;
	}
	return ext_obj;
}


void ext_obj_release(hext_obj_t ext_obj,InvokeTickDeclare){
	ext_obj_t * obj = EXT_OBJ_GET(ext_obj);
	if(obj){
        --obj->retain_count;
        assert(obj->retain_count>=0);
        if(obj->retain_count ==0){
            if(obj->ext_class && obj->ext_class->destroy){
                (*obj->ext_class->destroy)(ext_obj,obj->ext_class->size,InvokeTickArg);
            }
            mem_free( (hbyte *) obj );
        }
	}
}

