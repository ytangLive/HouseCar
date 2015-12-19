/*
 *  hdata_xml.c
 *  C Library
 *
 *  Created by hailong zhang on 10-11-12.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hlist.h"
#include "hmap.h"
#include "hrecursive.h"
#include "hstack.h"
#include "hstr.h"
#include "hdata_xml.h"
#include "hqueue.h"


#undef hdata_xml_alloc

#undef hdata_xml_dealloc

#undef hdata_xml_text

#undef hdata_xml_attr

#undef hdata_xml_value_encode

#undef hdata_xml_value_decode

#undef hdata_xpath

#undef hdata_xpath_selector_data

#undef hdata_xpath_selector_parent

#undef hdata_xpath_single

#undef hdata_xpath_single_data

#undef hdata_xpath_single_text

#undef hdata_xpath_single_attr_string

#define SPACE_CHAR(c)  ((c) ==' ' || (c) =='\t' || (c) =='\r' || (c) == '\n')
#define TRUE_EQUAL(p)  (p[0] =='t' && p[1] == 'r' && p[2] =='u' && p[3] =='e')
#define FALSE_EQUAL(p)  (p[0] =='f' && p[1] == 'a' && p[2] =='l' && p[3] =='s' && p[4] =='e')
#define NULL_EQUAL(p)	(p[0] =='n' && p[1] == 'u' && p[2] =='l' && p[3] =='l')
#define NUMBER_CHAR(c)	(((c) >='0' && (c) <='9') || (c) =='-' || (c) == '.' || (c) =='e' )

static hdata_t hdata_xml_value_parse(hcchar * value,InvokeTickDeclare){
	hchar * p = (hchar *)value;
	hdouble double_value = 0;
	hbuffer_t decode_buffer;
	hdata_t data;
	
	if(TRUE_EQUAL(p)){
		return hdata_boolean_alloc( hbool_true);
	}
	else if(FALSE_EQUAL(p)){
		return hdata_boolean_alloc( hbool_false);
	}
	while(*p != '\0'){
		if(!NUMBER_CHAR(*p)){
			break;
		}
		p++;
	}
	if(p != '\0'){
		decode_buffer = buffer_alloc(1024, 1024);
		hdata_xml_value_decode(value,decode_buffer,InvokeTickArg);
		data = hdata_string_alloc( buffer_to_str(decode_buffer));
		buffer_dealloc(decode_buffer);
		return data;
	}
	else{
		sscanf(value, "%lf",&double_value);
		if((hint16)double_value == double_value){
			return hdata_int16_alloc((hint16)double_value );
		}
		else if((hint32)double_value == double_value){
			return hdata_int32_alloc((hint32)double_value );
		}
		else if((hint64)double_value == double_value){
			return hdata_int64_alloc((hint64)double_value );
		}
		else{
			return hdata_double_alloc( double_value);
		}
	}
}

hdata_t hdata_xml_alloc(hcchar * xml_str,InvokeTickDeclare){
	hdata_t data = NULL,data_temp;
	hchar * p=(hchar *)xml_str;
	hstack_t data_stack = stack_alloc();
	hstack_t state_stack = stack_alloc();
	hbuffer_t tag_name_buffer = buffer_alloc(128,  256);
	hbuffer_t content_buffer = buffer_alloc(1024,  1024);
	hbuffer_t attr_name_buffer =buffer_alloc(128,  256);
	hbuffer_t attr_value_buffer = buffer_alloc(128,  256);
	hintptr s;
	hchar str_begin = 0;
	stack_push(state_stack, (hany) 0);
	stack_push(data_stack,hdata_array_alloc( 20, 20));
	
	while (*p != '\0') {
		s = (hintptr)stack_peek(state_stack);
		if(s == 0x00){
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '<' && p[1] == '?'){
				stack_push(state_stack, (hany)0x10);
				p++;
			}
			else if( *p == '<' && p[1] == '!'){
				stack_push(state_stack, (hany)0x11);
				p++;
			}
			else if( *p == '<' && p[1] == '/'){
				data_temp = stack_pop(data_stack);
				data = stack_pop(data_stack);
				if( hdata_array_size(&hdata_class, data_temp) ==0){
					hdata_object_remove(data, kCHILDS);
				}
				if(buffer_length(content_buffer) >0){
					hdata_object_put(data, kVALUE, hdata_xml_value_parse(buffer_to_str(content_buffer),InvokeTickArg));
					buffer_clear(content_buffer);
				}
				while( *p != '\0' && *p != '>'){
					p ++;
				}
				if(*p == '\0'){
					break;
				}
			}
			else if( *p == '<'){
				buffer_clear(content_buffer);
				buffer_clear(tag_name_buffer);
				data = hdata_object_alloc();
				hdata_array_add(stack_peek(data_stack), data);
				stack_push(data_stack, data);
				stack_push(state_stack, (hany)0x20);
			}
			else{
				buffer_append(content_buffer,p,1);
			}
		}
		else if(s ==0x10){
			// <? ?>
			if( p[0] == '?' && p[1] == '>'){
				p++;
				stack_pop(state_stack);
			}
		}
		else if(s == 0x11){
			// <!
			if(p[0] == '>' ){
				stack_pop(state_stack);
			}
		}
		else if(s == 0x20){
			// <tagname 
			if(SPACE_CHAR(*p)){
				hdata_object_put((hdata_t)stack_peek(data_stack), kTAGNAME, hdata_string_alloc( buffer_to_str(tag_name_buffer)));
				buffer_clear(tag_name_buffer);
				stack_pop(state_stack);
				stack_push(state_stack, (hany)0x21);
			}
			else if( *p == '/' && p[1] =='>'){
				hdata_object_put((hdata_t)stack_peek(data_stack), kTAGNAME, hdata_string_alloc( buffer_to_str(tag_name_buffer)));
				buffer_clear(tag_name_buffer);
				stack_pop(data_stack);
				stack_pop(state_stack);
				p++;
			}
			else if( *p == '>'){
				hdata_object_put((hdata_t)stack_peek(data_stack), kTAGNAME, hdata_string_alloc( buffer_to_str(tag_name_buffer)));
				buffer_clear(tag_name_buffer);
				data = hdata_array_alloc( 20, 20);
				hdata_object_put(stack_peek(data_stack), kCHILDS, data);
				stack_push(data_stack, data);
				stack_pop(state_stack);
			}
			else{
				buffer_append(tag_name_buffer, p, 1);
			}
		}
		else if(s == 0x21){
			// <tagname attrname=
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '/' && p[1] =='>'){
				stack_pop(data_stack);
				stack_pop(state_stack);
				p++;
			}
			else if( *p == '>'){
				data = hdata_array_alloc( 20, 20);
				hdata_object_put(stack_peek(data_stack), kCHILDS, data);
				stack_push(data_stack, data);
				stack_pop(state_stack);
			}
			else if( *p == '='){
				stack_push(state_stack, (hany)0x22);
			}
			else {
				buffer_append(attr_name_buffer,p,1);
			}
		}
		else if(s == 0x22){
			// <tagname attrname=valuebegin
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '\''){
				str_begin = '\'';
				stack_pop(state_stack);
				stack_push(state_stack, (hany)0x23);
			}
			else if( *p == '"'){
				str_begin = '"';
				stack_pop(state_stack);
				stack_push(state_stack, (hany)0x23);
			}
			else {
				str_begin = 0;
				stack_pop(state_stack);
				stack_push(state_stack, (hany)0x23);
				buffer_append(attr_value_buffer, p, 1);
			}

		}
		else if( s == 0x23){
			// <tagname attrname=value
			if(str_begin ==0){
				if(SPACE_CHAR(*p)){
					hdata_object_put((hdata_t)stack_peek(data_stack),buffer_to_str(attr_name_buffer),hdata_xml_value_parse(buffer_to_str(attr_value_buffer),InvokeTickArg));
					buffer_clear(attr_name_buffer);
					buffer_append_str(attr_name_buffer, "@");
					buffer_clear(attr_value_buffer);
					stack_pop(state_stack);
				}
				else{
					buffer_append(attr_value_buffer, p, 1);
				}
			}
			else {
				if( p[0] == '\\' ){
					if(p[1] == '\\'){
						buffer_append_str(attr_value_buffer, "\\");
					}
					else if(p[1] == 'n'){
						buffer_append_str(attr_value_buffer, "\n");
					}
					else if(p[1] == 'r'){
						buffer_append_str(attr_value_buffer, "\r");
					}
					else if(p[1] == 't'){
						buffer_append_str(attr_value_buffer, "\t");
					}
					else if(p[1] == '"'){
						buffer_append_str(attr_value_buffer, "\"");
					}
					else if(p[1] == '\''){
						buffer_append_str(attr_value_buffer, "\'");
					}
					else{
						buffer_append(attr_value_buffer,p+1,1);
					}
					p ++;
				}
				else if( p[0] == str_begin){
					hdata_object_put((hdata_t)stack_peek(data_stack),buffer_to_str(attr_name_buffer),hdata_xml_value_parse(buffer_to_str(attr_value_buffer),InvokeTickArg));
					buffer_clear(attr_name_buffer);
					buffer_append_str(attr_name_buffer, "@");
					buffer_clear(attr_value_buffer);
					stack_pop(state_stack);
				}
				else{
					buffer_append(attr_value_buffer,p,1);
				}
			}
		}

		
		p++;
	}
	
	
	
	data = (hdata_t)stack_pop(data_stack);
	data_temp = (hdata_t)stack_pop(data_stack);
	
	while(data_temp){
		hdata_dealloc(data);
		data = data_temp;
		data_temp = (hdata_t)stack_pop(data_stack);
	}
	

	buffer_dealloc(tag_name_buffer);
	buffer_dealloc(content_buffer);
	buffer_dealloc(attr_name_buffer);
	buffer_dealloc(attr_value_buffer);
	stack_dealloc(data_stack);
	stack_dealloc(state_stack);
	
	return data;
}

hcchar * hdata_xml_text(hdata_t data,hcchar * default_value,InvokeTickDeclare){
	return hdata_object_string(&hdata_xml_class,data,kVALUE,default_value);
}

hcchar * hdata_xml_attr(hdata_t data,hcchar * name,hcchar * default_value,InvokeTickDeclare){
	return hdata_object_string(&hdata_xml_class,data,name,default_value);
}

void hdata_xml_dealloc(hdata_t data,InvokeTickDeclare){
	hdata_dealloc(data);
}


static void ext_data_xml_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	hdata_t data = hdata_xml_alloc((hcchar *)arg,InvokeTickArg);
	if(data){
		memcpy(obj,&data,sizeof(hdata_t));
	}
}

static void ext_data_xml_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	hdata_xml_dealloc(*(hdata_t *)obj,InvokeTickArg);
}


ext_class_t ext_data_xml_class = {ext_data_xml_init,ext_data_xml_destroy,sizeof(hdata_t)};


void hdata_xml_value_encode(hcchar * value,hbuffer_t buffer,InvokeTickDeclare){
	hchar * p = (hchar *)value;
	while(*p != '\0'){
		
		if(*p == ' '){
			buffer_append_str(buffer, "&nbsp;");
		}
		else if(*p == '<'){
			buffer_append_str(buffer, "&lt;");
		}
		else if(*p == '<'){
			buffer_append_str(buffer, "&gt;");
		}
		else if(*p == '&'){
			buffer_append_str(buffer, "&amp;");
		}
		else {
			buffer_append(buffer,p,1);
		}
		p ++;
	}
}

void hdata_xml_value_decode(hcchar * value,hbuffer_t buffer,InvokeTickDeclare){
	hchar * p = (hchar *)value;
	while(*p != '\0'){
		
		if(p[0] == '&' && p[1] == 'n' && p[2] == 'b' && p[3] == 's' && p[4] == 'p' && p[5] == ';'){
			buffer_append_str(buffer, " ");
			p+= 5;
		}
		else if(p[0] == '&' && p[1] == 'l' && p[2] == 't' && p[3] == ';'){
			buffer_append_str(buffer, "<");
			p+= 3;
		}
		else if(p[0] == '&' && p[1] == 'g' && p[2] == 't' && p[3] == ';'){
			buffer_append_str(buffer, ">");
			p+= 3;
		}
		else if(p[0] == '&' && p[1] == 'a' && p[2] == 'm' && p[3] == 'p' && p[4] == ';' ){
			buffer_append_str(buffer, "&");
			p+= 4;
		}
		else {
			buffer_append(buffer,p,1);
		}
		p ++;
	}
}



typedef struct{
	hcchar * tagName;
	hlist_t selectors;
}data_xpath_tag_t;


typedef struct _data_xpath_selector_t{
	struct  _data_xpath_selector_t *parent;
	hdata_t data;
	hmap_t child_tags;
}data_xpath_selector_t;

typedef struct{
	data_xpath_selector_t *root;
}data_xpath_t;

data_xpath_selector_t * data_xpath_selector_alloc(data_xpath_selector_t *parent,hdata_t data,InvokeTickDeclare);

static data_xpath_tag_t * data_xpath_tag_alloc(hcchar * tagName,InvokeTickDeclare){
	data_xpath_tag_t * tag = (data_xpath_tag_t *)mem_malloc( sizeof(data_xpath_tag_t));
	memset(tag,0,sizeof(data_xpath_tag_t));
	tag->tagName = tagName;
	tag->selectors = list_alloc( 20, 20);
	return tag;
}

static void data_xpath_selector_handle_child_tags(data_xpath_selector_t *selector,hdata_t data,InvokeTickDeclare){
	if(hdata_type(&hdata_xml_class,data) == HDATA_TYPE_OBJECT){
		hdata_t childs = hdata_object(&hdata_xml_class,data,kCHILDS);
		hdata_t d;
		hint32 i,c;
		hcchar * tagName;
		data_xpath_tag_t * xpath_tag;
		if(childs && hdata_type(&hdata_xml_class,childs) == HDATA_TYPE_ARRAY){
			c = hdata_array_size(&hdata_xml_class,childs);
			for(i=0;i<c;i++){
				d = hdata_array(&hdata_xml_class,childs,i);
				if(hdata_type(&hdata_xml_class, d) == HDATA_TYPE_OBJECT
				   && (tagName = hdata_object_string(&hdata_xml_class,d,kTAGNAME,NULL))){
					xpath_tag = map_get(selector->child_tags, (hany)tagName);
					if(xpath_tag ==NULL){
						xpath_tag = data_xpath_tag_alloc(tagName,InvokeTickArg);
						map_put(selector->child_tags, (hany)tagName, xpath_tag);
					}
					list_add(xpath_tag->selectors, data_xpath_selector_alloc(selector,d,InvokeTickArg));
				}
			}
		}
	}
}

static void data_xpath_selector_handle(data_xpath_selector_t *selector,InvokeTickDeclare){
	if(selector && selector->data && selector->child_tags ==NULL){
		hdata_type_code type = hdata_type(&hdata_xml_class,selector->data);
		hint32 i,c;
		if(selector->child_tags ==NULL){
			selector->child_tags = map_alloc( hash_code_str, equal_str);
		}
		if(type==HDATA_TYPE_OBJECT){
			data_xpath_selector_handle_child_tags(selector,selector->data,InvokeTickArg);
		}
		else if(type == HDATA_TYPE_ARRAY){
			c = hdata_array_size(&hdata_xml_class, selector->data);
			for(i=0;i<c;i++){
				data_xpath_selector_handle_child_tags(selector,hdata_array(&hdata_xml_class,selector->data,i),InvokeTickArg);
			}
		}
	}
}

data_xpath_selector_t * data_xpath_selector_alloc(data_xpath_selector_t *parent,hdata_t data,InvokeTickDeclare){
	if(data){
		data_xpath_selector_t * selector = (data_xpath_selector_t *)mem_malloc( sizeof(data_xpath_selector_t));
		memset(selector,0,sizeof(data_xpath_selector_t));
		selector->data = data;
		selector->parent = parent;
		selector->child_tags = NULL;
		return selector;
	}
	return NULL;
}

void data_xpath_selector_dealloc(data_xpath_selector_t * selector,InvokeTickDeclare);

static void data_xpath_tag_dealloc(data_xpath_tag_t * tag,InvokeTickDeclare){
	if(tag){
		hint32 i,c;
		c = list_count(tag->selectors);
		for(i=0;i<c;i++){
			data_xpath_selector_dealloc((data_xpath_selector_t *)list_get(tag->selectors, i),InvokeTickArg);
		}
		list_dealloc(tag->selectors);
		mem_free( tag);
	}
}

static void data_xpath_selector_child_tags_dealloc_map_each(hmap_t map,hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
	data_xpath_tag_dealloc((data_xpath_tag_t *)value,InvokeTickArg);
}

void data_xpath_selector_dealloc(data_xpath_selector_t * selector,InvokeTickDeclare){
	if(selector){
		map_each(selector->child_tags, data_xpath_selector_child_tags_dealloc_map_each, NULL, NULL);
		map_dealloc(selector->child_tags);
		mem_free( selector);
	}
}


typedef struct{
	hbyte type;
}data_xpath_tag_filter_t;

typedef struct{
	hbyte type;
	hchar * name;
	hchar * value;
}data_xpath_tag_filter_attr_t;

#define DATA_XPATH_TAG_FILTER_NONE 0
#define DATA_XPATH_TAG_FILTER_ATTR	1


static data_xpath_tag_filter_t * data_xpath_tag_filter_attr_alloc(hcchar *attrName,hcchar * attrValue,InvokeTickDeclare){
	data_xpath_tag_filter_attr_t * filter = (data_xpath_tag_filter_attr_t *)mem_malloc( sizeof(data_xpath_tag_filter_attr_t));
	memset(filter,0,sizeof(data_xpath_tag_filter_attr_t));
	filter->type = DATA_XPATH_TAG_FILTER_ATTR;
	str_cpy(&filter->name, attrName);
	str_cpy(&filter->value, attrValue);
	return (data_xpath_tag_filter_t *)filter;
}

static void data_xpath_tag_filter_dealloc(data_xpath_tag_filter_t *filter,InvokeTickDeclare){
	if(filter->type == DATA_XPATH_TAG_FILTER_ATTR){
		data_xpath_tag_filter_attr_t * filterAttr = (data_xpath_tag_filter_attr_t *)filter;
		mem_free( filterAttr->name);
		mem_free( filterAttr->value);
	}
	mem_free( filter);
}


static hbool data_xpath_tag_filter_filte(data_xpath_selector_t * selector,data_xpath_tag_filter_t * filter,InvokeTickDeclare){
	if(filter->type == DATA_XPATH_TAG_FILTER_ATTR){
		data_xpath_tag_filter_attr_t * filterAttr = (data_xpath_tag_filter_attr_t *)filter;
		hcchar * value = hdata_object_string(&hdata_xml_class,selector->data,filterAttr->name,NULL);
		return filterAttr->value == NULL || filterAttr->value == 0 || str_cmp(filterAttr->value, value) == 0;
	}
	return hbool_true;
}

static hbool data_xpath_tag_filter_exists( data_xpath_tag_t * tag,hlist_t filters,InvokeTickDeclare){
	hint32 i,c;
	hint32 fi,fc;
	data_xpath_selector_t *selector;
	c = list_count(tag->selectors);
	fc = list_count(filters);
	for(i=0;i<c;i++){
		selector = list_get(tag->selectors, i);
		for(fi = 0;fi<fc;fi++){
			if(! data_xpath_tag_filter_filte(selector,list_get(filters, fi),InvokeTickArg)){
				break;
			}
		}
		if(fi >= fc){
			return hbool_true;
		}
	}
	return hbool_false;
}
static void data_xpath_tag_filter_handle( data_xpath_tag_t * tag, hlist_t selectors,hlist_t filters,InvokeTickDeclare){
	hint32 i,c;
	hint32 fi,fc;
	data_xpath_selector_t *selector;
	c = list_count(tag->selectors);
	fc = list_count(filters);
	for(i=0;i<c;i++){
		selector = list_get(tag->selectors, i);
		for(fi = 0;fi<fc;fi++){
			if(! data_xpath_tag_filter_filte(selector,list_get(filters, fi),InvokeTickArg)){
				break;
			}
		}
		if(fi >= fc){
			list_add(selectors, selector);
		}
	}
}


static hint32 data_xpath_tag_filter_parse(hlist_t filters,hcchar * exp,InvokeTickDeclare){
	hint32 index = -1;
	hchar * p = (hchar *)exp,number =1,s=0;
	hbuffer_t name = buffer_alloc(128,  128);
	hbuffer_t value = buffer_alloc(128,  128);
	
	while(*p){
		
		if(s ==0){
			if(SPACE_CHAR(*p)){
			}
			else{
				buffer_append(name, p, 1);
				s = 1;
				if(number && !NUMBER_CHAR(*p)){
					number = 0;
				}
			}
		}
		else if(s == 1){
			//name
			if(SPACE_CHAR(*p)){
			}
			else if(*p == '='){
				s = 2;
				if(number && !NUMBER_CHAR(*p)){
					number = 0;
				}
			}
			else {
				buffer_append(name, p, 1);
				if(number && !NUMBER_CHAR(*p)){
					number = 0;
				}
			}
		}
		else if(s == 2){
			// value
			if( *p == ','){
				s =1;
				if(number && !NUMBER_CHAR(*p)){
					number = 0;
				}
				if(buffer_length(name) >0){
					list_add(filters, data_xpath_tag_filter_attr_alloc(buffer_to_str(name),buffer_to_str(value),InvokeTickArg));
				}
				buffer_clear(name);
				buffer_clear(value);
			}
			else {
				buffer_append(value, p, 1);
			}
		}
		
		p++;
	}
	
	if(buffer_length(name) >0){
		if(number){
			index = atoi(buffer_to_str(name));
		}
		else{
			if(buffer_length(name) >0){
				list_add(filters, data_xpath_tag_filter_attr_alloc(buffer_to_str(name),buffer_to_str(value),InvokeTickArg));
			}
		}
	}
	
	buffer_dealloc(name);
	buffer_dealloc(value);
	
	return index;
}

static data_xpath_tag_t * data_xpath_find_tag(data_xpath_selector_t * selector,hcchar *tagName,hlist_t filters,InvokeTickDeclare){
	data_xpath_tag_t *tag;
	data_xpath_selector_handle(selector,InvokeTickArg);
	tag = (data_xpath_tag_t *)map_get(selector->child_tags, (hany)tagName);
	if(tag && filters && list_count(filters) >0){
		if(!data_xpath_tag_filter_exists( tag, filters,InvokeTickArg)){
			tag = NULL;
		}
	}
	return tag;
}

typedef struct{
	hqueue_t queue;
	data_xpath_tag_t * result;
	hcchar * tagName;
	hlist_t filters;
}data_xpath_search_tag_param_t;

static void data_xpath_search_tag_map_each(hmap_t map,hany key,hany value,hany arg0,hany arg1,InvokeTickDeclare){
	data_xpath_search_tag_param_t *param = (data_xpath_search_tag_param_t *)arg0;
	data_xpath_tag_t * tag = (data_xpath_tag_t * )value;
	data_xpath_selector_t * selector;
	hint32 i,c;
	if(param->result == NULL){
		c = list_count(tag->selectors);
		for(i=0;i<c;i++){
			selector = list_get(tag->selectors, i);
			param->result = data_xpath_find_tag(selector,param->tagName,param->filters,InvokeTickArg);
			if(param->result){
				break;
			}
			else{
				queue_enter(param->queue, selector,0);
			}
		}
	}
}

static data_xpath_tag_t * data_xpath_search_tag(data_xpath_selector_t * selector,hcchar *tagName,hlist_t filters,InvokeTickDeclare){
	data_xpath_tag_t * tag = data_xpath_find_tag(selector,tagName,filters,InvokeTickArg);
	if(tag == NULL){
		data_xpath_search_tag_param_t param = {queue_alloc(),NULL,tagName,filters};
		data_xpath_selector_t * s;
		map_each(selector->child_tags, data_xpath_search_tag_map_each, &param, NULL);
		
		while(param.result==NULL && (s = queue_out(param.queue))){
			map_each(s->child_tags, data_xpath_search_tag_map_each, &param, NULL);
		}
		
		queue_dealloc(param.queue);
		return param.result;
	}
	else{
		return tag;
	}
}

static void ext_data_xpath_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
	data_xpath_t * xpath = (data_xpath_t *)obj;
	xpath->root = data_xpath_selector_alloc(NULL,(hdata_t)arg,InvokeTickArg);
}

static void ext_data_xpath_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
	data_xpath_t * xpath = (data_xpath_t *)obj;
	data_xpath_selector_dealloc(xpath->root,InvokeTickArg);
}



ext_class_t ext_data_xpath_class = {ext_data_xpath_init,ext_data_xpath_destroy,sizeof(data_xpath_t)};



static void hdata_xpath_selctor(data_xpath_t * xpath,data_xpath_selector_t * selector,hlist_t result, hcchar * selector_name,hcchar * exp,hbool r,InvokeTickDeclare){
	data_xpath_tag_t * tag = NULL;
	hint32 i,c ;
	hlist_t filters = list_alloc( 10, 10);
	hint32 index = data_xpath_tag_filter_parse( filters, exp,InvokeTickArg);
	
	if(r ){
		tag = data_xpath_search_tag(selector,selector_name,index<0?filters:NULL,InvokeTickArg);
	}
	else{
		tag = data_xpath_find_tag(selector,selector_name,index<0?filters:NULL,InvokeTickArg);
	}
	
	if(tag){
		if(index <0){
			data_xpath_tag_filter_handle( tag, result, filters,InvokeTickArg);
		}
		else if(index < list_count(tag->selectors)){
			list_add(result, list_get(tag->selectors, index));
		}
	}
	
	c = list_count(filters);
	for(i=0;i<c;i++){
		data_xpath_tag_filter_dealloc( list_get(filters, i),InvokeTickArg);
	}
	
	list_dealloc(filters);
}

void hdata_xpath(hdata_xpath_t data_xpath, hcchar * path,hlist_t result,hdata_xpath_selector_t parent_selector,InvokeTickDeclare){
	if(data_xpath && path){
		data_xpath_t * xpath = (data_xpath_t *)data_xpath;
		hchar * p = (hchar *)path,s = 0,r = 1;
		hbuffer_t selector_name = buffer_alloc(128,  128);
		hbuffer_t selector_exp = buffer_alloc(128,  128);
		
		data_xpath_selector_t * selector = parent_selector?(data_xpath_selector_t * )parent_selector:xpath->root;
		hlist_t selector_list = list_alloc( 10, 20);
		hint32 i,c;
		
		while(*p != '\0'){
			if(s ==0){
				if(SPACE_CHAR(*p)){
				}
				else if(*p == '/'){
					if(buffer_length(selector_name) >0){
						list_clear(selector_list);
						hdata_xpath_selctor(xpath,selector,selector_list,buffer_to_str(selector_name),buffer_to_str(selector_exp),r,InvokeTickArg);
						if(list_count(selector_list) ==0){
							break;
						}
						else{
							selector = list_get(selector_list, 0);
						}
						buffer_clear(selector_name);
						buffer_clear(selector_exp);
						r = !r;
					}
					else{
						r = !r;
					}
				}
				else if(*p == '['){
					s = 1;
				}
				else{
					buffer_append(selector_name, p, 1);
				}
			}
			else if(s == 1){
				if(*p == ']'){
					s = 0;
				}
				else{
					buffer_append(selector_exp,p,1);
				}
			}
			p++;
		}
		if(buffer_length(selector_name) >0){
			list_clear(selector_list);
			hdata_xpath_selctor(xpath,selector,selector_list,buffer_to_str(selector_name),buffer_to_str(selector_exp),r,InvokeTickArg);
			if(list_count(selector_list) !=0){
				selector = list_get(selector_list, 0);
			}
		}
		buffer_dealloc(selector_name);
		buffer_dealloc(selector_exp);
		c = list_count(selector_list);
		for(i=0;i<c;i++){
			selector = list_get(selector_list, i);
			list_add(result, selector);
		}
		list_dealloc(selector_list);
	}
}

hdata_t hdata_xpath_single_data(hdata_xpath_t data_xpath, hcchar * path,hdata_xpath_selector_t parent_selector,InvokeTickDeclare){
	if(data_xpath && path){
		hlist_t  selectors = list_alloc(20, 20);
		hdata_t data = NULL;
		data_xpath_selector_t * s = NULL;
		hdata_xpath(data_xpath,path,selectors,parent_selector,InvokeTickArg);
		if(list_count(selectors) >0){
			s = list_get(selectors, 0);
			data = s ->data;
		}
		list_dealloc(selectors);
		return data;
	}
	return NULL;
}

hdata_xpath_selector_t hdata_xpath_single(hdata_xpath_t data_xpath, hcchar * path,hdata_xpath_selector_t parent_selector,InvokeTickDeclare){
	if(data_xpath && path){
		hlist_t  selectors = list_alloc( 20, 20);
		data_xpath_selector_t * s = NULL;
		hdata_xpath(data_xpath,path,selectors,parent_selector,InvokeTickArg);
		if(list_count(selectors) >0){
			s = list_get(selectors, 0);
		}
		list_dealloc(selectors);
		return (hdata_xpath_selector_t)s;
	}
	return NULL;
}

hcchar * hdata_xpath_single_text(hdata_xpath_t data_xpath, hcchar * xpath,hdata_xpath_selector_t parent_selector,InvokeTickDeclare){
	hdata_t data = hdata_xpath_single_data(data_xpath,xpath,parent_selector,InvokeTickArg);
	return hdata_object_string(&hdata_xml_class,data,kVALUE,NULL);
}

hcchar * hdata_xpath_single_attr_string(hdata_xpath_t data_xpath, hcchar * xpath,hcchar * attrName,hdata_xpath_selector_t parent_selector,InvokeTickDeclare){
	hdata_t data = hdata_xpath_single_data(data_xpath,xpath,parent_selector,InvokeTickArg);
	return hdata_object_string(&hdata_xml_class,data,attrName,NULL);
}

hdata_t hdata_xpath_selector_data(hdata_xpath_selector_t selecotr,InvokeTickDeclare){
	return selecotr? ((data_xpath_selector_t *)selecotr)->data:NULL;
}

hdata_xpath_selector_t hdata_xpath_selector_parent(hdata_xpath_selector_t selecotr,InvokeTickDeclare){
	return selecotr?(hdata_xpath_selector_t) ((data_xpath_selector_t *)selecotr)->parent:NULL;
}
