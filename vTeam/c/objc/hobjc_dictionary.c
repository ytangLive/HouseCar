//
//  hobj_dictionary.c
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hobjc_dictionary.h"
#include "hlist.h"
#include "hmap.h"
#include "hserial_list.h"
#include "hobjc_value.h"

HOBJ_KEY_IMP(Dictionary)
HOBJ_KEY_IMP(removeForKey)
HOBJ_KEY_IMP(keys)

static hobj_t * hobj_dictionary_valueForKey(hobj_dictionary_t * dict,hcchar * key,InvokeTickDeclare);

static void hobj_dictionary_setValueForKey(hobj_dictionary_t * dict,hcchar * key,hobj_t * value,InvokeTickDeclare);

static void hobj_dictionary_init (hobj_dictionary_t * dict,InvokeTickDeclare);

static void hobj_dictionary_destroy(hobj_dictionary_t * dict,InvokeTickDeclare);

HOBJ_CLASS_METHOD_IMP_BEGIN(Dictionary)

HOBJ_CLASS_METHOD_IMP(init, hobj_dictionary_init)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_dictionary_destroy)

HOBJ_CLASS_METHOD_IMP(removeForKey, hobj_dictionary_removeForKey)

HOBJ_CLASS_METHOD_IMP(keys, hobj_dictionary_keys)

HOBJ_CLASS_METHOD_IMP(clear, hobj_dictionary_clear)

HOBJ_CLASS_METHOD_IMP(valueForKey, hobj_dictionary_valueForKey)

HOBJ_CLASS_METHOD_IMP(setValueForKey, hobj_dictionary_setValueForKey)

HOBJ_CLASS_METHOD_IMP_END(Dictionary)

HOBJ_CLASS_IMP_I(Dictionary,  hobj_dictionary_t, &hobj_class)

typedef struct {
    hobj_value_t * key;
    hobj_t * value;
}hobj_dictionary_item_t;

static hobj_t * hobj_dictionary_item_release(hobj_dictionary_item_t * item,InvokeTickDeclare){
    hobj_t * v = hobj_autorelease(item->value, InvokeTickArg);
    hobj_release((hobj_t *)item->key, InvokeTickArg);
    mem_free(item);
    return v;
}

static hobj_dictionary_item_t * hobj_dictionary_item_alloc(hcchar * key,hobj_t * value,InvokeTickDeclare){
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t *) mem_malloc(sizeof(hobj_dictionary_item_t));
    memset(item, 0, sizeof(hobj_dictionary_item_t));
    item->key = hobj_value_string_alloc(key, InvokeTickArg);
    item->value = hobj_retain(value, InvokeTickArg);
    return item;
}


hobj_dictionary_t * hobj_dictionary_alloc(InvokeTickDeclare){
    hobj_dictionary_t * v = (hobj_dictionary_t *) hobj_alloc(&hobj_Dictionary_class, InvokeTickArg);
    return v;
}

hobj_dictionary_t * hobj_dictionary_alloc_with_objects(InvokeTickDeclare,...){
    hobj_dictionary_t * v = (hobj_dictionary_t *) hobj_alloc(&hobj_Dictionary_class, InvokeTickArg);
    hcchar * key;
    hobj_t * value;
    va_list va;
    va_start(va, __InvokeTick__);
    
    while((key = va_arg(va, hcchar *)) && (value = va_arg(va, hobj_t *))){
        hobj_setValueForKey((hobj_t *) v, key, value, InvokeTickArg);
    }
    
    return v;
}

hobj_dictionary_t * hobj_dictionary_new(InvokeTickDeclare){
    hobj_dictionary_t * v = (hobj_dictionary_t *) hobj_alloc(&hobj_Dictionary_class, InvokeTickArg);
    return (hobj_dictionary_t *)hobj_autorelease((hobj_t *)v, InvokeTickArg);
}

