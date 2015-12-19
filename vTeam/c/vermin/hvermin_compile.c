//
//  hvermin_compile.c
//  C Library
//
//  Created by hailong zhang on 11-6-8.
//  Copyright 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_compile.h"
#include "hbuffer.h"
#include "hlog.h"
#include "hstr.h"

#undef  vmCompileMetaCreate

#undef  vmCompileMetaCreateWithInt16

#undef vmCompileMetaCreateWithInt32

#undef vmCompileMetaCreateWithInt64

#undef vmCompileMetaCreateWithBoolean

#undef vmCompileMetaCreateWithDouble

#undef vmCompileMetaCreateWithString

#undef vmCompileMetaCreateWithObjectKey

#undef vmCompileMetaCreateWithOperator


#undef vmCompileMetaPrint

#undef vmCompileMetaCreateWithNumberString

#undef vmCompileMetaCreateWithArg

#undef vmCompileMetaOperatorCreate

#undef vmCompileMetaOperatorAddCompileMeta


#undef vmCompileObjectCreate
#undef vmCompileObjectRetain
#undef vmCompileObjectRelease

#undef vmCompileObjectArrayCreate
#undef vmCompileObjectArrayAdd
#undef vmCompileObjectArrayRemove
#undef vmCompileObjectArrayRemoveAt
#undef vmCompileObjectArrayRemoveLast
#undef vmCompileObjectArrayGet
#undef vmCompileObjectArrayLast
#undef vmCompileObjectArrayCount
#undef vmCompileObjectArrayRemoveAll

#undef vmCompileClassMetaCreate

#define vmCompileObjectArrayLengthExtendSize 20

vmCompileObject * vmCompileObjectCreate(hint32 size,vmCompileObjectDealloc dealloc,InvokeTickDeclare){
    vmCompileObject * obj = mem_malloc(size);
    mem_memset(obj, 0, size);
    obj->retainCount = 1;
    obj->dealloc = dealloc;
    return obj;
}

vmCompileObject * vmCompileObjectRetain( vmCompileObject * obj,InvokeTickDeclare){
    if(obj){
        obj->retainCount ++;
    }
    return obj;
}

void vmCompileObjectRelease(vmCompileObject * obj,InvokeTickDeclare){
    if(obj && --obj->retainCount ==0){
        if(obj->dealloc){
            (*obj->dealloc)(obj,InvokeTickArg);
        }
        mem_free(obj);
    }
}

static void _vmCompileObjectArrayDealloc(vmCompileObjectArray * array,InvokeTickDeclare){
    vmCompileObjectArrayRemoveAll(array,InvokeTickArg);
    if(array->items){
        mem_free(array->items);
        array->items =NULL;
    }
    array->length = 0;
    array->count = 0;
}

vmCompileObjectArray * vmCompileObjectArrayCreate(InvokeTickDeclare){
    return (vmCompileObjectArray *) vmCompileObjectCreate(sizeof(vmCompileObjectArray),(vmCompileObjectDealloc)_vmCompileObjectArrayDealloc,InvokeTickArg);
}

static void _vmCompileObjectArrayLengthExtend(vmCompileObjectArray * array,hint32 length,InvokeTickDeclare){
    if(array->length < length){
        hint32 size = array->length + vmCompileObjectArrayLengthExtendSize < length ? length : array->length + vmCompileObjectArrayLengthExtendSize;
        if(array->items ){
            array->items = mem_realloc(array->items, sizeof(vmCompileObject *) * size);
        }
        else{
            array->items = mem_malloc(sizeof(vmCompileObject *) * size);
        }
        array->length = size;
    }
}

void vmCompileObjectArrayAdd(vmCompileObjectArray * array,vmCompileObject * obj,InvokeTickDeclare){
    _vmCompileObjectArrayLengthExtend(array,array->count +1,InvokeTickArg);
    array->items[array->count ++] = vmCompileObjectRetain(obj, InvokeTickArg);
}

void vmCompileObjectArrayRemove(vmCompileObjectArray * array,vmCompileObject * obj,InvokeTickDeclare){
    hint32 i;
    for(i=0;i<array->count;i++){
        if(array->items[i] == obj){
            vmCompileObjectArrayRemoveAt(array,i,InvokeTickArg);
            break;
        }
    }
}

void vmCompileObjectArrayRemoveAt(vmCompileObjectArray * array,hint32 index,InvokeTickDeclare){
    hint32 i;
    if(index >=0 && index<array->count){
        vmCompileObjectRelease(array->items[index], InvokeTickArg);
        for(i = index;i<array->count -1;i++){
            array->items[i] = array->items[i+1];
        }
        array->count -- ;
    }
}

