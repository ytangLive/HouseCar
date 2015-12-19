//
//  hvermin_bin.h
//  C Library
//
//  Created by Hailong Zhang on 11-10-31.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_BIN_H
#define _HVERMIN_BIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hvermin_runtime.h"
#include "hvermin_compile.h"
    
    
    typedef struct{
        vmCompileObjectArray * classMetas;
        vmCompileObjectArray * operatorMetas;
        hbuffer_t uniqueKeys;
        hint32 uniqueKeyCount;
        vmClassMetaOffset operatorOffset;
        vmClassMetaOffset classOffset;
    }vmBinary;
    
    hbool vmBinaryInit(vmBinary * binary,InvokeTickDeclare);
    
    void vmBinaryDestroy(vmBinary * binary,InvokeTickDeclare);
    
    hbool vmBinaryAddClass(vmBinary * binary,vmCompileClassMeta * classMeta,hcchar * className,InvokeTickDeclare);
    
    hint32 vmBinaryLength(vmBinary * binary,InvokeTickDeclare);
    
    vmRuntimeClassLibraryBytes * vmBinaryBytes(vmBinary * binary,InvokeTickDeclare);
    
    void vmBinaryBytesDealloc(vmBinary * binary,vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare);
    
    void vmBinaryWriteFile(vmBinary * binary,hcchar * file,InvokeTickDeclare);
    
    
#define vmBinaryInit(a) vmBinaryInit((a),InvokeTickArg)
    
#define vmBinaryDestroy(a) vmBinaryDestroy((a),InvokeTickArg)
    
#define vmBinaryAddClass(a,b,c) vmBinaryAddClass((a),(b),(c),InvokeTickArg)
    
#define vmBinaryLength(a) vmBinaryLength((a),InvokeTickArg)
    
#define vmBinaryBytes(a) vmBinaryBytes((a),InvokeTickArg)
    
#define vmBinaryWriteFile(a,b) vmBinaryWriteFile((a),(b),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif


#endif
