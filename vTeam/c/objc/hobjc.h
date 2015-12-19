//
//  hobjc.h
//  hclib
//
//  Created by Zhang Hailong on 13-3-3.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef HOBJC_H
#define HOBJC_H

#ifdef __cplusplus
extern "C" {
#endif
   
    typedef hany hobj_imp_t;
    typedef huintptr hobj_hash_code_t;
    
    typedef struct _hobj_key_t{
        hcchar * READONLY name;
    } hobj_key_t;
    
    typedef struct _hobj_type_t{
        hcchar * READONLY name;
        huint32 READONLY size;
    } hobj_type_t;
    
    typedef struct _hobj_property_t{
        hobj_key_t * READONLY name;
        hobj_type_t * READONLY type;
        hobj_imp_t getter;
        hobj_imp_t setter;
        hbool isCoder;
    } hobj_property_t;
    
    typedef struct _hobj_method_t {
        hobj_key_t * READONLY name;
        hobj_imp_t imp;
    } hobj_method_t;
    
    typedef struct _hobj_class_t{
        struct _hobj_class_t * READONLY superClass;
        hobj_key_t * READONLY name;
        huint32 READONLY size;
        hobj_property_t * propertys;
        huint32 propertyCount;
        hobj_method_t * methods;
        huint16 methodCount;
        hbool initialized;
    } hobj_class_t;
    
    typedef struct _hobj_t{
        hobj_class_t * READONLY clazz;
        huint32 retainCount;
    } hobj_t;
    
    hobj_t * hobj_alloc(hobj_class_t * clazz,InvokeTickDeclare);
    
    hobj_t * hobj_new(hobj_class_t * clazz,InvokeTickDeclare);
    
    hobj_t * hobj_retain(hobj_t * obj,InvokeTickDeclare);
    
    void hobj_release(hobj_t * obj,InvokeTickDeclare);
    
    void hobj_autorelease_pool_begin(void);
    
    void hobj_autorelease_pool_end(void);
    
    hobj_t * hobj_autorelease(hobj_t * obj,InvokeTickDeclare);
    
    void hobj_class_initialize(hobj_class_t * clazz,InvokeTickDeclare);
    
    hobj_class_t * hobj_class_find(hcchar * className,InvokeTickDeclare);
    
    hobj_property_t * hobj_class_property(hobj_class_t * clazz,hobj_key_t * key,InvokeTickDeclare);
    
    hobj_property_t * hobj_class_property_for_name(hobj_class_t * clazz,hcchar * name,InvokeTickDeclare);
    
    hobj_method_t * hobj_class_method(hobj_class_t * clazz,hobj_key_t * key,InvokeTickDeclare);
    
    hobj_method_t * hobj_class_method_for_name(hobj_class_t * clazz,hcchar * name,InvokeTickDeclare);
    
    hobj_property_t * hobj_property(hobj_t * obj,hobj_key_t * key,InvokeTickDeclare);
    
    hobj_property_t * hobj_property_for_name(hobj_t * obj,hcchar * name,InvokeTickDeclare);
    
    hobj_method_t * hobj_method(hobj_t * obj,hobj_key_t * key,InvokeTickDeclare);
    
    hobj_method_t * hobj_method_for_name(hobj_t * obj,hcchar * name,InvokeTickDeclare);
    
    hbool hobj_is_kind(hobj_t * obj,hobj_class_t * clazz,InvokeTickDeclare);
    
    hbool hobj_class_is_kind(hobj_class_t * clazz,hobj_class_t * kindClazz,InvokeTickDeclare);
    
    hobj_t * hobj_propertyGet(hobj_t * obj,hobj_property_t * property,InvokeTickDeclare);
    
    void hobj_propertySet(hobj_t * obj,hobj_property_t * property,hobj_t * value,InvokeTickDeclare);
    
    hobj_t * hobj_valueForKey(hobj_t * obj,hcchar * key,InvokeTickDeclare);
    
    void hobj_setValueForKey(hobj_t * obj,hcchar * key,hobj_t * value,InvokeTickDeclare);
    
    hobj_t * hobj_valueForKeyPath(hobj_t * obj,hcchar * keyPath,InvokeTickDeclare);
    
    void hobj_setValueForKeyPath(hobj_t * obj,hcchar * keyPath,hobj_t * value,InvokeTickDeclare);
    
    hobj_t * hobj_toString(hobj_t * obj,InvokeTickDeclare);
    
    hcchar * hobj_toCString(hobj_t * obj,InvokeTickDeclare);
    
    hdouble hobj_toDouble(hobj_t * obj,InvokeTickDeclare);
    
    hobj_t * hobj_copy(hobj_t * obj,InvokeTickDeclare);
    
    void hobj_after_caller_pool_install_default(void);
    
    void hobj_after_caller_pool_install(void (* callback)(void * ,void (*)(hobj_t * ,hobj_t *) ,hobj_t *,hobj_t *),void * context);
    
    void hobj_after_caller_pool_async(void (*)(hobj_t * obj,hobj_t * value),hobj_t * obj,hobj_t * value,InvokeTickDeclare);
    
    void hobj_invoke(hobj_t * obj,hobj_method_t * method,hobj_t * userInfo,InvokeTickDeclare);
    
    
    void hobj_after_caller_install(void (*) (void * ,void *),void * context);
    
    void hobj_after_caller_async(void (*)(void *),void * userInfo);
    
    void hobj_invoke_after(hobj_t * obj,hobj_method_t * method,hobj_t * userInfo,InvokeTickDeclare);

    
    extern hobj_key_t hobj_key_initialize;
    
    extern hobj_key_t hobj_key_init;
    
    extern hobj_key_t hobj_key_destroy;
    
    extern hobj_key_t hobj_key_propertyGet;
    
    extern hobj_key_t hobj_key_propertySet;

    extern hobj_key_t hobj_key_valueForKey;
    
    extern hobj_key_t hobj_key_setValueForKey;
    
    extern hobj_key_t hobj_key_valueForKeyPath;
    
    extern hobj_key_t hobj_key_setValueForKeyPath;
    
    extern hobj_key_t hobj_key_toString;
    
    extern hobj_key_t hobj_key_toDouble;
    
    extern hobj_key_t hobj_key_copy;
    
    extern hobj_key_t hobj_key_Object;
    
    extern hobj_class_t hobj_class;
   
    typedef void (* hobj_method_init_impl_t) (hobj_t * obj,InvokeTickDeclare);
    
    typedef void (* hobj_method_destroy_impl_t) (hobj_t * obj,InvokeTickDeclare);
    
    typedef hobj_t * (* hobj_method_propertyGet_impl_t)(hobj_t * obj,hobj_property_t * property,InvokeTickDeclare);
    
    typedef void (* hobj_method_propertySet_impl_t)(hobj_t * obj,hobj_property_t * property,hobj_t * value,InvokeTickDeclare);
   
    typedef hobj_t * ( * hobj_method_valueForKey_imp_t)(hobj_t * obj,hcchar * key,InvokeTickDeclare);
    
    typedef void ( * hobj_method_setValueForKey_imp_t)(hobj_t * obj,hcchar * key,hobj_t * value,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_method_valueForKeyPath_imp_t)(hobj_t * obj,hcchar * keyPath,InvokeTickDeclare);
    
    typedef void ( * hobj_method_setValueForKeyPath_imp_t)(hobj_t * obj,hcchar * keyPath,hobj_t * value,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_method_toString_imp_t)(hobj_t * obj,InvokeTickDeclare);
    
    typedef hdouble ( * hobj_method_toDouble_imp_t)(hobj_t * obj,InvokeTickDeclare);
    
    typedef void (* hobj_class_initialize_imp_t)(hobj_class_t * clazz,InvokeTickDeclare);
    
    typedef hobj_t * ( * hobj_method_copy_imp_t)(hobj_t * obj,InvokeTickDeclare);
    
#define HOBJ_TYPE_DEC(T)        extern hobj_type_t hobj_type_##T;
#define HOBJ_TYPE(T)            (&hobj_type_##T)
#define HOBJ_TYPE_IMP(T,type)   hobj_type_t hobj_type_##T = {#type,sizeof(type)};
  
    
    HOBJ_TYPE_DEC(Integer)
    HOBJ_TYPE_DEC(Int64)
    HOBJ_TYPE_DEC(Double)
    HOBJ_TYPE_DEC(Boolean)
    HOBJ_TYPE_DEC(Float)
    HOBJ_TYPE_DEC(String)
    HOBJ_TYPE_DEC(Void)
    HOBJ_TYPE_DEC(Object)
    HOBJ_TYPE_DEC(Bytes)
    
#define HOBJ_TYPE_INTEGER   HOBJ_TYPE(Integer)
#define HOBJ_TYPE_INT64     HOBJ_TYPE(Int64)
#define HOBJ_TYPE_DOUBLE    HOBJ_TYPE(Double)
#define HOBJ_TYPE_BOOLEAN   HOBJ_TYPE(Boolean)
#define HOBJ_TYPE_FLOAT     HOBJ_TYPE(Float)
#define HOBJ_TYPE_STRING    HOBJ_TYPE(String)
#define HOBJ_TYPE_OBJECT    HOBJ_TYPE(Object) 
#define HOBJ_TYPE_BYTES     HOBJ_TYPE(Bytes)  
#define HOBJ_TYPE_VOID      HOBJ_TYPE(Void)
#define HOBJ_TYPE_NULL      HOBJ_TYPE_VOID
    
#define HOBJ_CLASS_DEC(clazz) extern hobj_class_t hobj_##clazz##_class;
    
#define HOBJ_CLASS_METHOD_IMP_BEGIN(clazz) static hobj_method_t hobj_##clazz##_class_methods[] = {

#define HOBJ_CLASS_METHOD_IMP(name,impl) {HOBJ_KEY(name),(hobj_imp_t)impl},
    
#define HOBJ_CLASS_METHOD_IMP_END(clazz) };
    
#define HOBJ_CLASS_PROPERTY_IMP_BEGIN(clazz) static hobj_property_t hobj_##clazz##_class_propertys[] = {
    
#define HOBJ_CLASS_PROPERTY_IMP(name,type,getter,setter) {HOBJ_KEY(name),type,(hobj_imp_t)getter,(hobj_imp_t)setter,hbool_false},
    
#define HOBJ_CLASS_PROPERTY_IMP_CODER(name,type,getter,setter) {HOBJ_KEY(name),type,(hobj_imp_t)getter,(hobj_imp_t)setter,hbool_true},
    
#define HOBJ_CLASS_PROPERTY_IMP_END(clazz) };
    
    
#define HOBJ_CLASS_IMP(clazz,obj,superClazz) hobj_class_t hobj_##clazz##_class = {superClazz,HOBJ_KEY(clazz),sizeof(obj),NULL,0,NULL,0};
    
#define HOBJ_CLASS_IMP_P(clazz,obj,superClazz) hobj_class_t hobj_##clazz##_class = {superClazz,HOBJ_KEY(clazz),sizeof(obj),hobj_##clazz##_class_propertys,sizeof(hobj_##clazz##_class_propertys) / sizeof(hobj_property_t),NULL,0,0};
    
#define HOBJ_CLASS_IMP_I(clazz,obj,superClazz) hobj_class_t hobj_##clazz##_class = {superClazz,HOBJ_KEY(clazz),sizeof(obj),NULL,0,hobj_##clazz##_class_methods,sizeof(hobj_##clazz##_class_methods) / sizeof(hobj_method_t),0};
    
#define HOBJ_CLASS_IMP_P_I(clazz,obj,superClazz) hobj_class_t hobj_##clazz##_class = {superClazz,HOBJ_KEY(clazz),sizeof(obj),hobj_##clazz##_class_propertys,sizeof(hobj_##clazz##_class_propertys) / sizeof(hobj_property_t),hobj_##clazz##_class_methods,sizeof(hobj_##clazz##_class_methods) / sizeof(hobj_method_t),0};
    
#define HOBJ_CLASS(clazz)   &hobj_##clazz##_class
  
#define HOBJPropertyValueGet(obj,property,T,I)  (*((T (*)(hobj_t * ,InvokeTickDeclare))(property)->getter))(obj,I)
#define HOBJPropertyValueSet(obj,property,T,value,I)  (((void (*)(hobj_t *,T,InvokeTickDeclare))(property)->setter))(obj,value,I)
    
    
#define HOBJ_KEY_DEC(key)   extern hobj_key_t hobj_key_##key;
#define HOBJ_KEY_IMP(key)   hobj_key_t hobj_key_##key = {#key};
#define HOBJ_KEY(key)       (&hobj_key_##key)
    
#ifdef __cplusplus
}
#endif

#endif
