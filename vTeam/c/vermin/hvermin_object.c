//
//  hvermin_object.c
//  C Library
//
//  Created by hailong zhang on 11-6-8.
//  Copyright 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_object.h"
#include "hbuffer.h"
#include "hvermin_runtime_variant.h"
#include "hserial_map.h"
#include "hstr.h"
#include "hlog.h"
#include "hlist.h"

#undef vmStringAlloc

#undef vmNumberDoubleAlloc

#undef vmNumberInt16Alloc

#undef vmNumberInt32Alloc

#undef vmNumberInt64Alloc

#undef vmNumberBooleanAlloc

#undef vmExceptionAlloc

#undef vmArrayAlloc

#undef vmDictionaryAlloc

#undef vmSystemAlloc

#undef vmArrayAdd

#undef vmDictionaryPut

#undef vmVariantPrint

#undef vmDataAlloc

#undef vmDataAppend

#undef vmDataDataPtr

#undef vmDataDataLength 


/**
 
 Return : Void or Throwable
 */

vmVariant vmObjectClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs=  {vmVariantTypeVoid,0};
    
    
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

vmVariant vmObjectClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */

vmVariant vmObjectClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    
    if(context->systemKeys.classNameKey == name){
        
        rs.type = vmVariantTypeString;
        rs.value.stringValue = vmRuntimeContextGetClassName(context, object->vmClass);
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */

vmVariant vmObjectClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

/**
 
 Return : any
 */

vmVariant vmObjectClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    if(context->systemKeys.functionEqualKey == name){
        {
            vmVariant var = vmVariantListGet(args, 0);
            if(var.type & vmVariantTypeObject){
                rs.type = vmVariantTypeBoolean;
                rs.value.booleanValue = object == var.value.objectValue;
            }
            else{
                rs.type = vmVariantTypeBoolean;
                rs.value.booleanValue = hbool_false;
            }
        }
    }
    else if(context->systemKeys.functionToStringKey == name){
        hbuffer_t str = buffer_alloc(128, 128);
        hint32 i,c;
        vmUniqueKeyList names = vmUniqueKeyListAlloc();
        vmUniqueKey key;
        vmVariant  value;
        
        vmObjectPropertyNames(context, object->vmClass, object, names);
        
        c = vmUniqueKeyListCount(names);
        
        buffer_append_str(str, "\n{\n");
        
        for(i = 0;i<c;i++){
            key = vmUniqueKeyListGet(names, i);
            value = vmObjectGetProperty(context,  object->vmClass, object, key);
            
            buffer_append_str(str, "\t");
            if(i !=0){
                buffer_append_str(str, ", ");
            }
            
            buffer_append_str(str, vmRuntimeContextUniqueKeyToString(context, key));
            buffer_append_str(str," : ");
            
            if(value.type & vmVariantTypeObject){
                buffer_append_str(str, "[Object]");
            }
            else{
                vmVariantToString(context, value, str);
            }
            
            buffer_append_str(str, "\n");
        }
        
        buffer_append_str(str,"}\n");
        
        rs = vmStringAlloc(context, buffer_to_str(str), InvokeTickArg);
        
        buffer_dealloc(str);
        
        vmUniqueKeyListDealloc(names);

    }
    else if(context->systemKeys.valueForKey == name){
        {
            hbuffer_t key = buffer_alloc(64, 64);
            
            vmVariantToString(context, vmVariantListGet(args, 0), key);
            
            rs = vmObjectGetProperty(context, object->vmClass, object, vmRuntimeContextGetUniqueKey(context, buffer_to_str(key)));
            
            buffer_dealloc(key);
        }
    }
    else if(context->systemKeys.setValueForKey == name){
        {
            hbuffer_t key = buffer_alloc(64, 64);
            
            vmVariantToString(context, vmVariantListGet(args, 0), key);
            
            rs = vmObjectSetProperty(context, object->vmClass, object, vmRuntimeContextGetUniqueKey(context, buffer_to_str(key)),vmVariantListGet(args, 1));
            
            buffer_dealloc(key);
        }
    }
    else if(context->systemKeys.valueForKeyPath == name){
        {
            hbuffer_t keyPath = buffer_alloc(64, 64);
            hchar *p;
            vmVariant v;
            vmVariantList vars = vmVariantListAlloc();
            
            vmVariantToString(context, vmVariantListGet(args, 0), keyPath);
            
            p = (hchar *) buffer_to_str(keyPath);
            
            while(p && *p != '\0'){
                if(*p == '.'){
                    break;
                }
                p++;
            }
            
            if(*p == '.'){
                * p = 0;
                
                v = vmStringAlloc(context, buffer_to_str(keyPath),InvokeTickArg);
                
                vmVariantListAdd(vars, v);
                
                rs = vmObjectInvoke(context, object->vmClass, object, context->systemKeys.valueForKey, vars);
                
                vmVariantListClear(vars);
                
                if(!(rs.type & vmVariantTypeThrowable)){
                    
                    rs = vmVariantToObject(context, rs);
                    
                    if((rs.type & vmVariantTypeObject) && rs.value.objectValue){
                        
                        v = vmStringAlloc(context, p + 1,InvokeTickArg);
                        
                        vmVariantListAdd(vars, v);
                        
                        rs = vmObjectInvoke(context, rs.value.objectValue->vmClass, rs.value.objectValue, context->systemKeys.valueForKeyPath, vars);
                        
                        vmVariantListClear(vars);
                    }
                }

            }
            else{
                rs = vmObjectInvoke(context, object->vmClass, object, context->systemKeys.valueForKey, args);
            }
            
            buffer_dealloc(keyPath);
            
            vmVariantListDealloc(vars);
        }
    }
    else if(context->systemKeys.setValueForKeyPath == name){
        {
            hbuffer_t keyPath = buffer_alloc(64, 64);
            hchar *p;
            vmVariant v;
            vmVariantList vars = vmVariantListAlloc();
            
            vmVariantToString(context, vmVariantListGet(args, 0), keyPath);
            
            p = (hchar *) buffer_to_str(keyPath);
            
            while(p && *p != '\0'){
                if(*p == '.'){
                    break;
                }
                p++;
            }
            
            if(*p == '.'){
                * p = 0;
                
                v = vmStringAlloc(context, buffer_to_str(keyPath),InvokeTickArg);
                
                vmVariantListAdd(vars, v);
                
                rs = vmObjectInvoke(context, object->vmClass, object, context->systemKeys.valueForKey, vars);
                
                vmVariantListClear(vars);
                
                if( !(rs.type & vmVariantTypeThrowable)){
                    
                    rs = vmVariantToObject(context, rs);
                    
                    if((rs.type & vmVariantTypeObject) && rs.value.objectValue){
                        v = vmStringAlloc(context, p + 1,InvokeTickArg);
                        
                        vmVariantListAdd(vars, v);
                        vmVariantListAdd(vars, vmVariantListGet(args, 1));
                        
                        rs = vmObjectInvoke(context, rs.value.objectValue->vmClass, rs.value.objectValue, context->systemKeys.setValueForKeyPath, vars);
                        
                        vmVariantListClear(vars);
                    }
                }
                
            }
            else{
                rs = vmObjectInvoke(context, object->vmClass, object, context->systemKeys.setValueForKey, args);
            }
            
            buffer_dealloc(keyPath);
            
            vmVariantListDealloc(vars);
        }
    }
    else if(context->systemKeys.functionToTextContentKey == name){
        {
            vmVariant v = vmVariantListGet(args, 0);
            vmVariant vThis = {vmVariantTypeObject,0};
            vmObject * data = NULL;
            hbuffer_t buf = buffer_alloc(64, 64);
            hbuffer_t textContent = buffer_alloc(64, 64);
            hbuffer_t keyPath = buffer_alloc(64, 64);
            hchar * p;
            hint32 s = 0;
            vmVariantList vars = vmVariantListAlloc();
            
            if((v.type & vmVariantTypeObject) && v.value.objectValue){
                data = v.value.objectValue;
            }
            
            vThis.value.objectValue = object;
            
            vmVariantToString(context, vThis, buf);
            
            p = (hchar *) buffer_to_str(buf);
            
            while(p){
                
                switch (s) {
                    case 0:
                    {
                        if(*p == '{'){
                            buffer_clear(keyPath);
                            s =1;
                        }
                        else{
                            buffer_append(textContent, p, 1);
                        }
                    }
                        break;
                    case 1:
                    {
                        if(*p == '}'){
                            
                            if(data){
                                
                                v = vmStringAlloc(context, buffer_to_str(keyPath), InvokeTickArg);
                            
                                vmVariantListClear(vars);
                            
                                vmVariantListAdd(vars, v);
                            
                                v = vmObjectInvoke(context, data->vmClass, data, context->systemKeys.valueForKeyPath, vars);
                                
                                vmVariantToString(context, v, textContent);
                            }
                            
                            s = 0;
                        }
                        else{
                            buffer_append(keyPath, p, 1);
                        }
                    }
                        break;
                    default:
                        break;
                }
                
                
                if(*p == '\0'){
                    break;
                }
                p ++;
            }
            
            rs = vmStringAlloc(context, buffer_to_str(textContent),InvokeTickArg);
            
            buffer_dealloc(buf);
            buffer_dealloc(textContent);
            buffer_dealloc(keyPath);
            vmVariantListDealloc(vars);
        }
    }
    
    return rs;
}

