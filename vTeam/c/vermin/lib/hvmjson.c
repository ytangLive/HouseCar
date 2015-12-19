//
//  hvmjson.c
//  C Library
//
//  Created by 张海龙 张海龙 on 11-11-15.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmjson.h"
#include "hbuffer.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hserial_list.h"
#include "hstr.h"

typedef struct{
    vmObject base;
    struct{
        vmUniqueKey encode;
        vmUniqueKey decode;
    }uniqueKeys;
}vmJSON;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmJSONClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmJSON * json = (vmJSON *) object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    
    json->uniqueKeys.encode = vmRuntimeContextGetUniqueKey(context, "encode");
    json->uniqueKeys.decode = vmRuntimeContextGetUniqueKey(context, "decode");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmJSONClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmJSONClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmJSONClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
        
    return rs;
}

static void vmJSONOUTLevel(hbuffer_t buf,hint32 level,InvokeTickDeclare){
    hint32 i;
    for(i=0;i<level;i++){
        buffer_append_str(buf, "\t");
    }
}

static void vmJSONVariantToJson(vmRuntimeContext context, vmVariant var,hbuffer_t buf,hint32 level,hbool pattern,InvokeTickDeclare){
    
    if(var.type & vmVariantTypeInt16 || var.type  & vmVariantTypeInt32 || var.type & vmVariantTypeInt64 || var.type & vmVariantTypeDouble
       || (var.type & vmVariantTypeObject && var.value.objectValue && vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmNumberClass))){
        vmVariantToString(context, var, buf);
    }
    else if(var.type & vmVariantTypeBoolean){
        buffer_append_str(buf, var.value.booleanValue ? "true":"false");
    }
    else if((var.type & vmVariantTypeString && var.value.stringValue)
            || (var.type & vmVariantTypeObject && var.value.objectValue 
                && vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmStringClass))){
        {
            hbuffer_t str = NULL;
            hchar * p = NULL;
            if(var.type & vmVariantTypeString){
                p = (hchar *)var.value.stringValue;
            }
            else{
                str = buffer_alloc(1024, 1024);
                vmVariantToString(context, var, str);
                p = (hchar *)buffer_to_str(str);
            }
            if(p){
                buffer_append_str(buf, "\"");
                while(*p !=0){
                    if(*p == '\n'){
                        buffer_append_str(buf, "\\n");
                    }
                    else if(*p == '\r'){
                        buffer_append_str(buf, "\\r");
                    }
                    else if(*p == '\t'){
                        buffer_append_str(buf, "\\t");
                    }
                    else if(*p == '"'){
                        buffer_append_str(buf, "\\\"");
                    }
                    else{
                        buffer_append(buf,p,1);
                    }
                    p++;
                }
                buffer_append_str(buf, "\"");
            }
            else{
                buffer_append_str(buf, "null");
            }
            if(str){
                buffer_dealloc(str);
            }
        }
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            if(vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmArrayClass)){
                {
                    hint32 length = vmVariantToInt32(context, vmObjectGetProperty(context, var.value.objectValue->vmClass, var.value.objectValue, vmRuntimeContextGetUniqueKey(context, "length")));
                    hint32 i;
                    
                    buffer_append_str(buf, "[");
                    if(pattern){
                        buffer_append_str(buf, "\n");
                    }
                    for(i=0;i<length;i++){
                        if(pattern){
                            vmJSONOUTLevel(buf,level + 1,InvokeTickArg);
                        }
                        if(i!=0){
                            buffer_append_str(buf, ",");
                        }
                        vmJSONVariantToJson(context, vmObjectGetProperty(context, var.value.objectValue->vmClass,var.value.objectValue, vmRuntimeContextGetUniqueKeyFromInteger(context, i)), buf, level +1, pattern, InvokeTickArg);
                        if(pattern){
                            buffer_append_str(buf, "\n");
                        }
                    }
                    if(pattern){
                        vmJSONOUTLevel(buf,level ,InvokeTickArg);
                    }
                    buffer_append_str(buf, "]");
                }
            }
            else{
                {
                    vmUniqueKeyList names = vmUniqueKeyListAlloc();
                    hint32 c,i;
                    vmVariant v = {0};
                    vmUniqueKey key;
                    vmObjectPropertyNames(context, var.value.objectValue->vmClass, var.value.objectValue, names);
                    
                    buffer_append_str(buf, "{");
                    if(pattern){
                        buffer_append_str(buf, "\n");
                    }
                    c = vmUniqueKeyListCount(names);
                    for(i=0;i<c;i++){
                        key = vmUniqueKeyListGet(names, i);
                        if(pattern){
                            vmJSONOUTLevel(buf,level + 1,InvokeTickArg);
                        }
                        if(i!=0){
                            buffer_append_str(buf, ",");
                        }
                        v.type = vmVariantTypeString;
                        v.value.stringValue = vmRuntimeContextUniqueKeyToString(context, key);
                        vmJSONVariantToJson(context, v, buf, level +1, pattern, InvokeTickArg);
                        buffer_append_str(buf, ":");
                        vmJSONVariantToJson(context, vmObjectGetProperty(context, var.value.objectValue->vmClass,var.value.objectValue, key), buf, level +1, pattern, InvokeTickArg);
                        if(pattern){
                            buffer_append_str(buf, "\n");
                        }
                    }
                    if(pattern){
                        vmJSONOUTLevel(buf,level ,InvokeTickArg);
                    }
                    buffer_append_str(buf, "}");
                    
                    vmUniqueKeyListDealloc(names);
                }
            }
        }
        else{
            buffer_append_str(buf, "null");
        }
    }
    else{
        buffer_append_str(buf, "null");
    }
}

