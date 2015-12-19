//
//  hobjc_value.c
//  hobjc
//
//  Created by Zhang Hailong on 13-3-11.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hobjc_value.h"
#include "hbuffer.h"
#include "md5.h"

HOBJ_KEY_IMP(Value)
HOBJ_KEY_IMP(intValue)
HOBJ_KEY_IMP(int64Value)
HOBJ_KEY_IMP(doubleValue)
HOBJ_KEY_IMP(floatValue)
HOBJ_KEY_IMP(stringValue)
HOBJ_KEY_IMP(booleanValue)

static hobj_t * hobj_value_method_toString_impl(hobj_value_t * obj,InvokeTickDeclare);

static hdouble  hobj_value_method_toDouble_impl(hobj_value_t * obj,InvokeTickDeclare);

static void  hobj_valeu_destroy_impl (hobj_value_t * obj,InvokeTickDeclare);

HOBJ_CLASS_METHOD_IMP_BEGIN(Value)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_valeu_destroy_impl)

HOBJ_CLASS_METHOD_IMP(intValue, hobj_value_intValue)

HOBJ_CLASS_METHOD_IMP(int64Value, hobj_value_int64Value)

HOBJ_CLASS_METHOD_IMP(doubleValue, hobj_value_doubleValue)

HOBJ_CLASS_METHOD_IMP(floatValue, hobj_value_floatValue)

HOBJ_CLASS_METHOD_IMP(stringValue, hobj_value_stringValue)

HOBJ_CLASS_METHOD_IMP(booleanValue, hobj_value_booleanValue)

HOBJ_CLASS_METHOD_IMP(toString, hobj_value_method_toString_impl)

HOBJ_CLASS_METHOD_IMP(toDouble, hobj_value_method_toDouble_impl)

HOBJ_CLASS_METHOD_IMP_END(Value)

HOBJ_CLASS_IMP_I(Value, hobj_value_t, &hobj_class)

hobj_value_t * hobj_value_alloc(hobj_type_t * valueType,hany data,hint32 length,InvokeTickDeclare){
    hobj_value_t * v = (hobj_value_t *) hobj_alloc(HOBJ_CLASS(Value), InvokeTickArg);
    v->length = length;
    if(length >0){
        v->data = mem_malloc(length);
        mem_memcpy(v->data, data, length);
    }
    v->valueType = valueType;
    return v;
}

static void  hobj_valeu_destroy_impl (hobj_value_t * obj,InvokeTickDeclare){
    if(obj->data){
        mem_free(obj->data);
    }
}

hobj_value_t * hobj_value_int_alloc(hint32 value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_INTEGER, &value, sizeof(hint32), InvokeTickArg);
}

hobj_value_t * hobj_value_int64_alloc(hint64 value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_INT64, &value, sizeof(hint64), InvokeTickArg);
}

hobj_value_t * hobj_value_double_alloc(hdouble value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_DOUBLE, &value, sizeof(hdouble), InvokeTickArg);
}

hobj_value_t * hobj_value_float_alloc(hfloat value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_FLOAT, &value, sizeof(hfloat), InvokeTickArg);
}

hobj_value_t * hobj_value_string_alloc(hcchar * value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_STRING, (hany)value, strlen(value) + 1, InvokeTickArg);
}

hobj_value_t * hobj_value_string_alloc_with_length(hcchar * value,hint32 length, InvokeTickDeclare){
    hobj_value_t * v = hobj_value_alloc(HOBJ_TYPE_STRING, (hany) value, length +1, InvokeTickArg);
    ((hbyte *)v->data)[length] = 0;
    return v;
}

hobj_value_t * hobj_value_boolean_alloc(hbool value,InvokeTickDeclare){
    return hobj_value_alloc(HOBJ_TYPE_BOOLEAN, &value, sizeof(hbool), InvokeTickArg);
}