/**
 
 Return : Void
 */

void vmObjectClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}


vmClassBase vmObjectClass = {vmClassTypeBase,sizeof(vmObject),vmObjectClassInitCallback,vmObjectClassDestroyCallback,vmObjectClassGetPropertyCallback,vmObjectClassSetPropertyCallback,vmObjectClassPropertyNamesCallback,vmObjectClassInvokeCallback};


typedef struct{
    vmObject base;
    hchar * cString;
    struct {
        vmUniqueKey length;
        vmUniqueKey hasPrefix;
        vmUniqueKey hasSuffix;
        vmUniqueKey substr;
        vmUniqueKey charAt;
        vmUniqueKey trim;
        vmUniqueKey indexOf;
        vmUniqueKey split;
    }uniqueKeys;
}vmString;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmStringClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= vmObjectClassInitCallback(context,clazz,object,args,InvokeTickArg);
    vmVariant arg = {vmVariantTypeVoid,0};
    hbuffer_t str ;
    vmString * vmStr = (vmString *)object;
    hint32 len;
    if(rs.type & vmVariantTypeThrowable){
        return rs;
    }
    
    str = buffer_alloc(128, 128);
    
    arg = vmVariantListGet(args, 0);
    
    vmVariantToString(context,arg, str);
    
    if(vmVariantListCount(args) >1){
        len = vmVariantToInt32(context, vmVariantListGet(args, 1));
    }
    else{
        len = buffer_length(str);
    }
    
    vmStr->cString = mem_malloc(len+1);
    
    mem_memcpy(vmStr->cString, buffer_data(str), len);
    
    vmStr->cString[len] = 0;
    
    buffer_dealloc(str);
    
    vmStr->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context, "length");
    vmStr->uniqueKeys.hasPrefix = vmRuntimeContextGetUniqueKey(context, "hasPrefix");
    vmStr->uniqueKeys.hasSuffix = vmRuntimeContextGetUniqueKey(context, "hasSuffix");
    vmStr->uniqueKeys.substr = vmRuntimeContextGetUniqueKey(context, "substr");
    vmStr->uniqueKeys.charAt = vmRuntimeContextGetUniqueKey(context, "charAt");
    vmStr->uniqueKeys.trim = vmRuntimeContextGetUniqueKey(context, "trim");
    vmStr->uniqueKeys.indexOf = vmRuntimeContextGetUniqueKey(context, "indexOf");
    vmStr->uniqueKeys.split = vmRuntimeContextGetUniqueKey(context, "split");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmStringClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmString * str = (vmString *)object;
    if( str->cString){
        mem_free(str->cString);
        str->cString = NULL;
    }
    return vmObjectClassDestroyCallback(context,object,InvokeTickArg);
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmStringClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmString * str = (vmString *)object;
    vmVariant rs = {vmVariantTypeVoid,0};
    if(str->uniqueKeys.length == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = str->cString ? (hint32)strlen(str->cString) : 0;
        return rs;
    }
    else{
        return vmObjectClassGetPropertyCallback(context,clazz,object,name,InvokeTickArg);
    }
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmStringClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    return vmObjectClassSetPropertyCallback(context,clazz,object,name,value,InvokeTickArg);
}

/**
 
 Return : any
 */
static vmVariant vmStringClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmString * vmStr = (vmString *)object;
    vmVariant rs ={vmVariantTypeVoid,0};
    
    if(vmStr->uniqueKeys.hasPrefix == name){
        
        rs.type = vmVariantTypeBoolean;
        {
            vmVariant v =vmVariantListGet(args, 0);
            hbuffer_t str;
            if(v.type & vmVariantTypeString){
                rs.value.booleanValue = str_has_prefix(vmStr->cString, v.value.stringValue);
            }
            else{
                str = buffer_alloc(128, 128);
                rs.value.booleanValue = str_has_prefix(vmStr->cString, buffer_to_str(str));
                buffer_dealloc(str);
            }
        }
        
    }
    else if(vmStr->uniqueKeys.hasSuffix == name){
        rs.type = vmVariantTypeBoolean;
        {
            vmVariant v =vmVariantListGet(args, 0);
            hbuffer_t str;
            if(v.type & vmVariantTypeString){
                rs.value.booleanValue = str_has_suffix(vmStr->cString, v.value.stringValue);
            }
            else{
                str = buffer_alloc(128, 128);
                rs.value.booleanValue = str_has_suffix(vmStr->cString, buffer_to_str(str));
                buffer_dealloc(str);
            }
        }
        
    }
    else if(vmStr->uniqueKeys.substr == name){
        {
            hint32 index = vmVariantToInt32(context, vmVariantListGet(args, 0));
            hint32 slen = (hint32) strlen(vmStr->cString);
            hint32 len;
            
            if(vmVariantListCount(args) >1){
                len = vmVariantToInt32(context, vmVariantListGet(args, 1));
                if(index + len > slen){
                    len = slen - index;
                }
                rs = vmStringAllocWithLength(context, vmStr->cString + index,len,InvokeTickArg);
            }
            else if(index < slen){
                rs = vmStringAlloc(context, vmStr->cString + index,InvokeTickArg);
            }
        }
    }
    else if(vmStr->uniqueKeys.charAt == name){
        rs.type = vmVariantTypeInt16;
        rs.value.int16Value = vmStr->cString[vmVariantToInt32(context, vmVariantListGet(args, 0))];
    }
    else if(vmStr->uniqueKeys.trim == name){
        {
            hchar * sp = (hchar *)vmStr->cString;
            hchar * ep = sp + strlen(sp) -1;
            
            while(sp && sp < ep){
                if(*sp == ' ' || *sp == '\r' || *sp == '\n' || *sp == '\t'){
                    sp ++;
                }
                else{
                    break;
                }
            }
            
            while(ep && sp < ep){
                if(*ep == ' ' || *ep == '\r' || *ep == '\n' || *ep == '\t'){
                    ep --;
                }
                else{
                    break;
                }
            }
            rs = vmStringAllocWithLength(context, sp, (hint32) (ep - sp + 1), InvokeTickArg);
          
        }
    }
    else if(vmStr->uniqueKeys.indexOf == name){
        {
            hbuffer_t s = buffer_alloc(64, 64);
            hchar * p;
            hchar * b;
            
            vmVariantToString(context, vmVariantListGet(args, 0), s);
            
            b = (hchar *)vmStr->cString + vmVariantToInt32(context,vmVariantListGet(args, 1));
            
            p = str_find_sub(b, buffer_to_str(s));
            
            if(p){
                rs.type = vmVariantTypeInt32;
                rs.value.int32Value = (hint32) (p - (hchar *)vmStr->cString);
            }
            else{
                rs.type = vmVariantTypeInt32;
                rs.value.int32Value = -1;
            }
            
            buffer_dealloc(s);
        }
    }
    else if(vmStr->uniqueKeys.split == name){
        {
            hlist_t list = list_alloc(4, 4);
            hbuffer_t s = buffer_alloc(32, 32);
            hint32 i,c;
            vmVariant v;
            
            rs = vmArrayAlloc(context,InvokeTickArg);
            
            vmVariantToString(context, vmVariantListGet(args, 0), s);
            
            list_split_str(list, vmStr->cString, buffer_to_str(s));
            
            c = list_count(list);
            
            for(i=0;i<c;i++){
                v = vmStringAlloc(context, list_get(list, i), InvokeTickArg);
                vmArrayAdd(context, rs.value.objectValue, v, InvokeTickArg);
            }
            
            buffer_dealloc(s);
        }
    }
    else if(context->systemKeys.functionToStringKey == name){
        rs.type = vmVariantTypeString;
        rs.value.stringValue = vmStr->cString;
    }
    else if(context->systemKeys.functionToIntKey == name){
        rs.type = vmVariantTypeInt64;
        if(vmStr->cString){
            if(str_has_prefix(vmStr->cString, "0x")){
                sscanf(vmStr->cString, "0x%llx",&rs.value.int64Value);
            }
            else{
                rs.value.int64Value = atoll(vmStr->cString);
            }
        }
    }
    else if(context->systemKeys.functionToDoubleKey == name){
        rs.type = vmVariantTypeDouble;
        if(vmStr->cString){
            rs.value.doubleValue = atof(vmStr->cString);
        }
    }
    else if(context->systemKeys.functionEqualKey == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = hbool_false;
        {
            hbuffer_t str = buffer_alloc(128, 128);
            
            vmVariantToString(context, vmVariantListGet(args, 0), str);
            
            rs.value.booleanValue = mem_strcmp(vmStr->cString, buffer_to_str(str)) ==0;
            
            buffer_dealloc(str);
        }
    }
    else{
        return vmObjectClassInvokeCallback(context,clazz,object,name,args,InvokeTickArg);
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmStringClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmObjectClassPropertyNamesCallback(context,clazz,object,names,InvokeTickArg);
}

