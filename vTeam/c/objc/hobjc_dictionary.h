//
//  hobjc_dictionary.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_dictionary_h
#define hobjc_hobjc_dictionary_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc.h"
#include "hobjc_array.h"
    
    struct _map_t;
    struct _serial_list_t;
    
    typedef struct _hobj_dictionary_t{
        hobj_t base;
        struct _map_t * map;
    } hobj_dictionary_t;
    
    HOBJ_KEY_DEC(Dictionary)
    HOBJ_KEY_DEC(removeForKey)
    HOBJ_KEY_DEC(keys)
    HOBJ_KEY_DEC(clear)
    
    HOBJ_CLASS_DEC(Dictionary)
    
    hobj_dictionary_t * hobj_dictionary_alloc(InvokeTickDeclare);
    
    hobj_dictionary_t * hobj_dictionary_alloc_with_objects(InvokeTickDeclare,...);
    
    hobj_dictionary_t * hobj_dictionary_new(InvokeTickDeclare);
    
    hobj_dictionary_t * hobj_dictionary_new_with_objects(InvokeTickDeclare,...);
    
    hobj_t * hobj_dictionary_removeForKey(hobj_dictionary_t * dict,hcchar * key,InvokeTickDeclare);
    
    hobj_array_t * hobj_dictionary_keys(hobj_dictionary_t * dict,InvokeTickDeclare);
    
    void hobj_dictionary_clear(hobj_dictionary_t * dict,InvokeTickDeclare);
    
    
#ifdef __cplusplus
}
#endif


#endif
