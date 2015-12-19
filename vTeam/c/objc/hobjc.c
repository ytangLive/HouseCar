//
//  hobjc.c
//  hclib
//
//  Created by Zhang Hailong on 13-3-3.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hobjc.h"
#include "hobjc_value.h"
#include "hbuffer.h"
#include "hthread.h"
#include "hlist.h"
#include "hlog.h"
#include "hmap.h"
#include "hserial_list.h"

HOBJ_TYPE_IMP(Integer,hint32)
HOBJ_TYPE_IMP(Int64,hint64)
HOBJ_TYPE_IMP(Double,hdouble)
HOBJ_TYPE_IMP(Boolean,hbool)
HOBJ_TYPE_IMP(Float,hfloat)
HOBJ_TYPE_IMP(String,hcchar *)
HOBJ_TYPE_IMP(Void,void)
HOBJ_TYPE_IMP(Object,hobj_t *)
HOBJ_TYPE_IMP(Bytes,hbyte *)

hobj_key_t hobj_key_initialize = {"initialize"};
hobj_key_t hobj_key_init = {"__init"};
hobj_key_t hobj_key_destroy = {"__destroy"};
hobj_key_t hobj_key_Object = {"Object"};
hobj_key_t hobj_key_propertyGet = {"propertyGet"};
hobj_key_t hobj_key_propertySet = {"propertySet"};
hobj_key_t hobj_key_valueForKey = {"valueForKey"};
hobj_key_t hobj_key_setValueForKey = {"setValueForKey"};
hobj_key_t hobj_key_valueForKeyPath = {"valueForKeyPath"};
hobj_key_t hobj_key_setValueForKeyPath = {"setValueForKeyPath"};
hobj_key_t hobj_key_toString = {"toString"};
hobj_key_t hobj_key_toDouble = {"toDouble"};
hobj_key_t hobj_key_copy = {"copy"};

static void hobj_init(hobj_t * obj,InvokeTickDeclare);

static void hobj_destroy(hobj_t * obj,InvokeTickDeclare);

static hobj_t * hobj_property_get_impl(hobj_t * obj,hobj_property_t * property,InvokeTickDeclare);

static void hobj_property_set_impl(hobj_t * obj,hobj_property_t * property,hobj_t * value,InvokeTickDeclare);

static hobj_t * hobj_method_valueForKey_impl(hobj_t * obj,hcchar * key,InvokeTickDeclare);

static void hobj_method_setValueForKey_impl(hobj_t * obj,hcchar * key,hobj_t * value,InvokeTickDeclare);

static hobj_t * hobj_method_valueForKeyPath_impl(hobj_t * obj,hcchar * keyPath,InvokeTickDeclare);

static void hobj_method_setValueForKeyPath_impl(hobj_t * obj,hcchar * keyPath,hobj_t * value,InvokeTickDeclare);

static hobj_t * hobj_method_toString_impl(hobj_t * obj,InvokeTickDeclare);

static hdouble hobj_method_toDouble_impl(hobj_t * obj,InvokeTickDeclare);

static hobj_t * hobj_method_copy_impl(hobj_t * obj,InvokeTickDeclare);


static hobj_method_t hobj_class_methods[] = {
    {&hobj_key_init,(hobj_imp_t) hobj_init},
    {&hobj_key_destroy,(hobj_imp_t) hobj_destroy},
    {&hobj_key_propertyGet,(hobj_imp_t) hobj_property_get_impl},
    {&hobj_key_propertySet,(hobj_imp_t) hobj_property_set_impl},
    {&hobj_key_valueForKey,(hobj_imp_t) hobj_method_valueForKey_impl},
    {&hobj_key_setValueForKey,(hobj_imp_t) hobj_method_setValueForKey_impl},
    {&hobj_key_valueForKeyPath,(hobj_imp_t) hobj_method_valueForKeyPath_impl},
    {&hobj_key_setValueForKeyPath,(hobj_imp_t) hobj_method_setValueForKeyPath_impl},
    {&hobj_key_toString,(hobj_imp_t) hobj_method_toString_impl},
    {&hobj_key_toDouble,(hobj_imp_t) hobj_method_toDouble_impl},
    {&hobj_key_copy,(hobj_imp_t) hobj_method_copy_impl},
};

