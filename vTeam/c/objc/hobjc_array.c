//
//  hobjc_array.c
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hobjc_array.h"
#include "hlist.h"

HOBJ_KEY_IMP(Array)
HOBJ_KEY_IMP(objectAt)
HOBJ_KEY_IMP(count)
HOBJ_KEY_IMP(add)
HOBJ_KEY_IMP(insert)
HOBJ_KEY_IMP(removeAt)
HOBJ_KEY_IMP(remove)
HOBJ_KEY_IMP(last)
HOBJ_KEY_IMP(removeLast)
HOBJ_KEY_IMP(clear)

static void hobj_array_method_init_impl (hobj_array_t * array,InvokeTickDeclare);

static void hobj_array_method_destroy_impl (hobj_array_t * array,InvokeTickDeclare);

HOBJ_CLASS_METHOD_IMP_BEGIN(Array)

HOBJ_CLASS_METHOD_IMP(init, hobj_array_method_init_impl)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_array_method_destroy_impl)

HOBJ_CLASS_METHOD_IMP(objectAt, hobj_array_objectAt)

HOBJ_CLASS_METHOD_IMP(count, hobj_array_count)

HOBJ_CLASS_METHOD_IMP(add, hobj_array_add)

HOBJ_CLASS_METHOD_IMP(insert, hobj_array_insert)

HOBJ_CLASS_METHOD_IMP(removeAt, hobj_array_removeAt)

HOBJ_CLASS_METHOD_IMP(remove, hobj_array_remove)

HOBJ_CLASS_METHOD_IMP(last, hobj_array_last)

HOBJ_CLASS_METHOD_IMP(removeLast, hobj_array_removeLast)

HOBJ_CLASS_METHOD_IMP(clear, hobj_array_clear)

HOBJ_CLASS_METHOD_IMP_END(Array)

HOBJ_CLASS_IMP_I(Array, hobj_array_t, &hobj_class);

hobj_t * hobj_array_objectAt(hobj_array_t * array,huint32 index,InvokeTickDeclare){
    if(array){
        return list_get(array->list, index);
    }
    return NULL;
}

huint32 hobj_array_count(hobj_array_t * array,InvokeTickDeclare){
    if(array){
        return list_count(array->list);
    }
    return 0;
}

hobj_t * hobj_array_add(hobj_array_t * array,hobj_t * item,InvokeTickDeclare){
    if(array && item){
        if(array->list == NULL){
            array->list = list_alloc(20, 20);
        }
        hobj_retain(item, InvokeTickArg);
        list_add(array->list, item);
        return item;
    }
    return NULL;
}

hobj_t * hobj_array_insert(hobj_array_t * array,hobj_t * item,huint16 index,InvokeTickDeclare){
    if(array && item ){
        if(array->list == NULL){
            array->list = list_alloc(20, 20);
        }
        hobj_retain(item, InvokeTickArg);
        list_insert(array->list, item,index);
        return item;
    }
    return NULL;
}

hobj_t * hobj_array_removeAt(hobj_array_t * array,huint32 index,InvokeTickDeclare){
    if(array && index < list_count(array->list)){
        hobj_t * v = list_get(array->list, index);
        hobj_autorelease(v, InvokeTickArg);
        list_remove_at(array->list, index);
        return v;
    }
    return NULL;
}

hobj_t * hobj_array_remove(hobj_array_t * array,hobj_t * item,InvokeTickDeclare){
    hint32 c;
    if(array && item && (c = list_count(array->list)) >0){
        list_remove(array->list, item);
        if(c != list_count(array->list)){
            return hobj_autorelease(item, InvokeTickArg);
        }
    }
    return NULL;
}

hobj_t * hobj_array_last(hobj_array_t * array,InvokeTickDeclare){
    return list_last(array->list);
}

hobj_t * hobj_array_removeLast(hobj_array_t * array,InvokeTickDeclare){
    hobj_t * v = list_last(array->list);
    list_remove_last(array->list);
    if(v){
        return hobj_autorelease(v, InvokeTickArg);
    }
    return NULL;
}

void hobj_array_clear(hobj_array_t * array,InvokeTickDeclare){
    hint32 i,c = list_count(array->list);
    for(i=0;i<c;i++){
        hobj_release((hobj_t *) list_get(array->list, i), InvokeTickArg);
    }
    list_clear(array->list);
}

void hobj_array_swap(hobj_array_t * array,hint32 index1,hint32 index2,InvokeTickDeclare){
    list_swap(array->list, index1, index2);
}

hobj_array_t * hobj_array_alloc(InvokeTickDeclare){
    return (hobj_array_t *)hobj_alloc(&hobj_Array_class, InvokeTickArg);
}

hobj_array_t * hobj_array_alloc_with_objects(InvokeTickDeclare,...){
    hobj_array_t * array = (hobj_array_t *)hobj_alloc(&hobj_Array_class, InvokeTickArg);
    hobj_t * obj;
    va_list va;
    va_start(va, __InvokeTick__);
    while((obj = va_arg(va, hobj_t *))){
        hobj_array_add(array, obj, InvokeTickArg);
    }
    va_end(va);
    return array;
}

hobj_array_t * hobj_array_new(InvokeTickDeclare){
    hobj_array_t * array = (hobj_array_t *)hobj_alloc(&hobj_Array_class, InvokeTickArg);
    return (hobj_array_t *)hobj_autorelease((hobj_t *)array, InvokeTickArg);
}

hobj_array_t * hobj_array_new_with_objects(InvokeTickDeclare,...){
    hobj_array_t * array = (hobj_array_t *)hobj_alloc(&hobj_Array_class, InvokeTickArg);
    hobj_t * obj;
    va_list va;
    va_start(va, __InvokeTick__);
    while((obj = va_arg(va, hobj_t *))){
        hobj_array_add(array, obj, InvokeTickArg);
    }
    va_end(va);
    return (hobj_array_t *)hobj_autorelease((hobj_t *)array, InvokeTickArg);
}


static void hobj_array_method_init_impl (hobj_array_t * array,InvokeTickDeclare){
    hobj_method_t * method = hobj_class_method(array->base.clazz->superClass, HOBJ_KEY(init), InvokeTickArg);
    if(method){
        (* (hobj_method_init_impl_t) method->imp)((hobj_t *)array,InvokeTickArg);
    }
}

static void hobj_array_method_destroy_impl (hobj_array_t * array,InvokeTickDeclare){
    hobj_method_t * method;
    hint32 i,c = list_count(array->list);
    for(i=0;i<c;i++){
        hobj_release((hobj_t *) list_get(array->list, i), InvokeTickArg);
    }
    list_dealloc(array->list);
    array->list = NULL;
    method = hobj_class_method(array->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *)array,InvokeTickArg);
    }
}




