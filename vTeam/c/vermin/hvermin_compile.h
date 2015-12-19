//
//  hvermin_compile.h
//  C Library
//
//  Created by hailong zhang on 11-6-8.
//  Copyright 2011年 hailong.org. All rights reserved.
//


#ifndef _HVERMIN_COMPILE_H
#define _HVERMIN_COMPILE_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin_runtime.h"
#include "hserial_list.h"
#include "hvermin_scanner.h"
#include "hbuffer.h"
    
#define ISSPACECHAR(c)  ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define ISNAME(c)       ( ( (c)>= 'A' && (c)<='Z' ) || ( (c)>= 'a' && (c)<='z' ) || ( (c)>= '0' && (c)<='9' ) || (c) =='_')
#define ISNAMEFIRST(c)  ( ( (c)>= 'A' && (c)<='Z' ) || ( (c)>= 'a' && (c)<='z' ) || (c) =='_')
#define ISNUMBER(c)     ( ( (c)>= '0' && (c)<='9' ) || (c) =='.' || (c) =='e' || (c) =='E' || (c) == 'x')
#define ISNUMBERFIRST(c)( ( (c)>= '0' && (c)<='9' ) || (c) =='-')
#define ISFALSE(p)      ( (p)[0] =='f' && (p)[1] == 'a' && (p)[2] =='l' && (p)[3] == 's' && (p)[4] == 'e' )
#define ISTRUE(p)       ( (p)[0] =='t' && (p)[1] == 'r' && (p)[2] =='u' && (p)[3] == 'e' ) 
#define ISNULL(p)       ( (p)[0] =='n' && (p)[1] == 'u' && (p)[2] =='l' && (p)[3] == 'l' )
#define ISOP(c)         ( (c) == '+' || (c) == '-' || (c) == '*' || (c) == '/' || (c) == '%' || (c) == '~' || (c) == '!' || (c) == '^' || (c) == '&' || (c) == '(' || (c) == ')' || (c) == '[' || (c) == '}' || (c) == '?' || (c) == ':' || (c) == ',' || (c) == ']' || (c) == ']' || (c) ==  '>' || (c) ==  '<' || (c) ==  '=' || (c) == '|' )
    
#define ISVAR(p)        ( (p)[0] == 'v' && (p)[1] == 'a' && (p)[2]== 'r' && (p)[3] == ' ' )
    
    HANDLER(vmCompile)
    
    struct _vmCompileObject;
    
    typedef void (* vmCompileObjectDealloc)(struct _vmCompileObject * obj,InvokeTickDeclare);
    
    typedef struct _vmCompileObject{
        hint32 retainCount;
        vmCompileObjectDealloc dealloc;
    }vmCompileObject;
    
    typedef struct {
        vmCompileObject base;
        hint32 count;
        vmCompileObject ** items;
        hint32 length;
    }vmCompileObjectArray;
    
    vmCompileObject * vmCompileObjectCreate(hint32 size,vmCompileObjectDealloc dealloc,InvokeTickDeclare);
    
    vmCompileObject * vmCompileObjectRetain( vmCompileObject * obj,InvokeTickDeclare);
    
    void vmCompileObjectRelease(vmCompileObject * obj,InvokeTickDeclare);
    
#define vmCompileObjectCreate(T,d)        (T *)vmCompileObjectCreate(sizeof(T),(vmCompileObjectDealloc)(d),InvokeTickArg)
#define vmCompileObjectRetain(obj)      vmCompileObjectRetain((vmCompileObject *)(obj),InvokeTickArg)
#define vmCompileObjectRelease(obj)     vmCompileObjectRelease((vmCompileObject *)(obj),InvokeTickArg)
    
    vmCompileObjectArray * vmCompileObjectArrayCreate(InvokeTickDeclare);
    
    void vmCompileObjectArrayAdd(vmCompileObjectArray * array,vmCompileObject * obj,InvokeTickDeclare);
    
    void vmCompileObjectArrayRemove(vmCompileObjectArray * array,vmCompileObject * obj,InvokeTickDeclare);
    
    void vmCompileObjectArrayRemoveAt(vmCompileObjectArray * array,hint32 i,InvokeTickDeclare);
    
    void vmCompileObjectArrayRemoveLast(vmCompileObjectArray * array,InvokeTickDeclare);
    
    void vmCompileObjectArrayRemoveAll(vmCompileObjectArray * array,InvokeTickDeclare);
    
    hint32 vmCompileObjectArrayCount(vmCompileObjectArray * array,InvokeTickDeclare);
    
    vmCompileObject * vmCompileObjectArrayGet(vmCompileObjectArray * array,hint32 i,InvokeTickDeclare);
    
    vmCompileObject * vmCompileObjectArrayLast(vmCompileObjectArray * array,InvokeTickDeclare);
 
