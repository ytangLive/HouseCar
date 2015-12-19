//
//  hvmcompiler_bin.c
//  vmcompiler
//
//  Created by Zhang Hailong on 13-3-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_bin.h"
#include "hbuffer.h"
#include "hlog.h"
#include "md5.h"
#include "hmap.h"

#define UNIQUE_KEY_OFFSET  10

static void hobj_vmBinary_destroy(vmBinary * obj,InvokeTickDeclare);

static void hobj_vmBinary_init(vmBinary * obj,InvokeTickDeclare);

HOBJ_KEY_IMP(vmBinary)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmBinary)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_vmBinary_destroy)

HOBJ_CLASS_METHOD_IMP(init, hobj_vmBinary_init)

HOBJ_CLASS_METHOD_IMP_END(vmBinary)

HOBJ_CLASS_IMP_I(vmBinary, vmBinary, &hobj_class)


static vmClassMetaOffset vmBinaryUniqueKey(vmBinary * binary,hcchar * key,hint32 length,hbool isNilString,InvokeTickDeclare){
    hint32 i;
    vmClassMetaOffset offset = 0;
    hchar * p = buffer_data(binary->uniqueKeys);
    hchar e = 0;
    hint32 len;
    
    if(key == NULL || (!isNilString && length == 0)){
        return 0;
    }
    
    for(i=0;i<binary->uniqueKeyCount;i++){
        len = strlen(p);
        if(len == length && strncmp(p, key, length) ==0){
            return (vmClassMetaOffset)(p - buffer_data(binary->uniqueKeys) + UNIQUE_KEY_OFFSET);
        }
        p = p + len +1;
    }
    offset = buffer_length(binary->uniqueKeys) + UNIQUE_KEY_OFFSET;
    buffer_append(binary->uniqueKeys, (hany)key,length);
    buffer_append(binary->uniqueKeys,&e,1);
    binary->uniqueKeyCount ++;
    return offset;
}

static hint32 vmBinaryOperatorMetaLength(vmCompilerMetaOperator *op,InvokeTickDeclare){
    return (hint32)(sizeof(vmRuntimeMetaOperator) + hobj_array_count(op->metas,InvokeTickArg) * sizeof(vmRuntimeMeta));
}