vmClassBase vmStringClass = {vmClassTypeBase,sizeof(vmString),vmStringClassInitCallback,vmStringClassDestroyCallback,vmStringClassGetPropertyCallback,vmStringClassSetPropertyCallback,vmStringClassPropertyNamesCallback,vmStringClassInvokeCallback};


vmVariant vmStringAlloc(vmRuntimeContext context,hcchar * str,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeString,0};
    vmVariant rs ;
    var.value.stringValue = str;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmStringClass, args);
    vmVariantListDealloc(args);
    return rs;
}

vmVariant vmStringAllocWithLength(vmRuntimeContext context,hcchar * str,hint32 length,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeString,0};
    vmVariant rs ;
    var.type = vmVariantTypeString;
    var.value.stringValue = str;
    vmVariantListAdd(args, var);
    
    var.type = vmVariantTypeInt32;
    var.value.int32Value = length;
    vmVariantListAdd(args, var);

    rs = vmObjectAlloc(context,(vmClass *)&vmStringClass, args);
    vmVariantListDealloc(args);
    return rs;
}

typedef struct{
    vmObject base;
    vmVariant value;
}vmNumber;
/**
 
 Return : Void or Throwable
 */

static vmVariant vmNumberClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmNumber * vmNum = (vmNumber *)object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmVariant value = vmVariantListGet(args, 0);
    if(value.type & vmVariantTypeInt16){
        vmNum->value.type = vmVariantTypeInt16;
        vmNum->value.value.int16Value = value.value.int16Value;
    }
    else if(value.type & vmVariantTypeInt32){
        vmNum->value.type = vmVariantTypeInt32;
        vmNum->value.value.int32Value = value.value.int32Value;
    }
    else if(value.type & vmVariantTypeInt64){
        vmNum->value.type = vmVariantTypeInt64;
        vmNum->value.value.int64Value = value.value.int64Value;
    }
    else if(value.type & vmVariantTypeDouble){
        vmNum->value.type = vmVariantTypeDouble;
        vmNum->value.value.doubleValue = value.value.doubleValue;
    }
    else if(value.type & vmVariantTypeBoolean){
        vmNum->value.type = vmVariantTypeBoolean;
        vmNum->value.value.booleanValue = value.value.booleanValue;
    }
    else{
        vmNum->value.type = vmVariantTypeInt32;
        vmNum->value.value.int32Value = 0;
    }
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmNumberClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmNumberClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmNumberClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    return vmObjectClassSetPropertyCallback(context, clazz, object, name, value,InvokeTickArg);
}


/**
 
 Return : any
 */
static vmVariant vmNumberClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmNumber * vmNum = (vmNumber *)object;
    
    if(context->systemKeys.functionToStringKey == name){
        {
            hbuffer_t str = buffer_alloc(128, 128);
            
            vmVariantToString(context, vmNum->value, str);
            
            rs = vmStringAlloc(context, buffer_to_str(str), InvokeTickArg);
            
            buffer_dealloc(str);
        }
    }
    else if(context->systemKeys.functionToIntKey == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = vmVariantToInt32(context, vmNum->value);
    }
    else if(context->systemKeys.functionToDoubleKey == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = vmVariantToDouble(context, vmNum->value);
    }
    else{
        return vmObjectClassInvokeCallback(context,clazz,object,name,args,InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmNumberClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}

vmClassBase vmNumberClass = {vmClassTypeBase,sizeof(vmNumber),vmNumberClassInitCallback,vmNumberClassDestroyCallback,vmNumberClassGetPropertyCallback,vmNumberClassSetPropertyCallback,vmNumberClassPropertyNamesCallback,vmNumberClassInvokeCallback};


vmVariant vmNumberDoubleAlloc(vmRuntimeContext context,hdouble value,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeDouble,0};
    vmVariant rs ;
    var.value.doubleValue = value;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmNumberClass, args);
    vmVariantListDealloc(args);
    return rs;
}

vmVariant vmNumberInt16Alloc(vmRuntimeContext context,hint16 value,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeInt16,0};
    vmVariant rs ;
    var.value.int16Value = value;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmNumberClass, args);
    vmVariantListDealloc(args);
    return rs;
}

vmVariant vmNumberInt32Alloc(vmRuntimeContext context,hint32 value,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeInt32,0};
    vmVariant rs ;
    var.value.int32Value = value;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmNumberClass, args);
    vmVariantListDealloc(args);
    return rs;
}

vmVariant vmNumberInt64Alloc(vmRuntimeContext context,hint64 value,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeInt64,0};
    vmVariant rs ;
    var.value.int64Value = value;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmNumberClass, args);
    vmVariantListDealloc(args);
    return rs;
}

vmVariant vmNumberBooleanAlloc(vmRuntimeContext context,hbool value,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeBoolean,0};
    vmVariant rs ;
    var.value.booleanValue = value;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmNumberClass, args);
    vmVariantListDealloc(args);
    return rs;
}


/**
 
 Return : Void or Throwable
 */

static vmVariant vmExceptionClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmException * vmExpr = (vmException *)object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmVariant code = vmVariantListGet(args, 0);
    vmVariant error = vmVariantListGet(args, 1);
    hbuffer_t buf = buffer_alloc(32, 32);
    vmExpr->code = vmVariantToInt32(context, code);
    vmVariantToString(context, error, buf);
    vmExpr->error = mem_malloc(buffer_length(buf) +1);
    mem_strcpy(vmExpr->error, buffer_to_str(buf));
    buffer_dealloc(buf);
    vmExpr->uniqueKeys.code = vmRuntimeContextGetUniqueKey(context, "code");
    vmExpr->uniqueKeys.error = vmRuntimeContextGetUniqueKey(context, "error");
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmExceptionClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmException * vmExpr = (vmException *)object;
    
    if(vmExpr->error){
        mem_free(vmExpr->error);
        vmExpr->error = NULL;
    }
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmExceptionClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmException * vmExpr = (vmException *)object;
    
    if(vmExpr->uniqueKeys.code == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = vmExpr->code;
    }
    else if(vmExpr->uniqueKeys.error == name){
        rs.type = vmVariantTypeString;
        rs.value.stringValue = vmExpr->error;
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmExceptionClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    return vmObjectClassSetPropertyCallback(context, clazz, object, name, value,InvokeTickArg);
}

