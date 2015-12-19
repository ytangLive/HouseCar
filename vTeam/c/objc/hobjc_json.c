//
//  hobjc_json.c
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hobjc_json.h"
#include "hobjc_value.h"
#include "hbuffer.h"
#include "hjson.h"
#include "hobjc_array.h"
#include "hobjc_dictionary.h"
#include "hstr.h"
#include "hbase64.h"

static hany hobjc_json_object_new(struct _hjson_t * json,InvokeTickDeclare){
    return hobj_dictionary_new(InvokeTickArg);
}

static void hobjc_json_object_put(struct _hjson_t * json,hany object,hcchar * key,hany value,InvokeTickDeclare){
    hobj_setValueForKey((hobj_t *) object, key, (hobj_t *) value, InvokeTickArg);
}

static hany hobjc_json_array_new(struct _hjson_t * json,InvokeTickDeclare){
    return hobj_array_new(InvokeTickArg);
}

static void hobjc_json_array_add(struct _hjson_t * json,hany array,hany value,InvokeTickDeclare){
    hobj_array_add((hobj_array_t *) array, (hobj_t *) value, InvokeTickArg);
}

static hany hobjc_json_number(struct _hjson_t * json,hcchar * value,InvokeTickDeclare){
    hint64 int64_value = 0;
    hdouble double_value = 0;
    if(str_exist(value, ".") || str_exist(value, "e")){
        sscanf(value, "%lf",&double_value);
        if((hint32)double_value == double_value){
            return hobj_value_int_new((hint32)double_value, InvokeTickArg) ;
        }
        else{
            return hobj_value_double_new(double_value, InvokeTickArg) ;
        }
    }
    else{
        sscanf(value, "%lld",&int64_value);
        if((hint32)int64_value == int64_value){
            return hobj_value_int_new((hint32)int64_value, InvokeTickArg) ;
        }
        else {
            return hobj_value_int64_new(int64_value, InvokeTickArg) ;
        }
    }
}

static hany hobjc_json_string(struct _hjson_t * json,hcchar * value,struct _buffer_t * base64_buffer,InvokeTickDeclare){
    if(str_has_prefix(value, "#bit64__")){
        buffer_clear(base64_buffer);
        if(hbase64_decode(value + 8,base64_buffer)){
            return hobj_value_new(HOBJ_TYPE_BYTES, buffer_data(base64_buffer),buffer_length(base64_buffer),InvokeTickArg);
        }
    }
    return hobj_value_string_new(value, InvokeTickArg);
}

static hany hobjc_json_boolean(struct _hjson_t * json,hbool value,InvokeTickDeclare){
    return hobj_value_boolean_new(value, InvokeTickArg);
}

static hany hobjc_json_null(struct _hjson_t * json,InvokeTickDeclare){
    return hobj_value_new(HOBJ_TYPE_NULL, NULL,0,InvokeTickArg);
}

static void hobjc_json_object_dealloc(struct _hjson_t * json,hany object,InvokeTickDeclare){
    
}

static hjson_t hobj_json = {
    hobjc_json_object_new,
    hobjc_json_object_put,
    hobjc_json_array_new,
    hobjc_json_array_add,
    hobjc_json_number,
    hobjc_json_string,
    hobjc_json_boolean,
    hobjc_json_null,
    hobjc_json_object_dealloc,
};

hobj_t * hobj_json_decode(hcchar * json,InvokeTickDeclare){
    return hjson_decode(&hobj_json, json, InvokeTickArg);
}

static void hobj_json_encode_level(hbuffer_t buf,hint32 level,InvokeTickDeclare){
    while(level >0){
        buffer_append(buf, (hany)"\t", 1);
        level --;
    }
}
static void hobj_json_encode_to(hobj_t * obj,hbuffer_t buf,hint32 level,InvokeTickDeclare){
    if(obj){
        
    }
    else{
        buffer_append(buf,(hany)"null",4);
    }
}

hobj_t * hobj_json_encode(hobj_t * obj,InvokeTickDeclare){
    hbuffer_t buf = buffer_alloc(256, 256);
    hobj_t * v = (hobj_t *)hobj_value_string_new(buffer_to_str(buf), InvokeTickArg);
    buffer_dealloc(buf);
    return v;
}

hcchar * hobj_json_encode_c(hobj_t * obj,InvokeTickDeclare){
    hobj_t * v = hobj_json_encode(obj, InvokeTickArg);
    if(v && hobj_is_kind(v, &hobj_Value_class, InvokeTickArg)){
        return hobj_value_stringValue((hobj_value_t *)v, InvokeTickArg);
    }
    return NULL;
}

