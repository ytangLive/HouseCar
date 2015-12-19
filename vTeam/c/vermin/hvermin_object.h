//
//  hvermin_object.h
//  C Library
//
//  Created by hailong zhang on 11-6-8.
//  Copyright 2011年 hailong.org. All rights reserved.
//


#ifndef _HVERMIN_OBJECT_H
#define _HVERMIN_OBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hvermin_runtime.h"

    struct _buffer_t;
    
    extern vmClassBase vmObjectClass;
    extern vmClassBase vmStringClass;
    extern vmClassBase vmNumberClass;
    extern vmClassBase vmExceptionClass;
    extern vmClassBase vmArrayClass;
    extern vmClassBase vmDictionaryClass;
    extern vmClassBase vmDataClass;
    extern vmClassBase vmSystemClass;
    extern vmClassBase vmMathClass;
    
    
    typedef struct _vmData{
        vmObject base;
        struct _buffer_t * bytes;
        struct{
            vmUniqueKey length;
            vmUniqueKey charAt;
            vmUniqueKey append;
            vmUniqueKey appendFormat;
            vmUniqueKey clear;
            vmUniqueKey removeLastPath;
            vmUniqueKey lastPath;
            vmUniqueKey indexOf;
            vmUniqueKey substr;
        }uniqueKeys;
    }vmData;

    
    typedef struct _vmException{
        vmObject base;
        struct _vmException *parent;
        hchar * error;
        hint32 code;
        union{
            struct{
                vmClass * vmClass;
                vmClassMetaOffset offset;
            }classMeta;
            struct{
                vmClass * vmClass;
                hcchar * file;
                hint32 line;
            }classBase;
        }location;
        struct{
            vmUniqueKey code;
            vmUniqueKey error;
        }uniqueKeys;
    }vmException;
    
    vmVariant vmStringAlloc(vmRuntimeContext context,hcchar * str,InvokeTickDeclare);
    
    vmVariant vmStringAllocWithLength(vmRuntimeContext context,hcchar * str,hint32 length,InvokeTickDeclare);
    
    vmVariant vmNumberDoubleAlloc(vmRuntimeContext context,hdouble value,InvokeTickDeclare);
    
    vmVariant vmNumberInt16Alloc(vmRuntimeContext context,hint16 value,InvokeTickDeclare);
    
    vmVariant vmNumberInt32Alloc(vmRuntimeContext context,hint32 value,InvokeTickDeclare);
    
    vmVariant vmNumberInt64Alloc(vmRuntimeContext context,hint64 value,InvokeTickDeclare);
    
    vmVariant vmNumberBooleanAlloc(vmRuntimeContext context,hbool value,InvokeTickDeclare);
    
    vmVariant vmExceptionAlloc(vmRuntimeContext context,hint32 code,hcchar * error,InvokeTickDeclare);
    
    vmVariant vmArrayAlloc(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmArrayAdd(vmRuntimeContext context,vmObject * object, vmVariant value, InvokeTickDeclare);
    
    vmVariant vmDictionaryAlloc(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmDictionaryPut(vmRuntimeContext context,vmObject * object,hcchar * key, vmVariant value,InvokeTickDeclare);
    
    vmVariant vmSystemAlloc(vmRuntimeContext context,FILE * inFile,FILE * outFile,InvokeTickDeclare);

    vmVariant vmDataAlloc(vmRuntimeContext context,hbyte * data,hint32 length,InvokeTickDeclare);
    
    void vmDataAppend(vmRuntimeContext context,vmObject * object,hbyte * data,hint32 length,InvokeTickDeclare);
    
    hbyte * vmDataDataPtr(vmRuntimeContext context,vmObject * object,InvokeTickDeclare);
    
    hint32 vmDataDataLength(vmRuntimeContext context,vmObject * object,InvokeTickDeclare); 
    
#define vmStringAlloc(a,b) vmStringAlloc((a),(b),InvokeTickArg)
    
#define vmNumberDoubleAlloc(a,b) vmNumberDoubleAlloc((a),(b),InvokeTickArg)
    
#define vmNumberInt16Alloc(a,b) vmNumberInt16Alloc((a),(b),InvokeTickArg)
    
#define vmNumberInt32Alloc(a,b) vmNumberInt32Alloc((a),(b),InvokeTickArg)
    
#define vmNumberInt64Alloc(a,b) vmNumberInt64Alloc((a),(b),InvokeTickArg)
    
#define vmNumberBooleanAlloc(a,b) vmNumberBooleanAlloc((a),(b),InvokeTickArg)
    
#define vmExceptionAlloc(a,b,c) vmExceptionAlloc((a),(b),(c),InvokeTickArg)
    
#define vmArrayAlloc(a) vmArrayAlloc((a),InvokeTickArg)
    
#define vmDictionaryAlloc(a) vmDictionaryAlloc((a),InvokeTickArg)
    
#define vmSystemAlloc(a,b,c) vmSystemAlloc((a),(b),(c),InvokeTickArg)
    
#define vmArrayAdd(a,b,c) vmArrayAdd((a),(b),(c),InvokeTickArg)
    
#define vmDictionaryPut(a,b,c,d) vmDictionaryPut((a),(b),(c),(d),InvokeTickArg)
 
#define vmDataAlloc(a,b,c) vmDataAlloc((a),(b),(c),InvokeTickArg)
    
#define vmDataAppend(a,b,c,d) vmDataAppend((a),(b),(c),(d),InvokeTickArg)
    
#define vmDataDataPtr(a,b) vmDataDataPtr((a),(b),InvokeTickArg)

#define vmDataDataLength(a,b) vmDataDataLength((a),(b),InvokeTickArg)

    
    
    /**
     
     Return : Void or Throwable
     */
    
    vmVariant vmObjectClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare);
    /**
     
     Return : Void or Throwable
     */
    
    vmVariant vmObjectClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare);
    /**
     
     Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
     */
    vmVariant vmObjectClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare);
    
    /**
     
     Return : Void or Throwable
     */
    vmVariant vmObjectClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare);
    /**
     
     Return : any
     */
    vmVariant vmObjectClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare);
    
    /**
     
     Return : Void
     */
    void vmObjectClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