/**
 
 Return : any
 */
static vmVariant vmExceptionClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmException * vmExpr = (vmException *)object;
    
    if(context->systemKeys.functionToStringKey){
        rs.type = vmVariantTypeString;
        rs.value.stringValue = vmExpr->error;
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args, InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmExceptionClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmException * vmExpr = (vmException *)object;
    vmUniqueKeyListAdd(names, vmExpr->uniqueKeys.code);
    vmUniqueKeyListAdd(names, vmExpr->uniqueKeys.error);
}

vmClassBase vmExceptionClass = {vmClassTypeBase,sizeof(vmException),vmExceptionClassInitCallback,vmExceptionClassDestroyCallback,vmExceptionClassGetPropertyCallback,vmExceptionClassSetPropertyCallback,vmExceptionClassPropertyNamesCallback,vmExceptionClassInvokeCallback};

vmVariant vmExceptionAlloc(vmRuntimeContext context,hint32 code,hcchar * error,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeInt32,0};
    vmVariant rs ;
    var.value.int32Value = code;
    vmVariantListAdd(args, var);
    var.type = vmVariantTypeString;
    var.value.stringValue = error;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmExceptionClass, args);
    vmVariantListDealloc(args);
    return rs;
}

typedef struct {
    vmObject base;
    vmVariantList variantList;
    struct {
        vmUniqueKey lengthKey;
        vmUniqueKey push;
        vmUniqueKey peek;
        vmUniqueKey pop;
        vmUniqueKey join;
        vmUniqueKey indexOf;
        vmUniqueKey remove;
        vmUniqueKey removeAt;
        vmUniqueKey insert;
        vmUniqueKey clear;
    }uniqueKeys;
}vmArray;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmArrayClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmArray * array = (vmArray *) object;
    hint32 i,c = vmVariantListCount(args);
    vmVariant value ;
    array->variantList = vmVariantListAlloc();
    for(i=0;i<c;i++){
        value = vmVariantListGet(args, i);
        vmRuntimeContextVariantRetain(context, value);
        vmVariantListAdd(array->variantList, value);
    }
    array->uniqueKeys.lengthKey = vmRuntimeContextGetUniqueKey(context, "length");
    array->uniqueKeys.push = vmRuntimeContextGetUniqueKey(context, "push");
    array->uniqueKeys.peek = vmRuntimeContextGetUniqueKey(context, "peek");
    array->uniqueKeys.pop = vmRuntimeContextGetUniqueKey(context, "pop");
    array->uniqueKeys.join = vmRuntimeContextGetUniqueKey(context, "join");
    array->uniqueKeys.indexOf = vmRuntimeContextGetUniqueKey(context, "indexOf");
    array->uniqueKeys.remove = vmRuntimeContextGetUniqueKey(context, "remove");
    array->uniqueKeys.removeAt = vmRuntimeContextGetUniqueKey(context, "removeAt");
    array->uniqueKeys.insert = vmRuntimeContextGetUniqueKey(context, "insert");
    array->uniqueKeys.clear = vmRuntimeContextGetUniqueKey(context, "clear");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmArrayClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmArray * array = (vmArray *) object;
    hint32 i,c = vmVariantListCount(array->variantList);
    vmVariant value ;
    
    for(i=0;i<c;i++){
        value = vmVariantListGet(array->variantList, i);
        vmRuntimeContextVariantRelease(context, value);
    }
    vmVariantListDealloc(array->variantList);
    
    array->variantList = NULL;
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmArrayClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    if(object){
        vmArray * array = (vmArray *) object;
        hint32 index = 0;
        
        if(name == array->uniqueKeys.lengthKey){
            rs.type = vmVariantTypeInt32;
            rs.value.int32Value = vmVariantListCount(array->variantList);
        }
        else if(name == context->systemKeys.classNameKey){
            return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
        }
        else{
            index = vmRuntimeContextUniqueKeyToInteger(context, name);
            if(index >=0 && index < vmVariantListCount(array->variantList)){
                rs = vmVariantListGet(array->variantList, index);
            }
            else{
                rs = vmRuntimeContextException(context, 0, "Array index out");
            }
        }
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmArrayClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    
    if(object){
        vmArray * array = (vmArray *) object;
        hint32 index = 0;
        vmVariant v = {vmVariantTypeVoid,0};
        
        if(name == array->uniqueKeys.lengthKey){
            rs = vmRuntimeContextException(context, 0, "Array length readonly");
        }
        else{
            index = vmRuntimeContextUniqueKeyToInteger(context, name);
            
            if(index >=0){
                while(index >= vmVariantListCount(array->variantList)){
                    vmVariantListAdd(array->variantList, v);
                }
                vmRuntimeContextVariantRetain(context, value);
                vmRuntimeContextVariantRelease(context, vmVariantListGet(array->variantList, index));
                vmVariantListSet(array->variantList, index, value);
            }
            else{
                rs = vmRuntimeContextException(context, 0, "Array index out");
            }
        }
    }
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmArrayClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmArray * array = (vmArray *) object;
    
    if(array->uniqueKeys.push == name){
        {
            vmVariant var = vmVariantListGet(args, 0);
            vmRuntimeContextVariantRetain(context, var);
            vmVariantListAdd(array->variantList, var);
        }
        
    }
    else if(array->uniqueKeys.peek == name){
        rs = vmVariantListLast(array->variantList);
    }
    else if(array->uniqueKeys.pop == name){
        rs = vmVariantListLast(array->variantList);
        
        if(rs.type & vmVariantTypeObject){
            vmRuntimeContextDomainAddObject(context, rs.value.objectValue);
        }
        vmRuntimeContextVariantRelease(context, rs);
        vmVariantListRemoveLast(array->variantList);
      
    }
    else if(array->uniqueKeys.join == name){
        {
            vmVariant var = vmVariantListGet(args, 0);
            hbuffer_t str = buffer_alloc(128, 128);
            hint32 i,c = vmVariantListCount(array->variantList);
            
            for(i=0;i<c;i++){
                if(i != 0){
                    vmVariantToString(context, var, str);
                }
                vmVariantToString(context, vmVariantListGet(array->variantList, i), str);
            }
            
            rs = vmStringAlloc(context, buffer_to_str(str),InvokeTickArg);
            
            buffer_dealloc(str);
        }
    }
    else if(name == array->uniqueKeys.indexOf){
        {
            hint32 i,c;
            vmVariant var = vmVariantListGet(args, 0);
            
            c = vmVariantListCount(array->variantList);
            rs.type = vmVariantTypeInt32;
            rs.value.int32Value = -1;
            for(i=0;i<c;i++){
                rs.value.booleanValue = vmVariantEqual(context, vmVariantListGet(array->variantList, i), var);
                if(rs.value.booleanValue){
                    rs.type = vmVariantTypeInt32;
                    rs.value.int32Value = i;
                    break;
                }
            }
        }
    }
    else if(name == array->uniqueKeys.remove){
        {
            hint32 i,c;
            vmVariant var = vmVariantListGet(args, 0);
            vmVariant item;
            
            c = vmVariantListCount(array->variantList);
            rs.type = vmVariantTypeBoolean;
            rs.value.booleanValue = hbool_false;
            for(i=0;i<c;i++){
                item = vmVariantListGet(array->variantList, i);
                rs.value.booleanValue = vmVariantEqual(context, item, var);
                if(rs.value.booleanValue){
                    vmRuntimeContextVariantRelease(context, item);
                    vmVariantListRemoveAt(array->variantList, i);
                }
            }
        }
    }
    else if(name == array->uniqueKeys.removeAt){
        {
            hint32 i;
            vmVariant var = vmVariantListGet(args, 0);
            i = vmVariantToInt32(context, var);
            var = vmVariantListGet(array->variantList, i);
            vmRuntimeContextVariantRelease(context, var);
            vmVariantListRemoveAt(array->variantList, i);
        }
    }
    else if(name == array->uniqueKeys.insert){
        {
            vmVariant var = vmVariantListGet(args, 0);
            vmVariant v2 = vmVariantListGet(args, 1);
            hint32 index = vmVariantToInt32(context, v2);
            vmRuntimeContextVariantRetain(context, var);
            if(v2.type == vmVariantTypeVoid){
                vmVariantListAdd(array->variantList, var);
            }
            else{
                vmVariantListInsert(array->variantList, index, var);
            }
        }
    }
    else if(name == array->uniqueKeys.clear){
        {
            hint32 i,c = vmVariantListCount(array->variantList);
            vmVariant value ;
            
            for(i=0;i<c;i++){
                value = vmVariantListGet(array->variantList, i);
                vmRuntimeContextVariantRelease(context, value);
            }
            vmVariantListClear(array->variantList);
   
        }
    }
    else if(name == context->systemKeys.functionToStringKey){
        {
            hbuffer_t str = buffer_alloc(128, 128);
            hint32 i,c;
            
            c = vmVariantListCount(array->variantList);
            
            buffer_append_str(str, "\n[\n");
            for(i=0;i<c;i++){
                buffer_append_str(str, "\t");
                if(i!=0){
                    buffer_append_str(str, ", ");
                }
                vmVariantToString(context, vmVariantListGet(array->variantList, i), str);
                buffer_append_str(str, "\n");
            }
            buffer_append_str(str, "]\n");
            
            rs = vmStringAlloc(context, buffer_to_str(str), InvokeTickArg);
            
            buffer_dealloc(str);
        }
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmArrayClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    if(object){
        vmArray * array = (vmArray *) object;
        hint32 i,c = vmVariantListCount(array->variantList);
        for(i=0;i<c;i++){
            vmUniqueKeyListAdd(names, vmRuntimeContextGetUniqueKeyFromInteger(context, i));
        }
        vmUniqueKeyListAdd(names, array->uniqueKeys.lengthKey);
    }
}

vmVariant vmArrayAlloc(vmRuntimeContext context,InvokeTickDeclare){
    return vmObjectAlloc(context, (vmClass *)&vmArrayClass, NULL);
}

void vmArrayAdd(vmRuntimeContext context,vmObject * object,vmVariant value, InvokeTickDeclare){
    vmArray * array = (vmArray *) object;
    vmRuntimeContextVariantRetain(context, value);
    vmVariantListAdd(array->variantList, value);
}

vmClassBase vmArrayClass = {vmClassTypeBase,sizeof(vmArray),vmArrayClassInitCallback,vmArrayClassDestroyCallback,vmArrayClassGetPropertyCallback,vmArrayClassSetPropertyCallback,vmArrayClassPropertyNamesCallback,vmArrayClassInvokeCallback};


typedef struct{
    vmObject base;
    hserial_map_t map;
    struct{
        vmUniqueKey length;
        vmUniqueKey keyAt;
        vmUniqueKey valueAt;
        vmUniqueKey clear;
    }uniqueKeys;
}vmDictionary;

static vmVariant vmDictionaryClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare);

