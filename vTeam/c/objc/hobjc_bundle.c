//
//  hobjc_bundle.c
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hobjc_bundle.h"
#include "hbuffer.h"
#include "hobjc_json.h"
#include "hlog.h"

static void hobj_bundle_destroy_impl(hobj_bundle_t * obj,InvokeTickDeclare);

static hobj_t * hobj_bundle_getStringContent_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);

static hobj_t * hobj_bundle_getBytesContent_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);

static hobj_t * hobj_bundle_getFilePath_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare);

HOBJ_KEY_IMP(Bundle)
HOBJ_KEY_IMP(infoObject)
HOBJ_KEY_IMP(bundlePath)
HOBJ_KEY_IMP(parent)
HOBJ_KEY_IMP(version)
HOBJ_KEY_IMP(getStringContent)
HOBJ_KEY_IMP(getBytesContent)
HOBJ_KEY_IMP(getFilePath)

HOBJ_CLASS_PROPERTY_IMP_BEGIN(Bundle)

HOBJ_CLASS_PROPERTY_IMP(parent, HOBJ_TYPE_OBJECT, hobj_bundle_parent, NULL)
HOBJ_CLASS_PROPERTY_IMP(bundlePath, HOBJ_TYPE_STRING, hobj_bundle_path, NULL)
HOBJ_CLASS_PROPERTY_IMP(infoObject, HOBJ_TYPE_OBJECT, hobj_bundle_info_object, NULL)

HOBJ_CLASS_PROPERTY_IMP_END(Bundle)

HOBJ_CLASS_METHOD_IMP_BEGIN(Bundle)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_bundle_destroy_impl)

HOBJ_CLASS_METHOD_IMP(getStringContent, hobj_bundle_getStringContent_impl)

HOBJ_CLASS_METHOD_IMP(getBytesContent, hobj_bundle_getBytesContent_impl)

HOBJ_CLASS_METHOD_IMP(getFilePath, hobj_bundle_getFilePath_impl)

HOBJ_CLASS_METHOD_IMP_END(Bundle)

HOBJ_CLASS_IMP_P_I(Bundle, hobj_bundle_t, &hobj_class)

hobj_bundle_t * hobj_bundle_alloc(hobj_bundle_t * parent,hcchar * bundlePath,hcchar * info,InvokeTickDeclare){
    hobj_bundle_t * v = (hobj_bundle_t *) hobj_alloc(&hobj_Bundle_class, InvokeTickArg);
    hobj_t * o;
    
    v->parent = (hobj_bundle_t *)hobj_retain((hobj_t *)parent, InvokeTickArg);
    v->bundlePath = hobj_value_string_alloc(bundlePath, InvokeTickArg);
   
    if(info){
        o = hobj_bundle_getStringContent(v, info, InvokeTickArg);
    }
    else{
        o = hobj_bundle_getStringContent(v, "info.json", InvokeTickArg);
    }
 
    if(o){
        v->infoObject = hobj_retain(hobj_json_decode(hobj_value_stringValue((hobj_value_t *)o, InvokeTickArg), InvokeTickArg),InvokeTickArg);
    }
    
    return v;
}

hobj_bundle_t * hobj_bundle_new(hobj_bundle_t * parent,hcchar * bundlePath,hcchar * info,InvokeTickDeclare){
    hobj_bundle_t * b = hobj_bundle_alloc(parent,bundlePath,info,InvokeTickArg);
    return (hobj_bundle_t *) hobj_autorelease((hobj_t *) b, InvokeTickArg);
}

hobj_bundle_t * hobj_bundle_parent(hobj_bundle_t * bundle,InvokeTickDeclare){
    return bundle->parent;
}

hcchar * hobj_bundle_path(hobj_bundle_t * bundle,InvokeTickDeclare){
    return hobj_value_stringValue(bundle->bundlePath, InvokeTickArg);
}

hobj_t * hobj_bundle_info_object(hobj_bundle_t * bundle,InvokeTickDeclare){
    return bundle->infoObject;
}


hobj_t * hobj_bundle_getStringContent(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    hobj_method_t * method = hobj_method((hobj_t *)bundle, HOBJ_KEY(getStringContent), InvokeTickArg);
    if(method){
        return ( * (hobj_bundle_getStringContent_impl_t) method->imp)(bundle,filePath,InvokeTickArg);
    }
    return NULL;
}

