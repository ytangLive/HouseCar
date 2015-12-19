/*
 *  str.c
 *  C Library
 *
 *  Created by hailong zhang on 10-7-31.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#include "hconfig.h"
#include "hstr.h"

#undef str_cpy

#undef str_exist

#undef str_exist_non_case

#undef str_cmp

#undef str_cmp_non_case

#undef str_has_suffix

#undef str_has_prefix

#undef str_len

#undef str_wlen

#undef str_to_wstr

#undef str_len_form_wstr

#undef wstr_to_str

#undef wstr_index_of

void str_cpy(hchar ** dest,hcchar *source,InvokeTickDeclare){
	if(*dest){
		mem_free( *dest);
		*dest = NULL;
	}
	if(source){
		*dest = (hchar *)mem_malloc((hint32)strlen(source) +1);
		strcpy(*dest,source);
	}
}

static hchar lower_char(hchar c){
	if(c >='A' && c <='Z'){
		return c + 'a' - 'A';
	}
	return c;
}

//static hchar upper_char(hchar c){
//	if(c >='a' && c <='z'){
//		return c - 'a' + 'A';
//	}
//	return c;
//}

hbool str_exist_non_case(hcchar *str,hcchar *substr,InvokeTickDeclare){
	if(str && substr){
		hchar *p = (hchar *)str;
		hint32 l = (hint32)strlen(substr),i=0;
		while(*p!='\0'){
			if(substr[i] == '\0'){
				break; 
			}
			if(lower_char(*p) == lower_char(substr[i])){
				i++;
			}
			else{
				p = p -i;
				i = 0;
			}
			p++;
		}
		return i==l;
	}
	return 0;
}

hbool str_exist(hcchar *str,hcchar *substr,InvokeTickDeclare){
	if(str && substr){
		hchar *p = (hchar *)str;
		hint32 l = (hint32)strlen(substr),i=0;
		while(*p!='\0'){
			if(substr[i] == '\0'){
				break; 
			}
			if(*p == substr[i]){
				i++;
			}
			else{
				p = p -i;
				i = 0;
			}
			p++;
		}
		return i==l;
	}
	return 0;
}

hint32 str_cmp(hcchar *str1,hcchar *str2,InvokeTickDeclare){
	if(str1 == NULL && str2 == NULL){
		return 0;
	}
	else if(str1 == NULL){
		return -1;
	}
	else if(str2 == NULL){
		return 1;
	}
	return strcmp(str1,str2);
}

hint32 str_cmp_non_case(hcchar *str1,hcchar *str2,InvokeTickDeclare){
	hchar *p1 = (hchar *)str1,*p2=(hchar *)str2,d;
	if(str1 == NULL && str2 == NULL){
		return 0;
	}
	else if(str1 == NULL){
		return -1;
	}
	else if(str2 == NULL){
		return 1;
	}
	while(*p1 != '\0' && *p2 !='\0'){
		d = lower_char(*p1) - lower_char(*p2);
		if(d){
			return d;
		}
		p1++;
		p2++;
	}
	if(*p1 == *p2){
		return 0;
	}
	if(*p1 =='\0'){
		return -1;
	}
	if(*p2 == '\0'){
		return 1;
	}
	return 0;
}


hbool str_has_suffix(hcchar *str,hcchar *substr,InvokeTickDeclare){
	hint32 len = (hint32)strlen(str);
	hint32 sublen =(hint32)strlen(substr);
	while(--sublen >=0 && --len >=0 ){
		if(str[len] != substr[sublen]){
			return 0;
		}
	}
	return sublen <0;
}

hbool str_has_prefix(hcchar *str,hcchar *substr,InvokeTickDeclare){
	hchar * strp = (hchar *)str;
	hchar * subp = (hchar *)substr;
	while(*strp !='\0' && *subp != '\0'){
		if(*strp != * subp){
			return 0;
		}
		strp ++;
		subp++;
	}
	return *subp == '\0';
}

hint32 str_len(hcchar * str,InvokeTickDeclare){
    return str ? (hint32)(hintptr)strlen(str):0;
}

hint32 str_wlen(hcchar * str,InvokeTickDeclare){
    huchar * p = (huchar *) str;
    huchar b;
    huint32 bits;
    huint32 bit;
    huint32 m;
    hint32 i;
    hint32 len = 0;
    while(p && *p !='\0'){
        bits = 0;
        b = *p;
        m = 0x80;
        bit = 0;
        while(m != 0x01){
            if(m & b){
                bits ++;
                b = b & ~m;
                m = m >>1;
            }
            else{
                if(m == 0x80){
                    bits ++;
                }
                bit = b;
                for(i=1;i<bits;i++){
                    bit = (bit << 6) | (p[i] & 0x7f);
                }
                break;
            }
        }
        p += bits;
        if(bit & 0xFFFF0000){
            len +=2;
        }
        else{
            len +=1;
        }
    }
    return len;
}

hint32 str_to_wstr(hcchar * str,hwchar * wstr,InvokeTickDeclare){
    huchar * p = (huchar *) str;
    huchar b;
    huint32 bits;
    huint32 bit;
    huint32 m;
    hint32 i;
    hint32 len = 0;
    while(p && *p !='\0'){
        bits = 0;
        b = *p;
        m = 0x80;
        bit = 0;
        while(m != 0x01){
            if(m & b){
                bits ++;
                b = b & ~m;
                m = m >>1;
            }
            else{
                if(m == 0x80){
                    bits ++;
                }
                bit = b;
                for(i=1;i<bits;i++){
                    bit = (bit << 6) | (p[i] & 0x7f);
                }
                break;
            }
        }
        p += bits;
        if(bit & 0xFFFF0000){
            * wstr ++ = ((bit - 0x10000) >> 10) | 0x0D800;
            * wstr ++ = (bit & 0x3ff) | 0x0DC00;
            len +=2;
        }
        else{
            * wstr ++ = bit;
            len +=1;
        }
    }
    return len;
}

hint32 str_len_form_wstr(hwchar * str,hint32 wlen,InvokeTickDeclare){
    hint32 rs = 0;
    
    while(wlen >0){
        
        if(* str <0x80){
            rs ++;
        }
        else if( * str < 0x800){
            rs += 2;
        }
        else{
            rs += 3;
        }

        str ++;
        wlen --;
    }
    return rs;
}

hint32 wstr_to_str(hwchar * str,hint32 wlen,hchar * targer,InvokeTickDeclare){
    hint32 rs = 0;
    
    while(wlen >0){
        
        if(* str <0x80){
            targer[rs ++] = * str;

        }
        else if( * str < 0x800){
            targer[rs ++] = 0xc0 | ( * str >> 6);
            targer[rs ++] = 0x80 | ( * str & 0x3f);
        }
        else{
            targer[rs ++] = 0xe0 | ( *str >> 12);
            targer[rs ++] = 0x80 | ( (* str >> 6) & 0x3f );
            targer[rs ++] = 0x80 | ( * str & 0x3f);
        }
        
        str ++;
        wlen --;
    }
    return rs;
}

hint32 wstr_index_of(hcchar * str,hchar * of,InvokeTickDeclare){
    huchar * p = (huchar *) str;
    huchar b;
    huint32 bits;
    huint32 bit;
    huint32 m;
    hint32 i;
    hint32 len = 0;
    while(p && *p !='\0' && (void *)p < (void *)of){
        bits = 0;
        b = *p;
        m = 0x80;
        bit = 0;
        while(m != 0x01){
            if(m & b){
                bits ++;
                b = b & ~m;
                m = m >>1;
            }
            else{
                if(m == 0x80){
                    bits ++;
                }
                bit = b;
                for(i=1;i<bits;i++){
                    bit = (bit << 6) | (p[i] & 0x7f);
                }
                break;
            }
        }
        p += bits;
        if(bit & 0xFFFF0000){
            len +=2;
        }
        else{
            len +=1;
        }
    }
    return len;

}

str_range_t str_find(hcchar * str,...){
    va_list va;
    str_range_t range = {0,0};
    hchar * p = (hchar *) str;
    hchar c;
    hint32 s = 0;
    while(p){
        
        c = 0;
        
        va_start(va,str);
    
        while((c = va_arg(va, int)) !=0){
            if( *p == c){
                break;
            }
        }
        
        va_end(va);
        
        if(s ==0){
            if(c != 0){
                s = 1;
                range.length = 1;
            }
            else{
                range.location ++;
            }
        }
        else{
            if(c == 0){
                break;
            }
            else{
                range.length ++;
            }
        }
        
        if(*p == '\0'){
            break;
        }
        p ++;
     
    }
    
    return range;
}

hchar * str_find_sub(hcchar * str,hcchar * sub){
    hchar * p = (hchar *) str;
    hchar * p1, * p2;
    
    while(p && *p != '\0'){
        p1 = p;
        p2 = (hchar *) sub;
        while(p2 && * p2 != '\0' && *p1 !='\0'){
            if(*p1 != *p2){
                break;
            }
            p1 ++;
            p2 ++;
        }
        if(p2 == NULL || *p2 =='\0'){
            return p;
        }
        
        p++;
    }
    
    return NULL;
}