void vmCompileObjectArrayRemoveLast(vmCompileObjectArray * array,InvokeTickDeclare){
    vmCompileObjectArrayRemoveAt(array,array->count -1,InvokeTickArg);
}

void vmCompileObjectArrayRemoveAll(vmCompileObjectArray * array,InvokeTickDeclare){
    hint32 i;

    for(i = 0;i<array->count;i++){
        vmCompileObjectRelease(array->items[i],InvokeTickArg);
    }
    array->count = 0;

}

vmCompileObject * vmCompileObjectArrayGet(vmCompileObjectArray * array,hint32 i,InvokeTickDeclare){
    if(i >=0 && i<array->count){
        return array->items[i];
    }
    return NULL;
}

vmCompileObject * vmCompileObjectArrayLast(vmCompileObjectArray * array,InvokeTickDeclare){
    return vmCompileObjectArrayGet(array,array->count -1,InvokeTickArg);
}

hint32 vmCompileObjectArrayCount(vmCompileObjectArray * array,InvokeTickDeclare){
    return array->count;
}

static void _vmCompileMetaOperatorDealloc(vmCompileMetaOperator *op ,InvokeTickDeclare){
    if(op){
        vmCompileObjectRelease((vmCompileObject *)op->compileMetas, InvokeTickArg);
        op->compileMetas = NULL;
        buffer_dealloc(op->uniqueKey);
        op->uniqueKey = NULL;
    }
}

vmCompileMetaOperator * vmCompileMetaOperatorCreate(vmRuntimeOperatorType type, hcchar * uniqueKey,InvokeTickDeclare){
    vmCompileMetaOperator * op = (vmCompileMetaOperator *)vmCompileObjectCreate(sizeof(vmCompileMetaOperator), (vmCompileObjectDealloc)_vmCompileMetaOperatorDealloc, InvokeTickArg);
    op->type = type;
    op->uniqueKey = buffer_alloc(20, 128);
    op->compileMetas = vmCompileObjectArrayCreate(InvokeTickArg);
    buffer_append_str(op->uniqueKey, uniqueKey);
    return op;
}


void vmCompileMetaOperatorAddCompileMeta(vmCompileMetaOperator * op, vmCompileMeta * compileMeta,InvokeTickDeclare){
    if(op && compileMeta){
        vmCompileObjectArrayAdd(op->compileMetas, (vmCompileObject *)compileMeta, InvokeTickArg);
    }
}

static void vmCompileMetaDealloc(vmCompileMeta *  meta,InvokeTickDeclare){

    if(meta->type & vmRuntimeMetaTypeString){
        buffer_dealloc(meta->value.stringValue);
    }
    else if(meta->type & vmRuntimeMetaTypeObject || meta->type & vmRuntimeMetaTypeArg){
        buffer_dealloc(meta->value.objectKey);
    }
    else if(meta->type & vmRuntimeMetaTypeOperator){
        vmCompileObjectRelease((vmCompileObject *)meta->value.operatorMeta, InvokeTickArg);
    }

}

vmCompileMeta * vmCompileMetaCreate(InvokeTickDeclare){
    return (vmCompileMeta *)vmCompileObjectCreate(sizeof(vmCompileMeta), (vmCompileObjectDealloc)vmCompileMetaDealloc, InvokeTickArg); 
}