hobj_class_t hobj_class = {
    NULL,&hobj_key_Object,sizeof(hobj_t),NULL,0,hobj_class_methods,sizeof(hobj_class_methods) / sizeof(hobj_method_t)
};

hobj_t * hobj_alloc(hobj_class_t * clazz,InvokeTickDeclare){
    hobj_t * obj = mem_malloc(clazz->size);
    hobj_method_t * method;
    mem_memset(obj, 0, clazz->size);
    
    hobj_class_initialize(clazz, InvokeTickArg);
    
    obj->retainCount = 1;
    obj->clazz = clazz;
    method = hobj_method(obj, &hobj_key_init, InvokeTickArg);
    if(method){
        (*(hobj_method_init_impl_t) method->imp)(obj,InvokeTickArg);
    }
    
    return obj;
}

hobj_t * hobj_new(hobj_class_t * clazz,InvokeTickDeclare){
    return hobj_autorelease(hobj_alloc(clazz,InvokeTickArg), InvokeTickArg);
}

hobj_t * hobj_retain(hobj_t * obj,InvokeTickDeclare){
    if(obj){
        obj->retainCount ++;
    }
    return obj;
}

void hobj_release(hobj_t * obj,InvokeTickDeclare){
    if(obj){
        hobj_method_t * method;
        assert(obj->retainCount > 0);
        if( -- obj->retainCount == 0){
            method = hobj_method(obj, &hobj_key_destroy, InvokeTickArg);
            if(method){
                (*(hobj_method_init_impl_t) method->imp)(obj,InvokeTickArg);
            }
            mem_free(obj);
        }
    }
}

static hthread_key_t hobj_autorelease_pool_thread_key = NULL;

static void hobj_atutorelease_pool_array_dealloc(void * array){
    InvokeTickBegin
    list_dealloc(array);
}

void hobj_autorelease_pool_begin(void){
    
    InvokeTickBegin
    hlist_t array, pool;
    
    if(hobj_autorelease_pool_thread_key == NULL){
        hobj_autorelease_pool_thread_key = hthread_key_create(hobj_atutorelease_pool_array_dealloc);
    }

    array = (hlist_t)hthread_key_value(hobj_autorelease_pool_thread_key);
    
    if(array == NULL){
        array = list_alloc(20, 20);
        hthread_key_value_set(hobj_autorelease_pool_thread_key, array);
    }
    
    pool = list_alloc(20, 20);
    
    list_add(array, pool);
}

static void hobj_autorelease_pool_callback(void * userInfo){
    InvokeTickBegin
    hint32 i,c = list_count((hlist_t) userInfo);
    if(c >0){
        hobj_autorelease_pool_begin();
        for(i=0;i<c;i++){
            hobj_release((hobj_t *) list_get(userInfo, i), InvokeTickArg);
        }
        hobj_autorelease_pool_end();
    }
    list_dealloc((hlist_t) userInfo);
}

void hobj_autorelease_pool_end(void){
    
    InvokeTickBegin
    
    hlist_t array, pool;

    if(hobj_autorelease_pool_thread_key){
        array = (hlist_t)hthread_key_value(hobj_autorelease_pool_thread_key);
        if(array){
            pool = list_last(array);
            list_remove_last(array);
            if(pool){
                hobj_after_caller_async(hobj_autorelease_pool_callback, pool);
            }
            else{
                hlog("not found hobj autorelease pool");
            }
        }
        else{
            hlog("not found hobj autorelease pool");
        }
    }
    else{
        hlog("not found hobj autorelease pool");
    }
}

