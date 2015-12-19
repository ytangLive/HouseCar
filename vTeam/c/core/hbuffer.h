/*
 *  buffer.h
 *  C Library
 *
 *  Created by hailong zhang on 10-7-31.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HBUFFER_H
#define HBUFFER_H

#include "hmem.h"

#define BUFFER_EXTEND_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _buffer_t{
     	hchar *buffer;
        hint32 length;   
    } * hbuffer_t;
	
	hbuffer_t buffer_alloc(hint32 capacity,hint32 extendSize, InvokeTickDeclare);
	
	void buffer_dealloc(hbuffer_t hBuffer, InvokeTickDeclare);
	
	void buffer_append(hbuffer_t hBuffer,hany data,hint32 length, InvokeTickDeclare);
	
	void buffer_append_str(hbuffer_t hBuffer,hcchar * str, InvokeTickDeclare);
	
	void buffer_clear(hbuffer_t hBuffer, InvokeTickDeclare);
	
	hcchar * buffer_to_str(hbuffer_t hBuffer, InvokeTickDeclare);
	
	void buffer_extend_size(hbuffer_t hBuffer,hint32 size, InvokeTickDeclare);
	
	hint32 buffer_size(hbuffer_t hBuffer, InvokeTickDeclare);
	
	hint32 buffer_length(hbuffer_t hBuffer, InvokeTickDeclare);
	
	void buffer_length_set(hbuffer_t hBuffer,hint32 length, InvokeTickDeclare);
	
	hchar * buffer_data(hbuffer_t hBuffer, InvokeTickDeclare);
	
	hchar * buffer_detach(hbuffer_t hBuffer, InvokeTickDeclare);
	
	void buffer_out(hbuffer_t hBuffer,hchar *data, InvokeTickDeclare);
	
	void buffer_append_format(hbuffer_t hbuffer,hcchar *format,InvokeTickDeclare,...);
	
    void buffer_append_format_va_list(hbuffer_t hbuffer,hcchar * format,va_list va_arg, InvokeTickDeclare);
	
	void buffer_append_md5_upper(hbuffer_t hbuffer,hbyte * data,hint32 length, InvokeTickDeclare);
	
	void buffer_append_md5(hbuffer_t hbuffer,hbyte * data,hint32 length, InvokeTickDeclare);
	
	void buffer_append_md5_str(hbuffer_t hbuffer,hcchar * str, InvokeTickDeclare);
	
	void buffer_append_md5_upper_str(hbuffer_t hbuffer,hcchar * str, InvokeTickDeclare);
    
    void buffer_append_url_encode(hbuffer_t hbuffer,hcchar * str,InvokeTickDeclare);
    
    hcchar * buffer_trim(hbuffer_t hbuffer, InvokeTickDeclare);

    hcchar * buffer_path_last(hbuffer_t hbuffer,InvokeTickDeclare);
    
    void buffer_path_last_remove(hbuffer_t hbuffer,InvokeTickDeclare);
    
    void buffer_append_int16le(hbuffer_t buffer,hint16 x,InvokeTickDeclare);
    
    void buffer_append_int16be(hbuffer_t buffer,hint16 x,InvokeTickDeclare);
    
    void buffer_append_int32le(hbuffer_t buffer,hint32 x,InvokeTickDeclare);
    
    void buffer_append_int32be(hbuffer_t buffer,hint32 x,InvokeTickDeclare);
    
    void buffer_append_int64le(hbuffer_t buffer,hint64 x,InvokeTickDeclare);
    
    void buffer_append_int64de(hbuffer_t buffer,hint64 x,InvokeTickDeclare);
    
#define buffer_alloc(a,b) buffer_alloc((a),(b),InvokeTickArg)
	
#define buffer_dealloc(a) buffer_dealloc((a),InvokeTickArg)
	
#define buffer_append(a,b,c) buffer_append((a),(b),(c),InvokeTickArg)
	
#define buffer_append_str(a,b) buffer_append_str((a),(b),InvokeTickArg)
	
#define buffer_clear(a) buffer_clear((a),InvokeTickArg)
	
#define buffer_to_str(a) buffer_to_str((a),InvokeTickArg)
	
#define buffer_extend_size(a,b) buffer_extend_size((a),(b),InvokeTickArg)
	
#define buffer_size(a) buffer_size((a),InvokeTickArg)
	
#define buffer_length(a) buffer_length((a),InvokeTickArg)
	
#define buffer_length_set(a,b) buffer_length_set((a),(b),InvokeTickArg)
	
#define buffer_data(a) buffer_data((a),InvokeTickArg)
	
#define buffer_detach(a) buffer_detach((a),InvokeTickArg)
	
#define buffer_out(a,b) buffer_out((a),(b),InvokeTickArg)
	
#define buffer_append_format_va_list(a,b,c) buffer_append_format_va_list((a),(b),(c),InvokeTickArg)
	
#define buffer_append_md5_upper(a,b,c) buffer_append_md5_upper((a),(b),(c),InvokeTickArg)
	
#define buffer_append_md5(a,b,c) buffer_append_md5((a),(b),(c),InvokeTickArg)
	
#define buffer_append_md5_str(a,b) buffer_append_md5_str((a),(b),InvokeTickArg)
	
#define buffer_append_md5_upper_str(a,b) buffer_append_md5_upper_str((a),(b),InvokeTickArg)
    
#define buffer_trim(a) buffer_trim((a),InvokeTickArg)
    
#define buffer_path_last(a) buffer_path_last((a),InvokeTickArg)
    
#define buffer_path_last_remove(a) buffer_path_last_remove((a),InvokeTickArg)
    
#define buffer_append_url_encode(a,b) buffer_append_url_encode((a),(b),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif


#endif


