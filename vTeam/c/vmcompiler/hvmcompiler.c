//
//  hvmcompiler.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-13.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hvmcompiler.h"
#include "hbuffer.h"
#include "hstr.h"
#include "hvmcompiler_class.h"
#include "hvmcompiler_tokenizer.h"
#include "hlog.h"

static void hobj_vmCompilerMetaOperator_destroy(vmCompilerMetaOperator * obj,InvokeTickDeclare);
static void hobj_vmCompilerMeta_destroy(vmCompilerMeta * obj,InvokeTickDeclare);
static void hobj_vmCompilerClassMeta_destroy(vmCompilerClassMeta * obj,InvokeTickDeclare);

static hobj_t * hobj_vmCompilerMetaOperator_copy(vmCompilerMetaOperator * obj,InvokeTickDeclare);
static hobj_t * hobj_vmCompilerMeta_copy(vmCompilerMeta * obj,InvokeTickDeclare);
static hobj_t * hobj_vmCompilerClassMeta_copy(vmCompilerClassMeta * obj,InvokeTickDeclare);

HOBJ_KEY_IMP(vmCompilerMetaOperator)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmCompilerMetaOperator)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_vmCompilerMetaOperator_destroy)

HOBJ_CLASS_METHOD_IMP(copy, hobj_vmCompilerMetaOperator_copy)

HOBJ_CLASS_METHOD_IMP_END(vmCompilerMetaOperator)

HOBJ_CLASS_IMP_I(vmCompilerMetaOperator, vmCompilerMetaOperator, &hobj_class)


HOBJ_KEY_IMP(vmCompilerMeta)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmCompilerMeta)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_vmCompilerMeta_destroy)

HOBJ_CLASS_METHOD_IMP(copy, hobj_vmCompilerMeta_copy)

HOBJ_CLASS_METHOD_IMP_END(vmCompilerMeta)

HOBJ_CLASS_IMP_I(vmCompilerMeta, vmCompilerMeta, &hobj_class)


HOBJ_KEY_IMP(vmCompilerClassMeta)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmCompilerClassMeta)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_vmCompilerClassMeta_destroy)

HOBJ_CLASS_METHOD_IMP(copy, hobj_vmCompilerClassMeta_copy)

HOBJ_CLASS_METHOD_IMP_END(vmCompilerClassMeta)

HOBJ_CLASS_IMP_I(vmCompilerClassMeta, vmCompilerClassMeta, &hobj_class)


HOBJ_KEY_IMP(vmCompilerError)

HOBJ_CLASS_IMP(vmCompilerError,vmCompilerError,&hobj_class)


vmCompilerMetaOperator * vmCompilerMetaOperatorNew(vmRuntimeOperatorType type,htokenizer_location_t location,huint32 length,InvokeTickDeclare){
    vmCompilerMetaOperator * op = (vmCompilerMetaOperator *) hobj_new(HOBJ_CLASS(vmCompilerMetaOperator), InvokeTickArg);
    op->type = type;
    op->uniqueKey.location = location;
    op->uniqueKey.length = length;
    return op;
}

void vmCompilerMetaOperatorAddCompilerMeta(vmCompilerMetaOperator * op,vmCompilerMeta * compilerMeta,InvokeTickDeclare){
    if(op->metas ==NULL){
        op->metas = hobj_array_alloc(InvokeTickArg);
    }
    hobj_array_add(op->metas, (hobj_t *) compilerMeta, InvokeTickArg);
}

vmCompilerMeta * vmCompilerMetaNew(InvokeTickDeclare){
    return (vmCompilerMeta *) hobj_new(HOBJ_CLASS(vmCompilerMeta), InvokeTickArg);
}

