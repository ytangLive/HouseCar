//
//  hvmfile.c
//  C Library
//
//  Created by Hailong Zhang on 11-11-3.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmfile.h"
#include "hbuffer.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"

typedef struct{
    vmObject base;
    FILE * file;
    struct{
        vmUniqueKey flush;
        vmUniqueKey read;
        vmUniqueKey write;
        vmUniqueKey seek;
        vmUniqueKey pos;
    }uniqueKeys;
}vmFile;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmFileClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmFile * file = (vmFile *) object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    hbuffer_t bPath = buffer_alloc(128, 128);
    hbuffer_t bMode = buffer_alloc(32, 32);
    vmVariant path = vmVariantListGet(args, 0);
    vmVariant mode = vmVariantListGet(args, 1);
    
    vmVariantToString(context, path, bPath);
    vmVariantToString(context, mode, bMode);
    
    
    if(buffer_length(bPath) ==0){
        rs = vmRuntimeContextException(context, 0, "not found argument 1 file path");
    }
    
    if(rs.type == vmVariantTypeVoid){
        file->file = fopen(buffer_to_str(bPath), buffer_to_str(bMode));
    }
    
    if(!file->file){
        rs = vmRuntimeContextException(context, 0, "not found argument 2 file mode");
    }
    
    buffer_dealloc(bPath);
    buffer_dealloc(bMode);

    file->uniqueKeys.flush = vmRuntimeContextGetUniqueKey(context, "flush");
    file->uniqueKeys.read = vmRuntimeContextGetUniqueKey(context, "read");
    file->uniqueKeys.write = vmRuntimeContextGetUniqueKey(context, "write");
    file->uniqueKeys.seek = vmRuntimeContextGetUniqueKey(context, "seek");
    file->uniqueKeys.pos = vmRuntimeContextGetUniqueKey(context, "pos");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmFileClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmFile * file = (vmFile *) object;
    if(file->file){
        fclose(file->file);
        file->file = NULL;
    }
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmFileClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmFile * file = (vmFile *) object;
    
    if(file->uniqueKeys.pos == name){
        fpos_t pos = 0;
        if(file->file){
            fgetpos(file->file, &pos);
        }
        rs.type= vmVariantTypeInt64;
        rs.value.int64Value = pos;
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmFileClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmFile * file = (vmFile *) object;
    
    if(file->uniqueKeys.pos == name){
        if(file->file){
            fpos_t post = vmVariantToInt64(context, value);
            fsetpos(file->file, &post);
        }
    }
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmFileClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmFile * file = (vmFile *) object;
    
    if(file->uniqueKeys.flush == name){
        if(file->file){
            fflush(file->file);
        }
    }
    else if(file->uniqueKeys.read == name){
        {
            hint32 length = vmVariantToInt32(context, vmVariantListGet(args, 0));
            hint32 len;
            hbyte buffer[1024];
            rs = vmDataAlloc(context, NULL, 0);
            if(length ==0){
                while(!feof(file->file) && (length = (hint32)fread(buffer, 1, sizeof(buffer), file->file))){
                    vmDataAppend(context, rs.value.objectValue, buffer, length);
                }
            }
            else if(length >0){
                len = sizeof(buffer);
                if(len > length){
                    len = length;
                }
                while(len >0 && !feof(file->file) && (len = (hint32)fread(buffer, 1, len, file->file))){
                    
                    vmDataAppend(context, rs.value.objectValue, buffer, len);
                    
                    length -= len;
                    len = sizeof(buffer);
                    if(len > length){
                        len = length;
                    }
                }
            }
        }
    }
    else if(file->uniqueKeys.write ==name){
        {
            vmVariant var = vmVariantListGet(args, 0);
            hbuffer_t buf = NULL;
            if(var.type & vmVariantTypeObject && var.value.objectValue && vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmDataClass)){
                
                rs.type = vmVariantTypeInt32;
                rs.value.int32Value = (hint32)fwrite(vmDataDataPtr(context, var.value.objectValue), 1, vmDataDataLength(context, var.value.objectValue), file->file);
            }
            else if(var.type != vmVariantTypeVoid){
                buf = buffer_alloc(1024, 1024);
                
                vmVariantToString(context, var, buf);
                
                rs.type = vmVariantTypeInt32;
                
                rs.value.int32Value = (hint32)fwrite(buffer_data(buf), 1, buffer_length(buf), file->file);
                
                buffer_dealloc(buf);
            }
        }
    }
    else if(file->uniqueKeys.seek == name){
        
    }
    else if(file->uniqueKeys.flush == name){
        fflush(file->file);
    }
    else if(context->systemKeys.functionToStringKey == name){
        {
            hbuffer_t buf = buffer_alloc(128, 128);
            hbyte buffer[1024];
            ssize_t len;
            
            while(!feof(file->file) && (len = fread(buffer, 1, sizeof(buffer), file->file)) >0){
                buffer_append(buf, buffer, (hint32) len);
            }
        
            rs = vmStringAlloc(context, buffer_to_str(buf));
            
            buffer_dealloc(buf);
        }
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmFileClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmFile * file = (vmFile *) object;
    vmUniqueKeyListAdd(names, file->uniqueKeys.pos);
}


vmClassBase vmFileClass = {vmClassTypeBase,sizeof(vmFile),vmFileClassInitCallback,vmFileClassDestroyCallback,vmFileClassGetPropertyCallback,vmFileClassSetPropertyCallback,vmFileClassPropertyNamesCallback,vmFileClassInvokeCallback};


