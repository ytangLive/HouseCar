//
//  hxml_scanf.c
//  hclib
//
//  Created by zhang hailong on 13-2-27.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hxml_scanf.h"

#include "hbuffer.h"
#include "hserial_list.h"


#define IS_SPACE_CHAR(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define IS_NBSP(p) ((p)[0] == 'n' && (p)[1] == 'b' && (p)[2] == 's' && (p)[3] == 'p' && (p)[4] == ';')
#define IS_GT(p) ((p)[0] == 'g' && (p)[1] == 't' && (p)[2] == ';')
#define IS_LT(p) ((p)[0] == 'l' && (p)[1] == 't' && (p)[2] == ';')
#define IS_AMP(p) ((p)[0] == 'a' && (p)[1] == 'm' && (p)[2] == 'p' && (p)[3] == ';')
#define IS_QUOT(p) ((p)[0] == 'q' && (p)[1] == 'u' && (p)[2] == 'o' && (p)[3] == 't' && (p)[4] == ';')
#define IS_LDQUO(p) ((p)[0] == 'l' && (p)[1] == 'd' && (p)[2] == 'q' && (p)[3] == 'u' && (p)[4] == 'o' && (p)[5] == ';')
#define IS_RDQUO(p) ((p)[0] == 'r' && (p)[1] == 'd' && (p)[2] == 'q' && (p)[3] == 'u' && (p)[4] == 'o' && (p)[5] == ';')
#define IS_HELLIP(p) ((p)[0] == 'h' && (p)[1] == 'e' && (p)[2] == 'l' && (p)[3] == 'l' && (p)[4] == 'i' && (p)[5] == 'p' && (p)[6] == ';')
#define IS_MDASH(p) ((p)[0] == 'm' && (p)[1] == 'd' && (p)[2] == 'a' && (p)[3] == 's' && (p)[4] == 'h' && (p)[5] == ';')
#define IS_BULL(p) ((p)[0] == 'b' && (p)[1] == 'u' && (p)[2] == 'l' && (p)[3] == 'l' && (p)[4] == ';' )
#define IS_MIDDOT(p) ((p)[0] == 'm' && (p)[1] == 'i' && (p)[2] == 'd' && (p)[3] == 'd' && (p)[4] == 'o' && (p)[5] == 't' && (p)[6] == ';')
#define IS_END(p) ((p)[0] == '/' && (p)[1] == '>')

static hchar * hxml_scanf_parse_decode(hbuffer_t target,hchar * p,InvokeTickDeclare);

static hchar * hxml_scanf_parse_string(hbuffer_t target,hchar * p,hchar endChar, InvokeTickDeclare){
    hint32 s = 0;
    while(*p != '\0'){
        switch (s) {
            case 0:
            {
                if(*p == '\\'){
                    s ++;
                }
                else if(*p == '&'){
                    p ++;
                    p = hxml_scanf_parse_decode(target,p,InvokeTickArg);
                    continue;
                }
                else if(*p == endChar || (endChar == 0 && (IS_SPACE_CHAR(*p) || *p == '<' || *p == '>' || *p == '/'))){
                    return p;
                }
                else{
                    buffer_append(target, p, 1);
                }
            }
                break;
            case 1:
            {
                buffer_append(target, p, 1);
                s = 0;
            }
                break;
            default:
                break;
        }
        p++;
    }
    return p;
}

static hchar * hxml_scanf_parse_decode(hbuffer_t target,hchar * p,InvokeTickDeclare){
    if(IS_NBSP(p)){
        buffer_append_str(target, " ");
        p +=5;
    }
    else if(IS_GT(p)){
        buffer_append_str(target, ">");
        p +=3;
    }
    else if(IS_LT(p)){
        buffer_append_str(target, "<");
        p +=3;
    }
    else if(IS_QUOT(p)){
        buffer_append_str(target, "\"");
        p +=5;
    }
    else if(IS_AMP(p)){
        buffer_append_str(target, "&");
        p +=4;
    }
    else if(IS_LDQUO(p)){
        buffer_append_str(target, "『");
        p +=6;
    }
    else if(IS_RDQUO(p)){
        buffer_append_str(target, "』");
        p +=6;
    }
    else if(IS_MDASH(p)){
        buffer_append_str(target, "－");
        p +=6;
    }
    else if(IS_HELLIP(p)){
        buffer_append_str(target, "…");
        p +=7;
    }
    else if(IS_BULL(p)){
        buffer_append_str(target, "•");
        p +=5;
    }
    else if(IS_MIDDOT(p)){
        buffer_append_str(target, "·");
        p +=7;
    }
    else{
        buffer_append_str(target, "&");
    }
    return p;
}

