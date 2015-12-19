/*
 *  hdata_storage.c
 *  C Library
 *
 *  Created by hailong zhang on 10-11-30.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hdata_storage.h"
#include "hlist.h"
#include "hdata_binary.h"
#include "hdata_json.h"
#include "hfile.h"
#include "hstr.h"
#include "hlog.h"


#undef data_storage_alloc

#undef data_storage_add

#undef data_storage_clear

#undef data_storage_total

#undef data_storage_item_at

#undef data_storage_available

#undef data_storage_unavailable

#undef data_storage_uninstall

typedef struct{
	hext_data_t data;
	hint32 count;
}data_storage_tick_t;

typedef struct{
	hany target;
	hint32 begin;
	hint32 end;
}data_storage_target_t;

typedef struct{
	hlist_t ticks;
	hint32 total;
	hlist_t targets;
	hchar * cache_path;
}data_storage_t; 

static data_storage_tick_t * data_storage_tick_alloc(hext_data_t data ,InvokeTickDeclare){
	data_storage_tick_t * tick = mem_malloc(sizeof(data_storage_tick_t));
	memset(tick,0,sizeof(data_storage_tick_t));
	
	tick->data = (hext_data_t)ext_data_retain(data);
	tick->count = hdata_array_size(ext_data_class(data),ext_data(data));
	return tick;
}

static void data_storage_tick_dealloc(data_storage_t *storage,data_storage_tick_t *tick, hcchar * cache_path ,InvokeTickDeclare){
	hbuffer_t path;
	
	path = buffer_alloc(1024,  1024);
	
	buffer_append_str(path, cache_path);
	
	buffer_append_format(path, "/%ld_%ld.cache",InvokeTickArg,(hintptr)storage,(hintptr)tick);

	file_delete(buffer_to_str(path));
	
	buffer_dealloc(path);
	
	ext_data_release(tick->data);
	
	mem_free( tick);
}

static void data_storage_tick_uninstall(data_storage_t * storage,data_storage_tick_t *tick , hcchar * cache_path ,InvokeTickDeclare){
	hbuffer_t path ,data;
	FILE * f;
	if( storage && tick && tick->data){
		path = buffer_alloc(1024,  1024);
		
		buffer_append_str(path, cache_path);
		
		buffer_append_format(path, "/%ld_%ld.cache",InvokeTickArg,(hintptr)storage,(hintptr)tick);
		
		if(!file_exist(buffer_to_str(path))){
			
			data = buffer_alloc(1024,  1024);
			
			hdata_write_json(ext_data_class(tick->data), ext_data(tick->data), data,hbool_true);
			
			if((f = fopen(buffer_to_str(path), "w"))){
				fwrite(buffer_data(data), buffer_length(data), 1, f);
				fclose(f);
			}
			
			buffer_dealloc(data);
		}
		
		ext_data_release(tick->data);
		
		tick->data = NULL;
		
		buffer_dealloc(path);
	}
}


static hext_data_t data_storage_tick_data_get(data_storage_t * storage,data_storage_tick_t *tick, hcchar * cache_path ,InvokeTickDeclare){
	hbuffer_t path ;
	FILE * f;
	hext_obj_t data_bytes;
	hint32 length;
	hdata_t data_binary;
	if(storage && tick){
		if(tick->data == NULL){
			path = buffer_alloc(1024,  1024);
			
			buffer_append_str(path, cache_path);
			
			buffer_append_format(path, "/%ld_%ld.cache",InvokeTickArg,(hintptr)storage,(hintptr)tick);
				
			if( (length = file_size(buffer_to_str(path))) >0 && (f = fopen(buffer_to_str(path), "r"))){
				
				data_bytes = ext_bytes_obj_alloc( length+1);
				
				if( 1== fread(ext_bytes_obj_data(data_bytes), length, 1, f)){
					ext_bytes_obj_data(data_bytes)[length] = 0;
					//hlog("%s\n",ext_bytes_obj_data(data_bytes));
					data_binary = hdata_json_alloc(ext_bytes_obj_data(data_bytes));
					if(data_binary){
						tick->data = ext_data_alloc( data_binary, &hdata_json_class, hdata_json_dealloc);
					}
				}
				ext_obj_release(data_bytes);
				
				fclose(f);
			}
			
			buffer_dealloc(path);
		}
		
		return tick->data;
	}
	
	return NULL;
}


static void ext_data_storage_init(hext_obj_t obj,hint32 size,hany arg ,InvokeTickDeclare){
	data_storage_t * storage = (data_storage_t *)obj;
	memset(storage,0,size);
	storage->ticks = list_alloc( 20, 20);
	str_cpy(&storage->cache_path, (hcchar *)arg);
	storage->targets = list_alloc( 10,10);
}

static void ext_data_storage_destroy(hext_obj_t obj,hint32 size ,InvokeTickDeclare){
	data_storage_t *storage  = (data_storage_t *)obj;
	hint32 i, c = list_count(storage->ticks);
	for(i=0;i<c;i++){
		data_storage_tick_dealloc(storage,list_get(storage->ticks, i),storage->cache_path,InvokeTickArg);
	}
	list_dealloc(storage->ticks);
	c = list_count(storage->targets);
	for(i=0;i<c;i++){
		mem_free( list_get(storage->targets, i));
	}
	list_dealloc(storage->targets);
	mem_free( storage->cache_path);
}


ext_class_t data_storage_class = {ext_data_storage_init,ext_data_storage_destroy,sizeof(data_storage_t)};

hdata_storage_t data_storage_alloc(hcchar * cache_path ,InvokeTickDeclare){
	return (hdata_storage_t)ext_obj_alloc( (hany)cache_path, &data_storage_class);
}


void data_storage_add(hdata_storage_t hstorage,hext_data_t data_array ,InvokeTickDeclare){
	if(hstorage && data_array && hdata_type(ext_data_class(data_array), ext_data(data_array)) == HDATA_TYPE_ARRAY){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		data_storage_tick_t * tick = data_storage_tick_alloc(data_array,InvokeTickArg);
		list_add(storage->ticks, tick);
		storage->total += tick->count;
	}
}

void data_storage_clear(hdata_storage_t hstorage ,InvokeTickDeclare){
	if(hstorage){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		hint32 i, c = list_count(storage->ticks);
		for(i=0;i<c;i++){
			data_storage_tick_dealloc(storage,list_get(storage->ticks, i),storage->cache_path,InvokeTickArg);
		}
		list_clear(storage->ticks);
		storage->total = 0;
	}
}

hint32 data_storage_total(hdata_storage_t storage ,InvokeTickDeclare){
	return storage ? ((data_storage_t *)storage)->total: 0;
}

hdata_t data_storage_item_at(hdata_storage_t hstorage,hint32 index,hext_data_t * ref_data ,InvokeTickDeclare){
	if(hstorage ){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		data_storage_tick_t * tick;
		hext_data_t data;
		hint32 i,c,ii=0;
		if(index >=0 && index < storage->total){
			c = list_count(storage->ticks);
			for(i=0;i<c;i++){
				tick = list_get(storage->ticks, i);
				if(ii + tick->count > index){
					data = data_storage_tick_data_get(storage,tick,storage->cache_path,InvokeTickArg);
					if(ref_data){
						*ref_data = data;
					}
					return hdata_array(ext_data_class(data),ext_data(data),index - ii);
				}
				ii += tick->count;
			}
		}
	}
	if(ref_data){
		*ref_data = NULL;
	}
	return NULL;
}

void data_storage_available(hdata_storage_t hstorage,hany target,hint32 begin_index,hint32 end_index ,InvokeTickDeclare){
	if(hstorage){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		data_storage_target_t * t = NULL;
		hint32 i,c = list_count(storage->targets);
		for(i=0;i<c;i++){
			t = list_get(storage->targets, i);
			if(t->target == target){
				break;
			}
		}
		if(i>=c){
			t = mem_malloc( sizeof(data_storage_target_t));
			memset(t,0,sizeof(data_storage_target_t));
			t->target = target;
			list_add(storage->targets, t);
		}
		t->begin = begin_index;
		t->end = end_index;
	}
}

void data_storage_unavailable(hdata_storage_t hstorage,hany target ,InvokeTickDeclare){
	if(hstorage){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		data_storage_target_t * t = NULL;
		hint32 i,c = list_count(storage->targets);
		for(i=0;i<c;i++){
			t = list_get(storage->targets, i);
			if(t->target == target){
				list_remove_at(storage->targets, i--);
				c --;
				mem_free( t);
			}
		}
	}
}

void data_storage_uninstall(hdata_storage_t hstorage ,InvokeTickDeclare){
	if(hstorage){
		data_storage_t *storage  = (data_storage_t *)hstorage;
		data_storage_target_t * t = NULL;
		data_storage_tick_t * tick;
		hint32 i,c = list_count(storage->targets),ii;
		hint32 begin = storage->total , end = -1;
		for(i=0;i<c;i++){
			t = list_get(storage->targets, i);
			if(t->begin < begin){
				begin = t->begin;
			}
			if(t->end > end){
				end = t->end;
			}
		}
		c = list_count(storage->ticks);
		ii = 0;
		for(i=0;i<c;i++){
			tick = list_get(storage->ticks, i);
			
			if(ii + tick->count <= begin || ii > end){
				data_storage_tick_uninstall( storage, tick, storage->cache_path,InvokeTickArg);
			}
			
			ii += tick->count;
		}
	}
	
}

