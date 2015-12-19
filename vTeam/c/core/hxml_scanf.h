//
//  hxml_scanf.h
//  hclib
//
//  Created by zhang hailong on 13-2-27.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef HXML_SCANF_H
#define HXML_SCANF_H

#ifdef __cplusplus
extern "C" {
#endif
    

    struct _hxml_scanf_t;

    typedef hany (* hxml_scanf_element_new_t)(struct _hxml_scanf_t * xml,hcchar * name,hcchar * ns, InvokeTickDeclare);
    typedef void (* hxml_scanf_element_attr_set_t)(struct _hxml_scanf_t * xml,hany element,hcchar * key,hcchar * value,InvokeTickDeclare);
    typedef void (* hxml_scanf_element_child_add_t)(struct _hxml_scanf_t * xml,hany element,hany child,InvokeTickDeclare);
    typedef hbool (* hxml_scanf_element_is_empty_t)(struct _hxml_scanf_t * xml,hany element,InvokeTickDeclare);
    typedef void (* hxml_scanf_element_text_set_t)(struct _hxml_scanf_t * xml,hany element,hcchar * text,InvokeTickDeclare);
    
    typedef hbool (* hxml_scanf_tag_has_children)(struct _hxml_scanf_t * xml,hcchar * name,hcchar * ns,InvokeTickDeclare);
    
    typedef void (* hxml_scanf_element_release_t)(struct _hxml_scanf_t * xml,hany element,InvokeTickDeclare);

    typedef enum _HXMLScanfDecodeCaseMode{
        HXMLScanfDecodeCaseModeNone,HXMLScanfDecodeCaseModeToUpper,HXMLScanfDecodeCaseModeToLower
    }HXMLScanfDecodeCaseMode;
    
    typedef struct _hxml_scanf_t{
        hxml_scanf_element_new_t element_new;
        hxml_scanf_element_attr_set_t attr_set;
        hxml_scanf_element_child_add_t child_add;
        hxml_scanf_element_is_empty_t is_empty;
        hxml_scanf_element_text_set_t text_set;
        hxml_scanf_tag_has_children tag_has_children;
        hxml_scanf_element_release_t release;
        HXMLScanfDecodeCaseMode caseMode;
    } hxml_scanf_t;

    hbool hxml_scanf(hxml_scanf_t * xml,hcchar * str,hany toElement, InvokeTickDeclare);

#ifdef __cplusplus
}
#endif

#endif
