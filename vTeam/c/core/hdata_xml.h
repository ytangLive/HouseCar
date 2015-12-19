/*
 *  hdata_xml.h
 *  C Library
 *
 *  Created by hailong zhang on 10-11-12.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */



#ifndef HDATA_XML_H
#define HDATA_XML_H

#include "hmem.h"
#include "hdata.h"
#include "hdata_alloc.h"
#include "hbuffer.h"
#include "hext_obj.h"
#include "hlist.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#define kTAGNAME	"__tagname__"
#define kVALUE		"__value__"
#define kCHILDS		"__childs__"
	
#define hdata_xml_class hdata_class
	
	hdata_t hdata_xml_alloc(hcchar * ,InvokeTickDeclare);
	
	void hdata_xml_dealloc(hdata_t data,InvokeTickDeclare);
	
	extern ext_class_t ext_data_xml_class;
	
	#define ext_data_xml_alloc(m,s)	ext_obj_alloc((m), (hany)(s), &ext_data_xml_class)
	
	hcchar * hdata_xml_text(hdata_t data,hcchar * default_value,InvokeTickDeclare);
	
	hcchar * hdata_xml_attr(hdata_t data,hcchar * name,hcchar * default_value,InvokeTickDeclare);
	
	void hdata_xml_value_encode(hcchar * value,hbuffer_t buffer,InvokeTickDeclare);
	
	void hdata_xml_value_decode(hcchar * value,hbuffer_t buffer,InvokeTickDeclare);
	
	
	
	HANDLER(hdata_xpath_t)
	
	extern ext_class_t ext_data_xpath_class;
	
#define hdata_xpath_alloc(data)		(hdata_xpath_t)ext_obj_alloc( (data), &ext_data_xpath_class)
#define hdata_xpath_retain(xpath)	(hdata_xpath_t)ext_obj_retain((hext_obj_t)(xpath))
#define hdata_xpath_release(xpath)	ext_obj_release((hext_obj_t)(xpath))
	
	HANDLER(hdata_xpath_selector_t)
	

	void hdata_xpath(hdata_xpath_t, hcchar * xpath,hlist_t selectors,hdata_xpath_selector_t parent_selector,InvokeTickDeclare);
	
	hdata_t hdata_xpath_selector_data(hdata_xpath_selector_t selecotr,InvokeTickDeclare);
	
	hdata_xpath_selector_t hdata_xpath_selector_parent(hdata_xpath_selector_t selecotr,InvokeTickDeclare);
	
	hdata_xpath_selector_t hdata_xpath_single(hdata_xpath_t, hcchar * xpath,hdata_xpath_selector_t parent_selector,InvokeTickDeclare);
	
	hdata_t hdata_xpath_single_data(hdata_xpath_t, hcchar * xpath,hdata_xpath_selector_t parent_selector,InvokeTickDeclare);

	hcchar * hdata_xpath_single_text(hdata_xpath_t, hcchar * xpath,hdata_xpath_selector_t parent_selector,InvokeTickDeclare);
	
	hcchar * hdata_xpath_single_attr_string(hdata_xpath_t, hcchar * xpath,hcchar * attrName,hdata_xpath_selector_t parent_selector,InvokeTickDeclare);
    
    
    
#define hdata_xml_alloc(a ) hdata_xml_alloc((a),InvokeTickArg)
	
#define hdata_xml_dealloc(a) hdata_xml_dealloc((a),InvokeTickArg)
	
#define hdata_xml_text(a,b) hdata_xml_text((a),(b),InvokeTickArg)
	
#define hdata_xml_attr(a,b,c) hdata_xml_attr((a),(b),(c),InvokeTickArg)
	
#define hdata_xml_value_encode(a,b) hdata_xml_value_encode((a),(b),InvokeTickArg)
	
#define hdata_xml_value_decode(a,b) hdata_xml_value_decode((a),(b),InvokeTickArg)
    
#define hdata_xpath(a,b,c,d) hdata_xpath((a),(b),(c),(d),InvokeTickArg)
	
#define hdata_xpath_selector_data(a) hdata_xpath_selector_data((a),InvokeTickArg)
	
#define hdata_xpath_selector_parent(a) hdata_xpath_selector_parent((a),InvokeTickArg)
	
#define hdata_xpath_single(a,b,c) hdata_xpath_single((a),(b),(c),InvokeTickArg)
	
#define hdata_xpath_single_data(a,b,c) hdata_xpath_single_data((a),(b),(c),InvokeTickArg)
    
#define hdata_xpath_single_text(a,b,c) hdata_xpath_single_text((a),(b),(c),InvokeTickArg)
	
#define hdata_xpath_single_attr_string(a,b,c,d) hdata_xpath_single_attr_string((a),(b),(c),(d),InvokeTickArg)
    
    
	
#ifdef __cplusplus
}
#endif


#endif