#define SPACE_CHAR(c)  ((c) ==' ' || (c) =='\t' || (c) =='\r' || (c) == '\n')
#define TRUE_EQUAL(p)  (p[0] =='t' && p[1] == 'r' && p[2] =='u' && p[3] =='e')
#define FALSE_EQUAL(p)  (p[0] =='f' && p[1] == 'a' && p[2] =='l' && p[3] =='s' && p[4] =='e')
#define NULL_EQUAL(p)	(p[0] =='n' && p[1] == 'u' && p[2] =='l' && p[3] =='l')
#define NUMBER_CHAR(c)	(((c) >='0' && (c) <='9') || (c) =='-' || (c) == '.' || (c) =='e' || (c) == 'x' )

static vmVariant vmJSONNumberParse(hcchar * value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
	hint64 int64_value = 0;
	hdouble double_value = 0;
	if(str_exist(value, ".") || str_exist(value, "e")){
		sscanf(value, "%lf",&double_value);
		if((hint32)double_value == double_value){
            rs.type = vmVariantTypeInt32;
            rs.value.int32Value = (hint32)double_value;
		}
		else{
            rs.type = vmVariantTypeDouble;
            rs.value.doubleValue = double_value;
		}
	}
	else{
		sscanf(value, "%lld",&int64_value);
		if((hint16)int64_value == int64_value){
            rs.type = vmVariantTypeInt16;
            rs.value.int16Value = (hint16)int64_value;
		}
		else if((hint32)int64_value == int64_value){
			rs.type = vmVariantTypeInt32;
            rs.value.int32Value = (hint32)int64_value;
		}
		else {
			rs.type = vmVariantTypeInt64;
            rs.value.int64Value = int64_value;
		}
	}
    return rs;
}