/**
 
 Return : Void or Throwable
 */

static vmVariant vmDictionaryClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmDictionary * dict = (vmDictionary *) object;
    hbuffer_t strKey  = buffer_alloc(32, 32);
    hint32 i,c;
    dict->map = serial_map_alloc(sizeof(vmUniqueKey), sizeof(vmVariant), 0);
    
    c = vmVariantListCount(args);
    
    for(i=0;i<c;i++){
        if(i +1 <c){
            buffer_clear(strKey);
            vmVariantToString(context, vmVariantListGet(args, i), strKey);
            rs = vmDictionaryClassSetPropertyCallback(context,clazz,object,vmRuntimeContextGetUniqueKey(context, buffer_to_str(strKey)),vmVariantListGet(args, i+1),InvokeTickArg);
            if(rs.type & vmVariantTypeThrowable){
                break;
            }
            i++;
        }
    }

    buffer_dealloc(strKey);
    
    dict->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context, "length");
    dict->uniqueKeys.keyAt = vmRuntimeContextGetUniqueKey(context, "keyAt");
    dict->uniqueKeys.valueAt = vmRuntimeContextGetUniqueKey(context, "valueAt");
    dict->uniqueKeys.clear = vmRuntimeContextGetUniqueKey(context, "clear");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmDictionaryClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmDictionary * dict = (vmDictionary *) object;
    hint32 c = serial_map_count(dict->map),i;
    vmVariant * var;
    
    for(i=0;i<c;i++){
        var = serial_map_value_at(dict->map, i);
        vmRuntimeContextVariantRelease(context, * var);
    }
    
    serial_map_dealloc(dict->map);
    
    dict->map = NULL;
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmDictionaryClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmDictionary * dict = (vmDictionary *) object;
    vmVariant * var;
    if(dict->uniqueKeys.length == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = serial_map_count(dict->map);
    }
    else if(context->systemKeys.classNameKey == name){
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    else
    {
        var = serial_map_get(dict->map, &name);
        if(var){
            rs = * var;
        }
    }
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmDictionaryClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmDictionary * dict = (vmDictionary *) object;
    vmVariant * var = serial_map_get(dict->map, &name);
    vmRuntimeContextVariantRetain(context, value);
    if(var){
        vmRuntimeContextVariantRelease(context, * var);
    }
    if(value.type == vmVariantTypeVoid){
        serial_map_remove(dict->map, &name);
    }
    else{
        serial_map_put(dict->map, &name, &value);
    }
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmDictionaryClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmDictionary * dict = (vmDictionary *) object;
    
    if(dict->uniqueKeys.keyAt == name){
        {
            vmUniqueKey * key = serial_map_key_at(dict->map, vmVariantToInt32(context, vmVariantListGet(args, 0)));
            if(key){
                rs.type =vmVariantTypeString;
                rs.value.stringValue = vmRuntimeContextUniqueKeyToString(context, * key);
            }
        }
    }
    else if(dict->uniqueKeys.valueAt == name){
        {
            vmVariant * value = (vmVariant *) serial_map_value_at(dict->map, vmVariantToInt32(context, vmVariantListGet(args, 0)));
            if(value){
                rs = * value;
            }
        }
    }
    else if(dict->uniqueKeys.clear == name){
        {
            hint32 c = serial_map_count(dict->map),i;
            vmVariant * var;
            
            for(i=0;i<c;i++){
                var = serial_map_value_at(dict->map, i);
                vmRuntimeContextVariantRelease(context, * var);
            }
            
            serial_map_clear(dict->map);
        }
    }
    else if(context->systemKeys.functionToStringKey == name){
        {
            hbuffer_t str = buffer_alloc(128, 128);
            hint32 i,c;
            vmUniqueKey * key;
            vmVariant * value;
            c = serial_map_count(dict->map);
            
            buffer_append_str(str, "\n{\n");
            
            for(i = 0;i<c;i++){
                key = ( vmUniqueKey *)serial_map_key_at(dict->map, i);
                value = (vmVariant *) serial_map_value_at(dict->map, i);
    
                buffer_append_str(str, "\t");
                if(i !=0){
                    buffer_append_str(str, ", ");
                }
                
                buffer_append_str(str, vmRuntimeContextUniqueKeyToString(context, * key));
                buffer_append_str(str," : ");
                vmVariantToString(context, * value, str);
                
                buffer_append_str(str, "\n");
            }
            
            buffer_append_str(str,"}\n");
            
            rs = vmStringAlloc(context, buffer_to_str(str), InvokeTickArg);
            
            buffer_dealloc(str);
        }
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args, InvokeTickArg);
    }

    return rs;
}

/**
 
 Return : Void
 */
static void vmDictionaryClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmDictionary * dict = (vmDictionary *) object;
    vmUniqueKey * key;
    hint32 c = serial_map_count(dict->map),i;
    for(i=0;i<c;i++){
        key =serial_map_key_at(dict->map, i);
        vmUniqueKeyListAdd(names, * key);
    }
}

vmClassBase vmDictionaryClass={vmClassTypeBase,sizeof(vmDictionary),vmDictionaryClassInitCallback,vmDictionaryClassDestroyCallback,vmDictionaryClassGetPropertyCallback,vmDictionaryClassSetPropertyCallback,vmDictionaryClassPropertyNamesCallback,vmDictionaryClassInvokeCallback};


