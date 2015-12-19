//
//  hxpath.c
//  hclib
//
//  Created by zhang hailong on 13-3-6.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hxpath.h"

#include "hserial_list.h"

#define LIMIT_MAX   0x7fffffff;

static hint32 _hxpath(hxpath_t * xpath, hcchar * selector,struct _serial_list_t * values,hany ofElement,hint32 limit,InvokeTickDeclare){
    hint32 rs = 0;
    hchar * p = (hchar *) selector;
    HXPathValue value = {HXPathValueTypeNone};
    
    while(p){
        
        if(p[0] == '/'){
            if(p[1] == '/'){
                
            }
        }
        else if(p[0] == '/'){
            
        }
        
        
        if(* p == '\0'){
            break;
        }
    }
}

hint32 hxpath(hxpath_t * xpath, hcchar * selector,struct _serial_list_t * values,hany ofElement,InvokeTickDeclare){
    return _hxpath(xpath, selector, values, ofElement, LIMIT_MAX, InvokeTickArg);
}

HXPathValue hxpath_value(hxpath_t * xpath, hcchar * selector,hany ofElement,InvokeTickDeclare){
    hserial_list_t values = serial_list_alloc(sizeof(HXPathValue), 0);
    HXPathValue value = {HXPathValueTypeNone};
    
    if( _hxpath(xpath, selector, values, ofElement, 1, InvokeTickArg) >0){
        value = * (HXPathValue *) serial_list_item_at(values, 0);
    }
    
    serial_list_dealloc(values);
    
    return value;
}

