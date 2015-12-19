//
//  hvermin_runtime.c
//  C Library
//
//  Created by hailong zhang on 11-6-3.
//  Copyright 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_runtime.h"
#include "hmem.h"
#include "hbuffer.h"
#include "hmap.h"
#include "hserial_map.h"
#include "hserial_list.h"
#include "hstr.h"
#include "hlist.h"
#include "hfile.h"
#include "hlog.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hvermin_debug.h"

#undef vmRuntimeContextGetVariant

#undef vmRuntimeContextSetVariant

#undef vmRuntimeContextRegisterUniqueKey

#undef vmRuntimeContextGetUniqueKey

#undef vmRuntimeContextGetUniqueKeyFromInteger

#undef vmRuntimeContextVariantToUniqueKey

#undef vmRuntimeContextUniqueKeyToString

#undef vmRuntimeContextUniqueKeyToInteger

#undef vmRuntimeContextGetClass

#undef vmRuntimeContextGetClassName

#undef vmRuntimeContextGetSuperClass

#undef vmRuntimeContextGetObjectPtrByClass

#undef vmRuntimeContextObjectIsKindClass

#undef vmRuntimeContextClassIsKindClass

#undef vmRuntimeContextGetClassTotalSize

#undef vmRuntimeContextGetClassSize

#undef vmRuntimeContextFunctionInitKey

#undef vmRuntimeContextFunctionDestroyKey

#undef vmRuntimeContextFunctionToStringKey

#undef vmRuntimeContextFunctionToIntKey

#undef vmRuntimeContextFunctionToDoubleKey

#undef vmRuntimeContextOperatorIncKey

#undef vmRuntimeContextOperatorDecKey

#undef vmRuntimeContextOperatorAddKey

#undef vmRuntimeContextOperatorSubKey

#undef vmRuntimeContextOperatorMultiplicationKey

#undef vmRuntimeContextOperatorDivisionKey

#undef vmRuntimeContextOperatorModulusKey

#undef vmRuntimeContextOperatorBitwiseAndKey

#undef vmRuntimeContextOperatorBitwiseOrKey

#undef vmRuntimeContextOperatorBitwiseExclusiveOrKey

#undef vmRuntimeContextOperatorBitwiseComplementKey

#undef vmRuntimeContextOperatorBitwiseShiftLeftKey

#undef vmRuntimeContextOperatorBitwiseShiftRightKey

#undef vmRuntimeContextOperatorNotKey

#undef vmRuntimeContextOperatorAndKey

#undef vmRuntimeContextOperatorOrKey

#undef vmRuntimeContextOperatorGreaterKey

#undef vmRuntimeContextOperatorGreaterEqualKey

#undef vmRuntimeContextOperatorLessKey

#undef vmRuntimeContextOperatorLessEqualKey

#undef vmRuntimeContextOperatorEqualKey

#undef vmRuntimeContextOperatorNotEqual

#undef vmRuntimeContextOperatorKey

#undef vmRuntimeContextThisKey

#undef vmRuntimeContextSuperKey

#undef vmRuntimeContextArgumentsKey

#undef vmRuntimeContextAlloc

#undef vmRuntimeContextRetain

#undef vmRuntimeContextRelease

#undef vmRuntimeContextInvokeBegin

#undef vmRuntimeContextInvokeEnd

#undef vmRuntimeContextVariantRetain

#undef vmRuntimeContextVariantRelease

#undef vmRuntimeContextException

#undef vmRuntimeContextUniqueKeyToVariant

#undef vmRuntimeContextVariantListToArrayObject

#undef vmRuntimeContextDomainAddObject


#undef vmRuntimeClassLoaderAlloc

#undef vmRuntimeClassLoaderRetain

#undef vmRuntimeClassLoaderRelease

#undef vmRuntimeClassLoaderGetClass

#undef vmRuntimeClassLoaderGetClassName

#undef vmRuntimeClassLoaderGetMainClass

#undef vmRuntimeClassLoaderRegister

#undef vmRuntimeClassLoaderLoadBytes

#undef vmRuntimeClassLoaderLoadBytesNotCopy

#undef vmRuntimeClassLoaderLoadFile


#undef vmObjectAlloc

#undef vmObjectRetain

#undef vmObjectRelease

#undef vmObjectGetProperty

#undef vmObjectSetProperty

#undef vmObjectPropertyNames

#undef vmObjectInvoke

/**
 
 
 vmRuntimeContext private functions
 */

void vmRuntimeContextInvokeBegin(vmRuntimeContext context,InvokeTickDeclare);

void vmRuntimeContextInvokeEnd(vmRuntimeContext context,InvokeTickDeclare);

void vmRuntimeContextDomainAddObject(vmRuntimeContext context,vmObject * obj,InvokeTickDeclare);

void vmRuntimeContextSuperDomainAddObject(vmRuntimeContext context,vmObject * obj,InvokeTickDeclare);




hbool vmRuntimeContextHasVariant(vmRuntimeContext context,vmUniqueKey name,InvokeTickDeclare);

hbool vmRuntimeClassLoaderUniqueKeysRegister(vmRuntimeClassLoader loader,vmRuntimeContext context,InvokeTickDeclare);


#define vmRuntimeMetaOperatorGet(classMeta,operatorOffset) (vmRuntimeMetaOperator *) ( (hbyte *) (classMeta) - (classMeta)->offset + (operatorOffset) )

#define vmRuntimeMetaByOperator(op,index)   (vmRuntimeMeta *) ( (hbyte *)(op) + sizeof(vmRuntimeMetaOperator) + sizeof(vmRuntimeMeta) * (index))

/**
 
 -------------------------------
 */


vmVariant vmRuntimeContextException(vmRuntimeContext context,hint32 code,hcchar * message,InvokeTickDeclare){
    vmVariant var = {vmVariantTypeObject,0};
    vmVariant arg0 = {vmVariantTypeInt32,0};
    vmVariant arg1 = {vmVariantTypeString,0};
    vmVariantList args = vmVariantListAlloc();

    arg0.value.int32Value = code;
    arg1.value.stringValue = (hchar *)message;
    

    
    vmVariantListAdd(args, arg0);
    vmVariantListAdd(args, arg1);
    
    var = vmObjectAlloc(context,(vmClass *) &vmExceptionClass, args,InvokeTickArg);
    
    var.type = var.type | vmVariantTypeThrowable;
    
    vmVariantListDealloc(args);
    
    return var;
}

vmVariant vmRuntimeContextExceptionFormat(vmRuntimeContext context,hint32 code,hcchar * format,InvokeTickDeclare,...){
    vmVariant rs = {vmVariantTypeVoid,0};
    hbuffer_t buf = buffer_alloc(64, 64);
    va_list va;
    
    va_start(va, __InvokeTick__);
    
    buffer_append_format_va_list(buf, format, va);
    
    va_end(va);
    
    rs = vmRuntimeContextException(context,code,buffer_to_str(buf),InvokeTickArg);
    
    buffer_dealloc(buf);
    
    return rs;
}

vmVariant vmRuntimeContextUniqueKeyToVariant(vmRuntimeContext context,vmUniqueKey key,InvokeTickDeclare){
    vmVariant var = {vmVariantTypeString,0};
    var.value.stringValue = vmRuntimeContextUniqueKeyToString(context, key,InvokeTickArg);
    return var;
}

vmVariant vmRuntimeContextVariantListToArrayObject(vmRuntimeContext context,vmVariantList vars,InvokeTickDeclare){
    return vmObjectAlloc(context, (vmClass *)& vmArrayClass, vars,InvokeTickArg);
}

void vmRuntimeContextVariantRetain(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    assert(context);
    if(var.type & vmVariantTypeObject){
        vmObjectRetain(context, var.value.objectValue,InvokeTickArg);
    }
}

void vmRuntimeContextVariantRelease(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    assert(context);
    if(var.type & vmVariantTypeObject){
        vmObjectRelease(context, var.value.objectValue,InvokeTickArg);
    }
}

static vmUniqueKey vmRuntimeContextGetUniqueKeyByMetaClass(vmRuntimeContext context,vmClassMeta * classMeta,vmClassMetaOffset offset,InvokeTickDeclare){
    if(offset == 0){
        return 0;
    }
    return vmRuntimeContextGetUniqueKey(context, (hcchar *)((hbyte *)classMeta - classMeta->offset + offset),InvokeTickArg);
}

static hbool vmRuntimeContextEqualUniqueKeyByMetaClass(vmRuntimeContext context,vmClassMeta * classMeta,vmClassMetaOffset offset,vmUniqueKey uniqueKey,InvokeTickDeclare){
    vmRuntimeClassLibraryBytes * bytes = (vmRuntimeClassLibraryBytes *)((hbyte *) classMeta - classMeta->offset) ;
    if(uniqueKey > (huintptr)bytes && uniqueKey < (huintptr)bytes + bytes->length){
        return (huintptr) bytes + offset == uniqueKey;
    }
    return vmRuntimeContextGetUniqueKeyByMetaClass(context,classMeta,offset,InvokeTickArg) == uniqueKey;
}

vmClass * vmRuntimeContextGetSuperClass(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare){
    assert(context && clazz);
    if(clazz->type == vmClassTypeBase){
        return NULL;
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta * classMeta = (vmClassMeta *)clazz;
        if(classMeta->superClass){
            return vmRuntimeContextGetClass(context, vmRuntimeContextGetUniqueKeyByMetaClass(context,classMeta,classMeta->superClass,InvokeTickArg),InvokeTickArg);
        }
        return NULL;
    }
    else{
        assert(0);
    }
}

hint32 vmRuntimeContextGetClassTotalSize(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare){
    assert(context && clazz);
    if(clazz->type == vmClassTypeBase){
        return ((vmClassBase *)clazz)->size;
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta * metaClass = (vmClassMeta *)clazz;
        hint32 size =  metaClass->propertys * sizeof(vmVariant);
        if(metaClass->superClass){
            size += vmRuntimeContextGetClassTotalSize(context,vmRuntimeContextGetSuperClass(context,clazz,InvokeTickArg),InvokeTickArg);
        }
        return size;
    }
    assert(0);
}

hint32 vmRuntimeContextGetClassSize(vmRuntimeContext context,vmClass * clazz,InvokeTickDeclare){
    assert(context && clazz);
    if(clazz->type == vmClassTypeBase){
        return ((vmClassBase *)clazz)->size;
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta * metaClass = (vmClassMeta *)clazz;
        return metaClass->propertys * sizeof(vmVariant);
    }
    assert(0);
}

hany vmRuntimeContextGetObjectPtrByClass(vmRuntimeContext context,vmObject * object,vmClass * clazz,InvokeTickDeclare){
	hint32 size,totalSize;
    assert(context && object && clazz && vmRuntimeContextObjectIsKindClass(context,object,clazz,InvokeTickArg));
    size = vmRuntimeContextGetClassSize(context, clazz,InvokeTickArg);
    totalSize= vmRuntimeContextGetClassTotalSize(context, clazz,InvokeTickArg);
    assert( size >=0 && totalSize >=0 && size < totalSize);
    return size >0 ? (hbyte *)object + totalSize - size : NULL;
}

hbool vmRuntimeContextObjectIsKindClass(vmRuntimeContext context,vmObject * object,vmClass * clazz,InvokeTickDeclare){
    vmClass * pClass;
	assert(context && object && clazz);
    pClass = object->vmClass; 
    while(pClass){
        if(pClass == clazz){
            return hbool_true;
        }
        pClass = vmRuntimeContextGetSuperClass(context, pClass,InvokeTickArg);
    }
    
    return hbool_false;
}

hbool vmRuntimeContextClassIsKindClass(vmRuntimeContext context,vmClass * clazz,vmClass * ofClazz,InvokeTickDeclare){
    vmClass * pClass = clazz;
    assert(context && clazz && ofClazz);
 
    while(pClass){
        if(pClass == ofClazz){
            return hbool_true;
        }
        pClass = vmRuntimeContextGetSuperClass(context, pClass,InvokeTickArg);
    }
    return hbool_false;
}


