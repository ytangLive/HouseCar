/*
 *  hclass.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hclass.h"

#undef hclass_alloc

#undef hclass_super_class

#undef hclass_is_kind_class

#undef hclass_size

#undef hclass_total_size

#undef hobject_class

#undef hobject_super_class

#undef hobject_is_kind_class

#undef hobject_ptr

#undef hobject_ptr_by_super

#undef hobject_ptr_by_class

typedef struct{
	hclass_t super_class;
	hint32 size;
	hint32 total_size;
	hclass_init_t init;
	hclass_destroy_t destroy;
}class_t;


hclass_t hclass_alloc(hclass_t super_class,hint32 size,hclass_init_t init,hclass_destroy_t destroy,InvokeTickDeclare){
	hint32 total_size =  hclass_total_size(super_class,InvokeTickArg) + size;
	class_t * clazz = (class_t *)mem_malloc(sizeof(class_t));
	clazz->super_class = super_class;
	clazz->size = size;
	clazz->total_size = total_size;
	clazz->init = init;
	clazz->destroy = destroy;
	return (hclass_t)clazz;
}

hclass_t hclass_super_class(hclass_t hclass,InvokeTickDeclare){
	return hclass?((class_t *)hclass)->super_class:NULL;
}

hbool hclass_is_kind_class(hclass_t subclass,hclass_t hclass,InvokeTickDeclare){
    hclass_t clazz = subclass;
    while(clazz){
        if(clazz == hclass){
            return hbool_true;
        }
        clazz = hclass_super_class(clazz,InvokeTickArg);
    }
    return hbool_false;
}

hint32 hclass_size(hclass_t hclass,InvokeTickDeclare){
	return hclass?((class_t *)hclass)->size:0;
}

hint32 hclass_total_size(hclass_t hclass,InvokeTickDeclare){
	return hclass?((class_t *)hclass)->total_size:0;
}





typedef struct{
	hclass_t hclass;
}object_t;

static void hclass_object_init(hobject_t obj,class_t * hclass,InvokeTickDeclare){
	if(hclass){
		hclass_object_init(obj,(class_t *)hclass->super_class,InvokeTickArg);
		if(hclass->init){
			(*hclass->init)((hclass_t)hclass,obj,hobject_ptr_by_class(obj,(hclass_t)hclass,InvokeTickArg),InvokeTickArg);
		}
	}
}

static void hclass_object_destroy(hobject_t obj,class_t * hclass,InvokeTickDeclare){
	if(hclass){
		if(hclass->destroy){
			(*hclass->destroy)((hclass_t)hclass,obj,hobject_ptr_by_class(obj, (hclass_t)hclass,InvokeTickArg),InvokeTickArg);
		}
		hclass_object_destroy(obj,(class_t *)hclass->super_class,InvokeTickArg);
	}
}

static void ext_hobject_init(hext_obj_t ext_obj,hint32 size,hany arg,InvokeTickDeclare){
	hclass_t hclass =(hclass_t)arg;
	hint32 length = hclass_total_size(hclass,InvokeTickArg) + sizeof(object_t);
	object_t *obj = (object_t *)mem_malloc( length);
	memset(obj,0,length);
	obj->hclass = hclass;
	*(object_t  **) ext_obj = obj;
	hclass_object_init((hobject_t)ext_obj,(class_t *) hclass,InvokeTickArg);
}

static void ext_hobject_destroy(hext_obj_t ext_obj,hint32 size,InvokeTickDeclare){
	hobject_t obj = (hobject_t )ext_obj;
	hclass_object_destroy(obj,(class_t *)hobject_class(obj,InvokeTickArg),InvokeTickArg);
	mem_free(*(object_t **)obj);
}

ext_class_t ext_hobject_class = {ext_hobject_init,ext_hobject_destroy,sizeof(hobject_t)};

#define OBJECT_GET(obj)		(obj ? (*(object_t **)obj):NULL)


hclass_t hobject_class(hobject_t hobj,InvokeTickDeclare){
	object_t * obj = OBJECT_GET(hobj);
	return obj?obj->hclass:NULL;
}

hclass_t hobject_super_class(hobject_t hobj,InvokeTickDeclare){
	return hclass_super_class(hobject_class(hobj,InvokeTickArg),InvokeTickArg);
}

hany hobject_ptr(hobject_t hobj,InvokeTickDeclare){
	return hobject_ptr_by_class(hobj, hobject_class(hobj,InvokeTickArg),InvokeTickArg);
}

hany hobject_ptr_by_super(hobject_t hobj,InvokeTickDeclare){
	return hobject_ptr_by_class(hobj, hobject_super_class(hobj,InvokeTickArg),InvokeTickArg);
}

hany hobject_ptr_by_class(hobject_t hobj,hclass_t hclass,InvokeTickDeclare){
	object_t * obj = OBJECT_GET(hobj);
	if(obj && hclass){
		hint32 size = hclass_size(hclass,InvokeTickArg);
		hint32 total_size = hclass_total_size(hclass,InvokeTickArg);
		if(size >0){
			return (hbyte *)obj + sizeof(object_t) + total_size - size;
		}
	}
	return NULL;
}

hbool hobject_is_kind_class(hobject_t hobj,hclass_t hclass,InvokeTickDeclare){
    return hclass_is_kind_class(hobject_class(hobj,InvokeTickArg),hclass,InvokeTickArg);
}