static vmVariant vmJSONParse(vmRuntimeContext context, hcchar * json_str,InvokeTickDeclare){
	vmVariant data = {0},data_temp = {0};
    vmVariant ex = {0};
	hchar * p=(hchar *)json_str;
    hserial_list_t data_stack = serial_list_alloc(sizeof(vmVariant), 0);
    hserial_list_t state_stack = serial_list_alloc(sizeof(hintptr), 0);
	hbuffer_t value_buffer = buffer_alloc(128,  256);
	hbuffer_t name_buffer = buffer_alloc(128,  256);
    hbuffer_t base64_buffer= buffer_alloc(1024,1024);
	hintptr s = 0;
	
    serial_list_item_add(state_stack, &s);	
	
	while (*p != '\0') {
		s = * (hintptr *)serial_list_item_last(state_stack);
		if(s == 0x00){
			if(SPACE_CHAR(*p)){
			}
			else if( *p == '{'){
                s = 0x10;
                serial_list_item_add(state_stack, &s);
                
                data = vmDictionaryAlloc(context);
                
                serial_list_item_add(data_stack, &data);
                serial_list_item_add(data_stack, &data);
				
			}
			else if( *p == '['){
                s = 0x20;
                serial_list_item_add(state_stack, &s);
				
				data = vmArrayAlloc(context);
				serial_list_item_add(data_stack, &data);
                serial_list_item_add(data_stack, &data);
			}
			else if( *p == '\"'){
                s = 0x01;
                serial_list_item_add(state_stack, &s);

                s = 0x30;
                serial_list_item_add(state_stack, &s);

			}
			else if( TRUE_EQUAL(p)){
                data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_true;
                serial_list_item_add(data_stack, &data);
				break;
			}
			else if( FALSE_EQUAL(p)){
				data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_false;
                serial_list_item_add(data_stack, &data);
				break;
			}
			else if( NULL_EQUAL(p)){
                data.type = vmVariantTypeVoid;
                data.value.int64Value = 0;
				serial_list_item_add(data_stack, &data);
				break;
			}
			else{
                s = 0x02;
                serial_list_item_add(state_stack, &s);
                
                s = 0x40;
                serial_list_item_add(state_stack, &s);
				continue;
			}
		}
		else if(s == 0x01){
			// string value end
            data = vmStringAlloc(context, buffer_to_str(value_buffer));
            serial_list_item_add(data_stack, &data);
			break;
		}
		else if(s == 0x02){
			// number value end
            data = vmJSONNumberParse(buffer_to_str(value_buffer),InvokeTickArg);
			serial_list_item_add(data_stack, &data);
			break;
		}		
		else if(s ==0x10) {
			// object
			if(SPACE_CHAR(*p) || *p ==','){
			}
			else if(*p == '\"'){
                s = 0x11;
                serial_list_item_add(state_stack, &s);
                
			}
			else if(*p == ':'){
                s = 0x12;
                serial_list_item_add(state_stack, &s);
			}
			else if(*p == '}'){
                serial_list_remove_last(state_stack);
                serial_list_remove_last(data_stack);
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
            }
			else{
				buffer_append(name_buffer,p,1);
                s = 0x11;
                serial_list_item_add(state_stack, &s);
			}
		}
		else if(s == 0x11){
			// object value key
			if(*p == '\\' && p[1] == '"'){
				p++;
				buffer_append(name_buffer, p, 1);
			}
			else if( *p == '"'){
                serial_list_remove_last(state_stack);
			}
			else if( *p == ':'){
                serial_list_remove_last(state_stack);
                s = 0x12;
                serial_list_item_add(state_stack, &s);
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
                serial_list_remove_last(state_stack);
                s = 0x10;
                serial_list_item_add(state_stack, &s);
                
                data = vmDictionaryAlloc(context);
				
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
                
                serial_list_item_add(data_stack,&data);
				
                buffer_clear(name_buffer);
				buffer_clear(value_buffer);
                
			}
			else if( *p == '['){
                serial_list_remove_last(state_stack);
                s = 0x20;
                serial_list_item_add(state_stack, &s);
                
				data = vmArrayAlloc(context);
				
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
                
                serial_list_item_add(data_stack,&data);
				
                buffer_clear(name_buffer);
				buffer_clear(value_buffer);
			}
			else if( *p =='"'){
				// string value
                
                serial_list_remove_last(state_stack);
                s = 0x13;
                serial_list_item_add(state_stack, &s);
                s = 0x30;
                serial_list_item_add(state_stack, &s);

			}
			else if( TRUE_EQUAL(p)){
                serial_list_remove_last(state_stack);
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_true;
                
                vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
                
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 3;
			}
			else if( FALSE_EQUAL(p)){
                serial_list_remove_last(state_stack);
				
				data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_false;
                
                vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
                

				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 4;
			}
			else if( NULL_EQUAL(p)){
				serial_list_remove_last(state_stack);
				
				data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeVoid;
                data.value.int64Value = 0;
                
                vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
                
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
				p += 3;
			}
			else {
                serial_list_remove_last(state_stack);
                s = 0x14;
                serial_list_item_add(state_stack, &s);
                s = 0x40;
                serial_list_item_add(state_stack, &s);
				continue;
			}
		}
		else if(s == 0x13){
			// string value end
            
            serial_list_remove_last(state_stack);
            
            data_temp = *(vmVariant *)serial_list_item_last(data_stack);
            
            if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                ex = vmExceptionAlloc(context, 0, "json format error");
                break;
            }
            
            data = vmStringAlloc(context, buffer_to_str(value_buffer));
            
            vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			continue;
		}
		else if(s == 0x14){
			// number value end
            serial_list_remove_last(state_stack);
            
            data_temp = *(vmVariant *)serial_list_item_last(data_stack);
            
            if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmDictionaryClass))){
                ex = vmExceptionAlloc(context, 0, "json format error");
                break;
            }
            
            data = vmJSONNumberParse(buffer_to_str(value_buffer), InvokeTickArg);
            
            vmDictionaryPut(context, data_temp.value.objectValue, buffer_to_str(name_buffer), data);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
            
			continue;
		}
		else if(s ==0x20){
			// array value
			if(SPACE_CHAR(*p) || *p ==','){
			}
			else if( *p ==']'){
                serial_list_remove_last(state_stack);
                serial_list_remove_last(data_stack);
				
				buffer_clear(name_buffer);
				buffer_clear(value_buffer);
                
			}
			else if( *p == '{'){
                s = 0x23;
                serial_list_item_add(state_stack, &s);
                s = 0x10;
                serial_list_item_add(state_stack, &s);
				
                data = vmDictionaryAlloc(context);
                
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                vmArrayAdd(context, data_temp.value.objectValue, data);
                
                serial_list_item_add(data_stack,&data);
                
			}
			else if( *p == '['){
                s = 0x23;
                serial_list_item_add(state_stack, &s);
                s = 0x20;
                serial_list_item_add(state_stack, &s);

                data = vmArrayAlloc(context);
                
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                vmArrayAdd(context, data_temp.value.objectValue, data);
                
                serial_list_item_add(data_stack,&data);
				
			}			
			else if( *p =='"'){
				// string value
                s = 0x21;
                serial_list_item_add(state_stack, &s);
                s = 0x30;
                serial_list_item_add(state_stack, &s);

			}
			else if( TRUE_EQUAL(p)){
                
                data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_true;
                
                vmArrayAdd(context, data_temp.value.objectValue, data);
                
				p += 3;
			}
			else if( FALSE_EQUAL(p)){
				data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeBoolean;
                data.value.booleanValue = hbool_false;
                
                vmArrayAdd(context, data_temp.value.objectValue, data);
                
				p += 4;
			}
			else if( NULL_EQUAL(p)){
				data_temp = *(vmVariant *)serial_list_item_last(data_stack);
                
                if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                    ex = vmExceptionAlloc(context, 0, "json format error");
                    break;
                }
                
                data.type = vmVariantTypeVoid;
                data.value.int64Value = 0;
                
                vmArrayAdd(context, data_temp.value.objectValue, data);
                
                p += 3;
			}
			else {
                s = 0x22;
                serial_list_item_add(state_stack, &s);
                s = 0x40;
                serial_list_item_add(state_stack, &s);
    
				continue;
			}
		}
		else if(s == 0x21){
			// string value end
            
            serial_list_remove_last(state_stack);
            
            data_temp = *(vmVariant *)serial_list_item_last(data_stack);
            
            if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                ex = vmExceptionAlloc(context, 0, "json format error");
                break;
            }
            
            data = vmStringAlloc(context, buffer_to_str(value_buffer));
            
            vmArrayAdd(context, data_temp.value.objectValue, data);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
            
			continue;
		}
		else if(s == 0x22){
			// number value end
            
            serial_list_remove_last(state_stack);
            
            data_temp = *(vmVariant *)serial_list_item_last(data_stack);
            
            if(!(data_temp.type & vmVariantTypeObject && vmRuntimeContextObjectIsKindClass(context, data_temp.value.objectValue, (vmClass *)&vmArrayClass))){
                ex = vmExceptionAlloc(context, 0, "json format error");
                break;
            }
            
            data = vmJSONNumberParse(buffer_to_str(value_buffer), InvokeTickArg);
            
            vmArrayAdd(context, data_temp.value.objectValue, data);
            
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
            
			
			continue;
		}
		else if(s == 0x23){
			// object || array value end
			
			buffer_clear(name_buffer);
			buffer_clear(value_buffer);
			serial_list_remove_last(state_stack);
			continue;
		}
		else if(s ==0x30){
			// string value
			if( *p == '\\'){
				if( p[1] == 'n'){
					buffer_append_str(value_buffer,"\n");
				}
				else if( p[1] == '\r'){
					buffer_append_str(value_buffer,"\r");
				}
				else if( p[1] == '\t'){
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
				else{
					buffer_append(value_buffer,p+1,1);
				}
				p++;
			}
			else if( *p == '\"'){
                serial_list_remove_last(state_stack);
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
				serial_list_remove_last(state_stack);
				continue;
			}
		}
		
		p++;
	}
	
    if(ex.type == vmVariantTypeVoid){
        if(serial_list_count(data_stack) ==0){
            ex = vmExceptionAlloc(context, 0, "json format error");
        }
        else{
            data = *(vmVariant *)serial_list_item_last(data_stack);
        } 
    }
    
    
	buffer_dealloc(value_buffer);
	buffer_dealloc(name_buffer);
    buffer_dealloc(base64_buffer);
    serial_list_dealloc(data_stack);
    serial_list_dealloc(state_stack);
	
    if(ex.type ==vmVariantTypeVoid){
        return data;
    }
    
	return ex;
}

