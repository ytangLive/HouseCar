//
//  hjson.c
//  hclib
//
//  Created by zhang hailong on 13-1-25.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hjson.h"

#include "hbuffer.h"
#include "hlist.h"
#include "hlog.h"
#include "hstr.h"

#define SPACE_CHAR(c)  ((c) ==' ' || (c) =='\t' || (c) =='\r' || (c) == '\n')
#define TRUE_EQUAL(p)  (p[0] =='t' && p[1] == 'r' && p[2] =='u' && p[3] =='e')
#define FALSE_EQUAL(p)  (p[0] =='f' && p[1] == 'a' && p[2] =='l' && p[3] =='s' && p[4] =='e')
#define NULL_EQUAL(p)	(p[0] =='n' && p[1] == 'u' && p[2] =='l' && p[3] =='l')
#define NUMBER_CHAR(c)	(((c) >='0' && (c) <='9') || (c) =='-' || (c) == '.' || (c) =='e' || (c) == 'x' )

hany hjson_decode(hjson_t * json,hcchar * str,InvokeTickDeclare){
    hany data = NULL;
	hchar * p=(hchar *)str;
	hlist_t data_stack = list_alloc(20,20);
	hlist_t state_stack =list_alloc(20,20);
	hbuffer_t value_buffer = buffer_alloc(128,  256);
	hbuffer_t name_buffer = buffer_alloc(128,  256);
    hbuffer_t base64_buffer= buffer_alloc(1024,1024);
	hintptr s;
    hwchar wchar;
    hint32 iwchar;
    hchar wchbuf[8];
	//hint32 object_level=0;
	//hint32 array_level =0;
	list_add(state_stack, (hany) 0);
	
	
	while (p && *p != '\0') {
		s = (hintptr)list_last(state_stack);
		if(s == 0x00){
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '{'){
				list_add(state_stack, (hany)0x10);
				data = (*json->object_new)(json,InvokeTickArg);
				list_add(data_stack, data);
				list_add(data_stack, data);
				//hlog("object begin %ld\n",object_level++);
			}
			else if( *p == '['){
				list_add(state_stack, (hany)0x20);
				data = (*json->array_new)(json,InvokeTickArg);
				list_add(data_stack, data);
				list_add(data_stack, data);
				//hlog("array begin %ld\n",array_level++);
			}
			else if( *p == '\"'){
				list_add(state_stack, (hany)0x01);
				list_add(state_stack, (hany)0x30);
			}
			else if( TRUE_EQUAL(p)){
				data = (*json->booleanValue)(json,hbool_true,InvokeTickArg);
				list_add(data_stack, data);
				break;
			}
			else if( FALSE_EQUAL(p)){
				data = (*json->booleanValue)(json,hbool_false,InvokeTickArg);
				list_add(data_stack, data);
				break;
			}
			else if( NULL_EQUAL(p)){
				data = (*json->nullValue)(json,InvokeTickArg);
				list_add(data_stack, data);
				break;
			}
			else{
				list_add(state_stack, (hany)0x02);
				list_add(state_stack, (hany)0x40);
				continue;
			}
		}
		else if(s == 0x01){
			// string value end
			data = (*json->stringValue)(json,buffer_to_str(value_buffer),base64_buffer,InvokeTickArg);
			list_add(data_stack, data);
			break;
		}
		else if(s == 0x02){
			// number value end
			data = (*json->numberValue)(json,buffer_to_str(value_buffer),InvokeTickArg);
			list_add(data_stack, data);
			break;
		}
		else if(s ==0x10) {
			// object
			if(SPACE_CHAR(*p) || *p ==','){
			}
			else if(*p == '\"'){
				list_add(state_stack, (hany)0x11);
			}
			else if(*p == ':'){
				list_add(state_stack, (hany)0x12);
			}
			else if(*p == '}'){
				list_pop(state_stack);
				list_pop(data_stack);
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				//hlog("object end %ld\n",--object_level);
                
			}
			else{
				buffer_append(name_buffer,p,1);
				list_add(state_stack, (hany)0x11);
			}
		}
		else if(s == 0x11){
			// object value key
			if(*p == '\\' && p[1] == '"'){
				p++;
				buffer_append(name_buffer, p, 1);
			}
			else if( *p == '"'){
				list_pop(state_stack);
			}
			else if( *p == ':'){
				list_pop(state_stack);
				list_add(state_stack, (hany)0x12);
			}
			else{
				buffer_append(name_buffer, p, 1);
			}
		}
		else if(s == 0x12){
			// object value
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '{'){
				list_pop(state_stack);
				list_add(state_stack, (hany)0x10);
				data = (*json->object_new)(json,InvokeTickArg);
                (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
				list_add(data_stack, data);
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				//hlog("object begin %ld\n",object_level++);
                
			}
			else if( *p == '['){
				list_pop(state_stack);
				list_add(state_stack, (hany)0x20);
				data = (*json->array_new)(json,InvokeTickArg);
				(*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
				list_add(data_stack, data);
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				//hlog("array begin %ld\n",array_level++);
			}
			else if( *p =='"'){
				// string value
				list_pop(state_stack);
				list_add(state_stack, (hany)0x13);
				list_add(state_stack, (hany)0x30);
			}
			else if( TRUE_EQUAL(p)){
				list_pop(state_stack);
                
                data = (*json->booleanValue)(json,hbool_true,InvokeTickArg);
                (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
                
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 3;
			}
			else if( FALSE_EQUAL(p)){
				list_pop(state_stack);
                data = (*json->booleanValue)(json,hbool_false,InvokeTickArg);
                (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
                
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 4;
			}
			else if( NULL_EQUAL(p)){
				list_pop(state_stack);
                data = (*json->nullValue)(json,InvokeTickArg);
                (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
                
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 3;
			}
			else {
				list_pop(state_stack);
				list_add(state_stack, (hany)0x14);
				list_add(state_stack, (hany)0x40);
				continue;
			}
		}
		else if(s == 0x13){
			// string value end
            data = (*json->stringValue)(json,buffer_to_str(value_buffer),base64_buffer,InvokeTickArg);
            (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			list_pop(state_stack);
			continue;
		}
		else if(s == 0x14){
			// number value end
			data = (*json->numberValue)(json,buffer_to_str(value_buffer),InvokeTickArg);
            (*json->object_put)(json,list_last(data_stack),buffer_to_str(name_buffer),data,InvokeTickArg);
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			list_pop(state_stack);
			continue;
		}
		else if(s ==0x20){
			// array value
			if(SPACE_CHAR(*p) || *p ==','){
			}
			else if( *p ==']'){
				list_pop(state_stack);
				list_pop(data_stack);
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				//hlog("array end %ld\n",--array_level);
			}
			else if( *p == '{'){
				list_add(state_stack, (hany)0x23);
				list_add(state_stack, (hany)0x10);
				data = (*json->object_new)(json,InvokeTickArg);
                (*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
				list_add(data_stack, data);
				//hlog("object begin %ld\n",object_level++);
                
			}
			else if( *p == '['){
				list_add(state_stack, (hany)0x23);
				list_add(state_stack, (hany)0x20);
                data = (*json->array_new)(json,InvokeTickArg);
				(*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
				list_add(data_stack, data);
				//hlog("array begin %ld\n",array_level++);
			}
			else if( *p =='"'){
				// string value
				list_add(state_stack, (hany)0x21);
				list_add(state_stack, (hany)0x30);
			}
			else if( TRUE_EQUAL(p)){
                data = (*json->booleanValue)(json,hbool_true,InvokeTickArg);
				(*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
				p += 3;
			}
			else if( FALSE_EQUAL(p)){
                data = (*json->booleanValue)(json,hbool_false,InvokeTickArg);
				(*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
				p += 4;
			}
			else if( NULL_EQUAL(p)){
                data = (*json->nullValue)(json,InvokeTickArg);
				(*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
				p += 3;
			}
			else {
				list_add(state_stack, (hany)0x22);
				list_add(state_stack, (hany)0x40);
				continue;
			}
		}
		else if(s == 0x21){
			// string value end
            
            data = (*json->stringValue)(json,buffer_to_str(value_buffer),base64_buffer,InvokeTickArg);
            (*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			list_pop(state_stack);
			continue;
		}
		else if(s == 0x22){
			// number value end
            data = (*json->numberValue)(json,buffer_to_str(value_buffer),InvokeTickArg);
            (*json->array_add)(json,list_last(data_stack),data,InvokeTickArg);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			list_pop(state_stack);
			continue;
		}
		else if(s == 0x23){
			// object || array value end
			/*
             data = list_pop(data_stack);
             if(data ){
             hdata_array_add((hdata_t)list_last(data_stack), data);
             }
			 */
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			list_pop(state_stack);
			continue;
		}
		else if(s ==0x30){
			// string value
			if( *p == '\\'){
				if( p[1] == 'n'){
					buffer_append_str(value_buffer,"\n");
				}
				else if( p[1] == 'r'){
					buffer_append_str(value_buffer,"\r");
				}
				else if( p[1] == 't'){
					buffer_append_str(value_buffer,"\t");
				}
				else if( p[1] == '\\'){
					buffer_append_str(value_buffer,"\\");
				}
				else if( p[1] == '\"'){
					buffer_append_str(value_buffer,"\"");
				}
				else if( p[1] == '\''){
					buffer_append_str(value_buffer,"\'");
				}
                else if( p[1] == 'u' ||  p[1] == 'U'){
                    iwchar = 0;
					sscanf(p + 2, "%04x",&iwchar);
                    wchar = iwchar;
                    strcpy(wchbuf, "");
                    buffer_append(value_buffer, wchbuf,wstr_to_str(&wchar, 1, wchbuf));
                    p+= 4;
				}
				else{
					buffer_append(value_buffer,p+1,1);
				}
				p++;
			}
			else if( *p == '\"'){
				list_pop(state_stack);
			}
			else{
				buffer_append(value_buffer,p,1);
			}
		}
		else if(s == 0x40){
			// number value
			if(NUMBER_CHAR(*p)){
				buffer_append(value_buffer, p, 1);
			}
			else{
				list_pop(state_stack);
				continue;
			}
		}
		
		p++;
	}
	
	while((data = list_pop(data_stack))){
        if(list_count(data_stack) ==0){
            break;
        }
        (*json->object_dealloc)(json,data,InvokeTickArg);
	}
	
	buffer_dealloc(value_buffer);
	buffer_dealloc(name_buffer);
    buffer_dealloc(base64_buffer);
	list_dealloc(data_stack);
	list_dealloc(state_stack);
    
    return data;
}

