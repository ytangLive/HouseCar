//
//  hvermin_runtime.h
//  C Library
//
//  Created by hailong zhang on 11-6-3.
//  Copyright 2011年 hailong.org. All rights reserved.
//


#ifndef _HVERMIN_RUNTIME_H
#define _HVERMIN_RUNTIME_H

#include "hvermin.h"
#include "hmem.h"

#ifdef __cplusplus
extern "C" {
#endif


#define vmRuntimeVersion  "1.1.0"
#define vmRuntimeHead     "__vermin__"
    
    typedef enum{
        vmRuntimeOperatorTypeNone
        
        ,vmRuntimeOperatorTypeNew             // new
        ,vmRuntimeOperatorTypeFunction        // ()
        
        ,vmRuntimeOperatorTypeInvoke      // {}
        ,vmRuntimeOperatorTypeProperty   // .　[]
        
        ,vmRuntimeOperatorTypeInc           // a++
        ,vmRuntimeOperatorTypeDec           // a--
        ,vmRuntimeOperatorTypeBeforeInc     // ++a
        ,vmRuntimeOperatorTypeBeforeDec     // --a
        ,vmRuntimeOperatorTypeAdd           // +
        ,vmRuntimeOperatorTypeSub           // -
        ,vmRuntimeOperatorTypeMultiplication// *
        ,vmRuntimeOperatorTypeDivision      // /
        ,vmRuntimeOperatorTypeModulus       // %
        ,vmRuntimeOperatorTypeBitwiseAnd    // &
        ,vmRuntimeOperatorTypeBitwiseOr     // |
        ,vmRuntimeOperatorTypeBitwiseExclusiveOr        // ^
        ,vmRuntimeOperatorTypeBitwiseComplement         // ~
        ,vmRuntimeOperatorTypeBitwiseShiftLeft          // <<
        ,vmRuntimeOperatorTypeBitwiseShiftRight         // >>
        ,vmRuntimeOperatorTypeNot           // !
        ,vmRuntimeOperatorTypeAnd           // &&
        ,vmRuntimeOperatorTypeOr            // ||
        ,vmRuntimeOperatorTypeGreater       // >
        ,vmRuntimeOperatorTypeGreaterEqual  // >=
        ,vmRuntimeOperatorTypeLess          // <
        ,vmRuntimeOperatorTypeLessEqual     // <=
        ,vmRuntimeOperatorTypeEqual         // ==
        ,vmRuntimeOperatorTypeAbsEqual      // ===
        ,vmRuntimeOperatorTypeNotEqual      // !=
        ,vmRuntimeOperatorTypeAbsNotEqual   // !==
        ,vmRuntimeOperatorTypeAssign        // =
        ,vmRuntimeOperatorTypeIfElse        // ? : 
        ,vmRuntimeOperatorTypeAntiNumber    // -
        
        ,vmRuntimeOperatorTypeIfElseIfElse  // if() {}  else if() {}  else {}
        ,vmRuntimeOperatorTypeFor           // for
        ,vmRuntimeOperatorTypeWhile         // while
        ,vmRuntimeOperatorTypeForeach       // for in
        
        ,vmRuntimeOperatorTypeTryCatchFinally // try

        ,vmRuntimeOperatorTypeThrow         // throw
        
        ,vmRuntimeOperatorTypeVar           // var
        
        ,vmRuntimeOperatorTypeIs            // is
        
        ,vmRuntimeOperatorTypeVarAssign     // var(assign)  
        
    }vmRuntimeOperatorType;
    
    typedef enum{
        vmRuntimeMetaTypeVoid = 0
        ,vmRuntimeMetaTypeInt16 = 1<<0
        ,vmRuntimeMetaTypeInt32 = 1<<1
        ,vmRuntimeMetaTypeInt64 = 1<<2
        ,vmRuntimeMetaTypeBoolean = 1<<3
        ,vmRuntimeMetaTypeDouble = 1<<4
        ,vmRuntimeMetaTypeObject = 1<<5
        ,vmRuntimeMetaTypeString = 1<<6
        ,vmRuntimeMetaTypeThrowable = 1 <<7
        
        ,vmRuntimeMetaTypeReturn   = 1<<8
        ,vmRuntimeMetaTypeOperator = 1<<9
        ,vmRuntimeMetaTypeBreak    = 1<<10
        ,vmRuntimeMetaTypeContinue  = 1 <<11
        ,vmRuntimeMetaTypeSuperObject = 1 << 12
        ,vmRuntimeMetaTypeArg   = 1 << 13
        ,vmRuntimeMetaTypeCatch = 1 << 14
        ,vmRuntimeMetaTypeFinally = 1 << 15
    }vmRuntimeMetaType;
    
    typedef huint32 vmClassMetaOffset;
    
	typedef union{
		hint16 int16Value;
        hint32 int32Value;
        hint64 int64Value;
        hbool booleanValue;
        hdouble doubleValue;
        vmClassMetaOffset stringKey;
        vmClassMetaOffset objectKey;
        vmClassMetaOffset operatorOffset;
	}vmRuntimeMetaValue;

    typedef struct{
        vmRuntimeMetaType type;
        vmRuntimeMetaValue value;
    }vmRuntimeMeta;
    
    typedef struct{
        vmRuntimeOperatorType type;
        vmClassMetaOffset uniqueKey;
        hint32 metaCount;
    }vmRuntimeMetaOperator;

    
    
    typedef struct{
        vmClassType type;
        vmClassMetaOffset offset;
        vmClassMetaOffset superClass;
        hint32 propertys;
        hint32 functions;
    }vmClassMeta;
    

    vmVariant vmRuntimeContextGetVariant(vmRuntimeContext context,vmUniqueKey name,InvokeTickDeclare);
    
    void vmRuntimeContextSetVariant(vmRuntimeContext context,vmUniqueKey name,vmVariant value,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextRegisterUniqueKey(vmRuntimeContext context,hcchar * key,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextGetUniqueKey(vmRuntimeContext context,hcchar *key,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextGetUniqueKeyFromInteger(vmRuntimeContext context,hint32 key,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextVariantToUniqueKey(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    hcchar * vmRuntimeContextUniqueKeyToString(vmRuntimeContext context,vmUniqueKey uniqueKey,InvokeTickDeclare);
    
    hint32 vmRuntimeContextUniqueKeyToInteger(vmRuntimeContext context,vmUniqueKey uniqueKey,InvokeTickDeclare);
    
    vmClass * vmRuntimeContextGetClass(vmRuntimeContext context, vmUniqueKey className,InvokeTickDeclare);
    
    hcchar * vmRuntimeContextGetClassName(vmRuntimeContext context, vmClass * clazz,InvokeTickDeclare);
    
    vmClass * vmRuntimeContextGetSuperClass(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare);
    
    hany vmRuntimeContextGetObjectPtrByClass(vmRuntimeContext context,vmObject * object,vmClass * clazz,InvokeTickDeclare);
    
    hbool vmRuntimeContextObjectIsKindClass(vmRuntimeContext context,vmObject * object,vmClass * clazz,InvokeTickDeclare);
    
    hbool vmRuntimeContextClassIsKindClass(vmRuntimeContext context,vmClass * clazz,vmClass * ofClazz,InvokeTickDeclare);
    
    hint32 vmRuntimeContextGetClassTotalSize(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare);
    
    hint32 vmRuntimeContextGetClassSize(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextFunctionInitKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextFunctionDestroyKey(vmRuntimeContext context,InvokeTickDeclare);
     
    vmUniqueKey vmRuntimeContextFunctionToStringKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextFunctionToIntKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextFunctionToDoubleKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextThisKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextSuperKey(vmRuntimeContext context,InvokeTickDeclare);
    
    vmUniqueKey vmRuntimeContextArgumentsKey(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextDomainAddObject(vmRuntimeContext context,vmObject * obj,InvokeTickDeclare);
    
    
    HANDLER(vmRuntimeClassLoader)
    
    HANDLER(vmRuntimeDebug)
    
    vmRuntimeContext vmRuntimeContextAlloc(vmRuntimeClassLoader loader,InvokeTickDeclare);
    
    vmRuntimeContext vmRuntimeContextRetain(vmRuntimeContext context,InvokeTickDeclare);
    
    vmRuntimeClassLoader vmRuntimeContextClassLoader(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextRelease(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextInvokeBegin(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextInvokeEnd(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextVariantRetain(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    void vmRuntimeContextVariantRelease(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    vmVariant vmRuntimeContextException(vmRuntimeContext context,hint32 code,hcchar * message,InvokeTickDeclare);
    
    vmVariant vmRuntimeContextExceptionFormat(vmRuntimeContext context,hint32 code,hcchar * format,InvokeTickDeclare,...);
    
    vmVariant vmRuntimeContextUniqueKeyToVariant(vmRuntimeContext context,vmUniqueKey key,InvokeTickDeclare);
    
    vmVariant vmRuntimeContextVariantListToArrayObject(vmRuntimeContext context,vmVariantList vars,InvokeTickDeclare);
    
    vmRuntimeDebug vmRuntimeContextGetDebug(vmRuntimeContext context,InvokeTickDeclare);
    
    void vmRuntimeContextSetDebug(vmRuntimeContext context,vmRuntimeDebug debug,InvokeTickDeclare);
    
    
    typedef struct{
        vmClassMetaOffset className;
        vmClassMetaOffset classMeta;
    }vmRuntimeClassMetaBytes;
    
    typedef struct{
        struct{
            hchar tag[16];
            hchar version[16];
            hchar high;
            hchar low;
        }head;
        huint32 classCount;
        huint32 uniqueKeyCount;
        hubyte UDID[16];
        huint32 length;
    }vmRuntimeClassLibraryBytes;
    
    void vmRuntimeClassLibraryBytesInit(vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare);
    
    hbool vmRuntimeClassLibraryBytesValidate(vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare);
    
    void vmRuntimeClassLibraryBytesPrint(vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare);
    
    vmRuntimeClassLoader vmRuntimeClassLoaderAlloc(InvokeTickDeclare);

    vmRuntimeClassLoader vmRuntimeClassLoaderRetain(vmRuntimeClassLoader loader,InvokeTickDeclare);
    
    void vmRuntimeClassLoaderRelease(vmRuntimeClassLoader loader,InvokeTickDeclare);
    
    hint32 vmRuntimeClassLoaderClassNameCount(vmRuntimeClassLoader loader,InvokeTickDeclare);
    
    hcchar * vmRuntimeClassLoaderClassNameAt(vmRuntimeClassLoader loader,hint32 index,InvokeTickDeclare);
    
    vmClass * vmRuntimeClassLoaderGetClass(vmRuntimeClassLoader loader,hcchar * className,InvokeTickDeclare);
    
    hcchar * vmRuntimeClassLoaderGetClassName(vmRuntimeClassLoader loader,vmClass * vmClass,InvokeTickDeclare);
    
    vmClass * vmRuntimeClassLoaderGetMainClass(vmRuntimeClassLoader loader,InvokeTickDeclare);
    
    hbool vmRuntimeClassLoaderRegister(vmRuntimeClassLoader loader,hcchar * className,vmClassBase * clazz,InvokeTickDeclare);
    
    hbool vmRuntimeClassLoaderLoadBytes(vmRuntimeClassLoader loader,vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare);
    
    hbool vmRuntimeClassLoaderLoadBytesNotCopy(vmRuntimeClassLoader loader,vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare);
    
    hbool vmRuntimeClassLoaderLoadFile(vmRuntimeClassLoader loader,hcchar * file,InvokeTickDeclare);
    
   
#define vmRuntimeContextGetVariant(a,b) vmRuntimeContextGetVariant((a),(b),InvokeTickArg)
    
#define vmRuntimeContextSetVariant(a,b,c) vmRuntimeContextSetVariant((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeContextRegisterUniqueKey(a,b) vmRuntimeContextRegisterUniqueKey((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetUniqueKey(a,b) vmRuntimeContextGetUniqueKey((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetUniqueKeyFromInteger(a,b) vmRuntimeContextGetUniqueKeyFromInteger((a),(b),InvokeTickArg)
    
#define vmRuntimeContextVariantToUniqueKey(a,b) vmRuntimeContextVariantToUniqueKey((a),(b),InvokeTickArg)
    
#define vmRuntimeContextUniqueKeyToString(a,b) vmRuntimeContextUniqueKeyToString((a),(b),InvokeTickArg)
    
#define vmRuntimeContextUniqueKeyToInteger(a,b) vmRuntimeContextUniqueKeyToInteger((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetClass(a,b) vmRuntimeContextGetClass((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetClassName(a,b) vmRuntimeContextGetClassName((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetSuperClass(a,b) vmRuntimeContextGetSuperClass((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetObjectPtrByClass(a,b,c) vmRuntimeContextGetObjectPtrByClass((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeContextObjectIsKindClass(a,b,c) vmRuntimeContextObjectIsKindClass((a),(b),(c),InvokeTickArg)
 
#define vmRuntimeContextClassIsKindClass(a,b,c) vmRuntimeContextClassIsKindClass((a),(b),(c),InvokeTickArg)

#define vmRuntimeContextGetClassTotalSize(a,b) vmRuntimeContextGetClassTotalSize((a),(b),InvokeTickArg)
    
#define vmRuntimeContextGetClassSize(a,b) vmRuntimeContextGetClassSize((a),(b),InvokeTickArg)
    
#define vmRuntimeContextFunctionInitKey(a) vmRuntimeContextFunctionInitKey((a),InvokeTickArg)
    
#define vmRuntimeContextFunctionDestroyKey(a) vmRuntimeContextFunctionDestroyKey((a),InvokeTickArg)
    
#define vmRuntimeContextFunctionToStringKey(a) vmRuntimeContextFunctionToStringKey((a),InvokeTickArg)
    
#define vmRuntimeContextFunctionToIntKey(a) vmRuntimeContextFunctionToIntKey((a),InvokeTickArg)
    
#define vmRuntimeContextFunctionToDoubleKey(a) vmRuntimeContextFunctionToDoubleKey((a),InvokeTickArg)
    
#define vmRuntimeContextThisKey(a) vmRuntimeContextThisKey((a),InvokeTickArg)
    
#define vmRuntimeContextSuperKey(a) vmRuntimeContextSuperKey((a),InvokeTickArg)
    
#define vmRuntimeContextArgumentsKey(a) vmRuntimeContextArgumentsKey((a),InvokeTickArg)
    
#define vmRuntimeContextAlloc(a) vmRuntimeContextAlloc((a),InvokeTickArg)
    
#define vmRuntimeContextRetain(a) vmRuntimeContextRetain((a),InvokeTickArg)
    
#define vmRuntimeContextRelease(a) vmRuntimeContextRelease((a),InvokeTickArg)
    
#define vmRuntimeContextInvokeBegin(a) vmRuntimeContextInvokeBegin((a),InvokeTickArg)
    
#define vmRuntimeContextInvokeEnd(a) vmRuntimeContextInvokeEnd((a),InvokeTickArg)
    
#define vmRuntimeContextVariantRetain(a,b) vmRuntimeContextVariantRetain((a),(b),InvokeTickArg)
    
#define vmRuntimeContextVariantRelease(a,b) vmRuntimeContextVariantRelease((a),(b),InvokeTickArg)
    
#define vmRuntimeContextException(a,b,c) vmRuntimeContextException((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeContextUniqueKeyToVariant(a,b) vmRuntimeContextUniqueKeyToVariant((a),(b),InvokeTickArg)
    
#define vmRuntimeContextVariantListToArrayObject(a,b) vmRuntimeContextVariantListToArrayObject((a),(b),InvokeTickArg)

#define vmRuntimeContextDomainAddObject(a,b) vmRuntimeContextDomainAddObject((a),(b),InvokeTickArg)
    
#define vmRuntimeClassLoaderAlloc() vmRuntimeClassLoaderAlloc(InvokeTickArg)
 
#define vmRuntimeClassLoaderRetain(a) vmRuntimeClassLoaderRetain((a),InvokeTickArg)
    
#define vmRuntimeClassLoaderRelease(a) vmRuntimeClassLoaderRelease((a),InvokeTickArg)
    
#define vmRuntimeClassLoaderGetClass(a,b) vmRuntimeClassLoaderGetClass((a),(b),InvokeTickArg)

#define vmRuntimeClassLoaderGetClassName(a,b) vmRuntimeClassLoaderGetClassName((a),(b),InvokeTickArg)
    
#define vmRuntimeClassLoaderGetMainClass(a) vmRuntimeClassLoaderGetMainClass((a),InvokeTickArg)
    
#define vmRuntimeClassLoaderRegister(a,b,c) vmRuntimeClassLoaderRegister((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeClassLoaderLoadBytes(a,b,c) vmRuntimeClassLoaderLoadBytes((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeClassLoaderLoadBytesNotCopy(a,b,c) vmRuntimeClassLoaderLoadBytesNotCopy((a),(b),(c),InvokeTickArg)
    
#define vmRuntimeClassLoaderLoadFile(a,b) vmRuntimeClassLoaderLoadFile((a),(b),InvokeTickArg)



    
#ifdef __cplusplus
}
#endif


#endif