hobj_t * hobj_bundle_getBytesContent(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    hobj_method_t * method = hobj_method((hobj_t *)bundle, HOBJ_KEY(getBytesContent), InvokeTickArg);
    if(method){
        return ( * (hobj_bundle_getBytesContent_impl_t) method->imp)(bundle,filePath,InvokeTickArg);
    }
    return NULL;
}

hobj_t * hobj_bundle_getFilePath(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    hobj_method_t * method = hobj_method((hobj_t *)bundle, HOBJ_KEY(getFilePath), InvokeTickArg);
    if(method){
        return ( * (hobj_bundle_getFilePath_impl_t) method->imp)(bundle,filePath,InvokeTickArg);
    }
    return NULL;
}

static void hobj_bundle_destroy_impl(hobj_bundle_t * obj,InvokeTickDeclare){
    hobj_release((hobj_t *)obj->bundlePath, InvokeTickArg);
    hobj_release((hobj_t *)obj->infoObject, InvokeTickArg);
    hobj_release((hobj_t *)obj->parent, InvokeTickArg);
}

static hobj_t * hobj_bundle_getFilePath_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    hbuffer_t path = buffer_alloc(128, 128);
    hchar s = PATH_SPLIT;
    hchar * p = (hchar *) filePath;
    hobj_t * v = NULL;
    buffer_append_str(path, hobj_bundle_path(bundle,InvokeTickArg));
    
    if(buffer_length(path) >0){
        if( * (buffer_data(path) + buffer_length(path) - 1) != s){
            buffer_append(path,&s,1);
        }
    }
    while(p && *p != '\0' && *p == s){
        p ++;
    }
    
    buffer_append_str(path, p);
    
    v = (hobj_t *)hobj_value_string_new(buffer_to_str(path), InvokeTickArg);
    
    buffer_dealloc(path);
    
    return v;
}

static hobj_t * hobj_bundle_getStringContent_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    hobj_t * path = hobj_bundle_getFilePath(bundle, filePath, InvokeTickArg);
    hobj_t * v = NULL;
    
    if(bundle->parent){
        v = hobj_bundle_getStringContent(bundle->parent,hobj_value_stringValue((hobj_value_t *)path, InvokeTickArg),InvokeTickArg);
    }
    else{
        {
            hbuffer_t buf = buffer_alloc(MAX_READ_BUF, MAX_READ_BUF);
            hchar b[MAX_READ_BUF];
            hint32 l;
            FILE * f = fopen(hobj_value_stringValue((hobj_value_t *)path, InvokeTickArg), "r");
            
            if(f){
                while(! feof(f) && (l = fread(b, 1, sizeof(b), f)) >0 ){
                    buffer_append(buf,b,l);
                }
                fclose(f);
                v = (hobj_t *)hobj_value_string_new_with_length(buffer_to_str(buf), buffer_length(buf), InvokeTickArg);
            }
            
            buffer_dealloc(buf);

        }
    }
    
    return v;
}

static hobj_t * hobj_bundle_getBytesContent_impl(hobj_bundle_t * bundle,hcchar * filePath,InvokeTickDeclare){
    
    hobj_t * path = hobj_bundle_getFilePath(bundle, filePath, InvokeTickArg);
    hobj_t * v = NULL;
    
    if(bundle->parent){
        v = hobj_bundle_getStringContent(bundle->parent,hobj_value_stringValue((hobj_value_t *)path, InvokeTickArg),InvokeTickArg);
    }
    else{
        {
            hbuffer_t buf = buffer_alloc(MAX_READ_BUF, MAX_READ_BUF);
            hchar b[MAX_READ_BUF];
            hint32 l;
            FILE * f = fopen(hobj_value_stringValue((hobj_value_t *)path, InvokeTickArg), "rb");
            
            if(f){
                while((l = fread(b, 1, sizeof(b), f)) >0 ){
                    buffer_append(buf,b,l);
                }
                fclose(f);
                v = (hobj_t *)hobj_value_new(HOBJ_TYPE_BYTES, buffer_data(buf), buffer_length(buf), InvokeTickArg);
            }
            
            buffer_dealloc(buf);
            
        }
    }
    

    return v;

}