static vmRuntimeMetaOperator * vmRuntimeContextFindProperty(vmRuntimeContext context,vmClassMeta * classMeta, vmUniqueKey name,hint32 * index,InvokeTickDeclare){
    vmRuntimeClassLibraryBytes * bytes = (vmRuntimeClassLibraryBytes *)((hbyte *) classMeta - classMeta->offset) ;
    vmClassMetaOffset * propertyBegin;
    hint32 i,bi,ei;
    vmRuntimeMetaOperator * op = NULL;
    vmClassMetaOffset off;
    
    assert(context && classMeta && name);

	propertyBegin = (vmClassMetaOffset *)((hbyte *) classMeta + sizeof(vmClassMeta));
    
    if(name > (huintptr)bytes && name < (huintptr)bytes + bytes->length){
        bi = 0;
        ei = classMeta->propertys - 1;
        off = (vmClassMetaOffset) (name - (huintptr)bytes);
        while(bi <= ei){
            i = (bi + ei) / 2;
            op = (vmRuntimeMetaOperator *)((hbyte *) bytes + propertyBegin[i]);
            if(op->uniqueKey == off){
                * index = i;
                return op;
            }
            else if(off > op->uniqueKey){
                bi = i +1;
            }
            else {
                ei = i -1;
            }
        }
    }
    else{
        for(i=0;i<classMeta->propertys;i++){
            op = (vmRuntimeMetaOperator *)((hbyte *) bytes + propertyBegin[i]);
            if(vmRuntimeContextEqualUniqueKeyByMetaClass(context,classMeta,op->uniqueKey,name,InvokeTickArg)){
                *index = i;
                return op;
            }
        }
    }
    return NULL;
}

static vmRuntimeMetaOperator * vmRuntimeContextFindFunction(vmRuntimeContext context,vmClassMeta * classMeta, vmUniqueKey name,InvokeTickDeclare){
    vmRuntimeClassLibraryBytes * bytes = (vmRuntimeClassLibraryBytes *)((hbyte *) classMeta - classMeta->offset) ;
    vmClassMetaOffset * functionBegin;
    hint32 i,bi,ei;
    vmRuntimeMetaOperator * op = NULL;
    vmClassMetaOffset off;
	assert(context && classMeta && name);

	functionBegin = (vmClassMetaOffset *)((hbyte *) classMeta + sizeof(vmClassMeta) + sizeof(vmClassMetaOffset) * classMeta->propertys);
    
    if(name > (huintptr)bytes && name < (huintptr)bytes + bytes->length){
        bi = 0;
        ei = classMeta->functions - 1;
        off = (vmClassMetaOffset)( name - (huintptr)bytes );
        while(bi <= ei){
            i = (bi + ei) / 2;
            op = (vmRuntimeMetaOperator *)((hbyte *) bytes + functionBegin[i]);
            if(op->uniqueKey == off){
                return op;
            }
            else if(off > op->uniqueKey){
                bi = i +1;
            }
            else {
                ei = i -1;
            }
        }
    }
    else{
        for(i=0;i<classMeta->functions;i++){
            op = (vmRuntimeMetaOperator *)((hbyte *) bytes + functionBegin[i]);
            if(vmRuntimeContextEqualUniqueKeyByMetaClass(context,classMeta,op->uniqueKey,name,InvokeTickArg)){
                return op;
            }
        }
    }
    return NULL;
}

vmVariant vmRuntimeContextOperator(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,InvokeTickDeclare);
vmVariant vmRuntimeContextFunction(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,vmVariantList args,InvokeTickDeclare);
vmVariant vmRuntimeContextInvoke(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,InvokeTickDeclare);

static vmVariant vmRuntimeContextRuntimeMetaToVariant(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject,vmRuntimeMeta * meta,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
	assert(context && classMeta && meta);
    if(meta->type & vmRuntimeMetaTypeOperator){
        rs = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
        rs.type = rs.type | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
    }
    else{
        if(meta->type & vmRuntimeMetaTypeInt16){
            rs.type = vmRuntimeMetaTypeInt16 | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            rs.value.int16Value = meta->value.int16Value;
        }
        else if(meta->type & vmRuntimeMetaTypeInt32){
            rs.type = vmRuntimeMetaTypeInt32 | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            rs.value.int32Value = meta->value.int32Value;
        }
        else if(meta->type & vmRuntimeMetaTypeInt64){
            rs.type = vmRuntimeMetaTypeInt64 | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            rs.value.int64Value = meta->value.int64Value;
        }
        else if(meta->type & vmRuntimeMetaTypeBoolean){
            rs.type = vmRuntimeMetaTypeBoolean | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            rs.value.booleanValue = meta->value.booleanValue;
        }
        else if(meta->type & vmRuntimeMetaTypeDouble){
            rs.type = vmRuntimeMetaTypeDouble | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            rs.value.doubleValue = meta->value.doubleValue;
        }
        else if(meta->type & vmRuntimeMetaTypeString){
            if(meta->value.stringKey){
                rs.type = vmRuntimeMetaTypeString | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
                rs.value.stringValue = vmRuntimeContextUniqueKeyToString(context, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, meta->value.stringKey,InvokeTickArg),InvokeTickArg);
            }
            else{
                rs.type =vmRuntimeMetaTypeString | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
                rs.value.stringValue = NULL;
            }
        }
        else if(meta->type & vmRuntimeMetaTypeObject){
            if(meta->value.objectKey){
                rs = vmRuntimeContextGetVariant(context, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, meta->value.objectKey,InvokeTickArg),InvokeTickArg);
                rs.type = rs.type | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
            }
            else{
                rs.type = vmRuntimeMetaTypeObject | (meta->type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeCatch | vmRuntimeMetaTypeFinally));
                rs.value.objectValue = NULL;
            }
        }
    }
    return rs;
}