hobj_value_t * hobj_value_new(hobj_type_t * valueType,hany data,hint32 length,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_alloc(valueType, data, length, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_int_new(hint32 value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_int_alloc(value, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_int64_new(hint64 value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_int64_alloc(value, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_double_new(hdouble value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_double_alloc(value, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_float_new(hfloat value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_float_alloc(value, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_string_new(hcchar * value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_string_alloc(value, InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_string_new_with_length(hcchar * value,hint32 length,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_string_alloc_with_length(value, length,InvokeTickArg), InvokeTickArg);
}

hobj_value_t * hobj_value_string_new_format(hcchar * format,InvokeTickDeclare,...){
    hbuffer_t buf = buffer_alloc(128, 128);
    hobj_value_t * v = NULL;
    va_list va;
    va_start(va, __InvokeTick__);
    
    buffer_append_format_va_list(buf, format, va);
    
    va_end(va);
    
    v = hobj_value_string_new(buffer_to_str(buf),InvokeTickArg);
    
    buffer_dealloc(buf);
    
    return v;
}

hobj_value_t * hobj_value_boolean_new(hbool value,InvokeTickDeclare){
    return (hobj_value_t *) hobj_autorelease((hobj_t *)hobj_value_boolean_alloc(value, InvokeTickArg), InvokeTickArg);
}

hint32 hobj_value_intValue(hobj_value_t * value,InvokeTickDeclare){
    if(value){
        if(value->valueType == HOBJ_TYPE_INTEGER){
            return * (hint32 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_INT64){
            return (hint32)* (hint64 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_DOUBLE){
            return * (hdouble *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_FLOAT){
            return * (hfloat *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_STRING){
            return atoi((hchar *) value->data);
        }
        if(value->valueType == HOBJ_TYPE_BOOLEAN){
            return * (hbool *) value->data;
        }
    }
    
    return 0;
}

hint64 hobj_value_int64Value(hobj_value_t * value,InvokeTickDeclare){
    if(value){
        if(value->valueType == HOBJ_TYPE_INTEGER){
            return * (hint32 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_INT64){
            return * (hint64 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_DOUBLE){
            return * (hdouble *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_FLOAT){
            return * (hfloat *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_STRING){
            return atoll((hchar *) value->data);
        }
        if(value->valueType == HOBJ_TYPE_BOOLEAN){
            return * (hbool *) value->data;
        }
    }
    return 0;
}

hdouble hobj_value_doubleValue(hobj_value_t * value,InvokeTickDeclare){
    if(value){
        if(value->valueType == HOBJ_TYPE_INTEGER){
            return * (hint32 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_INT64){
            return * (hint64 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_DOUBLE){
            return * (hdouble *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_FLOAT){
            return * (hfloat *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_STRING){
            return atof((hchar *) value->data);
        }
        if(value->valueType == HOBJ_TYPE_BOOLEAN){
            return * (hbool *) value->data;
        }
    }
    return 0.0;
}

hfloat hobj_value_floatValue(hobj_value_t * value,InvokeTickDeclare){
    if(value){
        if(value->valueType == HOBJ_TYPE_INTEGER){
            return * (hint32 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_INT64){
            return * (hint64 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_DOUBLE){
            return * (hdouble *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_FLOAT){
            return * (hfloat *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_STRING){
            return atof((hchar *) value->data);
        }
        if(value->valueType == HOBJ_TYPE_BOOLEAN){
            return * (hbool *) value->data;
        }
    }
    return 0.0f;
}

hcchar * hobj_value_stringValue(hobj_value_t * value,InvokeTickDeclare){
    if(value && value->valueType == HOBJ_TYPE_STRING){
        return (hchar *) value->data;
    }
    return NULL;
}

hbool hobj_value_booleanValue(hobj_value_t * value,InvokeTickDeclare){
    if(value){
        if(value->valueType == HOBJ_TYPE_INTEGER){
            return * (hint32 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_INT64){
            return (hint32) * (hint64 *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_DOUBLE){
            return * (hdouble *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_FLOAT){
            return * (hfloat *) value->data;
        }
        if(value->valueType == HOBJ_TYPE_STRING){
            if(value->data && strcmp(value->data, "false") !=0 && strcmp(value->data, "0") != 0){
                return hbool_true;
            }
            return hbool_false;
        }
    }
    return hbool_false;
}

static hobj_t * hobj_value_method_toString_impl(hobj_value_t * value,InvokeTickDeclare){
    hchar buf[128];
    if(value){
        if(value->valueType == HOBJ_TYPE_STRING){
            return (hobj_t *)value;
        }
        else if(value->valueType == HOBJ_TYPE_INTEGER){
            hint32 v = * (hint32 *) value->data;
            sprintf(buf,"%d",v);
            return (hobj_t *)hobj_value_string_new(buf, InvokeTickArg);
        }
        else if(value->valueType == HOBJ_TYPE_INT64){
            hint64 v = * (hint64 *) value->data;
            sprintf(buf,"%lld",v);
            return (hobj_t *)hobj_value_string_new(buf, InvokeTickArg);
        }
        else if(value->valueType == HOBJ_TYPE_DOUBLE){
            hdouble v = * (hdouble *) value->data;
            sprintf(buf,"%lf",v);
            return (hobj_t *)hobj_value_string_new(buf, InvokeTickArg);
        }
        else if(value->valueType == HOBJ_TYPE_FLOAT){
            hfloat v = * (hfloat *) value->data;
            sprintf(buf,"%f",v);
            return (hobj_t *)hobj_value_string_new(buf, InvokeTickArg);
        }
        else if(value->valueType == HOBJ_TYPE_BOOLEAN){
            hbool v = * (hbool *) value->data;
            return (hobj_t *)hobj_value_string_new(v ? "true":"false", InvokeTickArg);
        }
        {
            hobj_method_t * method = hobj_class_method(value->base.clazz->superClass, &hobj_key_toString, InvokeTickArg);
            if(method){
                return (* (hobj_method_toString_imp_t) method->imp)((hobj_t *) value,InvokeTickArg);
            }
        }
    }
    return NULL;
}

static hdouble hobj_value_method_toDouble_impl(hobj_value_t * value,InvokeTickDeclare){
    return hobj_value_doubleValue(value, InvokeTickArg);
}

static hobj_hash_code_t hobj_value_method_hashCode_impl (hobj_value_t * obj,InvokeTickDeclare){
    md5_state_t md5;
    md5_byte_t digset[16];
    hint32 i;
    huintptr hashCode = 0;
    
    md5_init(&md5);
    
    md5_append(&md5, obj->data, obj->length);
    
    md5_finish(&md5, digset);
    
    for(i=0;i<16;i++){
        hashCode += digset[i];
    }
    
    return hashCode;
}

static hbool hobj_value_method_equals_impl (hobj_t * obj,hobj_t * value,InvokeTickDeclare){
    if(obj && value){
        if(hobj_is_kind(obj, &hobj_Value_class, InvokeTickArg) && hobj_is_kind(value, &hobj_Value_class, InvokeTickArg)){
            hobj_value_t * v1 = (hobj_value_t *) obj;
            hobj_value_t * v2 = (hobj_value_t *) obj;
            if(v1->valueType == v2->valueType
               && v1->length == v2->length){
                return memcmp(v1->data, v2->data, v1->length);;
            }
        }
        return hbool_false;
    }
    else{
        return value == obj;
    }
}