vmVariant vmDictionaryAlloc(vmRuntimeContext context,InvokeTickDeclare){
    return vmObjectAlloc(context, (vmClass *)&vmDictionaryClass, NULL);
}

void vmDictionaryPut(vmRuntimeContext context,vmObject * object,hcchar * key, vmVariant value,InvokeTickDeclare){
    vmDictionaryClassSetPropertyCallback(context,(vmClass *)&vmDictionaryClass, object,vmRuntimeContextGetUniqueKey(context, key),value,InvokeTickArg);
}

typedef struct{
    vmObject base;
    FILE * in;
    FILE * out;
    struct{
        vmUniqueKey info;
        vmUniqueKey debug;
        vmUniqueKey error;
        vmUniqueKey sleep;
        vmUniqueKey usleep;
        vmUniqueKey create;
        vmUniqueKey hasClass;
        vmUniqueKey getenv;
        vmUniqueKey out;
        vmUniqueKey sysinfo;
        vmUniqueKey time;
        vmUniqueKey classIsKindOf;
    }uniqueKeys;
}vmSystem;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmSystemClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmSystem * vmSys = (vmSystem *) object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmVariant argIn = vmVariantListGet(args, 0);
    vmVariant argOut = vmVariantListGet(args, 1);
    
    if(argIn.type != vmVariantTypeVoid && argIn.value.stringValue){
        vmSys->in = (FILE *)argIn.value.stringValue;
    }
    else{
        vmSys->in = NULL;
    }
    
    if(argOut.type != vmVariantTypeVoid && argIn.value.stringValue){
        vmSys->out = (FILE *) argOut.value.stringValue;
    }
    else{
        vmSys->out = NULL;
    }
    
    vmSys->uniqueKeys.info = vmRuntimeContextGetUniqueKey(context, "info");
    vmSys->uniqueKeys.debug = vmRuntimeContextGetUniqueKey(context, "debug");
    vmSys->uniqueKeys.error = vmRuntimeContextGetUniqueKey(context, "error");
    vmSys->uniqueKeys.sleep = vmRuntimeContextGetUniqueKey(context, "sleep");
    vmSys->uniqueKeys.usleep = vmRuntimeContextGetUniqueKey(context, "usleep");
    vmSys->uniqueKeys.create = vmRuntimeContextGetUniqueKey(context, "create");
    vmSys->uniqueKeys.hasClass = vmRuntimeContextGetUniqueKey(context, "hasClass");
    vmSys->uniqueKeys.getenv = vmRuntimeContextGetUniqueKey(context, "getenv");
    vmSys->uniqueKeys.out = vmRuntimeContextGetUniqueKey(context, "out");
    vmSys->uniqueKeys.sysinfo = vmRuntimeContextGetUniqueKey(context, "sysinfo");
    vmSys->uniqueKeys.time = vmRuntimeContextGetUniqueKey(context, "time");
    vmSys->uniqueKeys.classIsKindOf = vmRuntimeContextGetUniqueKey(context, "classIsKindOf");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmSystemClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmSystemClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmSystemClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

static void vmSystemClassWriteToStream(hbuffer_t buf,FILE * out,InvokeTickDeclare){
    hint32 length = buffer_length(buf);
    hint32 index = 0;
    hint32 len;
    hbyte * b = (hbyte *)buffer_to_str(buf);
    if(out){
		while(index < length){

			len = length - index;
			if(len >128 ){
				len = 128;
			}

			fwrite(b + index, 1, len, out);
			fflush(out);
			index += len;
		}
    }
    else{
    	hlog("%s",b);
    }
}

/**
 
 Return : any
 */
static vmVariant vmSystemClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmSystem * vmSys = (vmSystem *) object;
    hbuffer_t str = buffer_alloc(32, 32);
    hint32 i,c;
    vmVariant var;

    if(vmSys->uniqueKeys.sleep == name){
        var = vmVariantListGet(args, 0);
        sleep(vmVariantToInt32(context,var));
    }
    else if(vmSys->uniqueKeys.usleep == name){
        var = vmVariantListGet(args, 0);
        usleep(vmVariantToInt32(context,var));
    }
    else if(vmSys->uniqueKeys.create == name){
        vmVariantToString(context, vmVariantListGet(args, 0), str);
        if(buffer_length(str)){
            {
                vmVariantList vars = vmVariantListAlloc();
                vmClass * clazz = NULL;
                c = vmVariantListCount(args);
                for(i=1;i<c;i++){
                    vmVariantListAdd(vars, vmVariantListGet(args, i));
                }
                
                clazz = vmRuntimeContextGetClass(context, vmRuntimeContextGetUniqueKey(context, buffer_to_str(str)));
                
                if(clazz){
                    rs = vmObjectAlloc(context, clazz, vars);
                }
                
                vmVariantListDealloc(vars);
            }
        }
    }
    else if(vmSys->uniqueKeys.hasClass == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = hbool_false;
        buffer_clear(str);
        vmVariantToString(context, vmVariantListGet(args, 0), str);
        if(buffer_length(str)){
            rs.value.booleanValue = vmRuntimeContextGetClass(context, vmRuntimeContextGetUniqueKey(context, buffer_to_str(str))) != NULL;
        }
    }
    else if(vmSys->uniqueKeys.getenv == name){
        buffer_clear(str);
        vmVariantToString(context, vmVariantListGet(args, 0), str);
        if(buffer_length(str) && getenv(buffer_to_str(str))){
            rs = vmStringAlloc(context, getenv(buffer_to_str(str)), InvokeTickArg);
        }
    }
#ifdef HVM_LOG
    else if(vmSys->uniqueKeys.info == name){
        buffer_append_str(str, "[INFO]");
        c = vmVariantListCount(args);
        for(i=0;i<c;i++){
            var = vmVariantListGet(args, i);
            buffer_append_str(str, " ");
            vmVariantToString(context, var, str);
        }
        buffer_append_str(str,"\n");
        vmSystemClassWriteToStream(str,vmSys->out,InvokeTickArg);
        
    }
    else if(vmSys->uniqueKeys.debug == name){
        buffer_append_str(str, "[DEBUG]");
        c = vmVariantListCount(args);
        for(i=0;i<c;i++){
            var = vmVariantListGet(args, i);
            buffer_append_str(str, " ");
            vmVariantToString(context, var, str);
        }
        buffer_append_str(str,"\n");
        vmSystemClassWriteToStream(str,vmSys->out,InvokeTickArg);
    }
    else if(vmSys->uniqueKeys.error == name){
        buffer_append_str(str, "[ERROR]");
        c = vmVariantListCount(args);
        for(i=0;i<c;i++){
            var = vmVariantListGet(args, i);
            buffer_append_str(str, " ");
            vmVariantToString(context, var, str);
        }
        buffer_append_str(str,"\n");
        vmSystemClassWriteToStream(str,vmSys->out,InvokeTickArg);
        
    }
    else if(vmSys->uniqueKeys.out == name){
        buffer_clear(str);
        c = vmVariantListCount(args);
        for(i=0;i<c;i++){
            var = vmVariantListGet(args, i);
            vmVariantToString(context, var, str);
        }
        vmSystemClassWriteToStream(str,vmSys->out,InvokeTickArg);
    }
#endif
    else if(vmSys->uniqueKeys.sysinfo == name){
        
        vmRuntimeClassLoader loader = vmRuntimeContextClassLoader(context, InvokeTickArg);
        
        c = vmRuntimeClassLoaderClassNameCount(loader, InvokeTickArg);
        
        buffer_clear(str);
        buffer_append_str(str, "Runtime Class:\n");
        

        for(i=0;i<c;i++){
            buffer_append_str(str, vmRuntimeClassLoaderClassNameAt(loader, i, InvokeTickArg));
            buffer_append_str(str, "\n");
        }
    
        vmSystemClassWriteToStream(str,vmSys->out,InvokeTickArg);
    }
    else if(vmSys->uniqueKeys.time == name){
        
        struct timeval tm;
        gettimeofday(&tm, NULL);
        
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
        
    }
    else if(vmSys->uniqueKeys.classIsKindOf == name){
    
        {
            vmClass * inClass = NULL;
            vmClass * ofClass = NULL;
            
            buffer_clear(str);
            
            vmVariantToString(context, vmVariantListGet(args, 0), str);
            
            inClass = vmRuntimeContextGetClass(context, vmRuntimeContextGetUniqueKey(context, buffer_to_str(str)));
            
            if(inClass){
                
                buffer_clear(str);
                
                vmVariantToString(context, vmVariantListGet(args, 1), str);
                
                ofClass = vmRuntimeContextGetClass(context, vmRuntimeContextGetUniqueKey(context, buffer_to_str(str)));
                
                if(ofClass){
                    rs.type = vmVariantTypeBoolean;
                    rs.value.booleanValue = vmRuntimeContextClassIsKindClass(context, inClass, ofClass);
                }
                
            }
        }
        
        
    }
    buffer_dealloc(str);
    return rs;
}