vmVariant vmRuntimeContextOperator(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
	assert(context && classMeta && op);
    
    vmRuntimeDebugExec(vmRuntimeContextGetDebug(context, InvokeTickArg), classMeta, op, InvokeTickArg);
    
    if(op->type == vmRuntimeOperatorTypeFunction){
        assert(op->uniqueKey && op->metaCount >0);
		{
			hint32 i;
			vmRuntimeMeta * meta = NULL;
			vmVariantList vars = vmVariantListAlloc();
			vmVariant obj = {vmVariantTypeVoid,0};
			vmClass * objClass = (vmClass *)&vmObjectClass;
        
			meta = vmRuntimeMetaByOperator(op,0);
        
			obj = vmRuntimeContextRuntimeMetaToVariant(context ,classMeta,vmObject,meta,InvokeTickArg);
        
			if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj;
			}
        
			obj = vmVariantToObject(context, obj);
        
			for(i=1;i<op->metaCount;i++){
				meta = vmRuntimeMetaByOperator(op,i);
				vmVariantListAdd(vars, vmRuntimeContextRuntimeMetaToVariant(context ,classMeta,vmObject,meta,InvokeTickArg));
			}

			if(obj.type & vmRuntimeMetaTypeSuperObject){
				objClass = obj.vmClass;
			}
			else{
				objClass = obj.value.objectValue->vmClass;
			}
        
			rs = vmObjectInvoke(context, objClass, obj.value.objectValue, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg), vars,InvokeTickArg);
        
			vmVariantListDealloc(vars);
		}
    }
    else if(op->type == vmRuntimeOperatorTypeNew){
        assert(op->uniqueKey);
		{
			hint32 i;
			vmRuntimeMeta * meta = NULL;
			vmVariantList vars = vmVariantListAlloc();
			vmClass * objClass = (vmClass *)&vmObjectClass;
            vmUniqueKey uniqueKey = vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg);
			objClass =vmRuntimeContextGetClass(context, uniqueKey, InvokeTickArg);
        
			if(objClass == NULL){
				vmVariantListDealloc(vars);
				return vmRuntimeContextExceptionFormat(context, 0, "[new] not found class %s",InvokeTickArg,vmRuntimeContextUniqueKeyToString(context, uniqueKey, InvokeTickArg));
			}
        
			for(i=0;i<op->metaCount;i++){
				meta = vmRuntimeMetaByOperator(op,i);
				vmVariantListAdd(vars, vmRuntimeContextRuntimeMetaToVariant(context ,classMeta,vmObject,meta,InvokeTickArg));
			}
    
			rs = vmObjectAlloc(context, objClass, vars,InvokeTickArg);
        
			vmVariantListDealloc(vars);
		}
    }
    else if(op->type == vmRuntimeOperatorTypeInvoke){
        rs = vmRuntimeContextInvoke(context,classMeta,vmObject,op,InvokeTickArg);
    }
    else if(op->type == vmRuntimeOperatorTypeProperty){
        assert(op->metaCount ==1 || op->metaCount ==2 || op->metaCount ==3);
		{
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op,0);
			vmVariant obj = {vmVariantTypeVoid,0};
			vmClass * objClass = (vmClass *)&vmObjectClass;
			vmUniqueKey uniqueKey = 0;
			if(meta->type & vmRuntimeMetaTypeOperator){
				obj = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
				if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}

			}
			else{
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
			}
        
			obj = vmVariantToObject(context, obj);
        
			if(obj.type & vmRuntimeMetaTypeSuperObject){
				objClass = obj.vmClass;
			}
			else{
				objClass = obj.value.objectValue->vmClass;
			}
        
			if(op->uniqueKey){
				uniqueKey = vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg);
			}
			else{
				uniqueKey = vmRuntimeContextVariantToUniqueKey(context, vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,vmRuntimeMetaByOperator(op,1),InvokeTickArg), InvokeTickArg);
			}
        
			if((op->uniqueKey && op->metaCount ==1) || (!op->uniqueKey && op->metaCount ==2)){

				rs = vmObjectGetProperty(context, objClass, obj.value.objectValue,uniqueKey ,InvokeTickArg);
			}
			else{
				rs = vmObjectSetProperty(context, objClass, obj.value.objectValue,uniqueKey, vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,vmRuntimeMetaByOperator(op,op->uniqueKey ? 1:2),InvokeTickArg),InvokeTickArg);
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeDec 
            || op->type == vmRuntimeOperatorTypeBeforeInc || op->type == vmRuntimeOperatorTypeBeforeDec){
        assert(op->metaCount == 0 || op->metaCount == 1);
		{
			vmVariant obj = {vmVariantTypeVoid,0},v;
			vmUniqueKey name;
			if(op->metaCount ==0){
				name = vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg);
				obj = vmRuntimeContextGetVariant(context, name,InvokeTickArg);
				if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}
            
				if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeDec){
					rs = obj;
				}
            
				obj.value.int64Value = vmVariantToInt16(context, obj);
				if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeBeforeInc){
					obj.value.int64Value ++;
				}
				else{
					obj.value.int64Value --;
				}
				obj.type = vmVariantTypeInt64;
            
				vmRuntimeContextSetVariant(context, name, obj, InvokeTickArg);
            
				if(op->type == vmRuntimeOperatorTypeBeforeInc || op->type == vmRuntimeOperatorTypeBeforeDec){
					rs = obj;
				}
            
				return rs;
			}
			else {
				vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op,0);
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
				if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}
            
				if(op->uniqueKey && (obj.type & vmVariantTypeObject) && obj.value.objectValue){
					name = vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg);
					v.value.int64Value = vmVariantToInt64(context, vmObjectGetProperty(context, obj.value.objectValue->vmClass, obj.value.objectValue, name, InvokeTickArg));
					v.type = vmVariantTypeInt64;
                
					if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeDec){
						rs = v;
					}
                
					if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeBeforeInc){
						v.value.int64Value ++;
					}
					else{
						v.value.int64Value --;
					}
                
					if(op->type == vmRuntimeOperatorTypeBeforeInc || op->type == vmRuntimeOperatorTypeBeforeDec){
						rs = v;
					}
                
					obj = vmObjectSetProperty(context, obj.value.objectValue->vmClass, obj.value.objectValue, name, v, InvokeTickArg);
                
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
					return rs;
				}
				else{
					v.value.int64Value = vmVariantToInt64(context, obj);
					v.type = vmVariantTypeInt64;
					if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeDec){
						rs = v;
					}
                
					if(op->type == vmRuntimeOperatorTypeInc || op->type == vmRuntimeOperatorTypeBeforeInc){
						v.value.int64Value ++;
					}
					else{
						v.value.int64Value --;
					}
                
					if(op->type == vmRuntimeOperatorTypeBeforeInc || op->type == vmRuntimeOperatorTypeBeforeDec){
						rs = obj;
					}
					return rs;
				}
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeAdd 
            || op->type == vmRuntimeOperatorTypeSub 
            || op->type == vmRuntimeOperatorTypeMultiplication 
            || op->type == vmRuntimeOperatorTypeDivision 
            || op->type == vmRuntimeOperatorTypeModulus
            || op->type == vmRuntimeOperatorTypeBitwiseAnd
            || op->type == vmRuntimeOperatorTypeBitwiseOr
            || op->type == vmRuntimeOperatorTypeBitwiseExclusiveOr
            || op->type == vmRuntimeOperatorTypeBitwiseShiftLeft
            || op->type == vmRuntimeOperatorTypeBitwiseShiftRight
            || op->type == vmRuntimeOperatorTypeGreater
            || op->type == vmRuntimeOperatorTypeGreaterEqual
            || op->type == vmRuntimeOperatorTypeLess
            || op->type == vmRuntimeOperatorTypeLessEqual
            || op->type == vmRuntimeOperatorTypeEqual
            || op->type == vmRuntimeOperatorTypeNotEqual
            || op->type == vmRuntimeOperatorTypeAbsEqual
            || op->type == vmRuntimeOperatorTypeAbsNotEqual)
    {
        assert(op->metaCount == 2);
		{
			vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op,0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op,1);
			vmVariant obj1,obj2;
			vmVariantType lowType = vmVariantTypeVoid,highType = vmVariantTypeVoid;
			hbuffer_t str1 = buffer_alloc(128, 128);
			hbuffer_t str2 = buffer_alloc(128,128);
        
			obj1 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta1,InvokeTickArg);
        
			if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj1;
			}
        
			obj2 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
        
			if(obj2.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj2;
			}
        
			vmVariantTypeFor(obj1,obj2,&lowType,&highType);
        
			rs.type = vmVariantTypeVoid;
        
			switch(op->type){
				case vmRuntimeOperatorTypeAdd:
					if(highType ==vmVariantTypeString || highType == vmVariantTypeObject){
						buffer_clear(str1);
						vmVariantToString(context,obj1,str1);
						vmVariantToString(context,obj2, str1);
						rs = vmStringAlloc(context,buffer_to_str(str1));
					}
					else if(highType == vmVariantTypeDouble){
						rs.type = vmVariantTypeDouble;
						rs.value.doubleValue = vmVariantToDouble(context,obj1) + vmVariantToDouble(context,obj2);
					}
					else{
						rs.type = vmVariantTypeInt64;
						rs.value.int64Value = vmVariantToInt64(context,obj1) + vmVariantToInt64(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeSub:
					if(highType == vmVariantTypeDouble || highType == vmVariantTypeString|| highType == vmVariantTypeObject){
						rs.type = vmVariantTypeDouble;
						rs.value.doubleValue = vmVariantToDouble(context,obj1) - vmVariantToDouble(context,obj2);
					}
					else{
						rs.type = vmVariantTypeInt64;
						rs.value.int64Value = vmVariantToInt64(context,obj1) - vmVariantToInt64(context,obj2);
					}
                    break;
				case vmRuntimeOperatorTypeMultiplication:
					if(highType == vmVariantTypeDouble || highType == vmVariantTypeString || highType == vmVariantTypeObject){
						rs.type = vmVariantTypeDouble;
						rs.value.doubleValue = vmVariantToDouble(context,obj1) * vmVariantToDouble(context,obj2);
					}
					else{
						rs.type = vmVariantTypeInt64;
						rs.value.int64Value = vmVariantToInt64(context,obj1) * vmVariantToInt64(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeDivision:
					if(highType == vmVariantTypeDouble || highType == vmVariantTypeString || highType == vmVariantTypeObject){
						rs.type = vmVariantTypeDouble;
						rs.value.doubleValue =  vmVariantToDouble(context,obj2);
                    
						if(rs.value.doubleValue ==0){
                            rs.type = vmVariantTypeBoolean;
                            rs.value.booleanValue = hbool_false;
						}
						else{
							rs.value.doubleValue =vmVariantToDouble(context,obj1) / rs.value.doubleValue;
						}
					}
					else{
						rs.type = vmVariantTypeInt64;
						rs.value.int64Value = vmVariantToInt64(context,obj2);
                    
						if(rs.value.int64Value ==0){
                            rs.type = vmVariantTypeBoolean;
                            rs.value.booleanValue = hbool_false;
						}
						else{
							rs.value.int64Value =vmVariantToInt64(context,obj1) / rs.value.int64Value;
						}
					}
					break;
				case vmRuntimeOperatorTypeModulus:

					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj2);
                
					if(rs.value.int64Value ==0){
                        rs.type = vmVariantTypeBoolean;
                        rs.value.booleanValue = hbool_false;
					}
					else{
						rs.value.int64Value =vmVariantToInt64(context,obj1) % rs.value.int64Value;
					}
 
					break;
				case vmRuntimeOperatorTypeBitwiseAnd:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj1) & vmVariantToInt64(context,obj2);

					break;
				case vmRuntimeOperatorTypeBitwiseOr:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj1) | vmVariantToInt64(context,obj2);
					break;
				case vmRuntimeOperatorTypeBitwiseExclusiveOr:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj1) ^ vmVariantToInt64(context,obj2);
					break;
				case vmRuntimeOperatorTypeBitwiseShiftLeft:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj1) << vmVariantToInt64(context,obj2);
					break;
				case vmRuntimeOperatorTypeBitwiseShiftRight:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = vmVariantToInt64(context,obj1) >> vmVariantToInt64(context,obj2);
					break;
				case vmRuntimeOperatorTypeGreater:
					rs.type = vmVariantTypeBoolean;
					if(highType == vmVariantTypeString || lowType == vmVariantTypeString){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) >0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) > vmVariantToDouble(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeGreaterEqual:
					rs.type = vmVariantTypeBoolean;
					if(highType == vmVariantTypeString || lowType == vmVariantTypeString){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) >=0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) >= vmVariantToDouble(context,obj2);
					}                
                    break;
				case vmRuntimeOperatorTypeLess:
					rs.type = vmVariantTypeBoolean;
					if(highType == vmVariantTypeString|| lowType == vmVariantTypeString){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) <0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) < vmVariantToDouble(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeLessEqual:
					rs.type = vmVariantTypeBoolean;
					if(highType == vmVariantTypeString || lowType == vmVariantTypeString){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) <=0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) <= vmVariantToDouble(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeAbsEqual:
					if(obj1.type != obj2.type){
						rs.type = vmVariantTypeBoolean;
						rs.value.booleanValue = hbool_false;
						break;
					}
				case vmRuntimeOperatorTypeEqual:
					rs.type = vmVariantTypeBoolean;
                    if(lowType == vmVariantTypeObject && highType == vmVariantTypeObject){
                        if(obj1.value.objectValue && obj2.value.objectValue){
                            {
                                vmVariantList vars = vmVariantListAlloc();
                                vmVariantListAdd(vars, obj2);
                                rs = vmObjectInvoke(context, obj1.value.objectValue->vmClass, obj1.value.objectValue, context->systemKeys.functionEqualKey,vars, InvokeTickArg);
                                vmVariantListDealloc(vars);
                            }

                        }
                        else{
                            rs.value.booleanValue = obj1.value.objectValue == obj2.value.objectValue;
                        }
                    }
                    else if(highType == vmVariantTypeString || highType == vmVariantTypeObject){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) ==0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) == vmVariantToDouble(context,obj2);
					}
					break;
				case vmRuntimeOperatorTypeAbsNotEqual:
					if(obj1.type != obj2.type){
						rs.type = vmVariantTypeBoolean;
						rs.value.booleanValue = hbool_true;
						break;
					}
				case vmRuntimeOperatorTypeNotEqual:
					rs.type = vmVariantTypeBoolean;
                    if(lowType == vmVariantTypeObject && highType == vmVariantTypeObject){
                        if(obj1.value.objectValue && obj2.value.objectValue){
                            {
                                vmVariantList vars = vmVariantListAlloc();
                                vmVariantListAdd(vars, obj2);
                                rs = vmObjectInvoke(context, obj1.value.objectValue->vmClass, obj1.value.objectValue, context->systemKeys.functionEqualKey,vars, InvokeTickArg);
                                if(rs.type & vmVariantTypeBoolean){
                                    rs.value.booleanValue = !rs.value.booleanValue;
                                }
                                vmVariantListDealloc(vars);
                            }

                        }
                        else{
                            rs.value.booleanValue = obj1.value.objectValue != obj2.value.objectValue;
                        }
                    }
					else if(highType == vmVariantTypeString || highType == vmVariantTypeObject){
						buffer_clear(str1);
						buffer_clear(str2);
						vmVariantToString(context, obj1, str1);
						vmVariantToString(context, obj2, str2);
						rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) !=0 ;
					}
					else{
						rs.value.booleanValue = vmVariantToDouble(context,obj1) != vmVariantToDouble(context,obj2);
					}
					break;
				default:
					assert(0);
			}
			buffer_dealloc(str1);
			buffer_dealloc(str2);
			return rs;
		}
    }
    else if(op->type == vmRuntimeOperatorTypeAnd
            || op->type == vmRuntimeOperatorTypeOr)
    {
        assert(op->metaCount == 2);
		{
			vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op,0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op,1);
			vmVariant obj1,obj2;
        
			rs.type = vmVariantTypeBoolean;
        
			obj1 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta1,InvokeTickArg);
        
			if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj1;
			}
        
			rs.value.booleanValue = vmVariantToBoolean(context,obj1);
        
			switch(op->type){
				case vmRuntimeOperatorTypeAnd:
					if(!rs.value.booleanValue){
						return rs;
					}
					break;
				case vmRuntimeOperatorTypeOr:
					if(rs.value.booleanValue){
						return rs;
					}
					break;
				default:
					assert(0);
			}
        
			obj2 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
        
			if(obj2.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj2;
			}

			switch(op->type){
				case vmRuntimeOperatorTypeAnd:
					rs.value.booleanValue = rs.value.booleanValue && vmVariantToBoolean(context,obj2);
					break;
				case vmRuntimeOperatorTypeOr:
					rs.value.booleanValue = rs.value.booleanValue || vmVariantToBoolean(context,obj2);
					break;
				default:
					assert(0);
			}


			return rs;
		}
    }
    else if(op->type == vmRuntimeOperatorTypeNot || op->type ==vmRuntimeOperatorTypeBitwiseComplement || op->type == vmRuntimeOperatorTypeAntiNumber){
        assert(op->metaCount == 0 && op->uniqueKey || op->metaCount == 1);
		{
			vmRuntimeMeta * meta;
			vmVariant obj = {vmVariantTypeVoid,0};
            
            if(op->metaCount == 0){
                obj = vmRuntimeContextGetVariant(context, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey, InvokeTickArg), InvokeTickArg);
            }
            else{
                meta = vmRuntimeMetaByOperator(op,0);
                if(meta->type & vmRuntimeMetaTypeOperator){
                    obj = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
                }
                else{
                    obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
                
                }
            }
        
			if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj;
			}
        
			switch (op->type) {
				case vmRuntimeOperatorTypeNot:
					rs.type = vmVariantTypeBoolean;
					rs.value.booleanValue = !vmVariantToBoolean(context, obj);
					break;
				case vmRuntimeOperatorTypeBitwiseComplement:
					rs.type = vmVariantTypeInt64;
					rs.value.int64Value = ~ vmVariantToInt64(context,obj);
					break;
				case vmRuntimeOperatorTypeAntiNumber:
					if(obj.type == vmVariantTypeDouble){
						rs.type = vmVariantTypeDouble;
						rs.value.doubleValue = - vmVariantToDouble(context,obj);
					}
					else{
						rs.type = vmVariantTypeInt64;
						rs.value.int64Value = - vmVariantToInt64(context,obj);
					}
					break;
				default:
					assert(0);
					break;
			}
			return rs;
		}
    }
    else if(op->type == vmRuntimeOperatorTypeAssign){
        assert( (op->uniqueKey && (op->metaCount ==1 || op->metaCount ==2)) || (!op->uniqueKey && op->metaCount ==3));
        {
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op,0);
			vmVariant obj = {vmVariantTypeVoid,0};
			vmVariant value = {vmVariantTypeVoid,0};
			vmClass * objClass = NULL;
			vmUniqueKey uniqueKey = 0;
        
			if(op->uniqueKey){
				if(op->metaCount ==1){
					if(meta->type & vmRuntimeMetaTypeOperator){
						obj = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
					}
					else{
						obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
					}
                
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
                
					vmRuntimeContextSetVariant(context, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg), obj,InvokeTickArg);
                
					return obj;
				}
				else if(op->metaCount == 2){
					vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op, 0);
					vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op, 1);
					vmVariant obj1 = {vmVariantTypeVoid,0};
					vmVariant obj2 = {vmVariantTypeVoid,0};
                
					if(meta1->type & vmRuntimeMetaTypeOperator){
						obj1 = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta1->value.operatorOffset),InvokeTickArg);
						if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
							return obj1;
						}
					}
					else{
						obj1 = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta1,InvokeTickArg);
						if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
							return obj1;
						}
					}
                
					if(meta2->type & vmRuntimeMetaTypeOperator){
						obj2 = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta2->value.operatorOffset),InvokeTickArg);
						if(obj2.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
							return obj2;
						}
					}
					else{
						obj2 = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
						if(obj2.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
							return obj2;
						}
					}
                    
                    if(obj1.type & vmVariantTypeObject){
                        
                        rs = vmObjectSetProperty(context, obj1.value.objectValue->vmClass, obj1.value.objectValue, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg), obj2,InvokeTickArg);
                    
                        if(rs.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
                            return rs;
                        }
                        
                    }
                
					return obj2;
				}
			}
			else{
				if(meta->type & vmRuntimeMetaTypeOperator){
					obj = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
                
				}
				else{
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
				}
            
				if(!(obj.type & vmVariantTypeObject)){
					return vmRuntimeContextException(context, 0, "vmRuntimeOperatorTypeProperty self not is Object",InvokeTickArg);
				}
 
				if(obj.type & vmRuntimeMetaTypeSuperObject){
					objClass = obj.vmClass;
				}
				else{
					objClass = obj.value.objectValue->vmClass;
				}
            
				uniqueKey = vmRuntimeContextVariantToUniqueKey(context, vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,vmRuntimeMetaByOperator(op,1),InvokeTickArg), InvokeTickArg);
            
				value = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,vmRuntimeMetaByOperator(op,2),InvokeTickArg);
            
				if(value.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return value;
				}
            
				rs = vmObjectSetProperty(context, objClass, obj.value.objectValue,uniqueKey, value,InvokeTickArg);
                
				if(rs.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return rs;
				}
            
				return value;
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeIfElse){
        assert(op->metaCount ==3);
		{
			vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op, 1);
			vmRuntimeMeta * meta3 = vmRuntimeMetaByOperator(op, 2);
			vmVariant obj1 = {vmVariantTypeVoid,0};
			vmVariant obj2 = {vmVariantTypeVoid,0};
			vmVariant obj3 = {vmVariantTypeVoid,0};
        
			if(meta1->type & vmRuntimeMetaTypeOperator){
				obj1 = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta1->value.operatorOffset),InvokeTickArg);
				if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj1;
				}
			}
			else{
				obj1 = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta1,InvokeTickArg);
				if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj1;
				}
			}
        
			if(vmVariantToBoolean(context, obj1)){
				if(meta2->type & vmRuntimeMetaTypeOperator){
					obj2 = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta2->value.operatorOffset),InvokeTickArg);
				}
				else{
					obj2 = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
				}
				return obj2;
			}
			else{
				if(meta3->type & vmRuntimeMetaTypeOperator){
					obj3 = vmRuntimeContextOperator(context,classMeta,vmObject,vmRuntimeMetaOperatorGet(classMeta,meta3->value.operatorOffset),InvokeTickArg);
				}
				else{
					obj3 = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta3,InvokeTickArg);
				}
				return obj3;
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeIfElseIfElse){
        assert(op->metaCount >=2);
        {
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
			vmVariant obj = {vmVariantTypeVoid,0};
			hint32 i=0;
        
			if(meta->type & vmRuntimeMetaTypeOperator){
				obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
			}
			else{
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
			}
        
			if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj;
			}
        
			if(vmVariantToBoolean(context,obj)){
				meta = vmRuntimeMetaByOperator(op,++i);
				if(meta->type & vmRuntimeMetaTypeReturn){
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
					obj.type = (vmVariantType)(obj.type | vmRuntimeMetaTypeReturn);
					return obj;
				}
				else if(meta->type & vmRuntimeMetaTypeOperator){
					obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta->value.operatorOffset),InvokeTickArg);
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
				}
			}
			else{
				i++;
				while( ++i <op->metaCount){
					assert(i+1 < op->metaCount);
					meta = vmRuntimeMetaByOperator(op, i);
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
					if(vmVariantToBoolean(context,obj)){
						meta = vmRuntimeMetaByOperator(op,++i);
						if(meta->type & vmRuntimeMetaTypeReturn){
							obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
							obj.type = (vmVariantType)(obj.type | vmRuntimeMetaTypeReturn);
							return obj;
						}
						else if(meta->type & vmRuntimeMetaTypeOperator){
							obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta->value.operatorOffset),InvokeTickArg);
							if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
								return obj;
							}
						}
                    
						break;
					}
					else{
						i++;
					}
				}
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeFor){
        assert(op->metaCount ==4);
		{
			vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op, 1);
			vmRuntimeMeta * meta3 = vmRuntimeMetaByOperator(op, 2);
			vmRuntimeMeta * meta4 = vmRuntimeMetaByOperator(op, 3);
			vmVariant obj = {vmVariantTypeVoid,0};
        
			obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta1,InvokeTickArg);
        
			if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj;
			}
        
			obj = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
        
			if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj;
			}
        
			while(vmVariantToBoolean(context,obj)){
            
				if(meta4->type & vmRuntimeMetaTypeReturn){
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta4,InvokeTickArg);
					obj.type =(vmVariantType)( obj.type | vmRuntimeMetaTypeReturn);
					return obj;
				}
				else if(meta4->type & vmRuntimeMetaTypeBreak){
					break;
				}
				else if(meta4->type & vmRuntimeMetaTypeContinue){
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta3,InvokeTickArg);
                
					if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
                
					obj = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
                
					if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
                
					continue;
				}
				else if(meta4->type & vmRuntimeMetaTypeOperator){
                
					obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta4->value.operatorOffset),InvokeTickArg);
                
					if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn)){
						return obj;
					}
                
					if(obj.type & vmRuntimeMetaTypeBreak){
						break;
					}
					else if(obj.type & vmRuntimeMetaTypeContinue){
						obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta3,InvokeTickArg);
                    
						if(obj.type & (vmRuntimeMetaTypeThrowable )){
							return obj;
						}
                    
						obj = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
                    
						if(obj.type & (vmVariantTypeThrowable )){
							return obj;
						}
                    
						continue;
					}
				}
            
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta3,InvokeTickArg);
            
				if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}
            
				obj = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta2,InvokeTickArg);
            
				if(obj.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}
			}
		}
    }
    else if(op->type ==vmRuntimeOperatorTypeWhile){
        assert(op->metaCount ==2);
		{
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op,1);
			vmVariant obj = {vmVariantTypeVoid,0};
        
			obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
        
			if(obj.type & (vmRuntimeMetaTypeThrowable)){
				return obj;
			}
        
			while(vmVariantToBoolean(context,obj)){
            
				if(meta2->type & vmRuntimeMetaTypeReturn){
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
					obj.type = (vmVariantType)(obj.type | vmRuntimeMetaTypeReturn);
					return obj;
				}
				else if(meta2->type & vmRuntimeMetaTypeBreak){
					break;
				}
				else if(meta2->type & vmRuntimeMetaTypeContinue){
					obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
                
					if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
						return obj;
					}
                
					continue;
				}
				else if(meta2->type & vmRuntimeMetaTypeOperator){
                
					obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta2->value.operatorOffset),InvokeTickArg);
                
					if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn)){
						return obj;
					}
                
					if(obj.type & vmRuntimeMetaTypeBreak){
						break;
					}
					else if(obj.type & vmRuntimeMetaTypeContinue){
						obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
                    
						if(obj.type & (vmRuntimeMetaTypeThrowable)){
							return obj;
						}
                    
						continue;
					}
				}
            
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
            
				if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}

			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeForeach){
        assert(op->metaCount == 2 && op->uniqueKey);
		{
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op,1);
			vmVariant obj = {vmVariantTypeVoid,0};
			vmVariant key = {vmVariantTypeVoid,0};
			vmUniqueKeyList names = vmUniqueKeyListAlloc();
			hint32 i,c;
			vmUniqueKey name;
			vmClass * objectClass;
            vmUniqueKey uniqueKey;
        
            uniqueKey = vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey, InvokeTickArg);
            
			obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
        
			if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn )){
				return obj;
			}
        
			if(obj.type & vmVariantTypeObject){
				if(obj.value.objectValue){
					if(obj.type & vmRuntimeMetaTypeSuperObject){
						objectClass = obj.vmClass;
					}
					else{
						objectClass = obj.value.objectValue->vmClass;
					}
					vmObjectPropertyNames(context, objectClass,obj.value.objectValue,names,InvokeTickArg);
				}
				c = vmUniqueKeyListCount(names);
				for(i=0;i<c;i++){
					name = vmUniqueKeyListGet(names, i);
					key = vmRuntimeContextUniqueKeyToVariant(context, name,InvokeTickArg);
					vmRuntimeContextSetVariant(context, uniqueKey, key,InvokeTickArg);
                
					if(meta2->type & vmRuntimeMetaTypeReturn){
						obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
                    
						if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn)){
							return obj;
						}
                    
						obj.type = (vmVariantType)( obj.type | vmRuntimeMetaTypeReturn);
                    
						return obj;
					}
					else if(meta2->type & vmRuntimeMetaTypeBreak){
						break;
					}
					else if(meta2->type & vmRuntimeMetaTypeContinue){
						continue;
					}
					else if(meta2->type & vmRuntimeMetaTypeOperator){
                    
						obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta2->value.operatorOffset),InvokeTickArg);
                    
						if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn)){
							return obj;
						}
                    
						if(obj.type & vmRuntimeMetaTypeBreak){
							break;
						}
						else if(obj.type & vmRuntimeMetaTypeContinue){
							continue;
						}
					}
				}
			}
        
			vmUniqueKeyListDealloc(names);
		}
    }
    else if(op->type == vmRuntimeOperatorTypeVar){
        assert(op->metaCount ==0 || op->metaCount ==1);
		{
			vmVariant obj = {vmVariantTypeVoid,0};
			if(op->metaCount ==1){
				vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
				obj = vmRuntimeContextRuntimeMetaToVariant(context, classMeta, vmObject, meta,InvokeTickArg);
            
				if(obj.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
					return obj;
				}
			}
        
			vmRuntimeContextSetVariant(context, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey,InvokeTickArg), obj,InvokeTickArg);
		}
    }
    else if(op->type == vmRuntimeOperatorTypeIs){
        assert(op->metaCount ==2);
		{
			vmVariant obj1 = {vmVariantTypeVoid,0};
			vmVariant obj2 = {vmVariantTypeVoid,0};
			vmUniqueKey uniqueKey;
			vmRuntimeMeta * meta1 = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta2 = vmRuntimeMetaByOperator(op, 1);
			vmClass * objClass;

            
			obj1 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta1,InvokeTickArg);
			if(obj1.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj1;
			}
        
			obj2 = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
			if(obj2.type & (vmVariantTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
				return obj2;
			}
        
			obj1 = vmVariantToObject(context, obj1);
			uniqueKey = vmRuntimeContextVariantToUniqueKey(context, obj2, InvokeTickArg);
        
			objClass = vmRuntimeContextGetClass(context, uniqueKey, InvokeTickArg);
        
			if(objClass == NULL){
				rs = vmRuntimeContextExceptionFormat(context, 0, "[is] not found class %s",InvokeTickArg,vmRuntimeContextUniqueKeyToString(context, uniqueKey, InvokeTickArg));
			}
			else{
				rs.type = vmVariantTypeBoolean;
				rs.value.booleanValue = vmRuntimeContextObjectIsKindClass(context, obj1.value.objectValue, objClass, InvokeTickArg);
			}
		}
    }
    else if(op->type == vmRuntimeOperatorTypeTryCatchFinally){
        assert(op->metaCount >=1);
		{
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
			vmRuntimeMeta * meta1, *meta2, * finallyMeta = NULL;
			vmVariant obj = {vmVariantTypeVoid,0};
			vmUniqueKey uniqueKey;
			vmClass * objClass;
			hint32 i=0;
        
			if(meta->type & vmRuntimeMetaTypeOperator){
				obj = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
			}
			else{
				obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
			}
        
			if(obj.type & (vmRuntimeMetaTypeReturn)){
				return obj;
			}
        

			for(i=1;i<op->metaCount;i++){
				meta = vmRuntimeMetaByOperator(op, i);
				if(meta->type & vmRuntimeMetaTypeFinally){
					finallyMeta = meta;
					break;
				}
			}
        
			if(obj.type & vmRuntimeMetaTypeThrowable){
				obj = vmVariantToObject(context, obj);
				obj.type = (vmVariantType)(obj.type | vmRuntimeMetaTypeThrowable);
				for(i=1;i<op->metaCount;i++){
					meta = vmRuntimeMetaByOperator(op, i);
					if(meta->type & vmRuntimeMetaTypeCatch){
						assert(i + 2 <op->metaCount);
						meta1 = vmRuntimeMetaByOperator(op, i+1);
						meta2 = vmRuntimeMetaByOperator(op, i+2);
						assert(meta->type & vmVariantTypeString && meta1->type & vmVariantTypeString);
						uniqueKey = vmRuntimeContextVariantToUniqueKey(context, vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg), InvokeTickArg);
						objClass = vmRuntimeContextGetClass(context, uniqueKey, InvokeTickArg);
						if(objClass && vmRuntimeContextObjectIsKindClass(context, obj.value.objectValue, objClass, InvokeTickArg)){
                        
							uniqueKey = vmRuntimeContextVariantToUniqueKey(context, vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta1,InvokeTickArg), InvokeTickArg);

							obj.type = (vmVariantType)(obj.type & (~ vmRuntimeMetaTypeThrowable));
                        
							vmRuntimeContextSetVariant(context, uniqueKey, obj, InvokeTickArg);
                        
							obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta2,InvokeTickArg);
                        
							break;
						}
						else{
							i +=2;
						}
					}
				}
			}
        
			if(finallyMeta){
				vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,finallyMeta,InvokeTickArg);
			}
            
            if(obj.type & (vmRuntimeMetaTypeReturn)){
				return obj;
			}
        
			return obj;
		}
    }
    else if(op->type == vmRuntimeOperatorTypeThrow){
        assert(op->metaCount ==1);
        
		{
			vmRuntimeMeta * meta = vmRuntimeMetaByOperator(op, 0);
			vmVariant obj = {vmVariantTypeVoid,0};
			obj = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
        
			obj.type =(vmVariantType)( obj.type | vmRuntimeMetaTypeThrowable);
            
			return obj;
		}
    }
    else {
        assert(0);
    }
    
    return rs;
}