vmCompilerMeta * vmCompilerMetaNewWithInt16(hint16 value,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt16;
    meta->int16Value = value;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithInt32(hint32 value,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt32;
    meta->int32Value = value;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithInt64(hint64 value,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt64;
    meta->int64Value = value;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithBoolean(hbool value,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeBoolean;
    meta->booleanValue = value;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithDouble(hdouble value,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeDouble;
    meta->doubleValue = value;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithString(htokenizer_location_t location,huint32 length,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeString;
    meta->stringValue.location = location;
    meta->stringValue.length = length;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithObjectKey(htokenizer_location_t location,huint32 length,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeObject;
    meta->stringValue.location = location;
    meta->stringValue.length = length;
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithOperator(vmCompilerMetaOperator * op,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeOperator;
    meta->operatorMeta = (vmCompilerMetaOperator *)hobj_retain((hobj_t *)op, InvokeTickArg);
    return meta;
}


vmCompilerMeta * vmCompilerMetaNewWithNumberString(htokenizer_location_t location,huint32 length,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    hbuffer_t buf = buffer_alloc(128, 128);
    hdouble doubleValue ;
    hint64 int64Value;
    hint32 int32Value;
    hint16 int16Value;
    hchar * value;
    
    buffer_append(buf, location.p, length);
    
    value = (hchar *)buffer_to_str(buf);
    
    doubleValue = atof(buffer_to_str(buf));
    int64Value = (hint64)doubleValue;
    int32Value = (hint32)doubleValue;
    int16Value = (hint16)doubleValue;
    
    if(value && value[0] == '0' && value[1] == 'x'){
        sscanf(value, "%llx",&int64Value);
        if(int64Value == (hint64)(hint32)int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->int32Value = (hint32)int64Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->int64Value = int64Value;
        }
    }
    else if(value && value[0] == '0' && value[1] != '.'){
        sscanf(value, "%llo",&int64Value);
        if(int64Value == (hint64)(hint32)int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->int32Value = (hint32)int64Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->int64Value = int64Value;
        }
    }
    else if(str_exist(value, ".") ||str_exist(value, "e")){
        meta->type = vmRuntimeMetaTypeDouble;
        meta->doubleValue = doubleValue;
    }
    else if(doubleValue == (hdouble)int64Value){
        if(int16Value == int64Value){
            meta->type = vmRuntimeMetaTypeInt16;
            meta->int16Value = int16Value;
        }
        else if(int32Value == int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->int32Value = int32Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->int64Value = int64Value;
        }
    }
    else{
        meta->type = vmRuntimeMetaTypeDouble;
        meta->doubleValue = doubleValue;
    }
    
    buffer_dealloc(buf);
    
    return meta;
}

vmCompilerMeta * vmCompilerMetaNewWithArg(htokenizer_location_t location,huint32 length,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeArg;
    meta->stringValue.location = location;
    meta->stringValue.length = length;
    
    return meta;
}

vmCompilerClassMeta * vmCompilerClassMetaNew(InvokeTickDeclare){
    vmCompilerClassMeta * meta = (vmCompilerClassMeta *) hobj_new(HOBJ_CLASS(vmCompilerClassMeta), InvokeTickArg);
    return meta;
}


vmCompilerError * vmCompilerErrorNew(htokenizer_location_t location,hcchar * error,InvokeTickDeclare){
    vmCompilerError * e = (vmCompilerError *) hobj_new(HOBJ_CLASS(vmCompilerError), InvokeTickArg);
    e->location = location;
    strncpy(e->error, error, sizeof(e->error));
    return e;
}


void vmCompilerClassAddProperty(vmCompilerClassMeta * classMeta,vmCompilerMetaOperator * op,InvokeTickDeclare){
    if(classMeta->propertys == NULL){
        classMeta->propertys = hobj_array_alloc(InvokeTickArg);
    }
    hobj_array_add(classMeta->propertys, (hobj_t *) op, InvokeTickArg);
}

void vmCompilerClassAddFunction(vmCompilerClassMeta * classMeta,vmCompilerMetaOperator * op,InvokeTickDeclare){
    if(classMeta->functions == NULL){
        classMeta->functions = hobj_array_alloc(InvokeTickArg);
    }
    hobj_array_add(classMeta->functions, (hobj_t *) op, InvokeTickArg);
}


static void vmCompilerStringLog(vmCompilerString string,InvokeTickDeclare){
    hint32 l = string.length;
    hchar * p = string.location.p;
    
    while(l >0){
        hlog("%c",*p);
        l --;
        p++;
    }
}

void vmCompilerMetaOperatorLog(vmCompilerMetaOperator * metaOperator,InvokeTickDeclare){
    vmCompilerStringLog(metaOperator->uniqueKey,InvokeTickArg);
    hlog("\n");
}

void vmCompilerClassMetaLog(vmCompilerClassMeta * classMeta,InvokeTickDeclare){
    hint32 i,c;
    vmCompilerMetaOperator * op;
    hlog("extends ");
    
    vmCompilerStringLog(classMeta->superClass,InvokeTickArg);
    
    hlog("\n");
    
    hlog("propertys:\n");
    
    c = hobj_array_count(classMeta->propertys, InvokeTickArg);
    
    for(i=0;i<c;i++){
        op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->propertys, i, InvokeTickArg);
        vmCompilerMetaOperatorLog(op,InvokeTickArg);
    }
    
    hlog("functions:\n");
    c = hobj_array_count(classMeta->functions, InvokeTickArg);
    
    for(i=0;i<c;i++){
        op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->functions, i, InvokeTickArg);
        vmCompilerMetaOperatorLog(op,InvokeTickArg);
    }
}



// private


static void hobj_vmCompilerMetaOperator_destroy(vmCompilerMetaOperator * obj,InvokeTickDeclare){
    
    hobj_method_t * method;
    
    hobj_release((hobj_t *)obj->metas, InvokeTickArg);
    
    method = hobj_class_method(obj->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *)obj,InvokeTickArg);
    }
    
}

static void hobj_vmCompilerMeta_destroy(vmCompilerMeta * obj,InvokeTickDeclare){
    
    hobj_method_t * method;
    
    if(obj->type & vmRuntimeMetaTypeOperator){
        hobj_release((hobj_t *)obj->operatorMeta, InvokeTickArg);
    }
    
    method = hobj_class_method(obj->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *)obj,InvokeTickArg);
    }
}

static void hobj_vmCompilerClassMeta_destroy(vmCompilerClassMeta * obj,InvokeTickDeclare){
    
    hobj_method_t * method;
    
    hobj_release((hobj_t *)obj->functions, InvokeTickArg);
    hobj_release((hobj_t *)obj->propertys, InvokeTickArg);
    
    method = hobj_class_method(obj->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *)obj,InvokeTickArg);
    }
    
}

