//
//  hvermin_bin.c
//  C Library
//
//  Created by Hailong Zhang on 11-10-31.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_bin.h"
#include "hlog.h"

#define UNIQUE_KEY_OFFSET  10

#undef vmBinaryInit

#undef vmBinaryDestroy

#undef vmBinaryAddClass

#undef vmBinaryLength

#undef vmBinaryBytes

#undef vmBinaryWriteFile

hbool vmBinaryInit(vmBinary * binary,InvokeTickDeclare){
    mem_memset(binary, 0, sizeof(vmBinary));
    binary->uniqueKeys = buffer_alloc(1024, 1024);
    binary->classMetas = vmCompileObjectArrayCreate();
    binary->operatorMetas = vmCompileObjectArrayCreate();
    
    return hbool_true;
}

void vmBinaryDestroy(vmBinary * binary,InvokeTickDeclare){
    if(binary){
        buffer_dealloc(binary->uniqueKeys);
        vmCompileObjectRelease(binary->classMetas);
        vmCompileObjectRelease(binary->operatorMetas);
        mem_memset(binary, 0, sizeof(vmBinary));
    }
}

static vmClassMetaOffset vmBinaryUniqueKey(vmBinary * binary,hcchar * key,hbool isNilString,InvokeTickDeclare){
    hint32 i;
    vmClassMetaOffset offset = 0;
    hchar * p = buffer_data(binary->uniqueKeys);
    hchar e = 0;
    
    if(key == NULL || (!isNilString && *key == 0)){
        return 0;
    }
    
    for(i=0;i<binary->uniqueKeyCount;i++){
        if(mem_strcmp(p, key) ==0){
            return (vmClassMetaOffset)(p - buffer_data(binary->uniqueKeys) + UNIQUE_KEY_OFFSET);
        }
        p = p + strlen(p) +1;
    }
    offset = buffer_length(binary->uniqueKeys) + UNIQUE_KEY_OFFSET;
    buffer_append_str(binary->uniqueKeys, key);
    buffer_append(binary->uniqueKeys,&e,1);
    binary->uniqueKeyCount ++;
    return offset;
}

static hint32 vmBinaryOperatorMetaLength(vmCompileMetaOperator *op,InvokeTickDeclare){
    return (hint32)(sizeof(vmRuntimeMetaOperator) + vmCompileObjectArrayCount(op->compileMetas) * sizeof(vmRuntimeMeta));
}