vmVariant vmRuntimeContextInvoke(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,InvokeTickDeclare){
    hint16 i;
    vmRuntimeMeta * meta = NULL;
    vmVariant rs = {vmVariantTypeVoid,0};
	assert(context && classMeta && op && op->type == vmRuntimeOperatorTypeInvoke);
    for(i=0;i<op->metaCount;i++){
        meta = vmRuntimeMetaByOperator(op,i);
        if(meta->type & vmRuntimeMetaTypeReturn){
            rs = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
            rs.type = (vmVariantType)( rs.type | vmRuntimeMetaTypeReturn);
            return rs;
        }
        if(meta->type & vmRuntimeMetaTypeThrowable){
            rs = vmRuntimeContextRuntimeMetaToVariant(context,classMeta,vmObject,meta,InvokeTickArg);
            rs.type = (vmVariantType)(rs.type | vmRuntimeMetaTypeThrowable);
            return rs;
        }
        else if(meta->type & vmRuntimeMetaTypeBreak){
            rs.type = (vmVariantType)vmRuntimeMetaTypeBreak;
            rs.value.int64Value = 0;
            return rs;
        }
        else if(meta->type & vmRuntimeMetaTypeContinue){
            rs.type = (vmVariantType)vmRuntimeMetaTypeContinue;
            rs.value.int64Value = 0;
            return rs;
        }
        else if(meta->type & vmRuntimeMetaTypeOperator){
            rs = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta, meta->value.operatorOffset),InvokeTickArg);
            if(rs.type & (vmRuntimeMetaTypeThrowable | vmRuntimeMetaTypeReturn | vmRuntimeMetaTypeBreak | vmRuntimeMetaTypeContinue)){
                return rs;
            }
        }
        rs.type = vmVariantTypeVoid;
        rs.value.objectValue = NULL;
    }
    
    return rs;
}