vmCompileMeta * vmCompileMetaCreateWithInt16(hint16 value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt16;
    meta->value.int16Value = value;
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithInt32(hint32 value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt32;
    meta->value.int32Value = value;
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithInt64(hint64 value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeInt64;
    meta->value.int64Value = value;
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithBoolean(hbool value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeBoolean;
    meta->value.booleanValue = value;
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithDouble(hdouble value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeDouble;
    meta->value.doubleValue = value;
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithString(hcchar * value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);        
    meta->type = vmRuntimeMetaTypeString;
    meta->value.stringValue = buffer_alloc(32, 128);
    buffer_append_str(meta->value.stringValue, value);
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithObjectKey(hcchar * objectKey,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);        
    meta->type = vmRuntimeMetaTypeObject;
    meta->value.objectKey = buffer_alloc(32, 128);
    buffer_append_str(meta->value.objectKey, objectKey);
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithOperator(vmCompileMetaOperator * op,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    assert(op);
    meta->type = vmRuntimeMetaTypeOperator;
    meta->value.operatorMeta = (vmCompileMetaOperator *) vmCompileObjectRetain((vmCompileObject *)op, InvokeTickArg);
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithNumberString(hcchar * value,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);
    hdouble doubleValue = atof(value);
    hint64 int64Value = (hint64)doubleValue;
    hint32 int32Value = (hint32)doubleValue;
    hint16 int16Value = (hint16)doubleValue;
    if(value && value[0] == '0' && value[1] == 'x'){
        sscanf(value, "%llx",&int64Value);
        if(int64Value == (hint64)(hint32)int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->value.int32Value = (hint32)int64Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->value.int64Value = int64Value;
        }
    }
    else if(value && value[0] == '0' && value[1] != '.'){
        sscanf(value, "%llo",&int64Value);
        if(int64Value == (hint64)(hint32)int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->value.int32Value = (hint32)int64Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->value.int64Value = int64Value;
        }
    }
    else if(str_exist(value, ".") ||str_exist(value, "e")){
        meta->type = vmRuntimeMetaTypeDouble;
        meta->value.doubleValue = doubleValue;
    }
    else if(doubleValue == (hdouble)int64Value){
        if(int16Value == int64Value){
            meta->type = vmRuntimeMetaTypeInt16;
            meta->value.int16Value = int16Value;
        }
        else if(int32Value == int64Value){
            meta->type = vmRuntimeMetaTypeInt32;
            meta->value.int32Value = int32Value;
        }
        else{
            meta->type = vmRuntimeMetaTypeInt64;
            meta->value.int64Value = int64Value;
        }
    }
    else{
        meta->type = vmRuntimeMetaTypeDouble;
        meta->value.doubleValue = doubleValue;
    }
    return meta;
}

vmCompileMeta * vmCompileMetaCreateWithArg(hcchar * arg,InvokeTickDeclare){
    vmCompileMeta * meta = vmCompileMetaCreate(InvokeTickArg);        
    meta->type = vmRuntimeMetaTypeArg;
    meta->value.objectKey = buffer_alloc(32, 128);
    buffer_append_str(meta->value.objectKey, arg);
    return meta;
}


static void vmCompileMetaOperatorArgumentsPrint(vmCompileObjectArray * args,hint32 form,InvokeTickDeclare){
    hint32 i,c;
    c = vmCompileObjectArrayCount(args,InvokeTickArg);
    for(i=form;i<c;i++){
        if(i != form){
            hlog(",");
        }
        vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(args, i,InvokeTickArg),InvokeTickArg);
    }
}

static void vmCompileMetaOperatorPrint(vmCompileMetaOperator * op,InvokeTickDeclare){
    vmCompileMeta * pmeta;
    hlog("( ");

    switch (op->type) {
        case vmRuntimeOperatorTypeNew:
            hlog("new %s(",buffer_to_str(op->uniqueKey));
            vmCompileMetaOperatorArgumentsPrint(op->compileMetas,0,InvokeTickArg);
            hlog(")");
            break;
        case vmRuntimeOperatorTypeFunction:
            pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
            vmCompileMetaPrint(pmeta,InvokeTickArg);
            hlog(".%s(",buffer_to_str(op->uniqueKey));
            vmCompileMetaOperatorArgumentsPrint(op->compileMetas,1,InvokeTickArg);
            hlog(")");
            break;
        case vmRuntimeOperatorTypeProperty:
            pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
            vmCompileMetaPrint(pmeta,InvokeTickArg);
            if(buffer_length(op->uniqueKey)){
                hlog(".%s",buffer_to_str(op->uniqueKey));
                if(vmCompileObjectArrayCount(op->compileMetas,InvokeTickArg) ==2){
                    hlog("=");
                    vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
                }
            }
            else{
                hlog("[");
                vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
                hlog("]");
                if(vmCompileObjectArrayCount(op->compileMetas,InvokeTickArg) ==3){
                    hlog("=");
                    vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,2,InvokeTickArg),InvokeTickArg);
                }
            }
            break;
        case vmRuntimeOperatorTypeInc:
            if(buffer_length(op->uniqueKey)){
                hlog("%s",buffer_to_str(op->uniqueKey));
            }
            else{
                pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
                vmCompileMetaPrint(pmeta,InvokeTickArg);
            }
            hlog("++");
            break;
        case vmRuntimeOperatorTypeDec:
            if(buffer_length(op->uniqueKey)){
                hlog("%s",buffer_to_str(op->uniqueKey));
            }
            else{
                pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
                vmCompileMetaPrint(pmeta,InvokeTickArg);
            }
            hlog("--");
            break;
        case vmRuntimeOperatorTypeBeforeInc:
            hlog("++");
            if(buffer_length(op->uniqueKey)){
                hlog("%s",buffer_to_str(op->uniqueKey));
            }
            else{
                pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
                vmCompileMetaPrint(pmeta,InvokeTickArg);
            }
            break;
        case vmRuntimeOperatorTypeBeforeDec:
            hlog("--");
            if(buffer_length(op->uniqueKey)){
                hlog("%s",buffer_to_str(op->uniqueKey));
            }
            else{
                pmeta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg);
                vmCompileMetaPrint(pmeta,InvokeTickArg);
            }
            break;
        case vmRuntimeOperatorTypeAdd:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("+");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeSub:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("-");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeMultiplication:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("*");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeDivision:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("/");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeModulus:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("%%");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseAnd:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("&");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseOr:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("|");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseExclusiveOr:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("^");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseComplement:
            hlog("~");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseShiftLeft:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("<<");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeBitwiseShiftRight:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog(">>");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeNot:
            hlog("!");
            if(buffer_length(op->uniqueKey)){
                hlog("%s",buffer_to_str(op->uniqueKey));
            }
            else{
                vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            }
            break;
        case vmRuntimeOperatorTypeAnd:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("&&");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeOr:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("||");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeGreater:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog(">");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeGreaterEqual :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog(">=");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeLess :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("<");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeLessEqual :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("<=");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeAbsEqual:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("===");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
            break;
        case vmRuntimeOperatorTypeEqual :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("==");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeAbsNotEqual :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("!==");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeNotEqual :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("!=");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeAssign :
            if(buffer_length(op->uniqueKey)){
                if(vmCompileObjectArrayCount(op->compileMetas, InvokeTickArg) ==1){
                    hlog("%s=",buffer_to_str(op->uniqueKey));
                    vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
                }
                else{
                    vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
                    hlog(".%s=",buffer_to_str(op->uniqueKey));
                    vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
                }
            }
            else{
                vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
                hlog("[");
                vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
                hlog("]=");
                vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,2,InvokeTickArg),InvokeTickArg);
            }
            
            break;
        case vmRuntimeOperatorTypeIfElse :
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog("?");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            hlog(":");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,2,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeAntiNumber :
            hlog("-");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            break;
        case vmRuntimeOperatorTypeIs:
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,0,InvokeTickArg),InvokeTickArg);
            hlog(" is ");
            vmCompileMetaPrint((vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,1,InvokeTickArg),InvokeTickArg);
            break;
        default:
            assert(0);
            break;
    }
    hlog(" )");
}

