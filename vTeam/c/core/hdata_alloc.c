/*
 *  hdata_writer.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hdata_alloc.h"
#include "hlist.h"
#include "hmap.h"
#include "hrecursive.h"
#include "hstr.h"
#include "hext_obj.h"


#undef hdata_null_alloc

#undef hdata_object_alloc

#undef hdata_array_alloc

#undef hdata_int16_alloc

#undef hdata_int32_alloc

#undef hdata_int64_alloc

#undef hdata_double_alloc

#undef hdata_boolean_alloc

#undef hdata_string_alloc

#undef hdata_bytes_alloc

#undef hdata_object_put

#undef hdata_object_remove

#undef hdata_object_map

#undef hdata_array_add

#undef hdata_array_remove

#undef hdata_dealloc

#undef hdata_clone


#define DATA_STRUCT_VERSION 0x0A1
#define DATA_STRUCT_ASSET(d)	(d && ((data_t *)d)->version ==DATA_STRUCT_VERSION )

typedef struct{
	hubyte version;	
	hdata_type_code type;
	union  {
		hint16 int16_value;
		hint32 int32_value;
		hint64 int64_value;
		hdouble double_value;
		hbool boolean_value;
		hchar * string_value;
		hlist_t array_value;
		hmap_t object_value;
        hext_obj_t binary_value;
	}value;
}data_t;


static hdata_type_code	_hdata_type(hdata_t,InvokeTickDeclare);
static hint16		_hdata_int16(hdata_t,hint16 ,InvokeTickDeclare);
static hint32		_hdata_int32(hdata_t,hint32 ,InvokeTickDeclare);
static hint64		_hdata_int64(hdata_t,hint64 ,InvokeTickDeclare);
static hdouble		_hdata_double(hdata_t,hdouble ,InvokeTickDeclare);
static hbool		_hdata_boolean(hdata_t,hbool ,InvokeTickDeclare);
static hcchar *	_hdata_string(hdata_t,hcchar * ,InvokeTickDeclare);
static hdata_t		_hdata_object(hdata_t data,hcchar *key,InvokeTickDeclare);
static void		_hdata_object_echo(hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare);
static hint32		_hdata_array_size(hdata_t data,InvokeTickDeclare);
static hdata_t		_hdata_array(hdata_t data,hint32 index,InvokeTickDeclare);
static hint32		_hdata_bytes_size(hdata_t data,InvokeTickDeclare);
static hbyte *		_hdata_bytes(hdata_t data,InvokeTickDeclare);

hdata_t hdata_null_alloc(InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_NULL;
	return (hdata_t)d;
}

hdata_t hdata_object_alloc(InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_OBJECT;
	d->value.object_value = map_alloc( hash_code_str, equal_str);
	return (hdata_t)d;
}

hdata_t hdata_array_alloc(hint32 capacity,hint32 extendSize,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc(sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_ARRAY;
	d->value.array_value = list_alloc( capacity, extendSize);
	return (hdata_t)d;
}

hdata_t hdata_int16_alloc(hint16 value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_INT16;
	d->value.int16_value = value;
	return (hdata_t)d;
}

hdata_t hdata_int32_alloc(hint32 value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_INT32;
	d->value.int32_value = value;
	return (hdata_t)d;
}

hdata_t hdata_int64_alloc(hint64 value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_INT64;
	d->value.int64_value = value;
	return (hdata_t)d;
}

hdata_t hdata_double_alloc(hdouble value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc(sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_DOUBLE;
	d->value.double_value = value;
	return (hdata_t)d;
}

hdata_t hdata_boolean_alloc(hbool value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
	d->type = HDATA_TYPE_BOOL;
	d->value.boolean_value = value;
	return (hdata_t)d;
}

hdata_t hdata_string_alloc(hcchar * value,InvokeTickDeclare){
	data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
    if(value){
        d->type = HDATA_TYPE_STRING;
        str_cpy(&d->value.string_value, value);
    }
    else{
        d->type = HDATA_TYPE_NULL;
    }
	return (hdata_t)d;
}

hdata_t hdata_bytes_alloc(hbyte * data,hint32 length,InvokeTickDeclare){
    data_t * d = (data_t *)mem_malloc( sizeof(data_t));
	memset(d,0,sizeof(data_t));
	d->version = DATA_STRUCT_VERSION;
    if(data && length >0){
        d->type = HDATA_TYPE_BYTES;
        d->value.binary_value = ext_bytes_obj_alloc( length);
        memcpy(ext_bytes_obj_data(d->value.binary_value), data, length);
    }
    else{
        d->type = HDATA_TYPE_NULL;
    }
	return (hdata_t)d;
}


typedef struct{
	hchar * key;
	hdata_t value;
}hdata_object_tick_t;

static hdata_object_tick_t * hdata_object_tick_alloc(hcchar * key, hdata_t value,InvokeTickDeclare){
	hdata_object_tick_t * tick = mem_malloc( sizeof(hdata_object_tick_t));
	memset(tick,0,sizeof(hdata_object_tick_t));
	str_cpy(&tick->key, key);
	tick->value = value;
	return tick;
}

static void hdata_object_tick_dealloc(hdata_object_tick_t * tick,InvokeTickDeclare){
	if(tick){
		mem_free( tick->key);
		hdata_dealloc(tick->value,InvokeTickArg);
		mem_free( tick);
	}
}

void hdata_object_put(hdata_t data,hcchar * key,hdata_t value,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) && key && value && _hdata_type(data,InvokeTickArg) == HDATA_TYPE_OBJECT){
		data_t *d = (data_t *)data;
		hdata_object_tick_t * tick = hdata_object_tick_alloc(key,value,InvokeTickArg);
		hdata_object_tick_dealloc(map_put(d->value.object_value, tick->key, tick),InvokeTickArg);
	}
}

void hdata_object_remove(hdata_t data,hcchar * key,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) && key && _hdata_type(data,InvokeTickArg) == HDATA_TYPE_OBJECT){
		data_t *d = (data_t *)data;
		hdata_object_tick_dealloc(map_remove(d->value.object_value, (hany)key),InvokeTickArg);
	}
}

/*
hmap_t hdata_object_map(hdata_t data){
	if(DATA_STRUCT_ASSET(data) && _hdata_type(data) == HDATA_TYPE_OBJECT){
		data_t *d = (data_t *)data;
		return d->value.object_value;
	}
	return NULL;
}
 */