typedef struct{
    huint32 key;
    huint32 value;
} hxml_scanf_attr_t;

typedef struct{
    hbuffer_t name;
    hbuffer_t ns;
    hbuffer_t text;
    hbuffer_t attr_name;
    hbuffer_t attr_value;
    hbuffer_t string_buffer;
    hserial_list_t attrs;
}hxml_scanf_decode_buffers;

static hchar * _hxml_scanf_skip_tag(hchar * p,hcchar * name,hcchar * ns,InvokeTickDeclare){
    hchar * rs = p;
    
    while( *p != '\0'){
        if(IS_SPACE_CHAR(*p)){
            p ++;//添加，防止死循环
        }
        else {
            if(* p != '<'){
                break;
            }
            
            p ++;
            
            if( *p != '/'){
                break;
            }
            
            p ++;
            
            if(ns && * ns != '\0'){
                while( *p == * ns ){
                    p ++;
                    ns ++;
                }
                
                if(*ns != '\0' || *p !=':'){
                    break;
                }
                p ++;
            }
            
            while( *p == * name ){
                p ++;
                name ++;
            }
            
            if(*name != '\0' || *p !='>'){
                break;
            }
            
            return p +1;
        }
    }
    
    return rs;
}

static void _hxml_scanf_element_text_gen(hxml_scanf_t * xml,hany toElement,hxml_scanf_decode_buffers * buffers, InvokeTickDeclare){
   
    hany element = NULL;
    
    if(buffer_length(buffers->text)){
        if(( * xml->is_empty)(xml,toElement,InvokeTickArg)){
            (* xml->text_set)(xml,toElement,buffer_to_str(buffers->text),InvokeTickArg);
        }
        else{
            element = (* xml->element_new)(xml,"_text","",InvokeTickArg);
            (* xml->text_set)(xml,element,buffer_to_str(buffers->text),InvokeTickArg);
            (* xml->child_add)(xml,toElement,element,InvokeTickArg);
            (* xml->release)(xml,element,InvokeTickArg);
        }
        buffer_clear(buffers->text);
        buffer_clear(buffers->name);
        buffer_clear(buffers->ns);
        buffer_clear(buffers->attr_name);
        buffer_clear(buffers->attr_value);
        buffer_clear(buffers->string_buffer);
        serial_list_clear(buffers->attrs);
    }
}

static huint32 _hxml_scanf_string_index(hxml_scanf_t * xml,hxml_scanf_decode_buffers * buffers,hcchar * string,InvokeTickDeclare){
    
    huint32 r = buffer_length(buffers->string_buffer);
    
    buffer_append(buffers->string_buffer, (hchar *)string, (hint32)strlen(string) +1);
    
    return r;
}

static hcchar * _hxml_scanf_string(hxml_scanf_t * xml,hxml_scanf_decode_buffers * buffers,huint32 index,InvokeTickDeclare){
    return buffer_data(buffers->string_buffer) + index;
}

static hchar * _hxml_scanf_decode(hxml_scanf_t * xml,hchar * p,hany toElement,hxml_scanf_decode_buffers * buffers, InvokeTickDeclare);

static hany _hxml_scanf_element_gen(hxml_scanf_t * xml,hany toElement,hxml_scanf_decode_buffers * buffers, hchar ** p,InvokeTickDeclare){
    
    hany element = NULL;
    hint32 i,c;
    hxml_scanf_attr_t * attr;
    
    
    element = (* xml->element_new)(xml,buffer_to_str(buffers->name), buffer_to_str(buffers->ns),InvokeTickArg);
    
    c = serial_list_count(buffers->attrs);
    
    for(i=0 ;i<c;i++){
        attr = (hxml_scanf_attr_t *) serial_list_item_at(buffers->attrs, i);
        (* xml->attr_set)(xml,element,_hxml_scanf_string(xml,buffers,attr->key,InvokeTickArg),_hxml_scanf_string(xml,buffers,attr->value,InvokeTickArg),InvokeTickArg);
    }
    
    (* xml->child_add)(xml,toElement,element,InvokeTickArg);
    
    if(p){
        
        if((* xml->tag_has_children)(xml,buffer_to_str(buffers->name), buffer_to_str(buffers->ns),InvokeTickArg)){
            
            buffer_clear(buffers->text);
            buffer_clear(buffers->name);
            buffer_clear(buffers->ns);
            buffer_clear(buffers->attr_name);
            buffer_clear(buffers->attr_value);
            buffer_clear(buffers->string_buffer);
            serial_list_clear(buffers->attrs);
            
            * p = _hxml_scanf_decode(xml, (* p) + 1,element, buffers, InvokeTickArg);
        }
        else{
            * p = _hxml_scanf_skip_tag((* p) +1 ,buffer_to_str(buffers->name), buffer_to_str(buffers->ns),InvokeTickArg);
        }
        
    }
    
    (* xml->release)(xml,element,InvokeTickArg);

    buffer_clear(buffers->text);
    buffer_clear(buffers->name);
    buffer_clear(buffers->ns);
    buffer_clear(buffers->attr_name);
    buffer_clear(buffers->attr_value);
    buffer_clear(buffers->string_buffer);
    serial_list_clear(buffers->attrs);


    return element;
}