hobj_t * hobj_autorelease(hobj_t * obj,InvokeTickDeclare){
    if(obj){
        hlist_t array, pool;
        if(hobj_autorelease_pool_thread_key){
            array = (hlist_t)hthread_key_value(hobj_autorelease_pool_thread_key);
            if(array){
                pool = list_last(array);
                if(pool){
                    list_add(pool, obj);
                }
                else{
                    hlog("not found hobj autorelease pool");
                }
            }
            else{
                hlog("not found hobj autorelease pool");
            }
        }
        else{
            hlog("not found hobj autorelease pool");
        }
    }
    return obj;
}

hobj_property_t * hobj_class_property(hobj_class_t * clazz,hobj_key_t * key,InvokeTickDeclare){
    hint32 c = clazz->propertyCount;
    hobj_property_t * prop = clazz->propertys;
    while(c >0){
        if(prop->name == key){
            return prop;
        }
        c -- ;
        prop ++;
    }
    if(clazz->superClass){
        return hobj_class_property(clazz->superClass, key, InvokeTickArg);
    }
    return NULL;
}

hobj_property_t * hobj_class_property_for_name(hobj_class_t * clazz,hcchar * name,InvokeTickDeclare){
    hint32 c = clazz->propertyCount;
    hobj_property_t * prop = clazz->propertys;
    while(c >0){
        if(prop->name->name == name || strcmp(prop->name->name, name) ==0){
            return prop;
        }
        c -- ;
        prop ++;
    }
    if(clazz->superClass){
        return hobj_class_property_for_name(clazz->superClass, name, InvokeTickArg);
    }
    return NULL;
}

hobj_method_t * hobj_class_method(hobj_class_t * clazz,hobj_key_t * key,InvokeTickDeclare){
    hint32 c = clazz->methodCount;
    hobj_method_t * method = clazz->methods;
    while(c >0){
        if(method->name == key){
            return method;
        }
        c--;
        method ++;
    }
    if(clazz->superClass){
        return hobj_class_method(clazz->superClass, key, InvokeTickArg);
    }
    return NULL;
}

hobj_method_t * hobj_class_method_for_name(hobj_class_t * clazz,hcchar * name,InvokeTickDeclare){
    hint32 c = clazz->methodCount;
    hobj_method_t * method = clazz->methods;
    while(c >0){
        if(method->name->name == name || strcmp(method->name->name, name) == 0){
            return method;
        }
        c--;
        method ++;
    }
    if(clazz->superClass){
        return hobj_class_method_for_name(clazz->superClass, name, InvokeTickArg);
    }
    return NULL;
}

hobj_property_t * hobj_property(hobj_t * obj,hobj_key_t * key,InvokeTickDeclare){
    return hobj_class_property(obj->clazz, key, InvokeTickArg);
}

hobj_property_t * hobj_property_for_name(hobj_t * obj,hcchar * name,InvokeTickDeclare){
    return hobj_class_property_for_name(obj->clazz, name, InvokeTickArg);
}

hobj_method_t * hobj_method(hobj_t * obj,hobj_key_t * key,InvokeTickDeclare){
    return hobj_class_method(obj->clazz, key, InvokeTickArg);
}

hobj_method_t * hobj_method_for_name(hobj_t * obj,hcchar * name,InvokeTickDeclare){
    return hobj_class_method_for_name(obj->clazz, name, InvokeTickArg);
}

hbool hobj_is_kind(hobj_t * obj,hobj_class_t * clazz,InvokeTickDeclare){
    if(obj){
        return hobj_class_is_kind(obj->clazz, clazz, InvokeTickArg);
    }
    return hbool_false;
}

hbool hobj_class_is_kind(hobj_class_t * clazz,hobj_class_t * kindClazz,InvokeTickDeclare){
    hobj_class_t * c = clazz;
    while(c && c != kindClazz){
        c = c->superClass;
    }
    return c != NULL;
}


hobj_t * hobj_propertyGet(hobj_t * obj,hobj_property_t * property,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_propertyGet, InvokeTickArg);
    if(method){
        return (* (hobj_method_propertyGet_impl_t)method->imp)(obj,property,InvokeTickArg);
    }
    return NULL;
}

