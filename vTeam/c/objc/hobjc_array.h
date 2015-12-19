//
//  hobjc_array.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_array_h
#define hobjc_hobjc_array_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc.h"
    
    struct _list_t;
    
    typedef struct _hobj_array_t{
        hobj_t base;
        struct _list_t * list;
    } hobj_array_t;
    
    HOBJ_KEY_DEC(Array)
    HOBJ_KEY_DEC(objectAt)
    HOBJ_KEY_DEC(count)
    HOBJ_KEY_DEC(add)
    HOBJ_KEY_DEC(insert)
    HOBJ_KEY_DEC(removeAt)
    HOBJ_KEY_DEC(remove)
    HOBJ_KEY_DEC(last)
    HOBJ_KEY_DEC(removeLast)
    HOBJ_KEY_DEC(clear);
    
    HOBJ_CLASS_DEC(Array)
    
    hobj_array_t * hobj_array_alloc(InvokeTickDeclare);
    
    hobj_array_t * hobj_array_alloc_with_objects(InvokeTickDeclare,...);
    
    hobj_array_t * hobj_array_new(InvokeTickDeclare);
    
    hobj_array_t * hobj_array_new_with_objects(InvokeTickDeclare,...);
    
    hobj_t * hobj_array_objectAt(hobj_array_t * array,huint32 index,InvokeTickDeclare);
    
    huint32 hobj_array_count(hobj_array_t * array,InvokeTickDeclare);
    
    hobj_t * hobj_array_add(hobj_array_t * array,hobj_t * item,InvokeTickDeclare);
    
    hobj_t * hobj_array_insert(hobj_array_t * array,hobj_t * item,huint16 index,InvokeTickDeclare);
    
    hobj_t * hobj_array_removeAt(hobj_array_t * array,huint32 index,InvokeTickDeclare);
    
    hobj_t * hobj_array_remove(hobj_array_t * array,hobj_t * item,InvokeTickDeclare);
    
    hobj_t * hobj_array_last(hobj_array_t * array,InvokeTickDeclare);
    
    hobj_t * hobj_array_removeLast(hobj_array_t * array,InvokeTickDeclare);
    
    void hobj_array_clear(hobj_array_t * array,InvokeTickDeclare);
    
    void hobj_array_swap(hobj_array_t * array,hint32 index1,hint32 index2,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