static void _hxml_scanf_buffer_append_char(hxml_scanf_t * xml,hchar c,hbuffer_t toBuffer,InvokeTickDeclare){
    switch (xml->caseMode) {
        case HXMLScanfDecodeCaseModeToLower:
            if(c >='A' && c <='Z'){
                c = c + 32;
            }
            break;
        case HXMLScanfDecodeCaseModeToUpper:
            if(c >='a' && c <='z'){
                c = c - 32;
            }
            break;
        default:
            break;
    }
    buffer_append(toBuffer, &c, 1);
}


static hchar * _hxml_scanf_decode(hxml_scanf_t * xml,hchar * p,hany toElement,hxml_scanf_decode_buffers * buffers, InvokeTickDeclare){
    hint32 s = 0x00;
    hxml_scanf_attr_t attr;
    
    while(p && *p != '\0'){
        
        switch (s) {
            case 0x00:
            {
                if(*p == '<' && p[1] == '/'){
                    p++;
                    s = 0x50;
                    
                    _hxml_scanf_element_text_gen(xml, toElement, buffers, InvokeTickArg);
                }
                else if(*p == '<' && p[1] == '!' && p[2] == '-' && p[3] == '-'){
                    p += 3;
                    s = 0x60;
                }
                else if(*p == '<'){
                    s = 0x10;
                    _hxml_scanf_element_text_gen(xml, toElement, buffers, InvokeTickArg);
                }
                else if(*p == '&'){
                    p = hxml_scanf_parse_decode(buffers->text,p+1,InvokeTickArg);
                    continue;
                }
                else if(IS_SPACE_CHAR(*p)){
                    
                }
                else if(*p == '\\'){
                    if(p[1] == 'n'){
                        buffer_append(buffers->text, "\n",1);
                    }
                    else if(p[1] == '\\'){
                        buffer_append(buffers->text, "\\",1);
                    }
                    else if(p[1] == '\t'){
                        buffer_append(buffers->text, "\t",1);
                    }
                    else if(p[1] == '\r'){
                        buffer_append(buffers->text, "\r",1);
                    }
                    else{
                        buffer_append(buffers->text, p + 1,1);
                    }
                    p ++;
                }
                else{
                    buffer_append(buffers->text,p,1);
                }
            }
                break;
            case 0x10:
            {
                if(IS_END(p)){
                    buffer_append_str(buffers->name, buffer_to_str(buffers->ns));
                    buffer_clear(buffers->ns);
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers, NULL,InvokeTickArg);
                   
                    s = 0x00;
                    p ++;
                }
                else if(*p == '>'){
                    buffer_append_str(buffers->name, buffer_to_str(buffers->ns));
                    buffer_clear(buffers->ns);

                    _hxml_scanf_element_gen(xml, toElement, buffers,&p, InvokeTickArg);

                    s = 0x00;
                    continue;
                }
                else if(*p == ':'){
                    s = 0x20;
                }
                else if(IS_SPACE_CHAR(*p)){
                    buffer_append_str(buffers->name, buffer_to_str(buffers->ns));
                    buffer_clear(buffers->ns);
                    s = 0x30;
                }
                else{
                    _hxml_scanf_buffer_append_char(xml, * p,buffers->ns,InvokeTickArg);
                }
            }
                break;
            case 0x20:
                if(IS_END(p)){
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers, NULL, InvokeTickArg);
                    
                    s = 0x00;
                    p++;
                }
                else if(*p == '>'){
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers,&p, InvokeTickArg);
                    
                    s = 0x00;
                    
                    continue;
                }
                else if(IS_SPACE_CHAR(*p)){
                    s = 0x30;
                }
                else{
                    _hxml_scanf_buffer_append_char(xml, * p,buffers->name,InvokeTickArg);
                }
                
                break;
            case 0x30:
            {
                if(IS_SPACE_CHAR(*p)){
                    
                }
                else if(IS_END(p)){
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers, NULL, InvokeTickArg);
                    
                    s = 0x00;
                    p++;
                }
                else if(*p == '>'){
                    if(buffer_length(buffers->name) ==0){
                        buffer_append_str(buffers->name, buffer_to_str(buffers->ns));
                        buffer_clear(buffers->ns);
                    }

                    _hxml_scanf_element_gen(xml, toElement, buffers, &p,InvokeTickArg);
                    
                    
                    s = 0x00;
                    continue;
                }
                else {
                    s = 0x31;
                    _hxml_scanf_buffer_append_char(xml, * p,buffers->attr_name,InvokeTickArg);
                }
            }
                break;
            case 0x31:
            {
                if(*p == '='){
                    s =0x40;
                }
                else if(IS_END(p)){
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers, NULL,InvokeTickArg);
                    
                    s = 0x00;
                    p ++;
                }
                else if(*p == '>'){
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers,&p, InvokeTickArg);
                    
                    
                    s = 0x00;
                    continue;
                }
                else{
                    _hxml_scanf_buffer_append_char(xml, * p,buffers->attr_name,InvokeTickArg);
                }
            }
                break;
            case 0x40:
            {
                if(IS_SPACE_CHAR(*p)){
                    
                }
                else if(IS_END(p)){
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers,NULL, InvokeTickArg);
                    
                    s = 0x00;
                    p ++;
                }
                else if(*p == '>'){
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    _hxml_scanf_element_gen(xml, toElement, buffers, &p,InvokeTickArg);

                    s = 0x00;
                    continue;
                }
                else if(*p == '\'' || *p == '"'){
                    p = hxml_scanf_parse_string(buffers->attr_value, p+1, *p, InvokeTickArg);
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    buffer_clear(buffers->attr_name);
                    buffer_clear(buffers->attr_value);
                    
                    s = 0x30;
                }
                else{
                    
                    p = hxml_scanf_parse_string(buffers->attr_value, p+1, 0, InvokeTickArg);
                    
                    memset(&attr,0,sizeof(attr));
                    
                    attr.key = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_name),InvokeTickArg);
                    attr.value = _hxml_scanf_string_index(xml, buffers, buffer_to_str(buffers->attr_value),InvokeTickArg);
                    
                    serial_list_item_add(buffers->attrs, &attr);
                    
                    buffer_clear(buffers->attr_name);
                    buffer_clear(buffers->attr_value);
                    
                    s = 0x30;
                    p --;
                }
            }
                break;
            case 0x50:
            {
                if(*p == '>'){
                    return p +1;
                }
            }
                break;
            case 0x60:
            {
                if(*p == '-' && p[1] == '-' && p[2] == '>'){
                    p += 2;
                    s = 0x00;
                }
            }
                break;
            default:
                break;
        }
        
        p++;
    }
    
    _hxml_scanf_element_text_gen(xml, toElement, buffers, InvokeTickArg);
    
    return p;

}

hbool hxml_scanf(hxml_scanf_t * xml,hcchar * str,hany toElement, InvokeTickDeclare){
    hchar * rs = NULL;
    
    hxml_scanf_decode_buffers buffers = {
        buffer_alloc(128, 32),
        buffer_alloc(128, 32),
        buffer_alloc(128, 128),
        buffer_alloc(128, 32),
        buffer_alloc(128, 128),
        buffer_alloc(256, 256),
        serial_list_alloc(20, 20),
    };
    
    rs = _hxml_scanf_decode(xml, (hchar * ) str, toElement, &buffers, InvokeTickArg);
    
    buffer_dealloc(buffers.name);
    buffer_dealloc(buffers.ns);
    buffer_dealloc(buffers.text);
    buffer_dealloc(buffers.attr_name);
    buffer_dealloc(buffers.attr_value);
    buffer_dealloc(buffers.string_buffer);
    serial_list_dealloc(buffers.attrs);
    
    return rs && * rs == '\0';
}