void vmCompileMetaPrint(vmCompileMeta * meta,InvokeTickDeclare){

    if(meta->type & vmRuntimeMetaTypeInt16){
        hlog("i16(%d)",meta->value.int16Value);
    }
    else if(meta->type & vmRuntimeMetaTypeInt32){
        hlog("i32(%d)",meta->value.int32Value);
    }
    else if(meta->type & vmRuntimeMetaTypeInt64){
        hlog("i64(%lld)",meta->value.int64Value);
    }
    else if(meta->type & vmRuntimeMetaTypeDouble){
        hlog("d(%lf)",meta->value.doubleValue);
    }
    else if(meta->type & vmRuntimeMetaTypeBoolean){
        hlog("b(%d)",meta->value.booleanValue);
    }
    else if(meta->type & vmRuntimeMetaTypeString){
        hlog("s\"%s\"",buffer_to_str(meta->value.stringValue));
    }
    else if(meta->type & vmRuntimeMetaTypeObject){
        hlog("OBJ(%s)",buffer_to_str(meta->value.objectKey));
    }
    else if(meta->type & vmRuntimeMetaTypeOperator){
        vmCompileMetaOperatorPrint(meta->value.operatorMeta,InvokeTickArg);
    }
    else{
        hlog("null");
    }

}

static void _vmCompileClassMetaDealloc(vmCompileClassMeta * meta,InvokeTickDeclare){
    buffer_dealloc(meta->superClass);
    vmCompileObjectRelease((vmCompileObject *)meta->propertys, InvokeTickArg);
    vmCompileObjectRelease((vmCompileObject *)meta->functions, InvokeTickArg);
}

vmCompileClassMeta * vmCompileClassMetaCreate(InvokeTickDeclare){
    vmCompileClassMeta * meta = (vmCompileClassMeta *) vmCompileObjectCreate(sizeof(vmCompileClassMeta), (vmCompileObjectDealloc)_vmCompileClassMetaDealloc, InvokeTickArg);
    meta->superClass = buffer_alloc(32, 32);
    meta->propertys = vmCompileObjectArrayCreate(InvokeTickArg);
    meta->functions = vmCompileObjectArrayCreate(InvokeTickArg);
    
    buffer_append_str(meta->superClass, "Object");
    
    return meta;
}