static vmClassMetaOffset vmBinaryAddOperatorMeta(vmBinary * binary,vmCompilerClassMeta * classMeta,vmCompilerMetaOperator *op,InvokeTickDeclare){
    hint32 i,c;
    vmCompilerMeta * meta;
    memcpy(op->binary.classUDID, classMeta->binary.UDID, sizeof(classMeta->binary.UDID));
    op->binary.offset = binary->operatorOffset;
    op->binary.uniqueKey = vmBinaryUniqueKey(binary,op->uniqueKey.location.p,op->uniqueKey.length,hbool_false,InvokeTickArg);
    
    hobj_array_add(binary->operatorMetas,(hobj_t *)op,InvokeTickArg);
    
    c = hobj_array_count(op->metas,InvokeTickArg);
    
    op->binary.length = sizeof(vmRuntimeMetaOperator) + sizeof(vmRuntimeMeta) * c;
    
    binary->operatorOffset += op->binary.length;
    
    for(i=0;i<c;i++){
        meta = (vmCompilerMeta *)hobj_array_objectAt(op->metas,i,InvokeTickArg);
        if(meta->type & vmRuntimeMetaTypeOperator){
            meta->binary.valueOffset = vmBinaryAddOperatorMeta(binary, classMeta,meta->operatorMeta,InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeString){
            {
                hbuffer_t buf = buffer_alloc(meta->stringValue.length, 20);
                hchar * p = meta->stringValue.location.p;
                hint32 len = meta->stringValue.length;
                hobj_t * v = NULL;
                if( *p == '"' || *p =='\''){
                    p ++;
                    len -= 2;
                }
                
                while(p && len >0){
                    
                    if(p[0] == '\\'){
                        if(p[1] == '\\'){
                            buffer_append(buf, (hchar *)"\\", 1);
                        }
                        else if(p[1] == 'n'){
                            buffer_append(buf, (hchar *)"\n", 1);
                        }
                        else if(p[1] == 'r'){
                            buffer_append(buf, (hchar *)"\r", 1);
                        }
                        else if(p[1] == 't'){
                            buffer_append(buf, (hchar *)"\t", 1);
                        }
                        else if(p[1] == '\''){
                            buffer_append(buf, (hchar *)"'", 1);
                        }
                        else if(p[1] == '"'){
                            buffer_append(buf, (hchar *)"\"", 1);
                        }
                        else{
                            buffer_append(buf,p + 1,1);
                        }
                        p++;
                    }
                    else{
                        buffer_append(buf,p,1);
                    }
                    
                    len --;
                    p ++;
                }
                
                if(* meta->stringValue.location.p == '\''){
                    if(buffer_length(buf) ==1){
                        meta->type = vmRuntimeMetaTypeInt16;
                        meta->int16Value = * buffer_data(buf);
                    }
                    else{
                        v = hobj_valueForKey((hobj_t *)binary->resources, buffer_to_str(buf), InvokeTickArg);
                    }
                }
                
                if(meta->type & vmRuntimeMetaTypeString){
                    if(v){
                        p = (hchar *)hobj_value_stringValue((hobj_value_t *) v, InvokeTickArg);
                        meta->binary.valueOffset = vmBinaryUniqueKey(binary,p ,strlen(p),hbool_true,InvokeTickArg);
                    }
                    else{
                        meta->binary.valueOffset = vmBinaryUniqueKey(binary,buffer_to_str(buf) ,buffer_length(buf),hbool_true,InvokeTickArg);
                    }
                }
                
                buffer_dealloc(buf);
            }
            
        }
        else if(meta->type & vmRuntimeMetaTypeObject){
            meta->binary.valueOffset = vmBinaryUniqueKey(binary,meta->objectKey.location.p,meta->objectKey.length,hbool_false,InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeArg){
            meta->binary.valueOffset = vmBinaryUniqueKey(binary,meta->objectKey.location.p,meta->objectKey.length,hbool_false,InvokeTickArg);
        }
    }
    return op->binary.offset;
}

hbool vmBinaryAddClass(vmBinary * binary,vmCompilerClassMeta * classMeta,hcchar * className,InvokeTickDeclare){
    hint32 i,c,n;
    vmCompilerMetaOperator * op1, *op2;
    
    classMeta->binary.className = vmBinaryUniqueKey(binary,className,strlen(className),hbool_true,InvokeTickArg);
    if(classMeta->superClass.length == 0){
        classMeta->binary.superClass = vmBinaryUniqueKey(binary,"Object",6,hbool_true,InvokeTickArg);
    }
    else{
        classMeta->binary.superClass = vmBinaryUniqueKey(binary, classMeta->superClass.location.p,classMeta->superClass.length,hbool_true,InvokeTickArg);
    }
    classMeta->binary.offset = binary->classOffset;
    
    hobj_array_add(binary->classMetas,(hobj_t *)classMeta,InvokeTickArg);
    
    c = hobj_array_count(classMeta->propertys,InvokeTickArg);
    
    for(i=0;i<c;i++){
        vmBinaryAddOperatorMeta(binary,classMeta,(vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->propertys,i,InvokeTickArg),InvokeTickArg);
    }
    
    for(i=0;i<c-1;i++){
        op1 = (vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->propertys,i,InvokeTickArg);
        for(n=i +1;n < c;n++){
            op2 = (vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->propertys,n,InvokeTickArg);
            if(op1->binary.uniqueKey > op2->binary.uniqueKey){
                hobj_array_swap(classMeta->propertys, i, n, InvokeTickArg);
                op1 = op2;
            }
        }
    }
    
    c = hobj_array_count(classMeta->functions,InvokeTickArg);
    for(i=0;i<c;i++){
        vmBinaryAddOperatorMeta(binary,classMeta,(vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->functions,i,InvokeTickArg),InvokeTickArg);
    }
    
    for(i=0;i<c-1;i++){
        op1 = (vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->functions,i,InvokeTickArg);
        for(n=i +1;n < c;n++){
            op2 = (vmCompilerMetaOperator *)hobj_array_objectAt(classMeta->functions,n,InvokeTickArg);
            if(op1->binary.uniqueKey > op2->binary.uniqueKey){
                hobj_array_swap(classMeta->functions, i, n, InvokeTickArg);
                op1 = op2;
            }
        }
    }
    
    binary->classOffset += sizeof(vmClassMeta) + hobj_array_count(classMeta->propertys,InvokeTickArg) * sizeof(vmClassMetaOffset) + hobj_array_count(classMeta->functions,InvokeTickArg) * sizeof(vmClassMetaOffset);
    
    return hbool_true;
}

hbool vmBinaryAddStringResource(vmBinary * binary,hcchar * key,hcchar * string,InvokeTickDeclare){
    hobj_t * v = hobj_valueForKey((hobj_t *)binary->resources, key, InvokeTickArg);
    if(v == NULL){
        v = (hobj_t *)hobj_value_string_new(string, InvokeTickArg);
        hobj_setValueForKey((hobj_t *) binary->resources, key, v, InvokeTickArg);
    }
    return hbool_true;
}

vmRuntimeClassLibraryBytes * vmBinaryBytes(vmBinary * binary,InvokeTickDeclare){
    hint32 i,c,j,length;
    vmClassMetaOffset uniqueKeyOffset = 0;
    vmClassMetaOffset operatorOffset = 0;
    vmClassMetaOffset offset = 0,t;
    vmClassMetaOffset classOffset = 0;
    vmCompilerClassMeta * classMeta;
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    vmRuntimeClassLibraryBytes * bytes;
    vmRuntimeMetaOperator rOperator;
    vmRuntimeMeta rMeta;
    vmClassMeta rClassMeta;
    vmRuntimeClassMetaBytes bClassMeta;
    hchar *p;
    md5_state_t md5;
    
    length = vmBinaryLength(binary,InvokeTickArg);
    
    bytes = mem_malloc(length);
    
    vmRuntimeClassLibraryBytesInit(bytes,InvokeTickArg);
    bytes->classCount = hobj_array_count(binary->classMetas,InvokeTickArg);
    bytes->uniqueKeyCount = binary->uniqueKeyCount;
    bytes->length = length;
    
    uniqueKeyOffset = length - buffer_length(binary->uniqueKeys);
    
    mem_memcpy((hbyte *)bytes + uniqueKeyOffset, buffer_data(binary->uniqueKeys), buffer_length(binary->uniqueKeys));
    
    
    operatorOffset = uniqueKeyOffset - binary->operatorOffset;
    
    c = hobj_array_count(binary->operatorMetas,InvokeTickArg);
    
    offset = operatorOffset;
    
    for(i=0;i<c;i++){
        op = (vmCompilerMetaOperator *)hobj_array_objectAt(binary->operatorMetas, i,InvokeTickArg);
        mem_memset(&rOperator, 0, sizeof(vmRuntimeMetaOperator));
        rOperator.uniqueKey = op->binary.uniqueKey ? op->binary.uniqueKey + uniqueKeyOffset - UNIQUE_KEY_OFFSET : 0;
        rOperator.type = op->type;
        rOperator.metaCount = hobj_array_count(op->metas,InvokeTickArg);
        mem_memcpy((hbyte *)bytes + offset, &rOperator, sizeof(vmRuntimeMetaOperator));
        offset += sizeof(vmRuntimeMetaOperator);
        for(j=0;j<rOperator.metaCount;j++){
            meta = (vmCompilerMeta *)hobj_array_objectAt(op->metas, j,InvokeTickArg);
            mem_memset(&rMeta, 0, sizeof(vmRuntimeMeta));
            rMeta.type = meta->type;
            if(rMeta.type & vmRuntimeMetaTypeBoolean){
                rMeta.value.booleanValue = meta->booleanValue;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt16){
                rMeta.value.int16Value = meta->int16Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt32){
                rMeta.value.int32Value = meta->int32Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt64){
                rMeta.value.int64Value = meta->int64Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeDouble){
                rMeta.value.doubleValue = meta->doubleValue;
            }
            else if(rMeta.type & vmRuntimeMetaTypeString){
                rMeta.value.stringKey = meta->binary.valueOffset ? uniqueKeyOffset + meta->binary.valueOffset - UNIQUE_KEY_OFFSET : 0;
            }
            else if(rMeta.type & vmRuntimeMetaTypeObject){
                rMeta.value.objectKey = meta->binary.valueOffset ? uniqueKeyOffset + meta->binary.valueOffset - UNIQUE_KEY_OFFSET : 0;
            }
            else if(rMeta.type & vmRuntimeMetaTypeOperator){
                rMeta.value.operatorOffset = meta->binary.valueOffset ? operatorOffset + meta->binary.valueOffset: 0;
            }
            else if(rMeta.type & vmRuntimeMetaTypeArg){
                rMeta.value.objectKey = meta->binary.valueOffset ? uniqueKeyOffset + meta->binary.valueOffset - UNIQUE_KEY_OFFSET : 0;
            }
            mem_memcpy((hbyte *)bytes + offset, &rMeta, sizeof(vmRuntimeMeta));
            offset += sizeof(vmRuntimeMeta);
        }
        
    }
    
    classOffset = operatorOffset - binary->classOffset;
    offset = classOffset;
    
    c = hobj_array_count(binary->classMetas,InvokeTickArg);
    
    for(i=0;i<c;i++){
        classMeta = (vmCompilerClassMeta *) hobj_array_objectAt(binary->classMetas, i,InvokeTickArg);
        mem_memset(&rClassMeta, 0, sizeof(vmClassMeta));
        rClassMeta.type = vmClassTypeMeta;
        rClassMeta.superClass = classMeta->binary.superClass ? classMeta->binary.superClass + uniqueKeyOffset - UNIQUE_KEY_OFFSET : 0;
        rClassMeta.offset = offset;
        rClassMeta.propertys = hobj_array_count(classMeta->propertys,InvokeTickArg);
        rClassMeta.functions = hobj_array_count(classMeta->functions,InvokeTickArg);
        mem_memcpy((hbyte *)bytes + offset, &rClassMeta, sizeof(vmClassMeta));
        offset += sizeof(vmClassMeta);
        
        for(j=0;j<rClassMeta.propertys;j++){
            op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->propertys,j,InvokeTickArg);
            t = operatorOffset + op->binary.offset;
            mem_memcpy((hbyte *)bytes + offset, &t, sizeof(vmClassMetaOffset));
            offset += sizeof(vmClassMetaOffset);
        }
        
        for(j=0;j<rClassMeta.functions;j++){
            op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->functions,j,InvokeTickArg);
            t = operatorOffset + op->binary.offset;
            mem_memcpy((hbyte *)bytes + offset, &t, sizeof(vmClassMetaOffset));
            offset += sizeof(vmClassMetaOffset);
        }
    }
    
    offset = sizeof(vmRuntimeClassLibraryBytes);
    
    for(i=0;i<c;i++){
        classMeta = (vmCompilerClassMeta *) hobj_array_objectAt(binary->classMetas, i,InvokeTickArg);
        mem_memset(&bClassMeta, 0, sizeof(vmRuntimeClassMetaBytes));
        bClassMeta.className = classMeta->binary.className ? classMeta->binary.className + uniqueKeyOffset - UNIQUE_KEY_OFFSET : 0;
        bClassMeta.classMeta = classOffset + classMeta->binary.offset;
        mem_memcpy((hbyte *)bytes + offset, &bClassMeta, sizeof(vmRuntimeClassMetaBytes));
        offset += sizeof(vmRuntimeClassMetaBytes);
    }
    
    p = (hchar *)buffer_to_str(binary->uniqueKeys);
    
    for(i=0;i<binary->uniqueKeyCount;i++){
        t = (vmClassMetaOffset)(uniqueKeyOffset + p - buffer_data(binary->uniqueKeys));
        mem_memcpy((hbyte *)bytes + offset, &t, sizeof(vmClassMetaOffset));
        offset += sizeof(vmClassMetaOffset);
        p += strlen(p) +1;
    }
    
    md5_init(&md5);
    
    if(length > sizeof(vmRuntimeClassLibraryBytes)){
        md5_append(&md5, (hany)bytes, length - sizeof(vmRuntimeClassLibraryBytes));
    }
    
    md5_finish(&md5, bytes->UDID);
    
    return bytes;
}