/**
 
 Return : Void
 */
static void vmSystemClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}


vmClassBase vmSystemClass = {vmClassTypeBase,sizeof(vmSystem),vmSystemClassInitCallback,vmSystemClassDestroyCallback,vmSystemClassGetPropertyCallback,vmSystemClassSetPropertyCallback,vmSystemClassPropertyNamesCallback,vmSystemClassInvokeCallback};


vmVariant vmSystemAlloc(vmRuntimeContext context,FILE * inFile,FILE * outFile,InvokeTickDeclare){
    vmVariantList args =vmVariantListAlloc();
    vmVariant var = {vmVariantTypeString,0};
    vmVariant rs ;
    var.value.stringValue = (hcchar *)inFile;
    vmVariantListAdd(args, var);
    var.type = vmVariantTypeString;
    var.value.stringValue = (hcchar *)outFile;
    vmVariantListAdd(args, var);
    rs = vmObjectAlloc(context,(vmClass *)&vmSystemClass, args);
    vmVariantListDealloc(args);
    return rs;
}


/**
 
 Return : Void or Throwable
 */

static vmVariant vmDataClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmData * data = (vmData *) object;
    hint32 capacity = vmVariantToInt32(context, vmVariantListGet(args, 0));
    hint32 extends = vmVariantToInt32(context, vmVariantListGet(args, 1));
    vmVariant d = vmVariantListGet(args,2);
    hint32 length = vmVariantToInt32(context, vmVariantListGet(args, 3));
    data->bytes = buffer_alloc(capacity>0 ? capacity: 32, extends >0 ?extends:32);
    
    if(d.type & vmVariantTypeString){
        buffer_append(data->bytes,(hany)d.value.stringValue,length);
    }
    
    data->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context, "length");
    data->uniqueKeys.charAt = vmRuntimeContextGetUniqueKey(context, "charAt");
    data->uniqueKeys.append = vmRuntimeContextGetUniqueKey(context, "append");
    data->uniqueKeys.appendFormat = vmRuntimeContextGetUniqueKey(context, "appendFormat");
    data->uniqueKeys.clear = vmRuntimeContextGetUniqueKey(context, "clear");
    data->uniqueKeys.removeLastPath = vmRuntimeContextGetUniqueKey(context, "removeLastPath");
    data->uniqueKeys.lastPath = vmRuntimeContextGetUniqueKey(context, "lastPath");
    data->uniqueKeys.indexOf = vmRuntimeContextGetUniqueKey(context, "indexOf");
    data->uniqueKeys.substr = vmRuntimeContextGetUniqueKey(context, "substr");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmDataClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmData * data = (vmData *) object;
    if(data->bytes){
        buffer_dealloc(data->bytes);
        data->bytes = NULL;
    }
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmDataClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmData * data = (vmData *) object;
    
    if(data->uniqueKeys.length == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = buffer_length(data->bytes);
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmDataClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    
    vmData * data = (vmData *) object;
    
    if(data->uniqueKeys.length == name){
        buffer_length_set(data->bytes, vmVariantToInt32(context, value));
    }
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmDataClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmData * data = (vmData *) object;
    
    if(data->uniqueKeys.charAt == name){
        hint32 index = vmVariantToInt32(context, vmVariantListGet(args, 0));
        if(index >=0 && index < buffer_length(data->bytes)){
            rs.type = vmVariantTypeInt32;
            rs.value.int32Value = * ((hubyte *) buffer_data(data->bytes) + index);
        }
    }
    else if(data->uniqueKeys.append == name){
        {
            vmVariant var =vmVariantListGet(args, 0);
            vmData * varData;
            if((var.type & vmVariantTypeObject)
            		&& var.value.objectValue
            		&& vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmDataClass)){
                varData = (vmData *)var.value.objectValue;
                buffer_append(data->bytes,buffer_data(varData->bytes),buffer_length(varData->bytes));
            }
            else{
                vmVariantToString(context, var, data->bytes);
            }
        }
    }
    else if(data->uniqueKeys.appendFormat == name){
        {
            hbuffer_t fmt = buffer_alloc(128, 128);
            hbuffer_t op = buffer_alloc(32, 32);
            hint32 i = 0,s = 0;
            hchar * p;
            hchar value[128];
            hint64 int64Value;
            hintptr intptrValue;
            hint32 int32Value;
            hfloat floatValue;
            hdouble doubleValue;
            hchar charValue;
            
            vmVariantToString(context, vmVariantListGet(args, i), fmt);
            
            i ++;
            
            p = (hchar *)buffer_to_str(fmt);
            
            while(*p != 0){
                
                switch (s) {
                    case 0:
                    {
                        if( *p == '%'){
                            s = 1;
                            buffer_clear(op);
                            buffer_append(op, p, 1);
                        }
                        else{
                            buffer_append(data->bytes, p, 1);
                        }
                    }
                        break;
                    case 1:
                        if( (*p >= '0' && *p <='9') || *p =='.'){
                            buffer_append(op, p, 1);
                        }
                        else if( p[0] == '%' ){
                            s = 0;
                            buffer_append(data->bytes, p, 1);
                        }
                        else if( p[0] == 'l' && p[1] == 'l' && p[2] == 'd'){
                            
                            buffer_append(op, p, 3);
                            
                            int64Value = vmVariantToInt64(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),int64Value);
                            
                            buffer_append_str(data->bytes, value);
                            
                            p += 2;
                        }
                        else if( p[0] == 'l' && p[1] == 'd'){
                            
                            buffer_append(op, p, 2);
                            
                            intptrValue = (hintptr)vmVariantToInt64(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),intptrValue);
                            
                            buffer_append_str(data->bytes, value);
                            
                            p += 1;
                        }
                        else if( p[0] == 'd' ){
                            
                            buffer_append(op, p, 1);
                            
                            int32Value = vmVariantToInt32(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),int32Value);
                            
                            buffer_append_str(data->bytes, value);
                            
                        }
                        else if( p[0] == 'l' && p[1] == 'f' ){
                            
                            buffer_append(op, p, 1);
                            
                            doubleValue = vmVariantToDouble(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),doubleValue);
                            
                            buffer_append_str(data->bytes, value);
                            
                            p += 1;
                            
                        }
                        else if( p[0] == 'f' ){
                            
                            buffer_append(op, p, 1);
                            
                            floatValue = vmVariantToDouble(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),floatValue);
                            
                            buffer_append_str(data->bytes, value);
                            
                        }
                        else if( p[0] == 'c' ){
                            
                            buffer_append(op, p, 1);
                            
                            charValue = vmVariantToInt16(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),charValue);
                            
                            buffer_append_str(data->bytes, value);
                            
                        }
                        else if( p[0] == 'x' || p[0] == 'X' || p[0] == 'o' || p[0] =='O'){
                            
                            buffer_append(op, p, 1);
                            
                            int32Value = vmVariantToInt32(context, vmVariantListGet(args,i));
                            
                            i++;
                            
                            sprintf(value, buffer_to_str(op),int32Value);
                            
                            buffer_append_str(data->bytes, value);
                            
                        }
                        else{
                            s = 0;
                            buffer_append(op, p, 1);
                            buffer_append(data->bytes, buffer_data(op), buffer_length(op));
                        }
                        break;
                    default:
                        break;
                }
                
                p ++;
            }
            
            buffer_dealloc(op);
            buffer_dealloc(fmt);
        }
    }
    else if(data->uniqueKeys.clear == name){
        buffer_clear(data->bytes);
    }
    else if(data->uniqueKeys.removeLastPath == name){
        buffer_path_last_remove(data->bytes);
    }
    else if(data->uniqueKeys.lastPath == name){
        rs.type = vmVariantTypeString;
        rs.value.stringValue = buffer_path_last(data->bytes);
    }
    else if(data->uniqueKeys.indexOf == name){
        {
            hbuffer_t s = buffer_alloc(64, 64);
            hchar * p;
            hchar * b;
            
            vmVariantToString(context, vmVariantListGet(args, 0), s);
            
            b = (hchar *)buffer_to_str(data->bytes) + vmVariantToInt32(context,vmVariantListGet(args, 1));
            
            p = str_find_sub(b, buffer_to_str(s));
            
            if(p){
                rs.type = vmVariantTypeInt32;
                rs.value.int32Value = (hint32) (p - buffer_data(data->bytes));
            }
            else{
                rs.type = vmVariantTypeInt32;
                rs.value.int32Value = -1;
            }
            
            buffer_dealloc(s);
        }
    }
    else if(data->uniqueKeys.substr == name){
        {
            hint32 index = vmVariantToInt32(context, vmVariantListGet(args, 0));
            hint32 slen = buffer_length(data->bytes);
            hint32 len = slen - index;
            
            if(vmVariantListCount(args) > 1){
                len = vmVariantToInt32(context, vmVariantListGet(args,1));
                if(index + len > slen){
                    len = slen - index;
                }
            }
            
            if(index < slen && len >0){
                rs = vmStringAllocWithLength(context, buffer_data(data->bytes) + index, len,InvokeTickArg);
            }
        }
    }
    else if(context->systemKeys.functionToStringKey == name){
        rs = vmStringAlloc(context, buffer_to_str(data->bytes),InvokeTickArg);
    }
    else if(context->systemKeys.functionEqualKey == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = hbool_false;
        {
            hbuffer_t str = buffer_alloc(128, 128);
            vmVariant var = vmVariantListGet(args, 0);
            vmData * varData;
            if(var.type & vmVariantTypeObject && var.value.objectValue && vmRuntimeContextObjectIsKindClass(context, var.value.objectValue, (vmClass *)&vmDataClass)){
                varData = (vmData *)var.value.objectValue;
                rs.value.booleanValue = mem_strcmp(buffer_to_str(varData->bytes), buffer_to_str(data->bytes)) ==0;
            }
            else{
                str = buffer_alloc(128, 128);
                vmVariantToString(context, var, data->bytes);
                rs.value.booleanValue = mem_strcmp(buffer_to_str(str), buffer_to_str(data->bytes)) ==0;
                
                buffer_dealloc(str);
            }
        }
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmDataClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmData * data = (vmData *) object;
    
    vmUniqueKeyListAdd(names, data->uniqueKeys.length);
}


