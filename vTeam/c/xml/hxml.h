//
//  hxml.h
//  hxml
//
//  Created by zhang hailong on 13-3-6.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hxml_h
#define hxml_h

#ifdef __cplusplus
extern "C" {
#endif
    
    struct _data_t;
    struct _list_t;
    
    typedef struct _hxml_element_t{
        struct _data_t * ns;
        struct _data_t * name;
        struct _data_t * attributes;
        struct _data_t * text;
        struct _list_t * childs;
    } hxml_element_t;
    
    hxml_element_t * hxml_element_alloc(hcchar * name,hcchar * ns, InvokeTickDeclare);
    
    hxml_element_t * hxml_element_retain(hxml_element_t * element,InvokeTickDeclare);
    
    void hxml_element_release(hxml_element_t * element,InvokeTickDeclare);
    
    hcchar * hxml_element_ns(hxml_element_t * element,hcchar * defaultValue,InvokeTickDeclare);
    
    hcchar * hxml_element_name(hxml_element_t * element,hcchar * defaultValue,InvokeTickDeclare);
    
    hcchar * hxml_element_attr(hxml_element_t * element,hcchar * name,hcchar * defaultValue,InvokeTickDeclare);
    
    void hxml_element_attr_set(hxml_element_t * element,hcchar * name,hcchar * value,InvokeTickDeclare);
    
    void hxml_element_attr_set_format(hxml_element_t * element,hcchar * name,hcchar * format,InvokeTickDeclare,...);
    
    hcchar * hxml_element_text(hxml_element_t * element,hcchar * defalutValue,InvokeTickDeclare);
    
    void hxml_element_text_set(hxml_element_t * element,hcchar * text,InvokeTickDeclare);
    
    void hxml_element_text_set_format(hxml_element_t * element,hcchar * format,InvokeTickDeclare,...);
    
    void hxml_element_child_add(hxml_element_t * element,hxml_element_t * child,InvokeTickDeclare);
    
    void hxml_element_child_insert(hxml_element_t * element,hxml_element_t * child,hint32 atIndex,InvokeTickDeclare);
    
    hint32 hxml_element_child_count(hxml_element_t * element,InvokeTickDeclare);
    
    void hxml_element_child_clear(hxml_element_t * element,InvokeTickDeclare);
    
    hxml_element_t * hxml_element_child_at(hxml_element_t * element, hint32 index,InvokeTickDeclare);
    
    void hxml_element_inner_xml_set(hxml_element_t * element,hcchar * innerXML,InvokeTickDeclare);
    
    void hxml_element_inner_xml_add(hxml_element_t * element,hcchar * innerXML,InvokeTickDeclare);
    
    void hxml_element_print(hxml_element_t * element,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
