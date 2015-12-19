/*
 *  hdata_json.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#include "hconfig.h"
#include "hdata_json.h"
#include "hlist.h"
#include "hmap.h"
#include "hrecursive.h"
#include "hstack.h"
#include "hstr.h"
#include "hbase64.h"
#include "hjson.h"

#undef hdata_json_alloc

#undef hdata_json_dealloc

#undef ext_data_json_alloc


static hany hdata_json_object_new(struct _hjson_t * json,InvokeTickDeclare){
    return hdata_object_alloc();
}

static void hdata_json_object_put(struct _hjson_t * json,hany object,hcchar * key,hany value,InvokeTickDeclare){
    return hdata_object_put((hdata_t) object, key, (hdata_t) value);
}

static hany hdata_json_array_new(struct _hjson_t * json,InvokeTickDeclare){
    return hdata_array_alloc(20, 20);
}

static void hdata_json_array_add(struct _hjson_t * json,hany array,hany value,InvokeTickDeclare){
    hdata_array_add(array, value);
}

static hany hdata_json_number(struct _hjson_t * json,hcchar * value,InvokeTickDeclare){
    hint64 int64_value = 0;
	hdouble double_value = 0;
	if(str_exist(value, ".") || str_exist(value, "e")){
		sscanf(value, "%lf",&double_value);
		if((hint32)double_value == double_value){
			return hdata_int32_alloc((hint32)double_value);
		}
		else{
			return hdata_double_alloc(double_value);
		}
	}
	else{
		sscanf(value, "%lld",&int64_value);
		if((hint16)int64_value == int64_value){
			return hdata_int16_alloc( (hint16)int64_value);
		}
		else if((hint32)int64_value == int64_value){
			return hdata_int32_alloc( (hint32)int64_value);
		}
		else {
			return hdata_int64_alloc( (hint64)int64_value);
		}
	}
}

static hany hdata_json_string(struct _hjson_t * json,hcchar * value,struct _buffer_t * base64_buffer,InvokeTickDeclare){
    if(str_has_prefix(value, HDATA_BASE64_TAG)){
        buffer_clear(base64_buffer);
        if(hbase64_decode(value + HDATA_BASE64_TAG_LENGTH,base64_buffer)){
            return hdata_bytes_alloc( buffer_data(base64_buffer), buffer_length(base64_buffer));
        }
        else{
            return hdata_string_alloc( value);
        }
    }
    else {
        return hdata_string_alloc( value);
    }
}

static hany hdata_json_boolean(struct _hjson_t * json,hbool value,InvokeTickDeclare){
    return hdata_boolean_alloc(value);
}

static hany hdata_json_null(struct _hjson_t * json,InvokeTickDeclare){
    return hdata_null_alloc();
}

static void hdata_json_object_dealloc(struct _hjson_t * json,hany object,InvokeTickDeclare){
    hdata_dealloc(object);
}

static hjson_t hdata_hjson = {
    hdata_json_object_new,hdata_json_object_put,hdata_json_array_new,hdata_json_array_add,
    hdata_json_number,hdata_json_string,hdata_json_boolean,hdata_json_null,hdata_json_object_dealloc
};

hdata_t hdata_json_alloc(hcchar * json_str,InvokeTickDeclare){
	return (hdata_t) hjson_decode(&hdata_hjson, json_str, InvokeTickArg);
}


void hdata_json_dealloc(hdata_t data,InvokeTickDeclare){
	hdata_dealloc(data);
}

hext_data_t ext_data_json_alloc(hcchar * json,InvokeTickDeclare){
	hdata_t data = json ? hdata_json_alloc( json,InvokeTickArg) : NULL;
	if(data){
		return ext_data_alloc(data,&hdata_json_class,hdata_json_dealloc);
	}
	return NULL;
}

