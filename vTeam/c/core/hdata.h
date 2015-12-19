/*
 *  hdata.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef HDATA_H
#define HDATA_H

#include "hbuffer.h"
#include "hext_obj.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#define HDATA_VERSION		0x0001
#define HDATA_BASE64_TAG    "#bit64__"
#define HDATA_BASE64_TAG_LENGTH 8
	
    typedef struct _data_t{
        long unused;
    } * hdata_t;
	
    typedef enum {
		HDATA_TYPE_NULL,
		HDATA_TYPE_INT16,
		HDATA_TYPE_INT32,
		HDATA_TYPE_INT64,
		HDATA_TYPE_DOUBLE,
		HDATA_TYPE_BOOL,
		HDATA_TYPE_STRING,
		HDATA_TYPE_ARRAY,
		HDATA_TYPE_OBJECT,
        HDATA_TYPE_BYTES,
	}hdata_type_code;
    
#define HDATA_TYPE_BEGIN    HDATA_TYPE_NULL
#define HDATA_TYPE_END      HDATA_TYPE_BYTES
    
	typedef void (*hdata_object_echo_callback_t)(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare);
	
	typedef hdata_type_code	(*hdata_type_t)(hdata_t,InvokeTickDeclare);
	typedef hint16		(*hdata_int16_t)(hdata_t,hint16 ,InvokeTickDeclare);
	typedef hint32		(*hdata_int32_t)(hdata_t,hint32 ,InvokeTickDeclare);
	typedef hint64		(*hdata_int64_t)(hdata_t,hint64 ,InvokeTickDeclare);
	typedef hdouble		(*hdata_double_t)(hdata_t,hdouble ,InvokeTickDeclare);
	typedef hbool		(*hdata_boolean_t)(hdata_t,hbool ,InvokeTickDeclare);
	typedef hcchar *	(*hdata_string_t)(hdata_t,hcchar * ,InvokeTickDeclare);
	typedef hdata_t		(*hdata_object_t)(hdata_t data,hcchar *key,InvokeTickDeclare);
	typedef hint32		(*hdata_array_size_t)(hdata_t data,InvokeTickDeclare);
	typedef hdata_t		(*hdata_array_t)(hdata_t data,hint32 index,InvokeTickDeclare);
	typedef void		(*hdata_object_echo_t)(hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare);
	typedef hint32      (*hdata_bytes_size_t)(hdata_t data,InvokeTickDeclare);
    typedef hbyte *     (*hdata_bytes_t)(hdata_t data,InvokeTickDeclare);
    
	typedef struct{
		hdata_type_t data_type;
		hdata_int16_t data_int16;
		hdata_int32_t data_int32;
		hdata_int64_t data_int64;
		hdata_double_t data_double;
		hdata_boolean_t data_boolean;
		hdata_string_t data_string;
		hdata_object_t data_object;
		hdata_object_echo_t data_object_echo;
		hdata_array_size_t data_array_size;
		hdata_array_t data_array;
        hdata_bytes_size_t data_bytes_size;
        hdata_bytes_t data_bytes;
	}hdata_class_t;
	
	extern hdata_class_t hdata_json_class;
	extern hdata_class_t hdata_binary_class;
	
	hdata_type_code hdata_type(const hdata_class_t * data_class,hdata_t,InvokeTickDeclare);
	
	hint16 hdata_int16(const hdata_class_t * data_class,hdata_t,hint16,InvokeTickDeclare);
	
	hint32 hdata_int32(const hdata_class_t * data_class,hdata_t,hint32,InvokeTickDeclare);
	
	hint64 hdata_int64(const hdata_class_t * data_class,hdata_t,hint64,InvokeTickDeclare);
	
	hdouble hdata_double(const hdata_class_t * data_class,hdata_t,hdouble,InvokeTickDeclare);
	
	hbool hdata_boolean(const hdata_class_t * data_class,hdata_t,hbool,InvokeTickDeclare);
	
	hcchar * hdata_string(const hdata_class_t * data_class,hdata_t,hcchar *,InvokeTickDeclare);
	
	hdata_t hdata_object(const hdata_class_t * data_class,hdata_t data,hcchar *key,InvokeTickDeclare);
	
	void hdata_object_each(const hdata_class_t * data_class,hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare);
	
	hint32 hdata_array_size(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare);
	
	hdata_t hdata_array(const hdata_class_t * data_class,hdata_t data,hint32 index,InvokeTickDeclare);
    
    hint32 hdata_bytes_size(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare);
    
    hbyte * hdata_bytes(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare);
	
	void hdata_write_json(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,hbool pattern,InvokeTickDeclare);
	
	void hdata_write_binary(const hdata_class_t * data_class,hdata_t data,hbuffer_t buffer,InvokeTickDeclare);
	
	void hdata_print_json(const hdata_class_t * data_class,hdata_t data,InvokeTickDeclare);

#define hdata_object_type(class,a,b)	hdata_type((class),hdata_object((class),(a),(b)))
#define hdata_object_int16(class,a,b,c)	hdata_int16((class),hdata_object((class),(a),(b)),(c))
#define hdata_object_int32(class,a,b,c)	hdata_int32((class),hdata_object((class),(a),(b)),(c))
#define hdata_object_int64(class,a,b,c)	hdata_int64((class),hdata_object((class),(a),(b)),(c))
#define hdata_object_double(class,a,b,c)	hdata_double((class),hdata_object((class),(a),(b)),(c))
#define hdata_object_boolean(class,a,b,c)	hdata_boolean((class),hdata_object((class),(a),(b)),(c))
#define hdata_object_string(class,a,b,c)	hdata_string((class),hdata_object((class),(a),(b)),(c))
	
#define hdata_array_type(class,a,b)	hdata_type((class),hdata_array((class),(a),(b)))
#define hdata_array_int16(class,a,b,c)	hdata_int16((class),hdata_array((class),(a),(b)),(c))
#define hdata_array_int32(class,a,b,c)	hdata_int32((class),hdata_array((class),(a),(b)),(c))
#define hdata_array_int64(class,a,b,c)	hdata_int64((class),hdata_array((class),(a),(b)),(c))
#define hdata_array_double(class,a,b,c)	hdata_double((class),hdata_array((class),(a),(b)),(c))
#define hdata_array_boolean(class,a,b,c)	hdata_boolean((class),hdata_array((class),(a),(b)),(c))
#define hdata_array_string(class,a,b,c)	hdata_string((class),hdata_array((class),(a),(b)),(c))
	
	
	typedef void (*ext_data_dealloc_t)(hdata_t ,InvokeTickDeclare);
	
	HANDLER(hext_data_t)
	
	hext_data_t ext_data_alloc(hdata_t data,const hdata_class_t * data_class,ext_data_dealloc_t dealloc,InvokeTickDeclare);
	
	hext_data_t ext_data_ref_alloc(hdata_t data, hext_data_t data_ref,InvokeTickDeclare);
	
    hext_data_t ext_data_retain(hext_data_t data,InvokeTickDeclare);
    
    hint32 ext_data_retain_count(hext_data_t data,InvokeTickDeclare);
    
    void ext_data_release(hext_data_t data,InvokeTickDeclare);
	
    hdata_t ext_data(hext_data_t,InvokeTickDeclare);
	
	const hdata_class_t * ext_data_class(hext_data_t,InvokeTickDeclare);
	
	hdata_t ext_data_detach(hext_data_t,InvokeTickDeclare);
	
	
#define ext_data_object_type(a,b)	hdata_type(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)))
#define ext_data_object_int16(a,b,c)	hdata_int16(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_object_int32(a,b,c)	hdata_int32(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_object_int64(a,b,c)	hdata_int64(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_object_double(a,b,c)	hdata_double(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_object_boolean(a,b,c)	hdata_boolean(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_object_string(a,b,c)	hdata_string(ext_data_class(a),hdata_object(ext_data_class(a),ext_data(a),(b)),(c))
	
#define ext_data_array_type(a,b)	hdata_type(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)))
#define ext_data_array_int16(a,b,c)	hdata_int16(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_array_int32(a,b,c)	hdata_int32(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_array_int64(a,b,c)	hdata_int64(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_array_double(a,b,c)	hdata_double(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_array_boolean(a,b,c)	hdata_boolean(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
#define ext_data_array_string(a,b,c)	hdata_string(ext_data_class(a),hdata_array(ext_data_class(a),ext_data(a),(b)),(c))
	
#define ext_data_object(a,b)	hdata_object(ext_data_class(a), ext_data(a), (b))
	
    
    
    
#define hdata_type(a,b) hdata_type((a),(b),InvokeTickArg)
	
#define hdata_int16(a,b,c) hdata_int16((a),(b),(c),InvokeTickArg)
	
#define hdata_int32(a,b,c) hdata_int32((a),(b),(c),InvokeTickArg) 
	
#define hdata_int64(a,b,c) hdata_int64((a),(b),(c),InvokeTickArg)
	
#define hdata_double(a,b,c) hdata_double((a),(b),(c),InvokeTickArg)
	
#define hdata_boolean(a,b,c) hdata_boolean((a),(b),(c),InvokeTickArg)
	
#define hdata_string(a,b,c) hdata_string((a),(b),(c),InvokeTickArg)
	
#define hdata_object(a,b,c) hdata_object((a),(b),(c),InvokeTickArg)
	
#define hdata_object_each(a,b,c,d) hdata_object_each((a),(b),(c),(d),InvokeTickArg)
	
#define hdata_array_size(a,b) hdata_array_size((a),(b),InvokeTickArg)
	
#define hdata_array(a,b,c) hdata_array((a),(b),(c),InvokeTickArg)
    
#define hdata_bytes_size(a,b) hdata_bytes_size((a),(b),InvokeTickArg)
    
#define hdata_bytes(a,b) hdata_bytes((a),(b),InvokeTickArg)
	
#define hdata_write_json(a,b,c,d) hdata_write_json((a),(b),(c),(d),InvokeTickArg)
	
#define hdata_write_binary(a,b,c) hdata_write_binary((a),(b),(c),InvokeTickArg)
	
#define hdata_print_json(a,b) hdata_print_json((a),(b),InvokeTickArg)
    
    
#define ext_data_alloc(a,b,c) ext_data_alloc((a),(b),(c),InvokeTickArg)
	
#define ext_data_ref_alloc(a,b) ext_data_ref_alloc((a),(b),InvokeTickArg)
	
#define ext_data_retain(a) ext_data_retain((a),InvokeTickArg)
    
#define ext_data_retain_count(a) ext_data_retain_count((a),InvokeTickArg)
    
#define ext_data_release(a) ext_data_release((a),InvokeTickArg)
	
#define ext_data(a) ext_data((a),InvokeTickArg)
	
#define ext_data_class(a) ext_data_class((a),InvokeTickArg)
	
#define ext_data_detach(a) ext_data_detach((a),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif


#endif
