/*
 *  hserial_map.h
 *  C_Library
 *
 *  Created by hailong zhang on 11-3-8.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */


#ifndef _HSERIAL_MAP_H
#define _HSERIAL_MAP_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "hmem.h"
#include "hbuffer.h"
    
    typedef struct _serial_map_t{
        hbyte * data;
        hint32 length;
        hint32 item_length;
    } * hserial_map_t;
    
	hserial_map_t serial_map_alloc(hint32 key_bytes,hint32 value_bytes,hint32 extra_bytes,InvokeTickDeclare);
	
	void serial_map_dealloc(hserial_map_t map,InvokeTickDeclare);
	
	hany serial_map_extra(hserial_map_t map,InvokeTickDeclare);
	
	hint32 serial_map_extra_bytes(hserial_map_t map,InvokeTickDeclare);
	
	hint32 serial_map_count(hserial_map_t map,InvokeTickDeclare);
	
	hany serial_map_key_at(hserial_map_t map,hint32 i,InvokeTickDeclare);
	
	hany serial_map_value_at(hserial_map_t map,hint32 i,InvokeTickDeclare);
	
	hbool serial_map_value_set(hserial_map_t map,hint32 i,hany value,InvokeTickDeclare);
	
	hany serial_map_get(hserial_map_t map,hany key,InvokeTickDeclare);
	
	hany serial_map_put(hserial_map_t map,hany key, hany value,InvokeTickDeclare);
	
	hbool serial_map_remove(hserial_map_t map,hany key,InvokeTickDeclare);
	
	hbool serial_map_clear(hserial_map_t map,InvokeTickDeclare);
    
    hint32 serial_map_load_bytes(hserial_map_t map,hbyte * bytes,hint32 length,InvokeTickDeclare);
    
    hint32 serial_map_save_bytes(hserial_map_t map,hbuffer_t buffer,InvokeTickDeclare);
    
#define serial_map_alloc(a,b,c) serial_map_alloc((a),(b),(c),InvokeTickArg)
	
#define serial_map_dealloc(a) serial_map_dealloc((a),InvokeTickArg)
	
#define serial_map_extra(a) serial_map_extra((a),InvokeTickArg)
	
#define serial_map_extra_bytes(a) serial_map_extra_bytes((a),InvokeTickArg)
	
#define serial_map_count(a) serial_map_count((a),InvokeTickArg)
	
#define serial_map_key_at(a,b) serial_map_key_at((a),(b),InvokeTickArg)
	
#define serial_map_value_at(a,b) serial_map_value_at((a),(b),InvokeTickArg)
	
#define serial_map_value_set(a,b,c) serial_map_value_set((a),(b),(c),InvokeTickArg)
	
#define serial_map_get(a,b) serial_map_get((a),(b),InvokeTickArg)
	
#define serial_map_put(a,b,c) serial_map_put((a),(b),(c),InvokeTickArg)
	
#define serial_map_remove(a,b) serial_map_remove((a),(b),InvokeTickArg)
	
#define serial_map_clear(a) serial_map_clear((a),InvokeTickArg)
	
#define serial_map_load_bytes(a,b,c) serial_map_load_bytes((a),(b),(c)InvokeTickArg)
    
#define serial_map_save_bytes(a,b) serial_map_save_bytes((a),(b),InvokeTickArg)

#ifdef __cplusplus
}
#endif


#endif