vmVariant vmRuntimeContextFunction(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * vmObject, vmRuntimeMetaOperator * op,vmVariantList args,InvokeTickDeclare){
    
    vmVariant rs = {vmVariantTypeVoid,0};
    vmVariant obj = {vmVariantTypeVoid,0};
    vmRuntimeMeta * meta = NULL;
    hint32 i;
    
	assert(context && classMeta && op && op->type == vmRuntimeOperatorTypeFunction);

    vmRuntimeContextInvokeBegin(context,InvokeTickArg);
    
    obj.type = vmVariantTypeObject;
    obj.value.objectValue = vmObject;
    
    vmRuntimeContextSetVariant(context, vmRuntimeContextThisKey(context,InvokeTickArg), obj,InvokeTickArg);
    
    obj.type = vmVariantTypeObject | vmRuntimeMetaTypeSuperObject;
    obj.vmClass = vmRuntimeContextGetSuperClass(context, (vmClass *)classMeta, InvokeTickArg);
    
    vmRuntimeContextSetVariant(context,vmRuntimeContextSuperKey(context,InvokeTickArg),obj,InvokeTickArg);
    
    obj = vmRuntimeContextVariantListToArrayObject(context,args,InvokeTickArg);
    
    vmRuntimeContextSetVariant(context, vmRuntimeContextArgumentsKey(context,InvokeTickArg), obj,InvokeTickArg);
    
    for(i=0;i<op->metaCount;i++){
        meta = vmRuntimeMetaByOperator(op, i);
        assert(meta->type & (vmRuntimeMetaTypeArg | vmRuntimeMetaTypeOperator));
        if(meta->type & vmRuntimeMetaTypeArg){
            vmRuntimeContextSetVariant(context, vmRuntimeContextGetUniqueKeyByMetaClass(context
                                                                                        , classMeta, meta->value.objectKey,InvokeTickArg) , vmVariantListGet(args, i),InvokeTickArg);
        }
        else if(meta->type & vmRuntimeMetaTypeOperator){
            rs = vmRuntimeContextOperator(context, classMeta, vmObject, vmRuntimeMetaOperatorGet(classMeta,meta->value.operatorOffset),InvokeTickArg);
            break;
        }
    }
    
    rs.type = rs.type & ~ vmRuntimeMetaTypeReturn;
    
    if((rs.type & vmVariantTypeObject) && rs.value.objectValue){
        vmRuntimeContextSuperDomainAddObject(context, rs.value.objectValue, InvokeTickArg);
    }
    
    vmRuntimeContextInvokeEnd(context,InvokeTickArg);
    
    return rs;
}

static void _vmObjectPropertyInitByClassMeta(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * object,InvokeTickDeclare){
    hbyte * classBegin;
    vmClassMetaOffset * propertyBegin;
    hint32 i;
    hint32 size = vmRuntimeContextGetClassSize(context, (vmClass *)classMeta,InvokeTickArg);
    hint32 totalSize= vmRuntimeContextGetClassTotalSize(context, (vmClass *)classMeta,InvokeTickArg);
    vmVariant * var;
    vmClass * superClass;
    vmRuntimeMetaOperator * op;

    superClass = vmRuntimeContextGetSuperClass(context, (vmClass *)classMeta, InvokeTickArg);
    if(superClass && superClass->type == vmClassTypeMeta){
        _vmObjectPropertyInitByClassMeta(context, (vmClassMeta *)superClass, object, InvokeTickArg);
    }
    
    classBegin = (hbyte *)classMeta - classMeta->offset;
	propertyBegin = (vmClassMetaOffset *)((hbyte *) classMeta + sizeof(vmClassMeta));
    for(i=0;i<classMeta->propertys;i++){
        var = (vmVariant *)((hbyte *)object + totalSize - size + sizeof(vmVariant) * i);
        op = (vmRuntimeMetaOperator *)((hbyte *) classBegin + propertyBegin[i]);
        if(op->metaCount >0){
            * var = vmRuntimeContextRuntimeMetaToVariant(context, classMeta,object,vmRuntimeMetaByOperator(op,0),InvokeTickArg);
        }
    }
}

/**
 
 Return : Object or Throwable
 */
vmVariant vmObjectAlloc(vmRuntimeContext context,vmClass * clazz,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeObject,0};
    assert(context);
    if(clazz){
        if(clazz->type == vmClassTypeBase){
            vmClassBase * classBase = (vmClassBase *)clazz;
            vmObject * object = (vmObject *)mem_malloc(classBase->size);
            mem_memset(object,0,classBase->size);
            object->retainCount = 0;
            object->vmClass = clazz;
            vmRuntimeContextDomainAddObject(context, object, InvokeTickArg);
            rs = (*classBase->init)(context,clazz,object,args,InvokeTickArg);
            if(!(rs.type & vmVariantTypeThrowable)){
                rs.type = vmVariantTypeObject;
                rs.value.objectValue = object;
            }

            return rs;
        }
        else if(clazz->type == vmClassTypeMeta){
            hint32 totalSize = vmRuntimeContextGetClassTotalSize(context, clazz,InvokeTickArg);
            vmObject * object = (vmObject *)mem_malloc( totalSize);
            mem_memset(object,0, totalSize);
            object->retainCount = 0;
            object->vmClass = clazz;
            vmRuntimeContextDomainAddObject(context, object, InvokeTickArg);
            _vmObjectPropertyInitByClassMeta(context,(vmClassMeta *) clazz,object,InvokeTickArg);
            rs = vmObjectInvoke(context, clazz, object, vmRuntimeContextFunctionInitKey(context,InvokeTickArg), args,InvokeTickArg);
            if(!(rs.type & vmVariantTypeThrowable)){
                rs.type = vmVariantTypeObject;
                rs.value.objectValue = object;
            }
            return rs;
        }
        else{
            assert(0);
        }
    }
    return rs;
}


vmObject * vmObjectRetain(vmRuntimeContext context,vmObject * object,InvokeTickDeclare){
    assert(context);
    if(object){
        object->retainCount ++;
    }
    return object;
}

