//
//  hobjc_bundle.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_bundle_h
#define hobjc_hobjc_bundle_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_value.h"
    
    typedef struct _hobj_bundle_t{
        hobj_t base;
        struct _hobj_bundle_t * READONLY parent;
        hobj_value_t * READONLY bundlePath;
        hobj_t * READONLY infoObject;
    } hobj_bundle_t;
    
    hobj_bundle_t * hobj_bundle_alloc(hobj_bundle_t * parent,hcchar * bundlePath,hcchar * info,InvokeTickDeclare);
    
    hobj_bundle_t * hobj_bundle_new(hobj_bundle_t * parent,hcchar * bundlePath,hcchar * info,InvokeTickDeclare);
    
    hobj_bundle_t * hobj_bundle_parent(hobj_bundle_t * bundle,InvokeTickDeclare);
    
    hcchar * hobj_bundle_path(hobj_bundle_t * bundle,InvokeTickDeclare);
    
    hobj_t * hobj_bundle_info_object(hobj_bundle_t * bundle,InvokeTickDeclare);
    
    hobj_t * hobj_bundle_getStringContent(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    hobj_t * hobj_bundle_getBytesContent(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    hobj_t * hobj_bundle_getFilePath(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_bundle_getStringContent_impl_t)(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_bundle_getBytesContent_impl_t)(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_bundle_getFilePath_impl_t)(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);
    
    HOBJ_CLASS_DEC(Bundle)
    
    HOBJ_KEY_DEC(Bundle)
    HOBJ_KEY_DEC(infoObject)
    HOBJ_KEY_DEC(bundlePath)
    HOBJ_KEY_DEC(parent)
    HOBJ_KEY_DEC(version)
    HOBJ_KEY_DEC(getStringContent)
    HOBJ_KEY_DEC(getBytesContent)
    HOBJ_KEY_DEC(getFilePath)
    
#ifdef __cplusplus
}
#endif


#endif
