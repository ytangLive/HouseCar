//
//  hobjc_value.h
//  hobjc
//
//  Created by Zhang Hailong on 13-3-11.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_value_h
#define hobjc_hobjc_value_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc.h"
   
    typedef struct _hobj_value_t{
        hobj_t base;
        hobj_type_t * READONLY valueType;
        hany READONLY data;
        hint32 READONLY length;
    } hobj_value_t;
    
    HOBJ_KEY_DEC(Value)
    HOBJ_KEY_DEC(intValue)
    HOBJ_KEY_DEC(int64Value)
    HOBJ_KEY_DEC(doubleValue)
    HOBJ_KEY_DEC(floatValue)
    HOBJ_KEY_DEC(stringValue)
    HOBJ_KEY_DEC(booleanValue)
    
    
    hobj_value_t * hobj_value_alloc(hobj_type_t * valueType,hany data,hint32 length,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_int_alloc(hint32 value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_int64_alloc(hint64 value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_double_alloc(hdouble value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_float_alloc(hfloat value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_string_alloc(hcchar * value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_string_alloc_with_length(hcchar * value,hint32 length, InvokeTickDeclare);
    
    hobj_value_t * hobj_value_boolean_alloc(hbool value,InvokeTickDeclare);


    hobj_value_t * hobj_value_new(hobj_type_t * valueType,hany data,hint32 length,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_int_new(hint32 value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_int64_new(hint64 value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_double_new(hdouble value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_float_new(hfloat value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_string_new(hcchar * value,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_string_new_with_length(hcchar * value,hint32 length,InvokeTickDeclare);
    
    hobj_value_t * hobj_value_string_new_format(hcchar * format,InvokeTickDeclare,...);
    
    hobj_value_t * hobj_value_boolean_new(hbool value,InvokeTickDeclare);
    
    hint32 hobj_value_intValue(hobj_value_t * value,InvokeTickDeclare);
    
    hint64 hobj_value_int64Value(hobj_value_t * value,InvokeTickDeclare);
    
    hdouble hobj_value_doubleValue(hobj_value_t * value,InvokeTickDeclare);
    
    hfloat hobj_value_floatValue(hobj_value_t * value,InvokeTickDeclare);
    
    hcchar * hobj_value_stringValue(hobj_value_t * value,InvokeTickDeclare);
    
    hbool hobj_value_booleanValue(hobj_value_t * value,InvokeTickDeclare);
    
    HOBJ_CLASS_DEC(Value)
    
    
#ifdef __cplusplus
}
#endif

#endif