/**
 
 Return : any
 */
static vmVariant vmJSONClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmJSON * json = (vmJSON *) object;
    
    if(json->uniqueKeys.encode == name){
        {
            hbuffer_t buf = buffer_alloc(1024, 1024);
            vmJSONVariantToJson(context,vmVariantListGet(args, 0),buf,0
                                , vmVariantToBoolean(context, vmVariantListGet(args, 1)), InvokeTickArg);
            
            rs = vmStringAlloc(context, buffer_to_str(buf));
            
            buffer_dealloc(buf);
        }
    }
    else if(json->uniqueKeys.decode == name){
        {
            hbuffer_t buf = buffer_alloc(1024, 1024);
            vmVariantToString(context, vmVariantListGet(args, 0), buf);
            if(buffer_length(buf)){
                rs = vmJSONParse(context, buffer_to_str(buf), InvokeTickArg);
            }
            buffer_dealloc(buf);
        }
    }
        
    return rs;
}

/**
 
 Return : Void
 */
static void vmJSONClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}


vmClassBase vmJSONClass = {vmClassTypeBase,sizeof(vmJSON),vmJSONClassInitCallback,vmJSONClassDestroyCallback,vmJSONClassGetPropertyCallback,vmJSONClassSetPropertyCallback,vmJSONClassPropertyNamesCallback,vmJSONClassInvokeCallback};