hobj_dictionary_t * hobj_dictionary_new_with_objects(InvokeTickDeclare,...){
    hobj_dictionary_t * v = (hobj_dictionary_t *) hobj_alloc(&hobj_Dictionary_class, InvokeTickArg);
    hcchar * key;
    hobj_t * value;
    va_list va;
    va_start(va, __InvokeTick__);
    
    while((key = va_arg(va, hcchar *)) && (value = va_arg(va, hobj_t *))){
        hobj_setValueForKey((hobj_t *) v, key, value, InvokeTickArg);
    }
    
    return (hobj_dictionary_t *)hobj_autorelease((hobj_t *)v, InvokeTickArg);
}

hobj_t * hobj_dictionary_removeForKey(hobj_dictionary_t * dict,hcchar * key,InvokeTickDeclare){
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t *)map_remove(dict->map, (hany) key);;
    hobj_t * v = NULL;
    if(item){
        v = hobj_dictionary_item_release(item,InvokeTickArg);
    }
    return v;
}

static void hobj_dictionary_keys_map_each(hmap_t map, hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
    hobj_array_t * keys = (hobj_array_t *) arg0;
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t *) value;
    hobj_array_add(keys, (hobj_t *)item->key, InvokeTickArg);
}

hobj_array_t * hobj_dictionary_keys(hobj_dictionary_t * dict,InvokeTickDeclare){
    
    hobj_array_t * keys = hobj_array_new(InvokeTickArg);
    
    map_each(dict->map, hobj_dictionary_keys_map_each, keys, NULL);
    
    return keys;
}

static void hobj_dictionary_clear_map_each(hmap_t map, hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t *) value;
    hobj_dictionary_item_release(item,InvokeTickArg);
}

void hobj_dictionary_clear(hobj_dictionary_t * dict,InvokeTickDeclare){
    map_each(dict->map, hobj_dictionary_clear_map_each, NULL, NULL);
    map_clear(dict->map);
}


static hobj_t * hobj_dictionary_valueForKey(hobj_dictionary_t * dict,hcchar * key,InvokeTickDeclare){
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t * ) map_get(dict->map, (hany) key);
    return item ? item->value : NULL;
}

static void hobj_dictionary_setValueForKey(hobj_dictionary_t * dict,hcchar * key,hobj_t * value,InvokeTickDeclare){
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t * ) map_get(dict->map, (hany) key);
    
    if(item){
        if(value){
            if(item->value != value){
                hobj_release(item->value, InvokeTickArg);
                item->value = hobj_retain(value, InvokeTickArg);
            }
        }
        else{
            map_remove(dict->map, (hany) key);
            hobj_dictionary_item_release(item,InvokeTickArg);
        }
    }
    else if(value){
        if(dict->map==NULL){
            dict->map = map_alloc(hash_code_str, equal_str);
        }
        item = hobj_dictionary_item_alloc(key, value, InvokeTickArg);
        map_put(dict->map, (hany)hobj_value_stringValue(item->key, InvokeTickArg), item);
    }
}

static void hobj_dictionary_init (hobj_dictionary_t * dict,InvokeTickDeclare){
    hobj_method_t * method = hobj_class_method(dict->base.clazz->superClass, HOBJ_KEY(init), InvokeTickArg);
    if(method){
        (* (hobj_method_init_impl_t) method->imp)((hobj_t *) dict,InvokeTickArg);
    }
}

static void hobj_dictionary_destroy_map_each(hmap_t map, hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
    
    hobj_dictionary_item_t * item = (hobj_dictionary_item_t *) value;
    
    hobj_dictionary_item_release(item, InvokeTickArg);
    
}

static void hobj_dictionary_destroy(hobj_dictionary_t * dict,InvokeTickDeclare){
    
    hobj_method_t * method;
    
    map_each(dict->map, hobj_dictionary_destroy_map_each, NULL, NULL);
    
    map_dealloc(dict->map);

    method = hobj_class_method(dict->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *) dict,InvokeTickArg);
    }
}
