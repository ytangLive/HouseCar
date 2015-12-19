/*
 *  hdata_json.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HDATA_JSON_H
#define HDATA_JSON_H

#include "hmem.h"
#include "hdata.h"
#include "hdata_alloc.h"
#include "hbuffer.h"
#include "hext_obj.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#define hdata_json_class hdata_class
	
	hdata_t hdata_json_alloc(hcchar * ,InvokeTickDeclare);
	
	void hdata_json_dealloc(hdata_t data,InvokeTickDeclare);
	
	hext_data_t ext_data_json_alloc(hcchar *,InvokeTickDeclare);
    
#define hdata_json_alloc(a) hdata_json_alloc((a),InvokeTickArg)
	
#define hdata_json_dealloc(a) hdata_json_dealloc((a),InvokeTickArg)
	
#define ext_data_json_alloc(a) ext_data_json_alloc((a),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif


#endif
