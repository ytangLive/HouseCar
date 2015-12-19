//
//  hvmcompiler_debug.h
//  vmcompiler
//
//  Created by Zhang Hailong on 13-3-16.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_debug_h
#define vmcompiler_hvmcompiler_debug_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_array.h"
#include "hvermin_runtime.h"
#include "hvmcompiler_bin.h"
    
    typedef struct {
        vmClassMetaOffset offset;
        hubyte classUDID;
        huint32 line;
        huint16 index;
    } vmBinaryDebugItem;
    
    typedef struct {
        hchar tag[16];
        hchar version[16];
        hubyte UDID[16];
        hint32 length;
    } vmBinaryDebug;
    
    vmBinaryDebug * vmBinaryDebugAlloc(hcchar * debugFile,InvokeTickDeclare);
    
    void vmBinaryDebugDealloc(vmBinaryDebug * debug,InvokeTickDeclare);
    
    vmBinaryDebugItem * vmBinaryDebugSearch(vmBinaryDebug * debug,vmClassMetaOffset offset,InvokeTickDeclare);
    
    hbool vmBinaryDebugWriteFile(vmBinary * binary,hcchar * binaryFile,hcchar * debugFile,InvokeTickDeclare);
    
    void vmBinaryDebugLog(vmBinaryDebug * debug,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