void hobj_propertySet(hobj_t * obj,hobj_property_t * property,hobj_t * value,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_propertyGet, InvokeTickArg);
    if(method){
        (* (hobj_method_propertySet_impl_t)method->imp)(obj,property,value,InvokeTickArg);
    }
}

hobj_t * hobj_valueForKey(hobj_t * obj,hcchar * key,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_valueForKey, InvokeTickArg);
    if(method){
        return (* (hobj_method_valueForKey_imp_t) method->imp)(obj,key,InvokeTickArg);
    }
    return NULL;
}

void hobj_setValueForKey(hobj_t * obj,hcchar * key,hobj_t * value,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_setValueForKey, InvokeTickArg);
    if(method){
        (* (hobj_method_setValueForKey_imp_t) method->imp)(obj,key,value,InvokeTickArg);
    }
}

hobj_t * hobj_valueForKeyPath(hobj_t * obj,hcchar * keyPath,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_valueForKeyPath, InvokeTickArg);
    if(method){
        return (* (hobj_method_valueForKeyPath_imp_t) method->imp)(obj,keyPath,InvokeTickArg);
    }
    return NULL;
}

void hobj_setValueForKeyPath(hobj_t * obj,hcchar * keyPath,hobj_t * value,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_setValueForKeyPath, InvokeTickArg);
    if(method){
        (* (hobj_method_setValueForKey_imp_t) method->imp)(obj,keyPath,value,InvokeTickArg);
    }
}

hobj_t * hobj_toString(hobj_t * obj,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_toString, InvokeTickArg);
    if(method){
        return (* (hobj_method_toString_imp_t) method->imp)(obj,InvokeTickArg);
    }
    return NULL;
}

hcchar * hobj_toCString(hobj_t * obj,InvokeTickDeclare){
    hobj_t * v = hobj_toString(obj,InvokeTickArg);
    if(v && hobj_is_kind(v, &hobj_Value_class, InvokeTickArg)){
        return hobj_value_stringValue((hobj_value_t *) v, InvokeTickArg);
    }
    return NULL;
}

hobj_t * hobj_copy(hobj_t * obj,InvokeTickDeclare){
    hobj_method_t * method = hobj_method(obj, &hobj_key_copy, InvokeTickArg);
    if(method){
        return (* (hobj_method_copy_imp_t) method->imp)(obj,InvokeTickArg);
    }
    return NULL;
}

void hobj_invoke(hobj_t * obj,hobj_method_t * method,hobj_t * userInfo,InvokeTickDeclare){
    (* ((void (*)(hobj_t * ,hobj_t * ,InvokeTickDeclare)) method->imp))(obj,userInfo,InvokeTickArg);
}


static hthread_key_t hobj_invoke_after_caller_thread_key = NULL;

static void hobj_invoke_after_caller_dealloc(void * ctx){
    free(ctx);
}

typedef struct{
    void (* callback) (void * ,void *);
    void * context;
} hobj_invoke_after_caller_context_t;


void hobj_after_caller_install(void (* callback) (void * ,void *),void * context){
    hobj_invoke_after_caller_context_t * ctx = NULL;
    if(hobj_invoke_after_caller_thread_key == NULL){
        hobj_invoke_after_caller_thread_key = hthread_key_create(hobj_invoke_after_caller_dealloc);
    }
    ctx = hthread_key_value(hobj_invoke_after_caller_thread_key);
    if(ctx){
        ctx->callback = callback;
        ctx->context = context;
    }
    else{
        ctx = (hobj_invoke_after_caller_context_t *) malloc(sizeof(hobj_invoke_after_caller_context_t));
        memset(ctx, 0, sizeof(hobj_invoke_after_caller_context_t));
        ctx->callback = callback;;
        ctx->context = context;
        hthread_key_value_set(hobj_invoke_after_caller_thread_key, ctx);
    }
}

