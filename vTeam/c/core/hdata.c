/*
 *  hdata.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#define HDATA_IMPLEMENT
#include "hconfig.h"
#include "hdata.h"
#include "hlog.h"
#include "hext_obj.h"
#include "hstr.h"
#include "hbase64.h"

#define DATA_STRUCT_ASSET(d) d


#undef hdata_type

#undef hdata_int16

#undef hdata_int32 

#undef hdata_int64

#undef hdata_double

#undef hdata_boolean

#undef hdata_string

#undef hdata_object

#undef hdata_object_each

#undef hdata_array_size

#undef hdata_array

#undef hdata_bytes_size

#undef hdata_bytes

#undef hdata_write_json

#undef hdata_write_binary

#undef hdata_print_json

#undef ext_data_alloc

#undef ext_data_ref_alloc

#undef ext_data_retain

#undef ext_data_retain_count

#undef ext_data_release

#undef ext_data

#undef ext_data_class

#undef ext_data_detach

hdata_type_code hdata_type(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare){
	return data_class && data_class->data_type?(*data_class->data_type)(data,InvokeTickArg):HDATA_TYPE_NULL;
}

hint16 hdata_int16(const hdata_class_t * data_class,hdata_t data,hint16 value,InvokeTickDeclare){
	
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	hcchar * string_value;
	
	if(type == HDATA_TYPE_INT16){
		return data_class->data_int16?(*data_class->data_int16)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT32){
		return data_class->data_int32?(hint16)(*data_class->data_int32)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT64){
		return data_class->data_int64?(hint16)(*data_class->data_int64)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_BOOL){
		return data_class->data_boolean?(hint16)(*data_class->data_boolean)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		return data_class->data_double?(hint16)(*data_class->data_double)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_STRING){
		string_value = data_class->data_string?(*data_class->data_string)(data, NULL,InvokeTickArg): NULL;
		return string_value?atoi(string_value):value;
	}
	else {
		return value;
	}
}

hint32 hdata_int32(const hdata_class_t * data_class,hdata_t data,hint32 value,InvokeTickDeclare){
	
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	hcchar * string_value;
	
	if(type == HDATA_TYPE_INT16){
		return data_class->data_int16?(hint32)(*data_class->data_int16)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT32){
		return data_class->data_int32?(hint32)(*data_class->data_int32)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT64){
		return data_class->data_int64?(hint32)(*data_class->data_int64)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_BOOL){
		return data_class->data_boolean?(hint32)(*data_class->data_boolean)(data,!!value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		return data_class->data_double?(hint32)(*data_class->data_double)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_STRING){
		string_value = data_class->data_string?(*data_class->data_string)(data,NULL,InvokeTickArg):NULL;
		return string_value?atoi(string_value):value;
	}
	else {
		return value;
	}
}

hint64 hdata_int64(const hdata_class_t * data_class,hdata_t data,hint64 value,InvokeTickDeclare){
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	hcchar * string_value;
	
	if(type == HDATA_TYPE_INT16){
		return data_class->data_int16?(hint64)(*data_class->data_int16)(data,(hint16)value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT32){
		return data_class->data_int32?(hint64)(*data_class->data_int32)(data,(hint32)value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT64){
		return data_class->data_int64?(hint64)(*data_class->data_int64)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_BOOL){
		return data_class->data_boolean?(hint64)(*data_class->data_boolean)(data,!!value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		return data_class->data_double?(hint64)(*data_class->data_double)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_STRING){
		string_value = data_class->data_string?(*data_class->data_string)(data,NULL,InvokeTickArg):NULL;
		return string_value?atoll(string_value):value;
	}
	else {
		return value;
	}
}

hdouble hdata_double(const hdata_class_t * data_class,hdata_t data,hdouble value,InvokeTickDeclare){
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	hcchar * string_value;
	
	if(type == HDATA_TYPE_INT16){
		return data_class->data_int16?(hdouble)(*data_class->data_int16)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT32){
		return data_class->data_int32?(hdouble)(*data_class->data_int32)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT64){
		return data_class->data_int64?(hdouble)(*data_class->data_int64)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_BOOL){
		return data_class->data_boolean?(hdouble)(*data_class->data_boolean)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		return data_class->data_double?(hdouble)(*data_class->data_double)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_STRING){
		string_value = data_class->data_string?(*data_class->data_string)(data,NULL,InvokeTickArg):NULL;
		return string_value?atof(string_value):value;
	}
	else {
		return value;
	}
}

hbool hdata_boolean(const hdata_class_t * data_class,hdata_t data,hbool value,InvokeTickDeclare){
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	hcchar * string_value;
	
	if(type == HDATA_TYPE_INT16){
		return data_class->data_int16?(hbool)(*data_class->data_int16)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT32){
		return data_class->data_int32?(hbool)(*data_class->data_int32)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_INT64){
		return data_class->data_int64?(hbool)(*data_class->data_int64)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_BOOL){
		return data_class->data_boolean?(hbool)(*data_class->data_boolean)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		return data_class->data_double?(hbool)(*data_class->data_double)(data,value,InvokeTickArg):value;
	}
	else if(type == HDATA_TYPE_STRING){
		string_value = data_class->data_string?(*data_class->data_string)(data,NULL,InvokeTickArg):NULL;
		return string_value?atoi(string_value):value;
	}
	else {
		return value;
	}
}

hcchar * hdata_string(const hdata_class_t * data_class,hdata_t data,hcchar * value,InvokeTickDeclare){
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	
	if(type == HDATA_TYPE_STRING){
		return data_class->data_string?(*data_class->data_string)(data,value,InvokeTickArg):value;
	}
	else {
		return value;
	}
}

hdata_t hdata_object(const hdata_class_t * data_class,hdata_t data,hcchar *key,InvokeTickDeclare){
	hdata_type_code type = hdata_type(data_class,data,InvokeTickArg);
	
	if(type == HDATA_TYPE_OBJECT){
		return data_class->data_object?(*data_class->data_object)(data,key,InvokeTickArg):NULL;
	}
	
	return NULL;
}

void hdata_object_each(const hdata_class_t * data_class,hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare){
	if(data_class && data_class->data_object_echo && hdata_type(data_class,data,InvokeTickArg) == HDATA_TYPE_OBJECT){
		(*data_class->data_object_echo)(data,callback,dest,InvokeTickArg);
	}
}

hint32 hdata_array_size(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare){
	return data_class && data_class->data_array_size && hdata_type(data_class,data,InvokeTickArg) == HDATA_TYPE_ARRAY ?(*data_class->data_array_size)(data,InvokeTickArg):0;
}

hdata_t hdata_array(const hdata_class_t * data_class,hdata_t data,hint32 index,InvokeTickDeclare){
	return data_class && data_class->data_array && hdata_type(data_class,data,InvokeTickArg) == HDATA_TYPE_ARRAY?(*data_class->data_array)(data,index,InvokeTickArg):NULL;
}

hint32 hdata_bytes_size(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare){
    return data_class && data_class->data_bytes_size && hdata_type(data_class,data,InvokeTickArg) == HDATA_TYPE_BYTES ?(*data_class->data_bytes_size)(data,InvokeTickArg):0;
}

hbyte * hdata_bytes(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare){
    return data_class && data_class->data_bytes && hdata_type(data_class,data,InvokeTickArg) == HDATA_TYPE_BYTES?(*data_class->data_bytes)(data,InvokeTickArg):NULL;
}

static void hdata_json_write_string(hcchar * str,hbuffer_t buffer,InvokeTickDeclare){
	hchar *p = (hchar *)str;
	buffer_append_str(buffer,"\"");
	while(*p != '\0'){
		if(*p == '\n'){
			buffer_append_str(buffer, "\\\n");
		}
		else if(*p == '\r'){
			buffer_append_str(buffer, "\\\r");
		}
		else if(*p == '\t'){
			buffer_append_str(buffer, "\\\t");
		}
		else if(*p == '\"'){
			buffer_append_str(buffer, "\\\"");
		}
		else{
			buffer_append(buffer,p,1);
		}
		p++;
	}
	buffer_append_str(buffer,"\"");
}

static void _hdata_json_write_level(hbuffer_t buffer,hint32 level,InvokeTickDeclare){
	hint32 i;
	for(i=0;i<level;i++){
		buffer_append_str(buffer, "\t");
	}
}

void _hdata_write(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,hbool pattern,hint32 level,InvokeTickDeclare);

typedef struct{
	hbuffer_t buffer;
	hbool pattern;
	hint32 level;
	hbool first;
	const hdata_class_t * data_class;
}_hdata_write_object_each_arg_t;

static void _hdata_json_write_object_echo(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
	_hdata_write_object_each_arg_t *arg = (_hdata_write_object_each_arg_t *)dest;
	if(arg->pattern){
		_hdata_json_write_level(arg->buffer,arg->level,InvokeTickArg);
	}
	
	if(arg->first && arg->pattern){
		buffer_append_str(arg->buffer, "\n");
		_hdata_json_write_level(arg->buffer,arg->level,InvokeTickArg);
	}
	if(!arg->first){
		buffer_append_str(arg->buffer, ",");
	}
	hdata_json_write_string((hcchar *)key,arg->buffer,InvokeTickArg);
	buffer_append_str(arg->buffer, ":");
	_hdata_write(arg->data_class,(hdata_t)value,arg->buffer,arg->pattern,arg->level,InvokeTickArg);
	if(arg->pattern){
		buffer_append_str(arg->buffer, "\n");
	}
	
	if(arg->first){
		arg->first = hbool_false;
	}
}

void _hdata_write(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,hbool pattern,hint32 level,InvokeTickDeclare){
	hint32 c,i;
	_hdata_write_object_each_arg_t arg;
	hdata_type_code type;
	if(data && data_class && buffer){
		type = hdata_type(data_class,data,InvokeTickArg);
		if(type == HDATA_TYPE_NULL){
			buffer_append_str(buffer, "null");
		}
		else if(type == HDATA_TYPE_INT16 ){
			buffer_append_format(buffer, "%d",InvokeTickArg, hdata_int16(data_class,data,0,InvokeTickArg));
		}
		else if(type == HDATA_TYPE_INT32){
			buffer_append_format(buffer, "%ld",InvokeTickArg,hdata_int32(data_class,data,0,InvokeTickArg));
		}
		else if(type == HDATA_TYPE_INT64){
			buffer_append_format(buffer, "%lld",InvokeTickArg,hdata_int64(data_class,data,0,InvokeTickArg));
		}
		else if(type == HDATA_TYPE_DOUBLE){
			buffer_append_format(buffer, "%f",InvokeTickArg,hdata_double(data_class,data,0,InvokeTickArg));
		}
		else if(type == HDATA_TYPE_BOOL){
			if(hdata_boolean(data_class,data,hbool_false,InvokeTickArg)){
				buffer_append_str(buffer, "true");
			}
			else{
				buffer_append_str(buffer, "false");
			}
		}
		else if(type == HDATA_TYPE_STRING){
			if(hdata_string(data_class,data,NULL,InvokeTickArg)){
				hdata_json_write_string(hdata_string(data_class,data,"",InvokeTickArg),buffer,InvokeTickArg);
			}
			else{
				buffer_append_str(buffer, "null");
			}
		}
		else if(type ==HDATA_TYPE_ARRAY){
			buffer_append_str(buffer, "[");
			c = hdata_array_size(data_class,data,InvokeTickArg);
			for(i=0;i<c;i++){
				if(i==0 && pattern){
					buffer_append_str(buffer, "\n");
					_hdata_json_write_level(buffer,level+1,InvokeTickArg);
				}
				else if(pattern){
					_hdata_json_write_level(buffer,level+1,InvokeTickArg);
				}
				_hdata_write(data_class,hdata_array(data_class,data,i,InvokeTickArg),buffer,pattern,level+1,InvokeTickArg);
				if(i != c-1){
					buffer_append_str(buffer, ",");
				}
				if(pattern){
					buffer_append_str(buffer, "\n");
				}
			}
			if(pattern && c>0){
				_hdata_json_write_level(buffer,level,InvokeTickArg);
				buffer_append_str(buffer, "]");
			}
			else{
				buffer_append_str(buffer, "]");
			}
		}
		else if(type ==HDATA_TYPE_OBJECT){
			buffer_append_str(buffer, "{");
			arg.buffer = buffer;
			arg.pattern = pattern;
			arg.level = level+1;
			arg.first = hbool_true;
			arg.data_class = data_class;
			
			hdata_object_each(data_class,data, _hdata_json_write_object_echo,&arg,InvokeTickArg);
			
			if(pattern && *((hchar *)buffer_data(buffer) + buffer_length(buffer) -1) != '{'){
				_hdata_json_write_level(buffer,level,InvokeTickArg);
				buffer_append_str(buffer, "}");
			}
			else{
				buffer_append_str(buffer, "}");
			}
		}
        else if(type == HDATA_TYPE_BYTES){
            buffer_append_str(buffer, "\"");
            buffer_append_str(buffer, HDATA_BASE64_TAG);
            hbase64_encode(hdata_bytes(data_class, data,InvokeTickArg),hdata_bytes_size(data_class, data,InvokeTickArg),buffer);
            buffer_append_str(buffer, "\"");
        }
	}
}

void hdata_write_json(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,hbool pattern,InvokeTickDeclare){
	_hdata_write(data_class,data,buffer,pattern,0,InvokeTickArg);
}


typedef struct{
	const hdata_class_t *data_class;
	hbuffer_t buffer;
	hbyte * value_bytes;
}_hdata_write_binary_object_each_arg_t;

void _hdata_write_binary(const hdata_class_t *data_class,hdata_t data,hbuffer_t buffer,hbyte * value_bytes,InvokeTickDeclare);

static void _hdata_write_binary_object_each(hdata_t data,hcchar * key,hdata_t value,hany dest,InvokeTickDeclare){
	_hdata_write_binary_object_each_arg_t *arg = (_hdata_write_binary_object_each_arg_t *)dest;
	hint32 length;
	memset(arg->value_bytes,0,HTYPE_MAX_BYTES);
	length = (hint32)str_len((hcchar *)key) +1;
	buffer_append(arg->buffer, arg->value_bytes, type_int32_to_bytes(arg->value_bytes,length));
	buffer_append(arg->buffer,(hany)key,length);
	
	_hdata_write_binary(arg->data_class,(hdata_t)value,arg->buffer,arg->value_bytes,InvokeTickArg);
}

void _hdata_write_binary(const hdata_class_t *data_class,hdata_t data,hbuffer_t buffer,hbyte * value_bytes,InvokeTickDeclare){
	hbuffer_t object_buffer;
	hint32 length ,i;
	hdata_type_code type;
	_hdata_write_binary_object_each_arg_t arg;
	if(data_class && DATA_STRUCT_ASSET(data) && buffer && value_bytes){
		type = hdata_type(data_class,data,InvokeTickArg);
		memset(value_bytes,0,HTYPE_MAX_BYTES);
		buffer_append(buffer, value_bytes, type_byte_to_bytes(value_bytes,type));
		if(type == HDATA_TYPE_INT16){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_int16_to_bytes(value_bytes,hdata_int16(data_class,data,0,InvokeTickArg)));
			
		}
		else if(type == HDATA_TYPE_INT32){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,hdata_int32(data_class,data,0,InvokeTickArg)));

		}
		else if(type == HDATA_TYPE_INT64){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_int64_to_bytes(value_bytes,hdata_int64(data_class,data,0,InvokeTickArg)));
		}
		else if(type == HDATA_TYPE_DOUBLE){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_double_to_bytes(value_bytes,hdata_double(data_class,data,0.0,InvokeTickArg)));
		}
		else if(type == HDATA_TYPE_BOOL){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_bool_to_bytes(value_bytes,hdata_boolean(data_class, data, 0,InvokeTickArg)));
		}
		else if(type == HDATA_TYPE_STRING){
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			length = (hint32)str_len(hdata_string(data_class,data,"",InvokeTickArg)) +1;
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,length));
			buffer_append(buffer,(hany) hdata_string(data_class,data,"",InvokeTickArg),length);
		}
		else if(type == HDATA_TYPE_ARRAY){
			length = hdata_array_size(data_class,data,InvokeTickArg);
			object_buffer = buffer_alloc(2048, 1024);
			for(i=0;i<length;i++){
				_hdata_write_binary(data_class,hdata_array(data_class,data,i,InvokeTickArg),object_buffer,value_bytes,InvokeTickArg);
			}
					
			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,buffer_length(object_buffer)));
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,length));
			buffer_append(buffer, buffer_data(object_buffer),buffer_length(object_buffer));
			buffer_dealloc(object_buffer);
		}
		else if(type == HDATA_TYPE_OBJECT){
			object_buffer = buffer_alloc(2048,1024);
			arg.buffer = object_buffer;
			arg.value_bytes = value_bytes;
			arg.data_class = data_class;
			
			hdata_object_each(data_class, data, _hdata_write_binary_object_each, &arg,InvokeTickArg);
			
			length = buffer_length(object_buffer);

			memset(value_bytes,0,HTYPE_MAX_BYTES);
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,length));
			buffer_append(buffer, buffer_data(object_buffer),length);
			buffer_dealloc(object_buffer);
		}
        else if(type == HDATA_TYPE_BYTES){
            memset(value_bytes,0,HTYPE_MAX_BYTES);
			length = hdata_bytes_size(data_class,data,InvokeTickArg);
			buffer_append(buffer, value_bytes, type_int32_to_bytes(value_bytes,length));
			buffer_append(buffer,(hany) hdata_bytes(data_class,data,InvokeTickArg),length);
        }
	}
}

void hdata_write_binary(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,InvokeTickDeclare){
	if(data_class && DATA_STRUCT_ASSET(data) && buffer){
		hbyte value_bytes[HTYPE_MAX_BYTES] = {0};
		buffer_append(buffer,value_bytes,type_int32_to_bytes(value_bytes, HDATA_VERSION));
		_hdata_write_binary(data_class,data,buffer,value_bytes,InvokeTickArg);
	}
}

void hdata_print_json(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare){
	if(data_class && DATA_STRUCT_ASSET(data)){
		hbuffer_t buffer = buffer_alloc(1024,  1024);
		hdata_write_json(data_class, data, buffer, hbool_true,InvokeTickArg);
		hlog("\n\n%s\n\n",buffer_to_str(buffer));
		buffer_dealloc(buffer);
	}
}

typedef struct {
	hdata_t data;
	const hdata_class_t * data_class;
	ext_data_dealloc_t dealloc;
}ext_data_t;

static void ext_data_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	if(arg){
		ext_data_t *d = (ext_data_t *)arg;
		if(d->data && d->data_class){
			memcpy(obj,d,size);
		}
	}
}

static void ext_data_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	ext_data_t *d  = (ext_data_t *)obj;
	if(d->dealloc){
		(*d->dealloc)(d->data,InvokeTickArg);
	}
}

ext_class_t ext_obj_data_class = {ext_data_init,ext_data_destroy,sizeof(ext_data_t)};

hext_data_t ext_data_alloc(hdata_t data,const hdata_class_t * data_class,ext_data_dealloc_t dealloc,InvokeTickDeclare){
    if(data){
        ext_data_t d = {data,data_class,dealloc};
        return (hext_data_t)ext_obj_alloc( &d, &ext_obj_data_class);
    }
    return NULL;
}

hext_data_t ext_data_retain(hext_data_t data,InvokeTickDeclare){
    return (hext_data_t)ext_obj_retain((hext_obj_t)data);
}

void ext_data_release(hext_data_t data,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)data);
}


hint32 ext_data_retain_count(hext_data_t data,InvokeTickDeclare){
    return ext_obj_retain_count((hext_obj_t)data);
}

typedef struct{
	ext_data_t data;
	hext_data_t data_ref;
}ext_data_ref_t;

static void ext_data_ref_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	if(arg){
		ext_data_ref_t *d = (ext_data_ref_t *)arg;
		if(d->data.data && d->data.data_class){
			memcpy(obj,d,size);
		}
		ext_data_retain(d->data_ref,InvokeTickArg);
	}
}

static void ext_data_ref_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	ext_data_ref_t *d  = (ext_data_ref_t *)obj;
	ext_data_release(d->data_ref,InvokeTickArg);
}

ext_class_t ext_obj_data_ref_class = {ext_data_ref_init,ext_data_ref_destroy,sizeof(ext_data_ref_t)};

hext_data_t ext_data_ref_alloc(hdata_t data, hext_data_t data_ref,InvokeTickDeclare){
	ext_data_ref_t d;
	d.data.data = data;
	d.data.data_class = ext_data_class(data_ref,InvokeTickArg);
	d.data.dealloc = NULL;
	d.data_ref = data_ref;
	return (hext_data_t)ext_obj_alloc( &d, &ext_obj_data_ref_class);
}

hdata_t ext_data(hext_data_t data,InvokeTickDeclare){
	return data?((ext_data_t *)data)->data:NULL;
}

const hdata_class_t * ext_data_class(hext_data_t data,InvokeTickDeclare){
	return data?((ext_data_t *)data)->data_class:NULL;
}

hdata_t ext_data_detach(hext_data_t data,InvokeTickDeclare){
	ext_data_t *d  = (ext_data_t *)data;
	d->dealloc = NULL;
	return d->data;
}
