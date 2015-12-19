//
//  hcss_scanf.h
//  vTeam
//
//  Created by Zhang Hailong on 13-8-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vTeam_hcss_scanf_h
#define vTeam_hcss_scanf_h


#ifdef __cplusplus
extern "C" {
#endif
    
    
    struct _hcss_scanf_t;
    
    typedef hany (* hcss_scanf_style_new_t)(struct _hcss_scanf_t * css,hcchar * name, InvokeTickDeclare);
    typedef void (* hcss_scanf_style_attr_set_t)(struct _hcss_scanf_t * css,hany style,hcchar * key,hcchar * value,InvokeTickDeclare);
    typedef void (* hcss_scanf_style_sheet_add_t)(struct _hcss_scanf_t * css,hany styleSheet,hany style,InvokeTickDeclare);
    typedef void (* hcss_scanf_style_release_t)(struct _hcss_scanf_t * css,hany element,InvokeTickDeclare);
  
    typedef struct _hcss_scanf_t{
        hcss_scanf_style_new_t style_new;
        hcss_scanf_style_attr_set_t attr_set;
        hcss_scanf_style_sheet_add_t style_add;
        hcss_scanf_style_release_t release;
    } hcss_scanf_t;
    
    hbool hcss_scanf(hcss_scanf_t * xml,hcchar * str,hany styleSheet, InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
