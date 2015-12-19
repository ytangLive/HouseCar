//
//  hxpath.h
//  hclib
//
//  Created by zhang hailong on 13-3-6.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hclib_hxpath_h
#define hclib_hxpath_h

#ifdef __cplusplus
extern "C" {
#endif

    struct _serial_list_t;
    struct _hxpath_t;
    
    typedef hint32 ( * hxpath_element_child_count_t )(struct _hxpath_t * xpath,hany element,InvokeTickDeclare);
    typedef hany ( * hxpath_element_child_at_t)(struct _hxpath_t * xpath,hany element,hint32 i,InvokeTickDeclare);
    typedef hcchar * ( * hxpath_element_tag_name_t)(struct _hxpath_t * xpath,hany element,InvokeTickDeclare);
    typedef hcchar * ( * hxpath_element_attr_value_t)(struct _hxpath_t * xpath,hany element,hcchar * name, InvokeTickDeclare);
    
    typedef struct _hxpath_t{
        hxpath_element_child_count_t childCount;
        hxpath_element_child_at_t childAt;
        hxpath_element_tag_name_t tagName;
        hxpath_element_attr_t attrValue;
    } hxpath_t;
    
    typedef enum _HXPathValueType{
        HXPathValueTypeNone,HXPathValueTypeElement,HXPathValueTypeStringValue
    } HXPathValueType;
    
    typedef struct _HXPathValue {
        HXPathValueType type;
        union {
            hany element;
            hcchar * stringValue;
        };
    } HXPathValue;
    
    hint32 hxpath(hxpath_t * xpath, hcchar * selector,struct _serial_list_t * values,hany ofElement,InvokeTickDeclare);
    
    HXPathValue hxpath_value(hxpath_t * xpath, hcchar * selector,hany ofElement,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