static void _vmObjectPropertyClearByClassMeta(vmRuntimeContext context,vmClassMeta * classMeta,vmObject * object,InvokeTickDeclare){
    hbyte * classBegin;
    vmClassMetaOffset * propertyBegin;
    vmRuntimeMetaOperator * op;
    hint32 i;
    hint32 size = vmRuntimeContextGetClassSize(context, (vmClass *)classMeta,InvokeTickArg);
    hint32 totalSize= vmRuntimeContextGetClassTotalSize(context, (vmClass *)classMeta,InvokeTickArg);
    vmVariant * var;
    vmClass * superClass;
	classBegin = (hbyte *)classMeta - classMeta->offset;
	propertyBegin = (vmClassMetaOffset *)((hbyte *) classMeta + sizeof(vmClassMeta));
    for(i=0;i<classMeta->propertys;i++){
        op = (vmRuntimeMetaOperator *)((hbyte *) classBegin + propertyBegin[i]);
        var = (vmVariant *)((hbyte *)object + totalSize - size + sizeof(vmVariant) * i);
        if(op->type != vmRuntimeOperatorTypeVarAssign){
            vmRuntimeContextVariantRelease(context, * var,InvokeTickArg);
        }
        var->type = vmVariantTypeVoid;
        var->value.int64Value = 0;
    }
    
    superClass = vmRuntimeContextGetSuperClass(context, (vmClass *)classMeta, InvokeTickArg);
    if(superClass && superClass->type == vmClassTypeMeta){
        _vmObjectPropertyClearByClassMeta(context, (vmClassMeta *)superClass, object, InvokeTickArg);
    }
}

/**
 
 Return : Void or Throwable
 */
vmVariant vmObjectRelease(vmRuntimeContext context,vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    assert(context);
    if(object){
        if(object->retainCount <=0){
            _InvoekTickPrintForMem(object);
        }
        assert(object->retainCount >0);
        
        if(--object->retainCount ==0){
            assert(object->vmClass);
            if(object->vmClass->type == vmClassTypeBase){
                object->retainCount ++;
                rs = (*((vmClassBase *)object->vmClass)->destroy)(context,object,InvokeTickArg);
                object->retainCount --;
                mem_free( object);
                return rs;
            }
            else{
                object->retainCount ++;
                rs = vmObjectInvoke(context, object->vmClass, object, vmRuntimeContextFunctionDestroyKey(context,InvokeTickArg), NULL,InvokeTickArg);
                _vmObjectPropertyClearByClassMeta(context,(vmClassMeta *)object->vmClass,object,InvokeTickArg);
                object->retainCount --;
                mem_free( object);
                return rs;
            }
        }
    }
    return rs;
}


/**
 
 Return : Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
vmVariant vmObjectGetProperty(vmRuntimeContext context,vmClass * clazz,vmObject * vmObject,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeObject,0};
    assert(context && clazz && vmObject);

    if(clazz->type == vmClassTypeBase){
        return (*((vmClassBase *)clazz)->getProperty)(context,clazz,vmObject,name,InvokeTickArg);
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta *classMeta = (vmClassMeta *)clazz;
        hint32 index = -1;
        vmRuntimeMetaOperator * op = vmRuntimeContextFindProperty(context, classMeta, name,& index,InvokeTickArg);
        if(op){
            hint32 size = vmRuntimeContextGetClassSize(context, clazz,InvokeTickArg);
            hint32 totalSize= vmRuntimeContextGetClassTotalSize(context, clazz,InvokeTickArg);
            vmVariant * var = (vmVariant *)((hbyte *)vmObject + totalSize - size + sizeof(vmVariant) * index);
            return * var;
        }
        else{
            return vmObjectGetProperty(context, vmRuntimeContextGetSuperClass(context, clazz,InvokeTickArg), vmObject, name,InvokeTickArg);
        }
    }
    else{
        assert(0);
    }
    
    return rs;
}


/**
 value : Not Void ,Not Throwable
 Return : Void or Throwable
 */
vmVariant vmObjectSetProperty(vmRuntimeContext context,vmClass * clazz,vmObject * vmObject,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    assert(context && clazz && vmObject);

    if(clazz->type == vmClassTypeBase){
        return (*((vmClassBase *)clazz)->setProperty)(context,clazz,vmObject,name,value,InvokeTickArg);
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta *classMeta = (vmClassMeta *)clazz;
        hint32 index = -1;
        vmRuntimeMetaOperator * op = vmRuntimeContextFindProperty(context, classMeta, name,& index,InvokeTickArg);
        if(op){
            hint32 size = vmRuntimeContextGetClassSize(context, clazz,InvokeTickArg);
            hint32 totalSize= vmRuntimeContextGetClassTotalSize(context, clazz,InvokeTickArg);
            vmVariant * var = (vmVariant *)((hbyte *)vmObject + totalSize - size + sizeof(vmVariant) * index);
            
            if(op->type != vmRuntimeOperatorTypeVarAssign){
                vmRuntimeContextVariantRetain(context, value, InvokeTickArg);
                vmRuntimeContextVariantRelease(context, * var,InvokeTickArg);
            }
            * var = value;
//            {
//                vmVariantList args = vmVariantListAlloc();
//                vmVariant v = vmRuntimeContextUniqueKeyToVariant(context, name, InvokeTickArg);
//                
//                vmVariantListAdd(args, v);
//                
//                vmVariantListAdd(args, value);
//                
//                rs = vmObjectInvoke(context, vmObject->vmClass, vmObject, context->systemKeys.propertySetted, args, InvokeTickArg);
//                
//                vmVariantListDealloc(args);
//            }
            return rs;
        }
        else{
            return vmObjectSetProperty(context, vmRuntimeContextGetSuperClass(context, clazz,InvokeTickArg), vmObject, name,value,InvokeTickArg);
        }
    }
    else{
        assert(0);
    }
 
    return rs;
}

/**
 
 Return : Void or Throwable
 */
void vmObjectPropertyNames(vmRuntimeContext context,vmClass * clazz,vmObject * vmObject,vmUniqueKeyList names,InvokeTickDeclare){
    assert(context && clazz && names);
    if(clazz->type == vmClassTypeBase){
        ((vmClassBase *) clazz)->propertyNames(context,clazz,vmObject,names,InvokeTickArg);
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta * classMeta = (vmClassMeta *) clazz;
        hbyte * classBegin = (hbyte *) classMeta - classMeta->offset;
        vmClassMetaOffset * propertyBegin = (vmClassMetaOffset *)((hbyte *)classMeta + sizeof(vmClassMeta) );
        hint32 i;
        vmRuntimeMetaOperator * op = NULL;
        vmObjectPropertyNames(context, vmRuntimeContextGetSuperClass(context, clazz,InvokeTickArg),vmObject, names,InvokeTickArg);
        for(i=0;i<classMeta->propertys;i++){
            op = (vmRuntimeMetaOperator *)(classBegin + propertyBegin[i]);
            vmUniqueKeyListAdd(names, vmRuntimeContextGetUniqueKeyByMetaClass(context, classMeta, op->uniqueKey, InvokeTickArg));
        }
    }
    else{
        assert(0);
    }
}


/**
 
 Return : any
 */
vmVariant vmObjectInvoke(vmRuntimeContext context,vmClass * clazz,vmObject * vmObject,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    assert(context && clazz);

    if(clazz->type == vmClassTypeBase){
        if(context->systemKeys.functionInitKey == name){
            return (*((vmClassBase *)clazz)->init)(context,clazz,vmObject,args,InvokeTickArg);
        }
        else if(context->systemKeys.functionDestroyKey == name){
            return (*((vmClassBase *)clazz)->destroy)(context,vmObject,InvokeTickArg);
        }
        return (*((vmClassBase *)clazz)->invoke)(context,clazz,vmObject,name,args,InvokeTickArg);
    }
    else if(clazz->type == vmClassTypeMeta){
        vmClassMeta *classMeta = (vmClassMeta *)clazz;
        vmRuntimeMetaOperator * op = vmRuntimeContextFindFunction(context, classMeta, name,InvokeTickArg);
        if(op){
            return vmRuntimeContextFunction(context,classMeta,vmObject,op,args,InvokeTickArg);
        }
        else{
            return vmObjectInvoke(context, vmRuntimeContextGetSuperClass(context, clazz,InvokeTickArg), vmObject, name,args,InvokeTickArg);
        }
    }
    else{
        assert(0);
    }
 
    return rs;
}



/**
 
 vmRuntimeContext alloc
 */


typedef struct{
    vmRuntimeSystemKeys systemKeys;
    hmap_t uniqueKeyMap;
    hlist_t uniqueKeyInstanceList;
    vmRuntimeClassLoader loader;
    hserial_map_t classMap;
    hserial_list_t domains;
    vmRuntimeDebug debug;
    hint32 retainCount;
}_vmRuntimeContext;

typedef struct{
    hserial_map_t variantMap;
    hlist_t retainObjects;
}_vmRuntimeContextDomain;

vmRuntimeContext vmRuntimeContextAlloc(vmRuntimeClassLoader loader,InvokeTickDeclare){

    _vmRuntimeContext * context =(_vmRuntimeContext *) mem_malloc( sizeof(_vmRuntimeContext));
    mem_memset(context, 0,sizeof(_vmRuntimeContext));

    context->uniqueKeyMap = map_alloc( hash_code_str, equal_str);
    context->uniqueKeyInstanceList = list_alloc( 32, 128);
    context->loader = vmRuntimeClassLoaderRetain(loader, InvokeTickArg);
    context->classMap = serial_map_alloc( sizeof(vmUniqueKey), sizeof(vmClass *), 0);
    context->domains = serial_list_alloc( sizeof(_vmRuntimeContextDomain), 0);
    context->retainCount = 1;
    
    vmRuntimeClassLoaderUniqueKeysRegister(loader,(vmRuntimeContext) context,InvokeTickArg);
    
    context->systemKeys.functionInitKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "__init",InvokeTickArg);
    context->systemKeys.functionDestroyKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "__destroy",InvokeTickArg);
    context->systemKeys.functionToStringKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "toString",InvokeTickArg);
    context->systemKeys.functionToIntKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "toInt",InvokeTickArg);
    context->systemKeys.functionToDoubleKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "toDouble",InvokeTickArg);
    context->systemKeys.functionEqualKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "equal",InvokeTickArg);
    context->systemKeys.thisKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "this",InvokeTickArg);
    context->systemKeys.superKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "super",InvokeTickArg);
    context->systemKeys.argumentsKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "arguments",InvokeTickArg);
    context->systemKeys.classNameKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "className",InvokeTickArg);
    context->systemKeys.valueForKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "valueForKey",InvokeTickArg);
    context->systemKeys.setValueForKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "setValueForKey",InvokeTickArg);
    context->systemKeys.valueForKeyPath = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "valueForKeyPath",InvokeTickArg);
    context->systemKeys.setValueForKeyPath = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "setValueForKeyPath",InvokeTickArg);
    context->systemKeys.functionToTextContentKey = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "toTextContent",InvokeTickArg);
    context->systemKeys.propertySetted = vmRuntimeContextGetUniqueKey((vmRuntimeContext)context, "propertySetted",InvokeTickArg);
    
    vmRuntimeContextInvokeBegin((vmRuntimeContext)context, InvokeTickArg);
    
    return (vmRuntimeContext)context;
}

vmRuntimeClassLoader vmRuntimeContextClassLoader(vmRuntimeContext context,InvokeTickDeclare){
    _vmRuntimeContext * ctx =(_vmRuntimeContext *) context;
    return ctx->loader;
}

static void vmRuntimeContextDealloc(vmRuntimeContext context,InvokeTickDeclare){
    if(context){
        _vmRuntimeContext * ctx = (_vmRuntimeContext *)context;
        hint32 i,c;
        
        vmRuntimeContextInvokeEnd(context, InvokeTickArg);
    
        c = list_count(ctx->uniqueKeyInstanceList);
        for(i=0;i<c;i++){
            mem_free(list_get(ctx->uniqueKeyInstanceList, i));
        }
        list_dealloc(ctx->uniqueKeyInstanceList);
        
        assert(serial_list_count(ctx->domains) ==0);
        
        serial_list_dealloc(ctx->domains);
        serial_map_dealloc(ctx->classMap);
        map_dealloc(ctx->uniqueKeyMap);
        
        vmRuntimeClassLoaderRelease(ctx->loader, InvokeTickArg);
        
        mem_free(ctx);
    }
}

vmRuntimeDebug vmRuntimeContextGetDebug(vmRuntimeContext context,InvokeTickDeclare){
    if(context){
        _vmRuntimeContext * ctx = (_vmRuntimeContext *)context;
        return ctx->debug;
    }
    return NULL;
}