void hdata_array_add(hdata_t data,hdata_t value,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) && value && _hdata_type(data,InvokeTickArg) == HDATA_TYPE_ARRAY){
		data_t *d = (data_t *)data;
		list_add(d->value.array_value, value);
	}
}

void hdata_array_remove(hdata_t data,hint32 index,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) && _hdata_type(data,InvokeTickArg) == HDATA_TYPE_ARRAY){
		data_t *d = (data_t *)data;
		hdata_dealloc((hdata_t)list_remove_at(d->value.array_value, index),InvokeTickArg);
	}
}


static void hdata_object_dealloc_each(hmap_t map, hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
	hdata_object_tick_dealloc(value,InvokeTickArg);
}

void hdata_dealloc(hdata_t data,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data)){
		data_t * d = (data_t *)data;
		hint32 i,c;
		if(d->type == HDATA_TYPE_STRING){
			mem_free( d->value.string_value);
		}
		else if(d->type == HDATA_TYPE_ARRAY){
			c = list_count(d->value.array_value);
			for(i=0;i<c;i++){
				hdata_dealloc((hdata_t)list_get(d->value.array_value, i),InvokeTickArg);
			}
			list_dealloc(d->value.array_value);
		}
		else if(d->type == HDATA_TYPE_OBJECT){
			map_each(d->value.object_value, hdata_object_dealloc_each, NULL, NULL);
			map_dealloc(d->value.object_value);
		}
        else if(d->type == HDATA_TYPE_BYTES){
            ext_obj_release(d->value.binary_value);
        }
		mem_free( d);
	}
}

typedef struct{
	hdata_t result;
	const hdata_class_t *data_class;
}hdata_clone_param_t;

static void hdata_clone_object_each(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
	hdata_clone_param_t * param = (hdata_clone_param_t *)dest;
	hdata_object_put(param->result, key, hdata_clone(param->data_class,value,InvokeTickArg),InvokeTickArg);
}