#define vmCompileObjectArrayCount(a) vmCompileObjectArrayCount((vmCompileObjectArray *)(a),InvokeTickArg)
#define vmCompileObjectArrayCreate()    vmCompileObjectArrayCreate(InvokeTickArg)
#define vmCompileObjectArrayAdd(a,b)    vmCompileObjectArrayAdd((vmCompileObjectArray *)(a), (vmCompileObject * )(b),InvokeTickArg)
#define vmCompileObjectArrayRemove(a,b) vmCompileObjectArrayRemove((vmCompileObjectArray *)(a), (vmCompileObject * )(b),InvokeTickArg)
#define vmCompileObjectArrayRemoveAt(a,b) vmCompileObjectArrayRemoveAt((vmCompileObjectArray *)(a), (b),InvokeTickArg)
#define vmCompileObjectArrayRemoveLast(a) vmCompileObjectArrayRemoveLast((vmCompileObjectArray *)(a),InvokeTickArg)
#define vmCompileObjectArrayRemoveAll(a) vmCompileObjectArrayRemoveAll((vmCompileObjectArray *)(a),InvokeTickArg)
#define vmCompileObjectArrayGet(a,b) vmCompileObjectArrayGet((vmCompileObjectArray *)(a),(b),InvokeTickArg)
#define vmCompileObjectArrayLast(a) vmCompileObjectArrayLast((vmCompileObjectArray *)(a),InvokeTickArg)

    
    typedef struct{
        hint32 line;
        hint32 offset;
    }vmCompileLocation;
    
    typedef struct{
        vmCompileLocation location;
        hint32 errorCode;
        hchar error[64];
    }vmCompileError;
    
    
    typedef struct{
        vmCompileObject base;
        vmRuntimeOperatorType type;
        hbuffer_t uniqueKey;
        vmCompileObjectArray * compileMetas;
        struct{
            vmClassMetaOffset uniqueKey;
            vmClassMetaOffset offset;
            hint32 length;
        } binary;
    }vmCompileMetaOperator;
    
    typedef struct{
        vmCompileObject base;
        vmRuntimeMetaType type;
        union {
            hint16 int16Value;
            hint32 int32Value;
            hint64 int64Value;
            hbool booleanValue;
            hdouble doubleValue;
            hbuffer_t stringValue;
            hbuffer_t objectKey;
            vmCompileMetaOperator * operatorMeta;
        } value;
        hint32 line;
        hint32 index;
        struct{
            vmClassMetaOffset valueOffset;
        }binary;
    }vmCompileMeta;
    
    
    typedef struct{
        vmCompileObject base;
        hbuffer_t superClass;
        vmCompileObjectArray * propertys;
        vmCompileObjectArray * functions;
        struct{
            vmClassMetaOffset superClass;
            vmClassMetaOffset className;
            vmClassMetaOffset offset;
        }binary;
    }vmCompileClassMeta;
    
   
    vmCompileMetaOperator * vmCompileMetaOperatorCreate(vmRuntimeOperatorType type, hcchar * uniqueKey,InvokeTickDeclare);
    
    void vmCompileMetaOperatorAddCompileMeta(vmCompileMetaOperator * op, vmCompileMeta * compileMeta,InvokeTickDeclare);
    
    
    vmCompileMeta * vmCompileMetaCreate(InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithInt16(hint16 value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithInt32(hint32 value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithInt64(hint64 value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithBoolean(hbool value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithDouble(hdouble value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithString(hcchar * value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithObjectKey(hcchar * objectKey,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithOperator(vmCompileMetaOperator * op,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithNumberString(hcchar * value,InvokeTickDeclare);
    
    vmCompileMeta * vmCompileMetaCreateWithArg(hcchar * arg,InvokeTickDeclare);
    
    void vmCompileMetaPrint(vmCompileMeta * meta,InvokeTickDeclare);
    
    
    vmCompileClassMeta * vmCompileClassMetaCreate(InvokeTickDeclare);
    
 
#define vmCompileMetaCreate() vmCompileMetaCreate(InvokeTickArg)
    
#define vmCompileMetaCreateWithInt16(a) vmCompileMetaCreateWithInt16((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithInt32(a) vmCompileMetaCreateWithInt32((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithInt64(a) vmCompileMetaCreateWithInt64((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithBoolean(a) vmCompileMetaCreateWithBoolean((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithDouble(a) vmCompileMetaCreateWithDouble((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithString(a) vmCompileMetaCreateWithString((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithObjectKey(a) vmCompileMetaCreateWithObjectKey((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithOperator(a) vmCompileMetaCreateWithOperator((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithNumberString(a) vmCompileMetaCreateWithNumberString((a),InvokeTickArg)
    
#define vmCompileMetaCreateWithArg(a) vmCompileMetaCreateWithArg((a),InvokeTickArg)

#define vmCompileMetaPrint(a) vmCompileMetaPrint((a),InvokeTickArg)
    
#define vmCompileMetaOperatorCreate(a,b) vmCompileMetaOperatorCreate((a),(b),InvokeTickArg)

#define vmCompileMetaOperatorAddCompileMeta(a,b) vmCompileMetaOperatorAddCompileMeta((a),(b),InvokeTickArg)
    
#define vmCompileClassMetaCreate() vmCompileClassMetaCreate(InvokeTickArg);
    
    
    
#ifdef __cplusplus
}
#endif


#endif