void vmBinaryBytesDealloc(vmBinary * binary,vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare){
    if(binary && bytes){
        mem_free(bytes);
    }
}

hint32 vmBinaryLength(vmBinary * binary,InvokeTickDeclare){
    hint32 length = sizeof(vmRuntimeClassLibraryBytes);
    
    length += sizeof(vmRuntimeClassMetaBytes) * hobj_array_count(binary->classMetas,InvokeTickArg);
    
    length += sizeof(vmClassMetaOffset) * binary->uniqueKeyCount;
    
    length += binary->classOffset;
    
    length += binary->operatorOffset;
    
    length += buffer_length(binary->uniqueKeys);
    
    return length;
}

void vmBinaryWriteFile(vmBinary * binary,hcchar * file,InvokeTickDeclare){
    vmRuntimeClassLibraryBytes * bytes = vmBinaryBytes(binary,InvokeTickArg);
    hint32 length = vmBinaryLength(binary,InvokeTickArg);
    FILE * f;
    
    //vmRuntimeClassLibraryBytesPrint(bytes,length,InvokeTickArg);
    
    f = fopen(file,"wb");
    if(f){
        
        fwrite(bytes, 1, length, f);
        
        fclose(f);
    }
    else{
        hlog("not write file %s\n",file);
    }
    
    vmBinaryBytesDealloc(binary,bytes,InvokeTickArg);
}

static void hobj_vmBinary_init(vmBinary * obj,InvokeTickDeclare){
    obj->classMetas = hobj_array_alloc(InvokeTickArg);
    obj->operatorMetas = hobj_array_alloc(InvokeTickArg);
    obj->uniqueKeys = buffer_alloc(128, 128);
    obj->resources = hobj_dictionary_alloc(InvokeTickArg);
}

static void hobj_vmBinary_destroy(vmBinary * obj,InvokeTickDeclare){
    
    hobj_method_t * method;
    
    hobj_release((hobj_t *)obj->classMetas, InvokeTickArg);
    hobj_release((hobj_t *)obj->operatorMetas, InvokeTickArg);
    hobj_release((hobj_t *)obj->resources, InvokeTickArg);
    
    buffer_dealloc(obj->uniqueKeys);
    
    method = hobj_class_method(obj->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *)obj,InvokeTickArg);
    }
    
}