void vmRuntimeContextSetDebug(vmRuntimeContext context,vmRuntimeDebug debug,InvokeTickDeclare){
    if(context){
        _vmRuntimeContext * ctx = (_vmRuntimeContext *)context;
        if(ctx->debug != debug){
            if(ctx->debug){
                vmRuntimeDebugRelease(ctx->debug, InvokeTickArg);
            }
            if(debug){
                vmRuntimeDebugRetain(debug, InvokeTickArg);
            }
            ctx->debug = debug;
        }
    }
}

vmRuntimeContext vmRuntimeContextRetain(vmRuntimeContext context,InvokeTickDeclare){
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    c->retainCount ++;
    return context;
}

void vmRuntimeContextRelease(vmRuntimeContext context,InvokeTickDeclare){
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    assert(c->retainCount >0);
    if(--c->retainCount == 0){
        vmRuntimeContextDealloc(context,InvokeTickArg);
    }
}

vmUniqueKey vmRuntimeContextRegisterUniqueKey(vmRuntimeContext context,hcchar * key,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    hchar * value = 0;
	assert(context);
    
    if(key){
        value = map_get(c->uniqueKeyMap, (hany)key);
        if(value == NULL){
            map_put(c->uniqueKeyMap,(hany)key, (hany)key);
        }
    }
    return (vmUniqueKey)value;
}

vmUniqueKey vmRuntimeContextGetUniqueKey(vmRuntimeContext context,hcchar *key,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    hchar * value = 0;
	assert(context);
    
    if(key){
        value = map_get(c->uniqueKeyMap, (hany)key);
        if(value == NULL){
            value = mem_malloc( strlen(key) + 1);
            mem_strcpy(value, key);
            list_add(c->uniqueKeyInstanceList, value);
            map_put(c->uniqueKeyMap, value, value);
        }
    }
    return (vmUniqueKey)value;
}

vmUniqueKey vmRuntimeContextGetUniqueKeyFromInteger(vmRuntimeContext context,hint32 key,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    hchar * value = 0;
    hchar intValue[1024];
	assert(context);
    
    sprintf(intValue, "%d",key);
    value = map_get(c->uniqueKeyMap, (hany)intValue);
    if(value == NULL){
        value = mem_malloc( strlen(intValue) + 1);
        strcpy(value, intValue);
        list_add(c->uniqueKeyInstanceList, value);
        map_put(c->uniqueKeyMap, value, value);
    }

    return (vmUniqueKey)value;
}

vmUniqueKey vmRuntimeContextVariantToUniqueKey(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    vmVariant v = var;
    if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            v = vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, vmRuntimeContextFunctionToStringKey(context,InvokeTickArg), NULL, InvokeTickArg);
        }
    }
    if(v.type & vmVariantTypeInt16){
        return vmRuntimeContextGetUniqueKeyFromInteger(context, v.value.int16Value, InvokeTickArg);
    }
    if(v.type & vmVariantTypeInt32){
        return vmRuntimeContextGetUniqueKeyFromInteger(context, v.value.int32Value, InvokeTickArg);
    }
    if(v.type & vmVariantTypeInt64){
        return vmRuntimeContextGetUniqueKeyFromInteger(context, (hint32)v.value.int64Value, InvokeTickArg);
    }
    if(v.type & vmVariantTypeDouble){
        return vmRuntimeContextGetUniqueKeyFromInteger(context, (hint32)v.value.doubleValue, InvokeTickArg);
    }
    if(v.type & vmVariantTypeString){
        return vmRuntimeContextGetUniqueKey(context, v.value.stringValue, InvokeTickArg);
    }
    return vmRuntimeContextGetUniqueKey(context, "", InvokeTickArg);
}

hcchar * vmRuntimeContextUniqueKeyToString(vmRuntimeContext context,vmUniqueKey uniqueKey,InvokeTickDeclare){
    assert(context);
    return (hcchar *)uniqueKey;
}

hint32 vmRuntimeContextUniqueKeyToInteger(vmRuntimeContext context,vmUniqueKey uniqueKey,InvokeTickDeclare){
    assert(context);
    if(uniqueKey){
        return hatoi((hcchar *)uniqueKey);
    }
    return 0;
}

vmClass * vmRuntimeContextGetClass(vmRuntimeContext context, vmUniqueKey className,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    vmClass * rs =NULL;
    hany value = NULL;
	assert(context);
    
    if(className){
        value = serial_map_get(c->classMap, &className);
        if(value){
            rs = *(vmClass **)value;
        }
        else{
            rs = vmRuntimeClassLoaderGetClass(c->loader, vmRuntimeContextUniqueKeyToString(context,className,InvokeTickArg),InvokeTickArg);
            if(rs){
                serial_map_put(c->classMap, &className, &rs);
            }
        }
    }
    return rs;
}

hcchar * vmRuntimeContextGetClassName(vmRuntimeContext context, vmClass * clazz,InvokeTickDeclare){
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    assert(context);
    return vmRuntimeClassLoaderGetClassName(c->loader,clazz,InvokeTickArg);
}

void vmRuntimeContextInvokeBegin(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain domain = {0};
    assert(context);
    
    serial_list_item_add(c->domains, &domain);
}

void vmRuntimeContextInvokeEnd(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = serial_list_item_last(c->domains);
    hint32 i,n;
    vmVariant * var;
    assert(context && domain);
    
    n = serial_map_count(domain->variantMap);
    for(i=0;i<n;i++){
        var = serial_map_value_at(domain->variantMap, i);
        if(var){
            vmRuntimeContextVariantRelease(context, * var,InvokeTickArg);
        }
    }
    
    serial_map_dealloc(domain->variantMap);
    
    n = list_count(domain->retainObjects);
    
    for(i=0;i<n;i++){
        vmObjectRelease(context, (vmObject *)list_get(domain->retainObjects, i),InvokeTickArg);
    }
    
    list_dealloc(domain->retainObjects);
    
    serial_list_remove_last(c->domains);
}

void vmRuntimeContextDomainAddObject(vmRuntimeContext context,vmObject * obj,InvokeTickDeclare){
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = serial_list_item_last(c->domains);
    assert(context && domain);
    
    if(domain->retainObjects == NULL){
        domain->retainObjects = list_alloc(32, 32);
    }
    
    if(obj){
        list_add(domain->retainObjects, vmObjectRetain(context, obj, InvokeTickArg));
    }
}

void vmRuntimeContextSuperDomainAddObject(vmRuntimeContext context,vmObject * obj,InvokeTickDeclare){

    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = NULL;
    hint32 n;
	assert(context );
	n = serial_list_count(c->domains);
    if(--n >0){
        domain =  serial_list_item_at(c->domains, n-1);
        if(domain->retainObjects == NULL){
            domain->retainObjects = list_alloc(32, 32);
        }
        if(obj){
            list_add(domain->retainObjects, vmObjectRetain(context, obj, InvokeTickArg));
        }
    }
}

vmVariant vmRuntimeContextGetVariant(vmRuntimeContext context,vmUniqueKey name,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = serial_list_item_last(c->domains);
    vmVariant * var = NULL;
    vmVariant rs = {vmVariantTypeVoid,0};
    assert(context && domain);

    var = serial_map_get(domain->variantMap, &name);
    if(var){
        return * var;
    }
    else{
        domain = serial_list_item_at(c->domains, 0);
        assert(domain);
        var = serial_map_get(domain->variantMap, &name);
        if(var){
            return * var;
        }
    }
    return rs;
}

void vmRuntimeContextSetVariant(vmRuntimeContext context,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = NULL;
    vmVariant * var = NULL;
	assert(context && name);
    domain = serial_list_item_last(c->domains);
    assert(domain);
    if(domain->variantMap ==NULL){
        domain->variantMap = serial_map_alloc( sizeof(vmUniqueKey), sizeof(vmVariant), 0);
    }
    var = serial_map_get(domain->variantMap, &name);
    vmRuntimeContextVariantRetain(context, value,InvokeTickArg);
    if(var){
        vmRuntimeContextVariantRelease(context, * var,InvokeTickArg);
    }
    serial_map_put(domain->variantMap, &name, &value);
    
}

hbool vmRuntimeContextHasVariant(vmRuntimeContext context,vmUniqueKey name,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
    _vmRuntimeContextDomain * domain = serial_list_item_last(c->domains);
    vmVariant * var = NULL;
    assert(context && domain);
    
    var = serial_map_get(domain->variantMap, &name);
    if(var){
        return hbool_true;
    }
    return hbool_false;
}

vmUniqueKey vmRuntimeContextFunctionInitKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.functionInitKey;
}

vmUniqueKey vmRuntimeContextFunctionDestroyKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.functionDestroyKey;
}

vmUniqueKey vmRuntimeContextFunctionToStringKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.functionToStringKey;
}

vmUniqueKey vmRuntimeContextFunctionToIntKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.functionToIntKey;
}

vmUniqueKey vmRuntimeContextFunctionToDoubleKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.functionToDoubleKey;
}


vmUniqueKey vmRuntimeContextThisKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.thisKey;
}

vmUniqueKey vmRuntimeContextSuperKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.superKey;
}

vmUniqueKey vmRuntimeContextArgumentsKey(vmRuntimeContext context,InvokeTickDeclare){
    
    _vmRuntimeContext * c = (_vmRuntimeContext *)context;
	assert(context);
    return c->systemKeys.argumentsKey;
}

typedef struct{
    hchar * className;
    vmClass * vmClass;
}_vmRuntimeClass;

typedef struct{
    hlist_t runtimeClasss;
    hmap_t classMap;
    hmap_t classNameMap;
    hlist_t bytesList;
    hlist_t classNames;
    _vmRuntimeClass * mainClass;
    hint32 retainCount;
}_vmRuntimeClassLoader;

vmRuntimeClassLoader vmRuntimeClassLoaderAlloc(InvokeTickDeclare){
    _vmRuntimeClassLoader * loader = (_vmRuntimeClassLoader *)mem_malloc( sizeof(_vmRuntimeClassLoader));
    mem_memset(loader, 0,sizeof(_vmRuntimeClassLoader));
    loader->classMap = map_alloc( hash_code_str, equal_str);
    loader->classNameMap = map_alloc( hash_code_any, equal_any);
    loader->bytesList = list_alloc( 16 , 16);
    loader->runtimeClasss = list_alloc(32, 32);
    loader->classNames = list_alloc(32, 32);
    loader->retainCount = 1;
    
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Object", &vmObjectClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "String", &vmStringClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Number", &vmNumberClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Exception", &vmExceptionClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Array", &vmArrayClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Dictionary", &vmDictionaryClass,InvokeTickArg);
    vmRuntimeClassLoaderRegister((vmRuntimeClassLoader)loader, "Data", &vmDataClass,InvokeTickArg);
    
    return (vmRuntimeClassLoader)loader;
}

hint32 vmRuntimeClassLoaderClassNameCount(vmRuntimeClassLoader loader,InvokeTickDeclare){
     _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    return list_count(l->classNames);
}

hcchar * vmRuntimeClassLoaderClassNameAt(vmRuntimeClassLoader loader,hint32 index,InvokeTickDeclare){
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    return (hcchar *)list_get(l->classNames,index);
}

static void vmRuntimeClassLoaderDealloc(vmRuntimeClassLoader loader,InvokeTickDeclare){
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    hint32 i,c;
    
    c = list_count(l->runtimeClasss);
    for(i=0;i<c;i++){
        mem_free(list_get(l->runtimeClasss, i));
    }
    
    c = list_count(l->bytesList);
    
    for(i=0;i<c;i++){
        mem_free(list_get(l->bytesList, i));
    }
    
    list_dealloc(l->runtimeClasss);
    list_dealloc(l->bytesList);
    list_dealloc(l->classNames);
    map_dealloc(l->classMap);
    map_dealloc(l->classNameMap);
    
    mem_free(l);
}

vmRuntimeClassLoader vmRuntimeClassLoaderRetain(vmRuntimeClassLoader loader,InvokeTickDeclare){
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    l->retainCount ++;
    return loader;
}

void vmRuntimeClassLoaderRelease(vmRuntimeClassLoader loader,InvokeTickDeclare){
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    assert(l->retainCount > 0);
    if(--l->retainCount ==0){
        vmRuntimeClassLoaderDealloc(loader,InvokeTickArg);
    }
}

