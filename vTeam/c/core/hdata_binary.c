/*
 *  hdata_binary.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-11.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hdata_binary.h"
#include "hstr.h"

#undef hdata_binary_alloc

#undef hdata_binary_dealloc

#undef hdata_binary_bytes

#undef hdata_binary_check

hdata_type_code	binary_hdata_type(hdata_t,InvokeTickDeclare);
hint16		binary_hdata_int16(hdata_t,hint16 ,InvokeTickDeclare);
hint32		binary_hdata_int32(hdata_t,hint32 ,InvokeTickDeclare);
hint64		binary_hdata_int64(hdata_t,hint64 ,InvokeTickDeclare);
hdouble		binary_hdata_double(hdata_t,hdouble ,InvokeTickDeclare);
hbool		binary_hdata_boolean(hdata_t,hbool ,InvokeTickDeclare);
hcchar *	binary_hdata_string(hdata_t,hcchar * ,InvokeTickDeclare);
hdata_t		binary_hdata_object(hdata_t data,hcchar *key,InvokeTickDeclare);
void		binary_hdata_object_echo(hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare);
hint32		binary_hdata_array_size(hdata_t data,InvokeTickDeclare);
hdata_t		binary_hdata_array(hdata_t data,hint32 index,InvokeTickDeclare);
hint32		binary_hdata_bytes_size(hdata_t data,InvokeTickDeclare);
hbyte *		binary_hdata_bytes(hdata_t data,InvokeTickDeclare);

hdata_t hdata_binary_alloc(hext_obj_t ext_bytes,InvokeTickDeclare){
	hbyte * binary = ext_bytes_obj_data(ext_bytes);
	if(binary && HDATA_VERSION == type_int32_from_bytes(binary)){
		ext_obj_retain(ext_bytes);
		return (hdata_t)(binary + HINT32_BYTES);
	}
	return NULL;
}

void hdata_binary_dealloc(hdata_t data,InvokeTickDeclare){
    hbyte * binary = data ? (hbyte *)data - HINT32_BYTES:NULL;
	ext_obj_release(ext_bytes_from_data(binary));
}

hext_obj_t hdata_binary_bytes(hdata_t data,InvokeTickDeclare){
	if(data){
		hbyte * b = ((hbyte *)data - HINT32_BYTES);
		return ext_bytes_from_data(b);
	}
	return NULL;
}

hdata_type_code	binary_hdata_type(hdata_t data,InvokeTickDeclare){
	return data ? (hdata_type_code)type_byte_from_bytes ((hbyte *)data):HDATA_TYPE_NULL;
}

hint16 binary_hdata_int16(hdata_t data,hint16 v,InvokeTickDeclare){
	return data?type_int16_from_bytes((hbyte *)data + HBYTE_BYTES):v;
}

hint32 binary_hdata_int32(hdata_t data,hint32 v,InvokeTickDeclare){
	return data?type_int32_from_bytes((hbyte *)data  + HBYTE_BYTES):v;
}

hint64 binary_hdata_int64(hdata_t data,hint64 v,InvokeTickDeclare){
	return data?type_int64_from_bytes((hbyte *)data  + HBYTE_BYTES):v;
}

hdouble	binary_hdata_double(hdata_t data,hdouble v,InvokeTickDeclare){
	return data?type_double_from_bytes((hbyte *)data  + HBYTE_BYTES):v;
}

hbool binary_hdata_boolean(hdata_t data,hbool v,InvokeTickDeclare){
	return data?type_bool_from_bytes((hbyte *)data  + HBYTE_BYTES):v;
}

hcchar * binary_hdata_string(hdata_t data,hcchar * v,InvokeTickDeclare){
	return data?((hchar *)data + HBYTE_BYTES + HINT32_BYTES):v;
}

static hint32 binary_hdata_length(hdata_t data,InvokeTickDeclare){
	hint32 type = type_byte_from_bytes((hbyte *)data);
	hint32 length = HBYTE_BYTES;
	if(type == HDATA_TYPE_INT16){
		length += HINT16_BYTES;
	}
	else if(type == HDATA_TYPE_INT32){
		length += HINT32_BYTES;
	}
	else if(type == HDATA_TYPE_INT64){
		length += HINT64_BYTES;
	}
	else if(type == HDATA_TYPE_DOUBLE){
		length += HDOUBLE_BYTES;
	}
	else if(type == HDATA_TYPE_BOOL){
		length += HBOOL_BYTES;
	}
    else if(type == HDATA_TYPE_STRING){
        length += HINT32_BYTES;
        length += type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
    }
	else if(type == HDATA_TYPE_OBJECT){
		length += HINT32_BYTES;
		length += type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
	}
	else if(type == HDATA_TYPE_ARRAY){
		length += HINT32_BYTES;
		length += type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
		length += HINT32_BYTES;
	}
    else if(type == HDATA_TYPE_BYTES){
        length += HINT32_BYTES;
        length += type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
    }
	return length;
}

hdata_t	binary_hdata_object(hdata_t data,hcchar *key,InvokeTickDeclare){
	if(binary_hdata_type(data,InvokeTickArg) == HDATA_TYPE_OBJECT){
		hint32 total_length = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES),length = 0,key_length=0;
		hbyte * d = (hbyte *)data + HBYTE_BYTES + HINT32_BYTES;
		while(length < total_length){
			key_length = type_int32_from_bytes(d + length);
			if(str_cmp(key, d +length + HINT32_BYTES) ==0){
				return (hdata_t)(d + length + HINT32_BYTES + key_length);
			}
			length += HINT32_BYTES + key_length + binary_hdata_length((hdata_t)(d + length + HINT32_BYTES + key_length),InvokeTickArg);
		}
		
	}
	return NULL;
}

void binary_hdata_object_echo(hdata_t data,hdata_object_echo_callback_t callback,hany dest,InvokeTickDeclare){
	if(binary_hdata_type(data,InvokeTickArg) == HDATA_TYPE_OBJECT && callback){
		hint32 total_length = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES),length = 0,key_length=0,data_length=0;
		hbyte * d = (hbyte *)data + HBYTE_BYTES + HINT32_BYTES;
		hchar * key;
		while(length < total_length){
			key_length = type_int32_from_bytes(d );
			key = (hchar *)(d + HINT32_BYTES);
			d = (hbyte *)key + key_length;
			(*callback)(data,key,(hdata_t)(d),dest,InvokeTickArg);
			data_length = binary_hdata_length((hdata_t)d,InvokeTickArg);
			length += HINT32_BYTES + key_length + data_length;
			d = d+ data_length;
		}
	}
}

hint32	binary_hdata_array_size(hdata_t data,InvokeTickDeclare){
	if(binary_hdata_type(data,InvokeTickArg) == HDATA_TYPE_ARRAY){
		return type_int32_from_bytes((hbyte *)data + HBYTE_BYTES + HINT32_BYTES);
	}
	return 0;
}

hdata_t binary_hdata_array(hdata_t data,hint32 index,InvokeTickDeclare){
	if(data && index >=0 && index<binary_hdata_array_size(data,InvokeTickArg)){
		hint32 total_length = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES),length = 0,i=0,data_length;
		hbyte * d = (hbyte *)data + HBYTE_BYTES + HINT32_BYTES + HINT32_BYTES;
		do{
			if(i == index){
				return (hdata_t)d;
			}
			data_length = binary_hdata_length((hdata_t)(d),InvokeTickArg);
			d +=  data_length;
			length +=  data_length;
			i++;
		}while(length < total_length);	
	}
	return NULL;
}

hint32	binary_hdata_bytes_size(hdata_t data,InvokeTickDeclare){
	if(binary_hdata_type(data,InvokeTickArg) == HDATA_TYPE_BYTES){
		return type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
	}
	return 0;
}

hbyte * binary_hdata_bytes(hdata_t data,InvokeTickDeclare){
	if(data && binary_hdata_bytes_size(data,InvokeTickArg) >0){
		return (hbyte *)data + HBYTE_BYTES;
	}
	return NULL;
}

static hbool hdata_binary_data_check(hdata_t data,hint32 length,InvokeTickDeclare){
    hint32 type = 0;
    hint32 len = length;
    hint32 child_len = 0;
    
    if(length < HBYTE_BYTES){
        return hbool_false;
    }
    
    type = type_byte_from_bytes((hbyte *)data);
    
    if(type <HDATA_TYPE_BEGIN || type > HDATA_TYPE_END){
        return hbool_false;
    }
    
    len -= HBYTE_BYTES;
    
	if(type == HDATA_TYPE_INT16){
        if(len < HINT16_BYTES){
            return hbool_false;
        }
		len -= HINT16_BYTES;
	}
	else if(type == HDATA_TYPE_INT32){
        if(len < HINT32_BYTES){
            return hbool_false;
        }
		len -= HINT32_BYTES;
	}
	else if(type == HDATA_TYPE_INT64){
        if(len < HINT64_BYTES){
            return hbool_false;
        }
		len -= HINT64_BYTES;
	}
	else if(type == HDATA_TYPE_DOUBLE){
        if(len < HDOUBLE_BYTES){
            return hbool_false;
        }
		len -= HDOUBLE_BYTES;
	}
	else if(type == HDATA_TYPE_BOOL){
        if(len < HBOOL_BYTES){
            return hbool_false;
        }
		len -= HBOOL_BYTES;
	}
    else if(type == HDATA_TYPE_STRING){
        if(len < HINT32_BYTES){
            return hbool_false;
        }
        len -= HINT32_BYTES;
        child_len = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
        if(child_len <0 || len < child_len){
            return hbool_false;
        }
        len -= child_len;
        if(*((hbyte *)data + HBYTE_BYTES + HINT32_BYTES + child_len -1)  !=0){
            return hbool_false;
        }
    }
	else if(type == HDATA_TYPE_OBJECT){
        if(len < HINT32_BYTES){
            return hbool_false;
        }
        len -= HINT32_BYTES;
        child_len = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
        
        if(child_len<0 || len < child_len){
            return hbool_false;
        }
        
        {
            hint32 total_length = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES),length = 0,key_length=0,data_length=0;
            hbyte * d = (hbyte *)data + HBYTE_BYTES + HINT32_BYTES;
            hchar * key;
            while(length < total_length){
                key_length = type_int32_from_bytes(d );
            
                if(key_length <0){
                    return hbool_false;
                }
                
                key = (hchar *)(d + HINT32_BYTES);
                
                if(*((hbyte *)key + key_length -1) !=0){
                    return hbool_false;
                }
                
                d = (hbyte *)key + key_length;

                data_length = binary_hdata_length((hdata_t)d,InvokeTickArg);
                
                if(!hdata_binary_data_check((hdata_t)d,data_length,InvokeTickArg)){
                    return hbool_false;
                }
                
                length += HINT32_BYTES + key_length + data_length;
                d = d+ data_length;
            }
            
            if(length != total_length){
                return hbool_false;
            }
        }
        
		len -= child_len;
	}
	else if(type == HDATA_TYPE_ARRAY){
        if(len < HINT32_BYTES){
            return hbool_false;
        }
		len -= HINT32_BYTES;
        
		child_len = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
        
        if(child_len<0 || len < child_len){
            return hbool_false;
        }
        
        {
            hint32 total_length = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES),length = 0,i=0,data_length;
            hbyte * d = (hbyte *)data + HBYTE_BYTES + HINT32_BYTES + HINT32_BYTES;
            do{
                data_length = binary_hdata_length((hdata_t)(d),InvokeTickArg);
                if(!hdata_binary_data_check((hdata_t)d,data_length,InvokeTickArg)){
                    return hbool_false;
                }
                d +=  data_length;
                length +=  data_length;
                i++;
            }while(length < total_length);	
            
            if(length != total_length){
                return hbool_false;
            }
        }
        
        len -= child_len;
        
        if(len < HINT32_BYTES){
            return hbool_false;
        }
		len -= HINT32_BYTES;
	}
    else if(type == HDATA_TYPE_BYTES){
        if(len < HINT32_BYTES){
            return hbool_false;
        }
        len -= HINT32_BYTES;
        child_len = type_int32_from_bytes((hbyte *)data + HBYTE_BYTES);
        if(child_len <0 || len < child_len){
            return hbool_false;
        }
        len -= child_len;
    }
    return len >= 0;
}

hbool hdata_binary_check(hext_obj_t ext_bytes,hint32 length,InvokeTickDeclare){
	hbyte * binary = ext_bytes_obj_data(ext_bytes);
	if(binary && HDATA_VERSION == type_int32_from_bytes(binary)){
        hdata_t data = (hdata_t)(binary + HINT32_BYTES);
        return hdata_binary_data_check(data, length - HINT32_BYTES,InvokeTickArg);
    }
    return hbool_false;
}

hdata_class_t hdata_binary_class = {binary_hdata_type,binary_hdata_int16,binary_hdata_int32,binary_hdata_int64
	,binary_hdata_double,binary_hdata_boolean,binary_hdata_string,binary_hdata_object,binary_hdata_object_echo,binary_hdata_array_size,binary_hdata_array,binary_hdata_bytes_size,binary_hdata_bytes}; 