static vmClassMetaOffset vmBinaryAddOperatorMeta(vmBinary * binary,vmCompileMetaOperator *op,InvokeTickDeclare){
    hint32 i,c;
    vmCompileMeta * meta;
    op->binary.offset = binary->operatorOffset;
    op->binary.uniqueKey = vmBinaryUniqueKey(binary,buffer_to_str(op->uniqueKey),hbool_false,InvokeTickArg);
    
    vmCompileObjectArrayAdd(binary->operatorMetas,op);
    
    c = vmCompileObjectArrayCount(op->compileMetas);
    
    op->binary.length = sizeof(vmRuntimeMetaOperator) + sizeof(vmRuntimeMeta) * c;
    
    binary->operatorOffset += op->binary.length;
    
    for(i=0;i<c;i++){
        meta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas,i);
        if(meta->type & vmRuntimeMetaTypeOperator){
            meta->binary.valueOffset = vmBinaryAddOperatorMeta(binary, meta->value.operatorMeta,InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeString){
            meta->binary.valueOffset = vmBinaryUniqueKey(binary,buffer_to_str(meta->value.stringValue),hbool_true,InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeObject){
            meta->binary.valueOffset = vmBinaryUniqueKey(binary,buffer_to_str(meta->value.objectKey),hbool_false,InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeArg){
            meta->binary.valueOffset = vmBinaryUniqueKey(binary,buffer_to_str(meta->value.objectKey),hbool_false,InvokeTickArg);
        }
    }
    return op->binary.offset;
}

hbool vmBinaryAddClass(vmBinary * binary,vmCompileClassMeta * classMeta,hcchar * className,InvokeTickDeclare){
    hint32 i,c;
    classMeta->binary.className = vmBinaryUniqueKey(binary,className,hbool_true,InvokeTickArg);
    classMeta->binary.superClass = vmBinaryUniqueKey(binary, buffer_to_str(classMeta->superClass),hbool_true,InvokeTickArg);
    classMeta->binary.offset = binary->classOffset;
    vmCompileObjectArrayAdd(binary->classMetas,classMeta);
    

    c = vmCompileObjectArrayCount(classMeta->propertys);
    for(i=0;i<c;i++){
        vmBinaryAddOperatorMeta(binary,(vmCompileMetaOperator *)vmCompileObjectArrayGet(classMeta->propertys,i),InvokeTickArg);
    }
    
    c = vmCompileObjectArrayCount(classMeta->functions);
    for(i=0;i<c;i++){
        vmBinaryAddOperatorMeta(binary,(vmCompileMetaOperator *)vmCompileObjectArrayGet(classMeta->functions,i),InvokeTickArg);
    }
    
    binary->classOffset += sizeof(vmClassMeta) + vmCompileObjectArrayCount(classMeta->propertys) * sizeof(vmClassMetaOffset) + vmCompileObjectArrayCount(classMeta->functions) * sizeof(vmClassMetaOffset);
    
    return hbool_true;
}

vmRuntimeClassLibraryBytes * vmBinaryBytes(vmBinary * binary,InvokeTickDeclare){
    hint32 i,c,j,length;
    vmClassMetaOffset uniqueKeyOffset = 0;
    vmClassMetaOffset operatorOffset = 0;
    vmClassMetaOffset offset = 0,t;
    vmClassMetaOffset classOffset = 0;
    vmCompileClassMeta * classMeta;
    vmCompileMetaOperator *op;
    vmCompileMeta * meta;
    vmRuntimeClassLibraryBytes * bytes;
    vmRuntimeMetaOperator rOperator;
    vmRuntimeMeta rMeta;
    vmClassMeta rClassMeta;
    vmRuntimeClassMetaBytes bClassMeta;
    hchar *p;
    
    length = vmBinaryLength(binary,InvokeTickArg);
    
    bytes = mem_malloc(length);
    
    vmRuntimeClassLibraryBytesInit(bytes,InvokeTickArg);
    bytes->classCount = vmCompileObjectArrayCount(binary->classMetas);
    bytes->uniqueKeyCount = binary->uniqueKeyCount;
    

    
    uniqueKeyOffset = length - buffer_length(binary->uniqueKeys);
    
    mem_memcpy((hbyte *)bytes + uniqueKeyOffset, buffer_data(binary->uniqueKeys), buffer_length(binary->uniqueKeys));
    
    
    operatorOffset = uniqueKeyOffset - binary->operatorOffset;
    
    c = vmCompileObjectArrayCount(binary->operatorMetas);
    
    offset = operatorOffset;
    
    for(i=0;i<c;i++){
        op = (vmCompileMetaOperator *)vmCompileObjectArrayGet(binary->operatorMetas, i);
        mem_memset(&rOperator, 0, sizeof(vmRuntimeMetaOperator));
        rOperator.uniqueKey = op->binary.uniqueKey ? op->binary.uniqueKey + uniqueKeyOffset - UNIQUE_KEY_OFFSET : 0;
        rOperator.type = op->type;
        rOperator.metaCount = vmCompileObjectArrayCount(op->compileMetas);
        mem_memcpy((hbyte *)bytes + offset, &rOperator, sizeof(vmRuntimeMetaOperator));
        offset += sizeof(vmRuntimeMetaOperator);
        for(j=0;j<rOperator.metaCount;j++){
            meta = (vmCompileMeta *)vmCompileObjectArrayGet(op->compileMetas, j);
            mem_memset(&rMeta, 0, sizeof(vmRuntimeMeta));
            rMeta.type = meta->type;
            if(rMeta.type & vmRuntimeMetaTypeBoolean){
                rMeta.value.booleanValue = meta->value.booleanValue;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt16){
                rMeta.value.int16Value = meta->value.int16Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt32){
                rMeta.value.int32Value = meta->value.int32Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeInt64){
                rMeta.value.int64Value = meta->value.int64Value;
            }
            else if(rMeta.type & vmRuntimeMetaTypeDouble){
                rMeta.value.doubleValue = meta->value.doubleValue;
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
    
    c = vmCompileObjectArrayCount(binary->classMetas);
    
    for(i=0;i<c;i++){
        classMeta = (vmCompileClassMeta *) vmCompileObjectArrayGet(binary->classMetas, i);
        mem_memset(&rClassMeta, 0, sizeof(vmClassMeta));
        rClassMeta.type = vmClassTypeMeta;
        rClassMeta.superClass = classMeta->binary.superClass ? classMeta->binary.superClass + uniqueKeyOffset - UNIQUE_KEY_OFFSET : 0;
        rClassMeta.offset = offset;
        rClassMeta.propertys = vmCompileObjectArrayCount(classMeta->propertys);
        rClassMeta.functions = vmCompileObjectArrayCount(classMeta->functions);
        mem_memcpy((hbyte *)bytes + offset, &rClassMeta, sizeof(vmClassMeta));
        offset += sizeof(vmClassMeta);
        
        for(j=0;j<rClassMeta.propertys;j++){
            op = (vmCompileMetaOperator *) vmCompileObjectArrayGet(classMeta->propertys,j);
            t = operatorOffset + op->binary.offset;
            mem_memcpy((hbyte *)bytes + offset, &t, sizeof(vmClassMetaOffset));
            offset += sizeof(vmClassMetaOffset);
        }
        
        for(j=0;j<rClassMeta.functions;j++){
            op = (vmCompileMetaOperator *) vmCompileObjectArrayGet(classMeta->functions,j);
            t = operatorOffset + op->binary.offset;
            mem_memcpy((hbyte *)bytes + offset, &t, sizeof(vmClassMetaOffset));
            offset += sizeof(vmClassMetaOffset);
        }
    }
    
    offset = sizeof(vmRuntimeClassLibraryBytes);
    
    for(i=0;i<c;i++){
        classMeta = (vmCompileClassMeta *) vmCompileObjectArrayGet(binary->classMetas, i);
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
    
    return bytes;
}

void vmBinaryBytesDealloc(vmBinary * binary,vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare){
    if(binary && bytes){
        mem_free(bytes);
    }
}

hint32 vmBinaryLength(vmBinary * binary,InvokeTickDeclare){
    hint32 length = sizeof(vmRuntimeClassLibraryBytes);

    length += sizeof(vmRuntimeClassMetaBytes) * vmCompileObjectArrayCount(binary->classMetas);
    
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

    vmRuntimeClassLibraryBytesPrint(bytes,length,InvokeTickArg);
    
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

