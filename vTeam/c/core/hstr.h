/*
 *  str.h
 *  C Library
 *
 *  Created by hailong zhang on 10-7-31.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hmem.h"

#ifndef HSTR_H
#define HSTR_H


#ifdef __cplusplus
extern "C" {
#endif
    
	void str_cpy(hchar ** dest,hcchar * source,InvokeTickDeclare);
	
	hbool str_exist(hcchar *str,hcchar *substr,InvokeTickDeclare);
	
	hbool str_exist_non_case(hcchar *str,hcchar *substr,InvokeTickDeclare);
	
	hint32 str_cmp(hcchar *str1,hcchar *str2,InvokeTickDeclare);
	
	hint32 str_cmp_non_case(hcchar * str1,hcchar * str2,InvokeTickDeclare);
	
	
	hbool str_has_suffix(hcchar * str,hcchar * substr,InvokeTickDeclare);
	
	hbool str_has_prefix(hcchar * str,hcchar * substr,InvokeTickDeclare);
    
    hint32 str_len(hcchar * str,InvokeTickDeclare);
    
    hint32 str_wlen(hcchar * str,InvokeTickDeclare);
    
    hint32 str_to_wstr(hcchar * str,hwchar * wstr,InvokeTickDeclare);
    
    hint32 str_len_form_wstr(hwchar * str,hint32 wlen,InvokeTickDeclare);
    
    hint32 wstr_to_str(hwchar * str,hint32 wlen,hchar * targer,InvokeTickDeclare);
    
    hint32 wstr_index_of(hcchar * str,hchar * of,InvokeTickDeclare);
    
    typedef struct{
        huint32 location;
        huint32 length;
    } str_range_t;
    
    str_range_t str_find(hcchar * str,...);
    
    hchar * str_find_sub(hcchar * str,hcchar * sub);
    
#define str_cpy(a,b) str_cpy((a),(b),InvokeTickArg)
    
#define str_exist(a,b) str_exist((a),(b),InvokeTickArg)
	
#define str_exist_non_case(a,b) str_exist_non_case((a),(b),InvokeTickArg)
	
#define str_cmp(a,b) str_cmp((a),(b),InvokeTickArg)
	
#define str_cmp_non_case(a,b) str_cmp_non_case((a),(b),InvokeTickArg)
	
#define str_has_suffix(a,b) str_has_suffix((a),(b),InvokeTickArg)
	
#define str_has_prefix(a,b) str_has_prefix((a),(b),InvokeTickArg)
    
#define str_len(a) str_len((a),InvokeTickArg)
    
#define str_wlen(a) str_wlen((a),InvokeTickArg)

#define str_to_wstr(a,b) str_to_wstr((a),(b),InvokeTickArg)
 
#define str_len_form_wstr(a,b) str_len_form_wstr((a),(b),InvokeTickArg)
    
#define wstr_to_str(a,b,c) wstr_to_str((a),(b),(c),InvokeTickArg)
	
#define wstr_index_of(a,b) wstr_index_of((a),(b),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif

#endif

