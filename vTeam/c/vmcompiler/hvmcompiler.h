//
//  hvmcompiler.h
//  vmcompiler
//
//  Created by zhang hailong on 13-3-13.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_h
#define vmcompiler_hvmcompiler_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_value.h"
#include "hobjc_array.h"
#include "hvermin_runtime.h"
#include "htokenizer.h"
    
    typedef struct {
        htokenizer_location_t location;
        huint32 length;
    } vmCompilerString;
    
    typedef struct{
        hobj_t base;
        vmRuntimeOperatorType type;
        vmCompilerString uniqueKey;
        hobj_array_t * metas;
        struct{
            vmClassMetaOffset uniqueKey;
            vmClassMetaOffset offset;
            hint32 length;
            hubyte classUDID[16];
        } binary;
    }vmCompilerMetaOperator;
    
    HOBJ_KEY_DEC(vmCompilerMetaOperator)
    HOBJ_CLASS_DEC(vmCompilerMetaOperator)
    
    typedef struct{
        hobj_t base;
        vmRuntimeMetaType type;
        union {
            hint16 int16Value;
            hint32 int32Value;
            hint64 int64Value;
            hbool booleanValue;
            hdouble doubleValue;
            vmCompilerString stringValue;
            vmCompilerString objectKey;
            vmCompilerMetaOperator * operatorMeta;
        };
        struct{
            vmClassMetaOffset valueOffset;
        }binary;
    }vmCompilerMeta;
    
    HOBJ_KEY_DEC(vmCompilerMeta)
    HOBJ_CLASS_DEC(vmCompilerMeta)
    
    typedef struct{
        hobj_t base;
        vmCompilerString superClass;
        hobj_array_t * propertys;
        hobj_array_t * functions;
        struct{
            vmClassMetaOffset superClass;
            vmClassMetaOffset className;
            vmClassMetaOffset offset;
            hubyte UDID[16];
        }binary;
    }vmCompilerClassMeta;
    
    HOBJ_KEY_DEC(vmCompilerClassMeta)
    HOBJ_CLASS_DEC(vmCompilerClassMeta)
    
    
    vmCompilerMetaOperator * vmCompilerMetaOperatorNew(vmRuntimeOperatorType,htokenizer_location_t location,huint32 length,InvokeTickDeclare);
    
    void vmCompilerMetaOperatorAddCompilerMeta(vmCompilerMetaOperator * op,vmCompilerMeta * compilerMeta,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNew(InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithInt16(hint16 value,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithInt32(hint32 value,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithInt64(hint64 value,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithBoolean(hbool value,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithDouble(hdouble value,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithString(htokenizer_location_t location,huint32 length,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithObjectKey(htokenizer_location_t location,huint32 length,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithOperator(vmCompilerMetaOperator * op,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithNumberString(htokenizer_location_t location,huint32 length,InvokeTickDeclare);
    
    vmCompilerMeta * vmCompilerMetaNewWithArg(htokenizer_location_t location,huint32 length,InvokeTickDeclare);
    
    vmCompilerClassMeta * vmCompilerClassMetaNew(InvokeTickDeclare);
    
    void vmCompilerClassAddProperty(vmCompilerClassMeta * classMeta,vmCompilerMetaOperator * op,InvokeTickDeclare);
    
    void vmCompilerClassAddFunction(vmCompilerClassMeta * classMeta,vmCompilerMetaOperator * op,InvokeTickDeclare);
    
    
    void vmCompilerClassMetaLog(vmCompilerClassMeta * classMeta,InvokeTickDeclare);
    
#define vmCompilerErrorMaxSize      128
    
    
    typedef struct {
        hobj_t base;
        htokenizer_location_t location;
        hchar error[vmCompilerErrorMaxSize];
    } vmCompilerError;
    
    HOBJ_KEY_DEC(vmCompilerError)
    HOBJ_CLASS_DEC(vmCompilerError)
    
    vmCompilerError * vmCompilerErrorNew(htokenizer_location_t location,hcchar * error,InvokeTickDeclare);
    
    
#define vmCompilerErrorSet(location,error) hobj_array_add(errors,(hobj_t *)vmCompilerErrorNew((location),(error),InvokeTickArg),InvokeTickArg)
    
    
#ifdef __cplusplus
}
#endif


#endif
