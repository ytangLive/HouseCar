//
//  hvermin.h
//  C Library
//
//  Created by hailong zhang on 11-6-3.
//  Copyright 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_H
#define _HVERMIN_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef huintptr vmUniqueKey;
    
    typedef struct {
        vmUniqueKey functionInitKey;
        vmUniqueKey functionDestroyKey;
        vmUniqueKey functionSettedKey;
        vmUniqueKey functionToStringKey;
        vmUniqueKey functionToIntKey;
        vmUniqueKey functionToDoubleKey;
        vmUniqueKey functionEqualKey;
        vmUniqueKey thisKey;
        vmUniqueKey superKey;
        vmUniqueKey argumentsKey;
        vmUniqueKey classNameKey;
        vmUniqueKey valueForKey;
        vmUniqueKey setValueForKey;
        vmUniqueKey valueForKeyPath;
        vmUniqueKey setValueForKeyPath;
        vmUniqueKey propertySetted;
        vmUniqueKey functionToTextContentKey;
    }vmRuntimeSystemKeys;
    
    typedef struct __vmRuntimeContext{
        vmRuntimeSystemKeys systemKeys;
    } * vmRuntimeContext;
    
    HANDLER(vmUniqueKeyList)

    vmUniqueKeyList vmUniqueKeyListAlloc(InvokeTickDeclare);
    
    void vmUniqueKeyListDealloc(vmUniqueKeyList list,InvokeTickDeclare);
    
    hint32 vmUniqueKeyListCount(vmUniqueKeyList,InvokeTickDeclare);
    
    vmUniqueKey vmUniqueKeyListGet(vmUniqueKeyList list,hint32 index,InvokeTickDeclare);
    
    void vmUniqueKeyListAdd(vmUniqueKeyList list,vmUniqueKey key,InvokeTickDeclare);
    
    void vmUniqueKeyListRemoveAt(vmUniqueKeyList list,hint32 index,InvokeTickDeclare);
    
    void vmUniqueKeyListRemove(vmUniqueKeyList list,vmUniqueKey key,InvokeTickDeclare);
    
    void vmUniqueKeyListClear(vmUniqueKeyList list,InvokeTickDeclare);
    
    
#define vmUniqueKeyListAlloc() vmUniqueKeyListAlloc(InvokeTickArg)
    
#define vmUniqueKeyListDealloc(a) vmUniqueKeyListDealloc((a),InvokeTickArg)
    
#define vmUniqueKeyListCount(a) vmUniqueKeyListCount((a),InvokeTickArg)
    
#define vmUniqueKeyListGet(a,b) vmUniqueKeyListGet((a),(b),InvokeTickArg)
    
#define vmUniqueKeyListAdd(a,b) vmUniqueKeyListAdd((a),(b),InvokeTickArg)
    
#define vmUniqueKeyListRemoveAt(a,b) vmUniqueKeyListRemoveAt((a),(b),InvokeTickArg)
    
#define vmUniqueKeyListRemove(a,b) vmUniqueKeyListRemove((a),(b),InvokeTickArg)
    
#define vmUniqueKeyListClear(a) vmUniqueKeyListClear((a),InvokeTickArg)
    
    
    typedef enum{
        vmClassTypeBase,vmClassTypeMeta
    }vmClassType;
    
    
    typedef struct _vmClass{
        vmClassType type;
    }vmClass;
    
    typedef struct _vmObject{
        vmClass * vmClass;
        hint32 retainCount;
    }vmObject;
    
    typedef enum{
        vmVariantTypeVoid
        ,vmVariantTypeInt16 = 1<<0
        ,vmVariantTypeInt32 = 1<<1
        ,vmVariantTypeInt64 = 1<<2
        ,vmVariantTypeBoolean = 1<<3
        ,vmVariantTypeDouble = 1<<4
        ,vmVariantTypeObject = 1<<5
        ,vmVariantTypeString = 1<<6
        ,vmVariantTypeThrowable = 1<<7
    }vmVariantType;
    
    typedef struct _vmVariant{
        vmVariantType type;
        union{
            hint16 int16Value;
            hint32 int32Value;
            hint64 int64Value;
            hbool booleanValue;
            hdouble doubleValue;
            hcchar * stringValue;
            vmObject * objectValue;
        } value;
        vmClass * vmClass;
    }vmVariant;
    
    
    HANDLER(vmVariantList)
    
    vmVariantList vmVariantListAlloc(InvokeTickDeclare);
    
    void vmVariantListDealloc(vmVariantList list,InvokeTickDeclare);
    
    hint32 vmVariantListCount(vmVariantList,InvokeTickDeclare);
    
    vmVariant vmVariantListGet(vmVariantList,hint32 index,InvokeTickDeclare);
    
    void vmVariantListSet(vmVariantList,hint32 index,vmVariant value,InvokeTickDeclare);
    
    void vmVariantListAdd(vmVariantList list,vmVariant value,InvokeTickDeclare);
    
    void vmVariantListRemoveAt(vmVariantList list,hint32 index,InvokeTickDeclare);
    
    void vmVariantListClear(vmVariantList list,InvokeTickDeclare);

    void vmVariantListInsert(vmVariantList list,hint32 index,vmVariant value,InvokeTickDeclare);
    
    vmVariant vmVariantListLast(vmVariantList list ,InvokeTickDeclare);
    
    void vmVariantListRemoveLast(vmVariantList list,InvokeTickDeclare);
    