vmClassBase vmDataClass = {vmClassTypeBase,sizeof(vmData),vmDataClassInitCallback,vmDataClassDestroyCallback,vmDataClassGetPropertyCallback,vmDataClassSetPropertyCallback,vmDataClassPropertyNamesCallback,vmDataClassInvokeCallback};

vmVariant vmDataAlloc(vmRuntimeContext context,hbyte * data,hint32 length,InvokeTickDeclare){
    vmVariantList args = vmVariantListAlloc();
    vmVariant var = {vmVariantTypeInt32,0};
    vmVariant rs = {vmVariantTypeVoid,0};
    var.value.int32Value = length;
    
    vmVariantListAdd(args, var);
    
    var.value.int32Value = 128;
    vmVariantListAdd(args,var);
    
    if(data && length >0){
        var.type = vmVariantTypeString;
        var.value.stringValue = data;
        
        vmVariantListAdd(args,var);
        
        var.type = vmVariantTypeInt32;
        var.value.int32Value = length;
        
        vmVariantListAdd(args, var);
    }
    
    rs = vmObjectAlloc(context, (vmClass *)&vmDataClass, args);
    
    vmVariantListDealloc(args);
    
    return rs;
    
}

void vmDataAppend(vmRuntimeContext context,vmObject * object,hbyte * data,hint32 length,InvokeTickDeclare){
    
    vmData * d = (vmData *) object;
    
    buffer_append(d->bytes,data,length);
}

hbyte * vmDataDataPtr(vmRuntimeContext context,vmObject * object,InvokeTickDeclare){
    vmData * d = (vmData *) object;
    
    return buffer_data(d->bytes);
}

hint32 vmDataDataLength(vmRuntimeContext context,vmObject * object,InvokeTickDeclare){
    vmData * d = (vmData *) object;
    
    return buffer_length(d->bytes);
}



typedef struct{
    vmObject base;
    struct{
        vmUniqueKey srand;
        vmUniqueKey rand;
        vmUniqueKey sin;
        vmUniqueKey cos;
        vmUniqueKey tan;
        vmUniqueKey asin;
        vmUniqueKey acos;
        vmUniqueKey atan;
        vmUniqueKey abs;
        vmUniqueKey sqrt;
        vmUniqueKey log;
        vmUniqueKey log2;
        vmUniqueKey log10;
    }uniqueKeys;
}vmMath;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmMathClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmMath * vmSys = (vmMath *) object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    
    vmSys->uniqueKeys.srand = vmRuntimeContextGetUniqueKey(context, "srand");
    vmSys->uniqueKeys.rand = vmRuntimeContextGetUniqueKey(context, "rand");
    vmSys->uniqueKeys.sin = vmRuntimeContextGetUniqueKey(context, "sin");
    vmSys->uniqueKeys.cos = vmRuntimeContextGetUniqueKey(context, "cos");
    vmSys->uniqueKeys.tan = vmRuntimeContextGetUniqueKey(context, "tan");
    vmSys->uniqueKeys.asin = vmRuntimeContextGetUniqueKey(context, "asin");
    vmSys->uniqueKeys.acos = vmRuntimeContextGetUniqueKey(context, "acos");
    vmSys->uniqueKeys.atan = vmRuntimeContextGetUniqueKey(context, "atan");
    vmSys->uniqueKeys.abs = vmRuntimeContextGetUniqueKey(context, "abs");
    vmSys->uniqueKeys.sqrt = vmRuntimeContextGetUniqueKey(context, "sqrt");
    vmSys->uniqueKeys.log = vmRuntimeContextGetUniqueKey(context, "log");
    vmSys->uniqueKeys.log2 = vmRuntimeContextGetUniqueKey(context, "log2");
    vmSys->uniqueKeys.log10 = vmRuntimeContextGetUniqueKey(context, "log10");


    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmMathClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmMathClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmMathClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}


/**
 
 Return : any
 */
static vmVariant vmMathClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmMath * vmSys = (vmMath *) object;
    if(vmSys->uniqueKeys.rand == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = rand();
    }
    else if(vmSys->uniqueKeys.srand == name){
        srand(vmVariantToInt32(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.sin == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = sin(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.cos == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = cos(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.tan == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = tan(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.asin == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = asin(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.acos == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = acos(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.atan == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = atan(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.abs == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = fabs(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.sqrt == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = sqrt(vmVariantToDouble(context, vmVariantListGet(args,0)));
    }
    else if(vmSys->uniqueKeys.log == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = log(vmVariantToDouble(context, vmVariantListGet(args, 0)));
    }
    else if(vmSys->uniqueKeys.log2 == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = log2(vmVariantToDouble(context, vmVariantListGet(args, 0)));
    }
    else if(vmSys->uniqueKeys.log10 == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = log10(vmVariantToDouble(context, vmVariantListGet(args, 0)));
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args, InvokeTickArg);
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmMathClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}


vmClassBase vmMathClass = {vmClassTypeBase,sizeof(vmMath),vmMathClassInitCallback,vmMathClassDestroyCallback,vmMathClassGetPropertyCallback,vmMathClassSetPropertyCallback,vmMathClassPropertyNamesCallback,vmMathClassInvokeCallback};



