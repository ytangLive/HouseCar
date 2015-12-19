/*
 *  buffer.c
 *  C Library
 *
 *  Created by hailong zhang on 10-7-31.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hbuffer.h"
#include "md5.h"
#include "hbytes.h"

#undef buffer_alloc

#undef buffer_dealloc

#undef buffer_append

#undef buffer_append_str

#undef buffer_clear

#undef buffer_to_str

#undef buffer_extend_size

#undef buffer_size

#undef buffer_length

#undef buffer_length_set

#undef buffer_data

#undef buffer_detach

#undef buffer_out

#undef buffer_append_format

#undef buffer_append_format_va_list

#undef buffer_append_md5_upper

#undef buffer_append_md5

#undef buffer_append_md5_str

#undef buffer_append_md5_upper_str

#undef buffer_trim

#undef buffer_path_last

#undef buffer_path_last_remove

#undef buffer_append_url_encode

#define ISSPACE(c)  ((c) == ' ' || (c) =='\t' || (c) == '\r' || (c) == '\n')

typedef struct{
	hchar *buffer;
    hint32 length;
	hint32 bufferSize;
	hint32 extendSize;
}Buffer;


hbuffer_t buffer_alloc(hint32 capacity,hint32 extendSize, InvokeTickDeclare){
	Buffer *buffer = (Buffer *)mem_malloc(sizeof(Buffer));
	buffer->buffer = (hchar *)mem_malloc( capacity);
	buffer->bufferSize = capacity;
	buffer->length = 0;
	buffer->extendSize = extendSize>0?extendSize:BUFFER_EXTEND_SIZE;
	return (hbuffer_t)buffer;
}

void buffer_dealloc(hbuffer_t hBuffer, InvokeTickDeclare){
	if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
		if(buffer->buffer){
			mem_free(buffer->buffer);
			buffer->buffer = NULL;
			buffer->bufferSize = 0;
			buffer->length = 0;
		}
		mem_free(buffer);
	}
}	

void buffer_append(hbuffer_t hBuffer,hany  data,hint32 length, InvokeTickDeclare){
	Buffer *buffer = (Buffer *)hBuffer;
	buffer_extend_size(hBuffer, buffer->length + length + 1,InvokeTickArg);
	mem_memcpy(buffer->buffer + buffer->length,data,length);
	buffer->length += length;
}

void buffer_append_str(hbuffer_t hBuffer,hcchar * str, InvokeTickDeclare){
	if(hBuffer && str){
		buffer_append(hBuffer,(void *)str,(int)strlen(str),InvokeTickArg);
	}
}

void buffer_clear(hbuffer_t hBuffer, InvokeTickDeclare){
	((Buffer *)hBuffer)->length = 0;
}

hcchar * buffer_to_str(hbuffer_t hBuffer, InvokeTickDeclare){
	if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
        if(buffer->buffer){
            buffer->buffer[buffer->length] = 0;
            return buffer->buffer;
        }
        else{
            return NULL;
        }
	}
	return NULL;
}

static void buffer_trim_left(hbuffer_t hBuffer, InvokeTickDeclare){
    if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
        hchar * p1,*p2;
        if(buffer->buffer){
            buffer->buffer[buffer->length] = 0;
            
            p1 = buffer->buffer;
            while(*p1 != 0){
                if(!ISSPACE(*p1)){
                    break;
                }
                p1++; 
            }
            p2 = buffer->buffer;
            if(p1 != p2){
                
                while(*p1 !=0){
                    *p2 ++  = *p1 ++ ;
                }
                *p2  = *p1;
            }
        }
	}
}

static void buffer_trim_right(hbuffer_t hBuffer, InvokeTickDeclare){
    if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
        hchar * p;
        if(buffer->buffer){
            buffer->buffer[buffer->length] = 0;
            if(buffer->length >0){
                p = buffer->buffer + buffer->length -1;
                while(p != buffer->buffer){
                    if(!ISSPACE(*p)){
                        break;
                    }
                    p--;
                }
                if(p != buffer->buffer){
                    p[1] = 0;
                }
                else if(ISSPACE(*p)){
                    *p = 0;
                }
            }
        }
	}
}
hcchar * buffer_trim(hbuffer_t hBuffer, InvokeTickDeclare){
    buffer_trim_left(hBuffer, InvokeTickArg);
    buffer_trim_right(hBuffer, InvokeTickArg);
    return buffer_to_str(hBuffer, InvokeTickArg);
}

void buffer_extend_size(hbuffer_t hBuffer,hint32 size, InvokeTickDeclare){
	Buffer *buffer = (Buffer *)hBuffer;
	if(buffer->bufferSize < size ){
		buffer->bufferSize = size + buffer->extendSize;
		if(buffer->buffer){
			buffer->buffer = mem_realloc( buffer->buffer, buffer->bufferSize);
		}
		else{
			buffer->buffer = mem_malloc( buffer->bufferSize);
		}
	}
}

hint32 buffer_size(hbuffer_t hBuffer, InvokeTickDeclare){
	return hBuffer?((Buffer *)hBuffer)->bufferSize:0;
}

hint32 buffer_length(hbuffer_t hBuffer, InvokeTickDeclare){
	return hBuffer?((Buffer *)hBuffer)->length:0;
}

void buffer_length_set(hbuffer_t hBuffer,hint32 length, InvokeTickDeclare){
	buffer_extend_size(hBuffer, length,InvokeTickArg);
	((Buffer *)hBuffer)->length = length;
}

hchar * buffer_data(hbuffer_t hBuffer, InvokeTickDeclare){
	return hBuffer?((Buffer *)hBuffer)->buffer:NULL;
}

hchar * buffer_detach(hbuffer_t hBuffer, InvokeTickDeclare){
	if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
		hchar *b = buffer->buffer;
		buffer->buffer = NULL;
		buffer->bufferSize = 0;
		buffer->length = 0;
		return b;
	}
	return NULL;
}

void buffer_out(hbuffer_t hBuffer,hchar *data, InvokeTickDeclare){
	if(hBuffer){
		Buffer *buffer = (Buffer *)hBuffer;
		if(buffer->buffer){
			mem_memcpy(data,buffer->buffer,buffer->length);
		}
	}
}

static hbool _buffer_format_arg_equal(hcchar * argname,hchar *p){
	hchar * p1 =(hchar *)argname, *p2 = p;
	while(*p1 != '\0' && *p2 !='\0'){
		if(*p1 != *p2){
			return hbool_false;
		}
		p1++;
		p2++;
	}
	
	return *p1 == '\0';
}

void buffer_append_md5_upper(hbuffer_t hbuffer,hbyte * data,hint32 length, InvokeTickDeclare){
	md5_state_t md5;
	md5_byte_t digest[16];
	hchar bits[3];
	hint32 i;
	md5_init(&md5);
	md5_append(&md5, (md5_byte_t *)data, length);
	md5_finish(&md5, digest);
	for(i=0;i<16;i++){
		sprintf(bits,"%02X",digest[i]);
		buffer_append(hbuffer,bits,2,InvokeTickArg);
	}
}
void buffer_append_md5(hbuffer_t hbuffer,hbyte * data,hint32 length, InvokeTickDeclare){
	md5_state_t md5;
	md5_byte_t digest[16];
	hchar bits[3];
	hint32 i;
	md5_init(&md5);
	md5_append(&md5, (md5_byte_t *)data, length);
	md5_finish(&md5, digest);
	for(i=0;i<16;i++){
		sprintf(bits,"%02x",digest[i]);
		buffer_append(hbuffer,bits,2,InvokeTickArg);
	}
}

void buffer_append_md5_str(hbuffer_t hbuffer,hcchar * str, InvokeTickDeclare){
	if(hbuffer && str){
		buffer_append_md5(hbuffer,(hbyte *)str,(hint32)strlen(str),InvokeTickArg);
	}
}

void buffer_append_md5_upper_str(hbuffer_t hbuffer,hcchar * str, InvokeTickDeclare){
	if(hbuffer && str){
		buffer_append_md5_upper(hbuffer,(hbyte *)str,(hint32)strlen(str),InvokeTickArg);
	}
}

void buffer_append_url_encode(hbuffer_t hbuffer,hcchar * str,InvokeTickDeclare){
    if(hbuffer && str){
        huchar * p = (huchar *) str;
        hchar buf[4];
        while(p && *p !='\0'){
            
            if(*p == ' '){
                buffer_append_str(hbuffer, "+",InvokeTickArg);
            }
            else if((*p >= '0' && *p<='9') || (*p >= 'A' && *p<='Z') || (*p >= 'a' && *p<='z')){
                buffer_append(hbuffer,p,1,InvokeTickArg);
            }
            else{
                sprintf(buf, "%%%02X",*p);
                buffer_append(hbuffer,buf,3,InvokeTickArg);
            }
            
            p++;
        }
    }
}

void buffer_append_format_va_list(hbuffer_t hbuffer,hcchar * format,va_list va_arg, InvokeTickDeclare){
    if(hbuffer && format){
		hchar * p = (hchar *)format,s=0;
		hchar va_value[128] = {0};
		hint64 value = 0;
        hdouble fvalue = 0;
		hbuffer_t arg = buffer_alloc(128,1028,InvokeTickArg);
        
		while(*p != '\0'){
			
			if(s ==0){
				if(*p =='%'){
					buffer_clear(arg,InvokeTickArg);
					buffer_append(arg,p,1,InvokeTickArg);
					s ++;
				}
				else{
					buffer_append(hbuffer,p,1,InvokeTickArg);
				}
			}
			else if(s ==1){
				buffer_append(arg,p,1,InvokeTickArg);
				if(*p =='%'){
					buffer_append_str(hbuffer, "%",InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("lld",p)){
					value = va_arg(va_arg,hint64);
					buffer_append_str(arg, "ld",InvokeTickArg);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					p += 2;
					s = 0;
				}
				else if(_buffer_format_arg_equal("ld",p)){
					value = va_arg(va_arg,hintptr);
					buffer_append_str(arg, "d",InvokeTickArg);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(long)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					p += 1;
					s = 0;
				}
				else if(_buffer_format_arg_equal("d",p)){
					value = va_arg(va_arg,hint32);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(int)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("llu",p)){
					value = va_arg(va_arg,huint64);
					buffer_append_str(arg, "lu",InvokeTickArg);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(unsigned long long)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					p += 2;
					s = 0;
				}
				else if(_buffer_format_arg_equal("lu",p)){
					value = va_arg(va_arg,huintptr);
					buffer_append_str(arg, "u",InvokeTickArg);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(unsigned long)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					p += 1;
					s = 0;
				}
				else if(_buffer_format_arg_equal("u",p)){
					value = va_arg(va_arg,huint32);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(unsigned int)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("c",p)){
					value = va_arg(va_arg,hint32);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),(int)value);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("f",p)){
					fvalue = va_arg(va_arg,hdouble);
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),fvalue);
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("x",p) || _buffer_format_arg_equal("X",p) || _buffer_format_arg_equal("o",p)){
					sprintf(va_value,buffer_to_str(arg,InvokeTickArg),va_arg(va_arg,huint32));
					buffer_append_str(hbuffer, va_value,InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("s",p)){
					buffer_append_str(hbuffer, va_arg(va_arg,char *),InvokeTickArg);
					s = 0;
				}
				else if(_buffer_format_arg_equal("ms",p)){
					buffer_append_md5_str(hbuffer,va_arg(va_arg,char *),InvokeTickArg);
					p += 1;
					s = 0;
				}
				else if(_buffer_format_arg_equal("mus",p)){
					buffer_append_md5_upper_str(hbuffer,va_arg(va_arg,char *),InvokeTickArg);
					p += 2;
					s = 0;
				}
                else if(_buffer_format_arg_equal("es",p)){
					buffer_append_url_encode(hbuffer,va_arg(va_arg,char *),InvokeTickArg);
					p += 1;
					s = 0;
				}
			}
			
			p++;
		}
		
		buffer_dealloc(arg,InvokeTickArg);
	}
}

void buffer_append_format(hbuffer_t hbuffer,hcchar *format, InvokeTickDeclare,...){
    va_list va_arg;
    va_start(va_arg, __InvokeTick__);
    buffer_append_format_va_list(hbuffer, format, va_arg,InvokeTickArg);
    va_end(va_arg);
}

hcchar * buffer_path_last(hbuffer_t hbuffer,InvokeTickDeclare){
    if(hbuffer){
        hchar * p = (hchar *)buffer_to_str(hbuffer, InvokeTickArg);
        hint32 len = (hint32)strlen(p);
        while(len >0){
            
            if(p[len -1] == '\\' || p[len -1] == '/'){
                return p + len;
            }
            len --;
        }
        return p;
    }
    return NULL;
}

void buffer_path_last_remove(hbuffer_t hbuffer,InvokeTickDeclare){
    if(hbuffer){
        hchar * p = (hchar *)buffer_to_str(hbuffer, InvokeTickArg);
        hint32 len = (hint32)strlen(p);
        while(len >0){
            
            if(p[len -1] == '\\' || p[len -1] == '/'){
                p[len] = 0;
                break;
            }
            len --;
            hbuffer->length --;
        }
    }
}

void buffer_append_int16le(hbuffer_t buffer,hint16 x,InvokeTickDeclare){
    hint16 v = htoles(x);
    buffer_append(buffer,&v,sizeof(hint16),InvokeTickArg);
}

void buffer_append_int16be(hbuffer_t buffer,hint16 x,InvokeTickDeclare){
    hint16 v = htobes(x);
    buffer_append(buffer,&v,sizeof(hint16),InvokeTickArg);
}

void buffer_append_int32le(hbuffer_t buffer,hint32 x,InvokeTickDeclare){
    hint32 v = htolel(x);
    buffer_append(buffer,&v,sizeof(hint32),InvokeTickArg);
}

void buffer_append_int32be(hbuffer_t buffer,hint32 x,InvokeTickDeclare){
    hint32 v = htobel(x);
    buffer_append(buffer,&v,sizeof(hint32),InvokeTickArg);
}

void buffer_append_int64le(hbuffer_t buffer,hint64 x,InvokeTickDeclare){
    hint64 v = htoleq(x);
    buffer_append(buffer,&v,sizeof(hint64),InvokeTickArg);
}

void buffer_append_int64de(hbuffer_t buffer,hint64 x,InvokeTickDeclare){
    hint64 v = htobeq(x);
    buffer_append(buffer,&v,sizeof(hint64),InvokeTickArg);
}