vmClass * vmRuntimeClassLoaderGetClass(vmRuntimeClassLoader loader,hcchar * className,InvokeTickDeclare){
    
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    _vmRuntimeClass * runtimeClass = map_get(l->classMap, (hany)className);
	assert(loader && className);
    return runtimeClass ? runtimeClass->vmClass : NULL;
}

vmClass * vmRuntimeClassLoaderGetMainClass(vmRuntimeClassLoader loader,InvokeTickDeclare){
    
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    _vmRuntimeClass * runtimeClass = l->mainClass;
	assert(loader);
    return runtimeClass ? runtimeClass->vmClass : NULL;
}

hcchar * vmRuntimeClassLoaderGetClassName(vmRuntimeClassLoader loader,vmClass * vmClass,InvokeTickDeclare){
    _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
    return (hcchar *)map_get(l->classNameMap, vmClass);
}

hbool vmRuntimeClassLoaderRegister(vmRuntimeClassLoader loader,hcchar * className,vmClassBase * clazz,InvokeTickDeclare){
    if(loader && className && clazz 
       && clazz->init && clazz->destroy && clazz->getProperty && clazz->setProperty 
       && clazz->invoke && clazz->propertyNames && clazz->size >= sizeof(vmObject)){
        _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
        _vmRuntimeClass * runtimeClass = map_get(l->classMap, (hany)className);
        if(runtimeClass ==NULL){
            runtimeClass = mem_malloc( sizeof(_vmRuntimeClass) + strlen(className) +1);
            mem_memset(runtimeClass, 0,sizeof(_vmRuntimeClass));
            runtimeClass->vmClass = (vmClass *)clazz;
            runtimeClass->className = (hchar *)runtimeClass + sizeof(_vmRuntimeClass);
            mem_strcpy(runtimeClass->className, className);
            
            map_put(l->classMap, runtimeClass->className, runtimeClass);
            map_put(l->classNameMap,runtimeClass->vmClass,runtimeClass->className);
            list_add(l->runtimeClasss, runtimeClass);
            list_add(l->classNames, runtimeClass->className);
        }
        else{
            runtimeClass->vmClass = (vmClass *)clazz;
        }
        return hbool_true;
    }
    return hbool_false;
}

typedef union{
    struct{
        hchar high;
        hchar low;
    }bits;
    huint16 int16;
}BitsSign;

void vmRuntimeClassLibraryBytesInit(vmRuntimeClassLibraryBytes * bytes,InvokeTickDeclare){
    BitsSign bits;
    bits.int16 = 0x0100;
    mem_memset(bytes, 0, sizeof(vmRuntimeClassLibraryBytes));
    mem_strcpy(bytes->head.tag, vmRuntimeHead);
    mem_strcpy(bytes->head.version, vmRuntimeVersion);
    bytes->head.high = bits.bits.high;
    bytes->head.low = bits.bits.low;
}

hbool vmRuntimeClassLibraryBytesValidate(vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare){
    BitsSign bits;
    bits.int16 = 0x0100;
    if(!bytes){
        return hbool_false;
    }
    if(length < sizeof(vmRuntimeClassLibraryBytes)){
        return hbool_false;
    }
    if(mem_strcmp(bytes->head.tag, vmRuntimeHead)){
        return hbool_false;
    }
    if(mem_strcmp(bytes->head.version, vmRuntimeVersion)){
        return hbool_false;
    }
    if(bytes->head.high != bits.bits.high || bytes->head.low != bits.bits.low){
        return hbool_false;
    }
    return hbool_true;
}

void vmRuntimeClassLibraryBytesPrint(vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare){
    if(vmRuntimeClassLibraryBytesValidate(bytes,length,InvokeTickArg)){
        hint32 i,j;
        vmRuntimeClassMetaBytes * classMeta;
        vmClassMetaOffset * uniqueKeyOffset;
        vmClassMeta * rClassMeta;
        vmRuntimeMetaOperator *op;
        vmClassMetaOffset * opOffset;
        hchar * className;
        hchar * superClass;
        hchar * uniqueKey;
        hlog("%s\n",bytes->head.tag);
        hlog("version: %s\n",bytes->head.version);
        hlog("bits: %d%d\n",bytes->head.high,bytes->head.low);
        hlog("class count: %d\n",bytes->classCount);
        hlog("unique key count: %d\n",bytes->uniqueKeyCount);
        hlog("class:\n");
        
        classMeta = (vmRuntimeClassMetaBytes *)((hbyte *)bytes +sizeof(vmRuntimeClassLibraryBytes));
        for(i=0;i<bytes->classCount;i++){
            className = (hchar *)((hbyte *)bytes + classMeta[i].className);
            rClassMeta = (vmClassMeta *)((hbyte *)bytes + classMeta[i].classMeta);
            superClass = (hchar *)((hbyte *)rClassMeta - rClassMeta->offset + rClassMeta->superClass);
            hlog("\t%s extends %s\n",className,superClass);
            hlog("\t\tpropertys %d:\n",rClassMeta->propertys);
            opOffset = (vmClassMetaOffset *)((hbyte *)rClassMeta + sizeof(vmClassMeta));
            for(j=0;j<rClassMeta->propertys;j++){
                op =  (vmRuntimeMetaOperator *)( (hbyte *)rClassMeta - rClassMeta->offset +opOffset[j]);
                uniqueKey = (hchar *)((hbyte *)rClassMeta - rClassMeta->offset  + op->uniqueKey);
                hlog("\t\t\t%s\n",uniqueKey);
            }
            opOffset = (vmClassMetaOffset *)((hbyte *)rClassMeta + sizeof(vmClassMeta) + sizeof(vmClassMetaOffset) * rClassMeta->propertys);
            hlog("\t\tfunctions %d:\n",rClassMeta->functions);
            for(j=0;j<rClassMeta->functions;j++){
                op =  (vmRuntimeMetaOperator *)( (hbyte *)rClassMeta - rClassMeta->offset +opOffset[j]);
                uniqueKey = (hchar *)((hbyte *)rClassMeta - rClassMeta->offset  + op->uniqueKey);
                hlog("\t\t\t%s\n",uniqueKey);
            }
        }
        
        hlog("unique key:\n");
        uniqueKeyOffset =(vmClassMetaOffset *) ((hbyte *)bytes +sizeof(vmRuntimeClassLibraryBytes) + bytes->classCount * sizeof(vmRuntimeClassMetaBytes));
        for(i=0;i<bytes->uniqueKeyCount;i++){
            uniqueKey = (hchar *)((hbyte *)bytes + uniqueKeyOffset[i]);
            hlog("\t%s\n",uniqueKey);
        }
    }
    else{
        hlog("library bytes validate error.\n");
    }
}

static hbool vmRuntimeClassLoaderClassHasFunction(vmClassMeta * classMeta,hcchar * name,InvokeTickDeclare){
    hint32 i;
    vmClassMetaOffset * pOffset = (vmClassMetaOffset *)((hbyte *)classMeta + sizeof(vmClassMeta) + sizeof(vmClassMetaOffset) * classMeta->propertys);
    vmRuntimeMetaOperator * op;
    hcchar * function;
    for(i=0;i<classMeta->functions;i++){
        op = (vmRuntimeMetaOperator *) ( (hbyte *) classMeta - classMeta->offset + pOffset[i] );
        function = (hcchar *)( (hbyte *) classMeta - classMeta->offset + op->uniqueKey);
        if(mem_strcmp(name, function) ==0){
            return hbool_true;
        }
    }
    return hbool_false;
}

hbool vmRuntimeClassLoaderLoadBytesNotCopy(vmRuntimeClassLoader loader,vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare){
    if(loader && vmRuntimeClassLibraryBytesValidate(bytes,length,InvokeTickArg)){
        _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
        hbyte * b = (hbyte *)bytes;
        hint32 i;
        vmRuntimeClassMetaBytes * bytesMeta;
        _vmRuntimeClass * runtimeClass;
        vmClassMeta * classMeta;
        hchar * className;

        bytesMeta = (vmRuntimeClassMetaBytes *)(b + sizeof(vmRuntimeClassLibraryBytes));
        
        for(i=0;i<bytes->classCount;i++){
            className = (hchar *)(b + bytesMeta[i].className);
            classMeta = (vmClassMeta *)(b + bytesMeta[i].classMeta);
            assert(classMeta->type == vmClassTypeMeta);
            
            runtimeClass = map_get(l->classMap, (hany)className);
            if(runtimeClass ==NULL){
                runtimeClass = mem_malloc( sizeof(_vmRuntimeClass));
                mem_memset(runtimeClass, 0,sizeof(_vmRuntimeClass));
                runtimeClass->vmClass = (vmClass *)classMeta;
                runtimeClass->className = className;
                map_put(l->classMap, runtimeClass->className, runtimeClass);
                map_put(l->classNameMap,classMeta,runtimeClass->className);
                list_add(l->runtimeClasss, runtimeClass);
                list_add(l->classNames, runtimeClass->className);
            }
            else{
                runtimeClass->className = className;
                runtimeClass->vmClass = (vmClass *)classMeta;
            }
            if(l->mainClass == NULL){
                if(vmRuntimeClassLoaderClassHasFunction(classMeta,"main",InvokeTickArg)){
                    l->mainClass = runtimeClass;
                }
            }
        }
        
        list_add(l->bytesList, bytes); 
        
        return hbool_true;

    }
    return hbool_false;
}

hbool vmRuntimeClassLoaderLoadBytes(vmRuntimeClassLoader loader,vmRuntimeClassLibraryBytes * bytes,hint32 length,InvokeTickDeclare){
    
    if(loader && vmRuntimeClassLibraryBytesValidate(bytes,length,InvokeTickArg)){
        _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
        hbyte * b = mem_malloc( length);
        mem_memcpy(b, bytes, length);
        
        if(vmRuntimeClassLoaderLoadBytesNotCopy(loader,(vmRuntimeClassLibraryBytes *)b,length,InvokeTickArg)){
            list_add(l->bytesList, b);
        }
        else{
            mem_free( b);
        }
    }
    
    return hbool_false;

}

hbool vmRuntimeClassLoaderLoadFile(vmRuntimeClassLoader loader,hcchar * file,InvokeTickDeclare){
    if(loader && file && file_exist(file)){
        hint32 length = file_size(file);
        hbyte * b;
        FILE * f = fopen(file, "rb");
        hbool rs = hbool_false;
        if(f){
            
            b = mem_malloc( length);
            
            if(fread(b,length, 1, f) ==1){
                rs = vmRuntimeClassLoaderLoadBytesNotCopy(loader,(vmRuntimeClassLibraryBytes *)b,length,InvokeTickArg);
                if(!rs){
                    mem_free( b);
                }
            }
            else{
                mem_free( b);
            }
            
            fclose(f);
        }
        
        return rs;
    }
    return hbool_false;
}

hbool vmRuntimeClassLoaderUniqueKeysRegister(vmRuntimeClassLoader loader,vmRuntimeContext context,InvokeTickDeclare){
    if(loader && context){
        _vmRuntimeClassLoader * l = (_vmRuntimeClassLoader *)loader;
        vmRuntimeClassLibraryBytes * classBytes = NULL;
        vmClassMetaOffset * uniqueKeyOffset;
        hcchar * uniqueKey;
        hint32 i,j,c = list_count(l->bytesList);
        for(i=0;i<c;i++){
            classBytes = (vmRuntimeClassLibraryBytes *) list_get(l->bytesList, i);
            uniqueKeyOffset = (vmClassMetaOffset *)((hbyte *) classBytes + sizeof(vmRuntimeClassLibraryBytes) + sizeof(vmRuntimeClassMetaBytes) * classBytes->classCount);
            for(j=0;j<classBytes->uniqueKeyCount;j++){
                uniqueKey = (hcchar *)( (hbyte *) classBytes + uniqueKeyOffset[j] );
                vmRuntimeContextRegisterUniqueKey(context, uniqueKey ,InvokeTickArg);
            }
        }
    }
    return hbool_false;
}