#define vmVariantListAlloc() vmVariantListAlloc(InvokeTickArg)
    
#define vmVariantListDealloc(a) vmVariantListDealloc((a),InvokeTickArg)
    
#define vmVariantListCount(a) vmVariantListCount((a),InvokeTickArg)
    
#define vmVariantListGet(a,b) vmVariantListGet((a),(b),InvokeTickArg)
    
#define vmVariantListSet(a,b,c) vmVariantListSet((a),(b),(c),InvokeTickArg)
    
#define vmVariantListAdd(a,b) vmVariantListAdd((a),(b),InvokeTickArg)
    
#define vmVariantListRemoveAt(a,b) vmVariantListRemoveAt((a),(b),InvokeTickArg)
    
#define vmVariantListClear(a) vmVariantListClear((a),InvokeTickArg)
   
#define vmVariantListInsert(a,b,c) vmVariantListInsert((a),(b),(c),InvokeTickArg)
    
#define vmVariantListLast(a) vmVariantListLast((a),InvokeTickArg)
    
#define vmVariantListRemoveLast(a) vmVariantListRemoveLast((a),InvokeTickArg)
    
    /**
     
     Return : Void or Throwable
     */
    
    typedef vmVariant (*vmClassInitCallback)(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare);
    
    /**
     
     Return : Void or Throwable
     */
    
    typedef vmVariant (*vmClassDestroyCallback)(vmRuntimeContext context, vmObject * object,InvokeTickDeclare);

    /**
     
     Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
     */
    typedef vmVariant (*vmClassGetPropertyCallback)(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare);
    
    
    /**
     
     Return : Void or Throwable
     */
    typedef vmVariant (*vmClassSetPropertyCallback)(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare);
    
    /**
    
     Return : any
     */
    typedef vmVariant (*vmClassInvokeCallback)(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare);
    
    /**
     
     Return : Void
     */
    typedef void (*vmClassPropertyNamesCallback)(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare);

    typedef struct _vmClassBase{
        vmClassType type;
        hint32 size;
        vmClassInitCallback init;
        vmClassDestroyCallback destroy;
        vmClassGetPropertyCallback getProperty;
        vmClassSetPropertyCallback setProperty;
        vmClassPropertyNamesCallback propertyNames;
        vmClassInvokeCallback invoke;
    }vmClassBase;
    
    extern vmClassBase vmObjectClass;
    extern vmClassBase vmStringClass;
    extern vmClassBase vmNumberClass;
    extern vmClassBase vmExceptionClass;
    extern vmClassBase vmArrayClass;
    extern vmClassBase vmDictionaryClass;
    
    /**
     
     Return : Object or Throwable
     */
    vmVariant vmObjectAlloc(vmRuntimeContext context,vmClass * vmClass,vmVariantList args,InvokeTickDeclare);
    
    
    vmObject * vmObjectRetain(vmRuntimeContext context,vmObject * vmObject,InvokeTickDeclare);
    
    /**
     
     Return : Void or Throwable
     */
    vmVariant vmObjectRelease(vmRuntimeContext context,vmObject * vmObject,InvokeTickDeclare);
    
    

    /**
     
     Return : Int16,Int32,Int64,Boolean,Double,Object or Throwable
     */
    vmVariant vmObjectGetProperty(vmRuntimeContext context,vmClass * vmClass,vmObject * vmObject,vmUniqueKey name,InvokeTickDeclare);
    
    
    /**
     value : Not Void ,Not Throwable
     Return : Void or Throwable
     */
    vmVariant vmObjectSetProperty(vmRuntimeContext context,vmClass * vmClass,vmObject * vmObject,vmUniqueKey name,vmVariant value,InvokeTickDeclare);
    
    /**
     
     Return : Void 
     */
    void vmObjectPropertyNames(vmRuntimeContext context,vmClass * vmClass,vmObject * vmObject,vmUniqueKeyList names,InvokeTickDeclare);
    
    
    /**
     
     Return : any
     */
    vmVariant vmObjectInvoke(vmRuntimeContext context,vmClass * vmClass,vmObject * vmObject,vmUniqueKey name, vmVariantList args,InvokeTickDeclare);
    
    
    

#define vmObjectAlloc(a,b,c) vmObjectAlloc((a),(vmClass *)(b),(c),InvokeTickArg)
    
#define vmObjectRetain(a,b) vmObjectRetain((a),(b),InvokeTickArg)
    
#define vmObjectRelease(a,b) vmObjectRelease((a),(b),InvokeTickArg)
    
#define vmObjectGetProperty(a,b,c,d) vmObjectGetProperty((a),(b),(c),(d),InvokeTickArg)
    
#define vmObjectSetProperty(a,b,c,d,e) vmObjectSetProperty((a),(b),(c),(d),(e),InvokeTickArg)
    
#define vmObjectPropertyNames(a,b,c,d) vmObjectPropertyNames((a),(b),(c),(d),InvokeTickArg)
    
#define vmObjectInvoke(a,b,c,d,e) vmObjectInvoke((a),(b),(c),(d),(e),InvokeTickArg)
    
    
#ifdef __cplusplus
}
#endif


#endif