void hobj_after_caller_async(void (* callback)(void *) ,void * userInfo){
    hobj_invoke_after_caller_context_t * ctx = NULL;
    if(hobj_invoke_after_caller_thread_key){
        ctx = (hobj_invoke_after_caller_context_t *)hthread_key_value(hobj_invoke_after_caller_thread_key);
    }
    if(ctx){
        ( * ctx->callback)(ctx->context,userInfo);
    }
    else{
        ( * callback)(userInfo);
    }
}

typedef struct {
    hobj_t * obj;
    hobj_method_t * method;
    hobj_t * userInfo;
} hobj_invoke_after_user_info_t;

static void hobj_invoke_after_callback(void * userInfo){
    hobj_invoke_after_user_info_t * u = (hobj_invoke_after_user_info_t *) userInfo;
    
    hobj_invoke(u->obj, u->method, u->userInfo, InvokeTickRoot);
    
    hobj_release(u->obj, InvokeTickRoot);
    
    hobj_release(u->userInfo, InvokeTickRoot);
    
    free(u);
}

void hobj_invoke_after(hobj_t * obj,hobj_method_t * method,hobj_t * userInfo,InvokeTickDeclare){
    
    hobj_invoke_after_user_info_t * u = (hobj_invoke_after_user_info_t *) malloc(sizeof(hobj_invoke_after_user_info_t));
    
    memset(u, 0, sizeof(hobj_invoke_after_user_info_t));
    
    u->obj = hobj_retain(obj, InvokeTickArg);
    u->method = method;
    u->userInfo = hobj_retain(userInfo, InvokeTickArg);
    
    hobj_after_caller_async(hobj_invoke_after_callback,u);
}

static hmap_t hobj_classs = NULL;

void hobj_class_initialize(hobj_class_t * clazz,InvokeTickDeclare){
    if(clazz && !clazz->initialized){
        hint32 c = clazz->methodCount;
        hobj_method_t * method = clazz->methods;
        
        if(hobj_classs == NULL){
            hobj_classs = map_alloc(hash_code_str, equal_str);
        }
        map_put(hobj_classs, (hany)clazz->name->name, clazz);
        while(c >0){
            if(method->name == &hobj_key_initialize){
                (* (hobj_class_initialize_imp_t) method->imp)(clazz,InvokeTickArg);
                break;
            }
            c--;
            method ++;
        }
        clazz->initialized = hbool_true;
        if(clazz->superClass){
            hobj_class_initialize(clazz->superClass,InvokeTickArg);
        }
    }
}

hobj_class_t * hobj_class_find(hcchar * className,InvokeTickDeclare){
    return (hobj_class_t *)map_get(hobj_classs, (hany)className);
}

static hobj_t * hobj_property_get_impl(hobj_t * obj,hobj_property_t * property,InvokeTickDeclare){
    if(obj && property && property->getter){
        if(property->type == HOBJ_TYPE_INTEGER){
            hint32 v = HOBJPropertyValueGet(obj,property,hint32,InvokeTickArg);
            return (hobj_t *)hobj_value_int_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_INT64){
            hint64 v = HOBJPropertyValueGet(obj,property,hint64,InvokeTickArg);
            return (hobj_t *)hobj_value_int64_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_DOUBLE){
            hdouble v = HOBJPropertyValueGet(obj,property,hdouble,InvokeTickArg);
            return (hobj_t *)hobj_value_double_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_FLOAT){
            hfloat v = HOBJPropertyValueGet(obj,property,hfloat,InvokeTickArg);
            return (hobj_t *)hobj_value_float_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_STRING){
            hcchar * v = HOBJPropertyValueGet(obj,property,hcchar *,InvokeTickArg);
            return (hobj_t *)hobj_value_string_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_BOOLEAN){
            hbool v = HOBJPropertyValueGet(obj,property,hbool,InvokeTickArg);
            return (hobj_t *)hobj_value_boolean_new(v, InvokeTickArg);
        }
        else if(property->type == HOBJ_TYPE_OBJECT){
            return HOBJPropertyValueGet(obj,property,hobj_t *,InvokeTickArg);
        }
    }
    return NULL;
}