static hobj_t * hobj_vmCompilerMetaOperator_copy(vmCompilerMetaOperator * obj,InvokeTickDeclare){
    vmCompilerMetaOperator * op = vmCompilerMetaOperatorNew(obj->type, obj->uniqueKey.location, obj->uniqueKey.length, InvokeTickArg);
    vmCompilerMeta * meta;
    hint32 i,c = hobj_array_count(obj->metas, InvokeTickArg);
    for(i=0;i<c;i++){
        meta = (vmCompilerMeta *) hobj_array_objectAt(obj->metas, i, InvokeTickArg);
        vmCompilerMetaOperatorAddCompilerMeta(op, (vmCompilerMeta *) hobj_copy((hobj_t *) meta, InvokeTickArg), InvokeTickArg);
    }
    return (hobj_t *) op;
}

static hobj_t * hobj_vmCompilerMeta_copy(vmCompilerMeta * obj,InvokeTickDeclare){
    vmCompilerMeta * meta = vmCompilerMetaNew(InvokeTickArg);
    meta->type = obj->type;
    if(meta->type & vmRuntimeMetaTypeOperator){
        meta->operatorMeta = (vmCompilerMetaOperator *) hobj_retain(hobj_copy((hobj_t *)meta->operatorMeta, InvokeTickArg), InvokeTickArg) ;
    }
    else{
        memcpy(&meta->objectKey ,& obj->objectKey,sizeof(obj->objectKey));
    }
    return (hobj_t *)meta;
}

static hobj_t * hobj_vmCompilerClassMeta_copy(vmCompilerClassMeta * obj,InvokeTickDeclare){
    vmCompilerClassMeta * m = vmCompilerClassMetaNew(InvokeTickArg);
    vmCompilerMetaOperator * op;
    hint32 i,c;
    m->superClass = obj->superClass;
    
    c = hobj_array_count(obj->propertys, InvokeTickArg);
    
    for(i=0;i<c;i++){
        op = (vmCompilerMetaOperator *) hobj_copy( hobj_array_objectAt(obj->propertys, i, InvokeTickArg) ,InvokeTickArg);
        vmCompilerClassAddProperty(m, op, InvokeTickArg);
    }
    
    c = hobj_array_count(obj->functions, InvokeTickArg);
    
    for(i=0;i<c;i++){
        op = (vmCompilerMetaOperator *) hobj_copy( hobj_array_objectAt(obj->functions, i, InvokeTickArg) ,InvokeTickArg);
        vmCompilerClassAddFunction(m, op, InvokeTickArg);
    }
    
    return (hobj_t *)m;
}