hdata_t hdata_clone(const hdata_class_t *data_class,hdata_t data,InvokeTickDeclare){
	if(data_class && data){
		hdata_type_code type = hdata_type(data_class,data);
		hdata_t result = NULL;
		hint32 c,i;
		hdata_clone_param_t param;
		
		if(type == HDATA_TYPE_NULL){
			result = hdata_null_alloc(InvokeTickArg);
		}
		else if(type == HDATA_TYPE_INT16){
			result = hdata_int16_alloc( hdata_int16(data_class,data,0),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_INT32){
			result = hdata_int32_alloc( hdata_int32(data_class,data,0),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_INT64){
			result = hdata_int64_alloc( hdata_int64(data_class,data,0),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_BOOL){
			result = hdata_boolean_alloc( hdata_boolean(data_class,data,0),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_DOUBLE){
			result = hdata_double_alloc( hdata_double(data_class,data,0),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_STRING){
			result = hdata_string_alloc( hdata_string(data_class,data,NULL),InvokeTickArg);
		}
		else if(type == HDATA_TYPE_ARRAY){
			c = hdata_array_size(data_class, data);
			result = hdata_array_alloc( c>0?c:10, 20,InvokeTickArg);
			for(i=0;i<c;i++){
				hdata_array_add(result,hdata_clone(  data_class,hdata_array(data_class,data,i),InvokeTickArg),InvokeTickArg);
			}
		}
		else if(type == HDATA_TYPE_OBJECT){
			result = hdata_object_alloc(InvokeTickArg);
			param.result = result;
			param.data_class = data_class;
			hdata_object_each(data_class, data, hdata_clone_object_each,&param);
		}
		return result;
	}
	return NULL;
}

static hdata_type_code	_hdata_type(hdata_t data,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ?((data_t *)data)->type:HDATA_TYPE_NULL;
}

static hint16 _hdata_int16(hdata_t data,hint16 value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.int16_value:value;
}

static hint32 _hdata_int32(hdata_t data,hint32 value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.int32_value:value;
}

static hint64 _hdata_int64(hdata_t data,hint64 value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.int64_value:value;
}

static hdouble _hdata_double(hdata_t data,hdouble value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.double_value:value;
}

static hbool _hdata_boolean(hdata_t data,hbool value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.boolean_value:value;
}

static hcchar * _hdata_string(hdata_t data,hcchar * value,InvokeTickDeclare){
	return DATA_STRUCT_ASSET(data) ? ((data_t *)data)->value.string_value:value;
}

static hdata_t _hdata_object(hdata_t data,hcchar *key,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		hdata_object_tick_t *tick;
		if(d->type == HDATA_TYPE_OBJECT){
			tick = map_get(d->value.object_value, (hany)key);
			return tick?tick->value:NULL;
		}
	}
	return NULL;
}

typedef struct{
	hdata_object_echo_callback_t callback;
	hany dest;
}_hdata_object_map_arg_t;

static void _hdata_object_map_echo(hmap_t map,hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
	hdata_t data = (hdata_t)arg0;
	_hdata_object_map_arg_t * arg = (_hdata_object_map_arg_t *)arg1;
	hdata_object_tick_t *tick = value;
	(*arg->callback)(data,(hcchar *)key,tick->value,arg->dest,InvokeTickArg);
}

static void _hdata_object_echo(hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		_hdata_object_map_arg_t arg ;
		arg.callback = callback;
		arg.dest =dest;
		if(d->type == HDATA_TYPE_OBJECT){
			map_each(d->value.object_value, _hdata_object_map_echo, data, &arg);
		}
	}
}

static hint32 _hdata_array_size(hdata_t data,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		if(d->type == HDATA_TYPE_ARRAY){
			return list_count(d->value.array_value);
		}
	}
	return 0;
}

static hdata_t	_hdata_array(hdata_t data,hint32 index,InvokeTickDeclare){
	if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		if(d->type == HDATA_TYPE_ARRAY){
			return (hdata_t)list_get(d->value.array_value, index);
		}
	}
	return NULL;
}

static hint32 _hdata_bytes_size(hdata_t data,InvokeTickDeclare){
    if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		if(d->type == HDATA_TYPE_BYTES){
			return ext_bytes_obj_length(d->value.binary_value);
		}
	}
	return 0;
}

static hbyte *	_hdata_bytes(hdata_t data,InvokeTickDeclare){
    if(DATA_STRUCT_ASSET(data) ){
		data_t *d = (data_t *)data;
		if(d->type == HDATA_TYPE_BYTES){
			return ext_bytes_obj_data(d->value.binary_value);
		}
	}
	return NULL;
}


hdata_class_t hdata_class = {_hdata_type,_hdata_int16,_hdata_int32,_hdata_int64
	,_hdata_double,_hdata_boolean,_hdata_string,_hdata_object,_hdata_object_echo,_hdata_array_size,_hdata_array,_hdata_bytes_size,_hdata_bytes};

