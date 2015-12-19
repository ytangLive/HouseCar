//
//  hcss_scanf.c
//  vTeam
//
//  Created by Zhang Hailong on 13-8-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hcss_scanf.h"
#include "hbuffer.h"


#define IS_SPACE(c) ((c) == ' ' || (c) == '\t')
#define IS_SPACE_NEW_LINE(c)    ( IS_SPACE(c) || (c) == '\r' || (c) == '\n')

hbool hcss_scanf(hcss_scanf_t * css,hcchar * str,hany styleSheet, InvokeTickDeclare){
    
    hchar * p = (hchar *) str;
    hbuffer_t name = buffer_alloc(128, 128);
    hbuffer_t key = buffer_alloc(128, 128);
    hbuffer_t value = buffer_alloc(128, 128);
    hany style = NULL;
    
    int s = 0;
    
    while (p && *p != '\0') {
        
        switch (s) {
            case 0:
            {
                // 
                if(p[0] == '/' && p[1] == '/'){
                    s = 1;
                    p ++;
                }
                else if(p[0] == '/' && p[1] == '*'){
                    s = 2;
                    p ++;
                }
                else if(!IS_SPACE_NEW_LINE(*p)){
                    buffer_append(name, p, 1);
                    buffer_clear(key);
                    s = 3;
                }
            }
                break;
            case 1:
            {
                if(*p == '\n'){
                    s = 0;
                }
            }
                break;
            case 2:
            {
                if(*p == '*' && p[1] == '/'){
                    s = 0;
                    p ++;
                }
            }
                break;
            case 3:
            {
                // name
                if( IS_SPACE_NEW_LINE(*p)){
                    
                }
                else if(*p == '{'){
                    s = 4;
                    style = (* css->style_new)(css,buffer_to_str(name),InvokeTickArg);
                    buffer_clear(name);
                    buffer_clear(key);
                }
                else{
                    buffer_append(name, p, 1);
                }
            }
                break;
            case 4:
            {
                // key
                if(IS_SPACE_NEW_LINE(*p)){
                    
                }
                else if(*p == ':'){
                    s = 5;
                    buffer_clear(value);
                }
                else if(*p == '}'){
                    (*css->style_add)(css,styleSheet,style,InvokeTickArg);
                    (*css->release)(css,style,InvokeTickArg);
                    style = NULL;
                    s = 0;
                }
                else{
                    buffer_append(key, p, 1);
                }
            }
                break;
            case 5:
            {
                // value
                if(IS_SPACE_NEW_LINE(*p)){
                    
                }
                else{
                    buffer_append(value, p, 1);
                    s = 6;
                }
            }
                break;
            case 6:
            {
                if(*p == ';'){
                    (*css->attr_set)(css,style,buffer_to_str(key),buffer_to_str(value),InvokeTickArg);
                    buffer_clear(key);
                    buffer_clear(value);
                    s = 4;
                }
                else if(*p == '}'){
                    (*css->attr_set)(css,style,buffer_to_str(key),buffer_to_str(value),InvokeTickArg);
                    buffer_clear(key);
                    buffer_clear(value);
                    (*css->style_add)(css,styleSheet,style,InvokeTickArg);
                    (*css->release)(css,style,InvokeTickArg);
                    style = NULL;
                    s = 0;
                }
                else{
                    buffer_append(value, p, 1);
                }
            }
                break;
            default:
                break;
        }
        
        p++;
    }
    
    buffer_dealloc(name);
    buffer_dealloc(key);
    buffer_dealloc(value);
    
    return hbool_true;
}

