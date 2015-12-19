//
//  hvmcompiler_debug.c
//  vmcompiler
//
//  Created by Zhang Hailong on 13-3-16.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_debug.h"
#include "hfile.h"
#include "md5.h"
#include "hbuffer.h"
#include "hlog.h"

#define VM_DEBUG_TAG    "VMDEBUG"
#define VM_DEBUG_VERSION    "1.0.0"

vmBinaryDebug * vmBinaryDebugAlloc(hcchar * debugFile,InvokeTickDeclare){
    if(file_exist(debugFile)){
        hint32 length = file_size(debugFile);
        vmBinaryDebug * d;
        FILE * f;
        if(length >= sizeof(vmBinaryDebug)){
            d = (vmBinaryDebug *) mem_malloc(length);
            mem_memset(d, 0, length);
            f = fopen(debugFile, "r");
            if(f){
                if(length != fread(d, 1, length, f)){
                    mem_free(d);
                    fclose(f);
                    return NULL;
                }
                fclose(f);
            }
            if(strcmp(d->tag, VM_DEBUG_TAG) == 0
               && strcmp(d->version, VM_DEBUG_VERSION) ==0){
                return d;
            }
            else{
                mem_free(d);
                return NULL;
            }
        }
    }
    return NULL;
}

void vmBinaryDebugDealloc(vmBinaryDebug * debug,InvokeTickDeclare){
    if(debug){
        mem_free(debug);
    }
}

vmBinaryDebugItem * vmBinaryDebugSearch(vmBinaryDebug * debug,vmClassMetaOffset offset,InvokeTickDeclare){
    if(debug){
        vmBinaryDebugItem * item = (vmBinaryDebugItem *)((hbyte *) debug + sizeof(vmBinaryDebug));
        hint32 pIndex = 0,nIndex = debug->length -1;
        hint32 index;
        hint32 rs;
        while(pIndex < nIndex){
            index = (pIndex + nIndex) / 2;
            rs = (hint32)offset - (hint32) item[index].offset ;
            if(rs == 0){
                nIndex = pIndex = index;
                break;
            }
            else if(rs <0){
                nIndex = index -1;
            }
            else{
                pIndex = index +1;
            }
        }
        
        if(pIndex == nIndex){
            rs = (hint32)offset - (hint32) item[pIndex].offset ;
            if(rs == 0){
                return item + pIndex;
            }
        }
	}
	return NULL;
}

hbool vmBinaryDebugWriteFile(vmBinary * binary,hcchar * binaryFile,hcchar * debugFile,InvokeTickDeclare){
    vmBinaryDebug debug;
    huchar buf[1024];
    hint32 len;
    FILE * f;
    md5_state_t md5;
    hint32 i,c,length;
    vmClassMetaOffset uniqueKeyOffset = 0;
    vmClassMetaOffset operatorOffset = 0;
    vmClassMetaOffset offset = 0;
    vmCompilerMetaOperator * op;
    vmBinaryDebugItem debugItem;
    
    memset(&debug, 0, sizeof(vmBinaryDebug));
    
    strcpy(debug.tag, VM_DEBUG_TAG);
    strcpy(debug.version, VM_DEBUG_VERSION);
    
        
    f = fopen(binaryFile, "rb");
    
    if(f){
        
        md5_init(&md5);
        
        while (!feof(f) && (len =fread(buf, 1, sizeof(buf), f)) >0) {
            md5_append(&md5, buf, len);
        }
        
        fclose(f);
        
        md5_finish(&md5, debug.UDID);
    }
    
    f = fopen(debugFile, "wb");
    
    if(f){
    
        debug.length = hobj_array_count(binary->operatorMetas, InvokeTickArg);
        
        fwrite(&debug, 1, sizeof(vmBinaryDebug), f);
        
        length = vmBinaryLength(binary,InvokeTickArg);
        uniqueKeyOffset = length - buffer_length(binary->uniqueKeys);
        operatorOffset = uniqueKeyOffset - binary->operatorOffset;
        
        c = hobj_array_count(binary->operatorMetas,InvokeTickArg);
        
        offset = operatorOffset;
        
        for(i=0;i<c;i++){
            op = (vmCompilerMetaOperator *)hobj_array_objectAt(binary->operatorMetas, i,InvokeTickArg);
    
            memset(&debugItem, 0, sizeof(debugItem));
            
            debugItem.offset = offset;
            debugItem.line = op->uniqueKey.location.line;
            debugItem.index = op->uniqueKey.location.index;
            
            fwrite(&debugItem, 1, sizeof(debugItem), f);
            
            offset += sizeof(vmRuntimeMetaOperator) + hobj_array_count(op->metas, InvokeTickArg) * sizeof(vmRuntimeMeta);
            
        }
        
        fclose(f);
        
        return hbool_true;
    }
    
    return hbool_false;
}

void vmBinaryDebugLog(vmBinaryDebug * debug,InvokeTickDeclare){
    if(debug){
        vmBinaryDebugItem * item = (vmBinaryDebugItem *)((hbyte *) debug + sizeof(vmBinaryDebug));
        hint32 len = debug->length;
        while(len){
            hlog("0x%0x %d:%d\n",item->offset,item->line,item->index);
            len --;
            item ++;
        }
    }
}