static void hobj_property_set_impl(hobj_t * obj,hobj_property_t * property,hobj_t * value,InvokeTickDeclare){
    if(obj && property && property->setter){
        if(property->type == HOBJ_TYPE_INTEGER){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hint32 v = hobj_value_intValue((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hint32, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_INT64){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hint64 v = hobj_value_int64Value((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hint64, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_DOUBLE){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hdouble v = hobj_value_doubleValue((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hdouble, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_FLOAT){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hfloat v = hobj_value_floatValue((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hfloat, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_STRING){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hcchar * v = hobj_value_stringValue((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hcchar *, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_BOOLEAN){
            if(hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
                hbool v = hobj_value_booleanValue((hobj_value_t *)value, InvokeTickArg);
                HOBJPropertyValueSet(obj, property, hbool, v, InvokeTickArg);
            }
        }
        else if(property->type == HOBJ_TYPE_OBJECT){
            HOBJPropertyValueSet(obj, property, hobj_t *, value, InvokeTickArg);
        }
    }

}

static void hobj_init(hobj_t * obj,InvokeTickDeclare){
    
}

static void hobj_destroy(hobj_t * obj,InvokeTickDeclare){
    
}


static hobj_t * hobj_method_valueForKey_impl(hobj_t * obj,hcchar * key,InvokeTickDeclare){
    hobj_property_t * prop = hobj_property_for_name(obj, key, InvokeTickArg);
    if(prop){
        return hobj_propertyGet(obj, prop, InvokeTickArg);
    }
    return NULL;
}

static void hobj_method_setValueForKey_impl(hobj_t * obj,hcchar * key,hobj_t * value,InvokeTickDeclare){
    hobj_property_t * prop = hobj_property_for_name(obj, key, InvokeTickArg);
    if(prop){
        hobj_propertySet(obj, prop, value, InvokeTickArg);
    }
}

static hobj_t * hobj_method_valueForKeyPath_impl(hobj_t * obj,hcchar * keyPath,InvokeTickDeclare){
    hbuffer_t key = buffer_alloc(64, 64);
    hchar * p = (hchar *) keyPath;
    hobj_t * v = NULL;
    while(p && *p != '\0' && *p !='.'){
        buffer_append(key, p, 1);
        p++;
    }
    
    v = hobj_valueForKey(obj, buffer_to_str(key), InvokeTickArg);

    buffer_dealloc(key);
    
    if(*p == '.'){
        if(v){
            v = hobj_valueForKeyPath(obj, p + 1, InvokeTickArg);
        }
    }

    return v;
}

static void hobj_method_setValueForKeyPath_impl(hobj_t * obj,hcchar * keyPath,hobj_t * value,InvokeTickDeclare){
    hbuffer_t key = buffer_alloc(64, 64);
    hchar * p = (hchar *) keyPath;
    hobj_t * v = NULL;
    while(p && *p != '\0' && *p !='.'){
        buffer_append(key, p, 1);
        p++;
    }
    
    if( *p == '.' ){
        v = hobj_valueForKey(obj, buffer_to_str(key), InvokeTickArg);
        if(v){
            hobj_setValueForKeyPath(v, p + 1, value, InvokeTickArg);
        }
    }
    
    
    if(*p == '.'){
        if(v){
            v = hobj_valueForKeyPath(obj, p + 1, InvokeTickArg);
        }
    }
    
    buffer_dealloc(key);

}

static hobj_t * hobj_method_toString_impl(hobj_t * obj,InvokeTickDeclare){
    if(obj){
        return (hobj_t *) hobj_value_string_new_format("[%s]",InvokeTickArg, obj->clazz->name->name) ;
    }
    return NULL;
}

static hdouble hobj_method_toDouble_impl(hobj_t * obj,InvokeTickDeclare){
    return 0.0;
}

static hobj_t * hobj_method_copy_impl(hobj_t * obj,InvokeTickDeclare){
    return obj;
}
