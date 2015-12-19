//
//  hvmcompiler_bin.h
//  vmcompiler
//
//  Created by zhang hailong on 13-3-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_bin_h
#define vmcompiler_hvmcompiler_bin_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_array.h"
#include "hobjc_dictionary.h"
#include "hvermin_runtime.h"
#include "hvmcompiler.h"
    
    struct _buffer_t;
    struct _map_t;
    
    typedef struct{
        hobj_t base;
        hobj_array_t * classMetas;
        hobj_array_t * operatorMetas;
        struct _buffer_t * uniqueKeys;
        hint32 uniqueKeyCount;
        vmClassMetaOffset operatorOffset;
        vmClassMetaOffset classOffset;
        hobj_dictionary_t * resources;
    }vmBinary;

    HOBJ_KEY_DEC(vmBinary)
    
    HOBJ_CLASS_DEC(vmBinary)

    hbool vmBinaryAddClass(vmBinary * binary,vmCompilerClassMeta * classMeta,hcchar * className,InvokeTickDeclare);

    hbool vmBinaryAddStringResource(vmBinary * binary,hcchar * key,hcchar * string,InvokeTickDeclare);
    
    hint32 vmBinaryLength(vmBinary * binary,InvokeTickDeclare);

    vmRuntimeClassLibraryBytes * vmBinaryBytes(vmBinary * binary,InvokeTickDeclare);

    void vmBinaryBytesDealloc(vmBinary * binary,vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare);

    void vmBinaryWriteFile(vmBinary * binary,hcchar * file,InvokeTickDeclare);

    
#ifdef __cplusplus
}
#endif


#endif
