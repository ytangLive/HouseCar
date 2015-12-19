/*
 *  hdata_storage.h
 *  C Library
 *
 *  Created by hailong zhang on 10-11-30.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */


#ifndef HDATA_XML_H
#define HDATA_XML_H

#include "hmem.h"
#include "hbuffer.h"
#include "hext_obj.h"
#include "hdata.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hdata_storage_t)
	
	extern ext_class_t data_storage_class;
	
	hdata_storage_t data_storage_alloc(hcchar * ,InvokeTickDeclare);
	
#define data_storage_retain(a)		(hdata_storage_t)(ext_obj_retain((hext_obj_t)(a)))
#define data_storage_release(a)		ext_obj_release((hext_obj_t)(a))
	
	void data_storage_add(hdata_storage_t storage,hext_data_t data_array,InvokeTickDeclare);
	
	void data_storage_clear(hdata_storage_t storage,InvokeTickDeclare);
	
	hint32 data_storage_total(hdata_storage_t storage,InvokeTickDeclare);
	
	hdata_t data_storage_item_at(hdata_storage_t storage,hint32 index,hext_data_t * ref_data,InvokeTickDeclare);
	
	void data_storage_available(hdata_storage_t storage,hany target,hint32 begin_index,hint32 end_index,InvokeTickDeclare);
	
	void data_storage_unavailable(hdata_storage_t storage,hany target,InvokeTickDeclare);
	
	void data_storage_uninstall(hdata_storage_t storage,InvokeTickDeclare);
    
   
    
#define data_storage_alloc(a) data_storage_alloc((a),InvokeTickArg)
	
#define data_storage_add(a,b) data_storage_add((a),(b),InvokeTickArg)
	
#define data_storage_clear(a) data_storage_clear((a),InvokeTickArg)
	
#define data_storage_total(a) data_storage_total((a),InvokeTickArg)
	
#define data_storage_item_at(a,b,c) data_storage_item_at((a),(b),(c),InvokeTickArg)
	
#define data_storage_available(a,b,c,d) data_storage_available((a),(b),(c),(d),InvokeTickArg)
	
#define data_storage_unavailable(a,b) data_storage_unavailable((a),(b),InvokeTickArg)
	
#define data_storage_uninstall(a) data_storage_uninstall((a),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif


#endif
