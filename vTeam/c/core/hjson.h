//
//  hjson.h
//  hclib
//
//  Created by zhang hailong on 13-1-25.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hclib_hjson_h
#define hclib_hjson_h

#ifdef __cplusplus
extern "C" {
#endif
    
    struct _hjson_t;
    struct _buffer_t;
    
    typedef hany (* hjson_object_new_t)(struct _hjson_t * json,InvokeTickDeclare);
    typedef void (* hjson_object_put_t)(struct _hjson_t * json,hany object,hcchar * key,hany value,InvokeTickDeclare);
    typedef hany (* hjson_array_new_t)(struct _hjson_t * json,InvokeTickDeclare);
    typedef void (* hjson_array_add_t)(struct _hjson_t * json,hany array,hany value,InvokeTickDeclare);
    typedef hany (* hjson_number_t)(struct _hjson_t * json,hcchar * value,InvokeTickDeclare);
    typedef hany (* hjson_string_t)(struct _hjson_t * json,hcchar * value,struct _buffer_t * base64_buffer,InvokeTickDeclare);
    typedef hany (* hjson_boolean_t)(struct _hjson_t * json,hbool value,InvokeTickDeclare);
    typedef hany (* hjson_null_t)(struct _hjson_t * json,InvokeTickDeclare);
    typedef void (* hjson_object_dealloc_t)(struct _hjson_t * json,hany object,InvokeTickDeclare);
    
    typedef struct _hjson_t{
        hjson_object_new_t object_new;
        hjson_object_put_t object_put;
        hjson_array_new_t array_new;
        hjson_array_add_t array_add;
        hjson_number_t numberValue;
        hjson_string_t stringValue;
        hjson_boolean_t booleanValue;
        hjson_null_t nullValue;
        hjson_object_dealloc_t object_dealloc;
    } hjson_t;
    
    
    hany hjson_decode(hjson_t * json,hcchar * str,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
