//
//  hvermin_runtime_variant.c
//  C Library
//
//  Created by Hailong Zhang on 11-11-1.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_runtime_variant.h"
#include "hvermin_object.h"
#include "hstr.h"
#include "hlog.h"

#undef vmVariantToDouble

#undef vmVariantToInt64

#undef vmVariantToInt32

#undef vmVariantToInt16

#undef vmVariantToBoolean

#undef vmVariantToString

#undef vmVariantToObject

#undef vmVariantTypeFor

#undef vmVariantTypeForMin

#undef vmVariantPrint

#undef vmVariantEqual

hdouble vmVariantToDouble(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    if(var.type & vmVariantTypeDouble){
        return var.value.doubleValue;
    }
    else if(var.type & vmVariantTypeString){
        return var.value.stringValue ? atof(var.value.stringValue) : 0;
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            return vmVariantToDouble(context,vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, context->systemKeys.functionToDoubleKey, NULL),InvokeTickArg);
        }
        else{
            return 0.0;
        }
    }
    else if(var.type & vmVariantTypeInt16){
        return var.value.int16Value;
    }
    else if(var.type & vmVariantTypeInt32){
        return var.value.int32Value;
    }
    else if(var.type & vmVariantTypeInt64){
        return var.value.int64Value;
    }
    else if(var.type & vmVariantTypeBoolean){
        return var.value.booleanValue;
    }
    return 0.0;
}

hint64 vmVariantToInt64(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    if(var.type & vmVariantTypeDouble){
        return (hint64)var.value.doubleValue;
    }
    else if(var.type & vmVariantTypeString){
        if(str_has_prefix(var.value.stringValue, "0x")){
            hint64 v =0;
            sscanf(var.value.stringValue, "0x%llx",&v);
            return v;
        }
        return var.value.stringValue ? atoll(var.value.stringValue) : 0;
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            return vmVariantToInt64(context,vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, context->systemKeys.functionToIntKey, NULL),InvokeTickArg);
        }
        else{
            return 0;
        }
    }
    else if(var.type & vmVariantTypeInt16){
        return var.value.int16Value;
    }
    else if(var.type & vmVariantTypeInt32){
        return var.value.int32Value;
    }
    else if(var.type & vmVariantTypeInt64){
        return var.value.int64Value;
    }
    else if(var.type & vmVariantTypeBoolean){
        return var.value.booleanValue;
    }
    return 0;
}

hint32 vmVariantToInt32(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    if(var.type & vmVariantTypeDouble){
        return (hint32)var.value.doubleValue;
    }
    else if(var.type & vmVariantTypeString){
        return var.value.stringValue ? atoi(var.value.stringValue) : 0;
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            return vmVariantToInt32(context,vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, context->systemKeys.functionToIntKey, NULL),InvokeTickArg);
        }
        else{
            return 0;
        }
    }
    else if(var.type & vmVariantTypeInt16){
        return var.value.int16Value;
    }
    else if(var.type & vmVariantTypeInt32){
        return var.value.int32Value;
    }
    else if(var.type & vmVariantTypeInt64){
        return (hint32)var.value.int64Value;
    }
    else if(var.type & vmVariantTypeBoolean){
        return var.value.booleanValue;
    }
    return 0;
}

hint16 vmVariantToInt16(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    if(var.type & vmVariantTypeDouble){
        return (hint16)var.value.doubleValue;
    }
    else if(var.type & vmVariantTypeString){
        return var.value.stringValue ? atoi(var.value.stringValue) : 0;
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            return vmVariantToInt16(context,vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, context->systemKeys.functionToIntKey, NULL),InvokeTickArg);
        }
        else{
            return 0;
        }
    }
    else if(var.type & vmVariantTypeInt16){
        return var.value.int16Value;
    }
    else if(var.type & vmVariantTypeInt32){
        return var.value.int32Value;
    }
    else if(var.type & vmVariantTypeInt64){
        return var.value.int64Value;
    }
    else if(var.type & vmVariantTypeBoolean){
        return var.value.booleanValue;
    }
    return 0;
}

hbool vmVariantToBoolean(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    if(var.type & vmVariantTypeDouble){
        return var.value.doubleValue ? hbool_true : hbool_false;
    }
    else if(var.type & vmVariantTypeString){
        return var.value.stringValue ? hbool_true : hbool_false;
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            return hbool_true;
        }
        else{
            return hbool_false;
        }
    }
    else if(var.type & vmVariantTypeInt16){
        return var.value.int16Value ? hbool_true : hbool_false;
    }
    else if(var.type & vmVariantTypeInt32){
        return var.value.int32Value ? hbool_true : hbool_false;
    }
    else if(var.type & vmVariantTypeInt64){
        return var.value.int64Value ? hbool_true : hbool_false;
    }
    else if(var.type & vmVariantTypeBoolean){
        return var.value.booleanValue ? hbool_true : hbool_false;
    }
    return hbool_false;
}

void vmVariantToString(vmRuntimeContext context,vmVariant var,hbuffer_t buffer,InvokeTickDeclare){
    vmVariant rs;
    if(var.type & vmVariantTypeDouble){
        buffer_append_format(buffer, "%f", InvokeTickArg,var.value.doubleValue);
    }
    else if(var.type & vmVariantTypeString){
        buffer_append_str(buffer, var.value.stringValue);
    }
    else if(var.type & vmVariantTypeBoolean){
        buffer_append_str(buffer, var.value.booleanValue ? "true":"false");
    }
    else if(var.type & vmVariantTypeObject){
        if(var.value.objectValue){
            rs = vmObjectInvoke(context, var.value.objectValue->vmClass, var.value.objectValue, context->systemKeys.functionToStringKey, NULL);
            vmVariantToString(context,rs,buffer,InvokeTickArg);
        }
    }
    else if(var.type & vmVariantTypeInt16){
        buffer_append_format(buffer, "%d", InvokeTickArg,var.value.int16Value);
    }
    else if(var.type & vmVariantTypeInt32){
        buffer_append_format(buffer, "%d", InvokeTickArg,var.value.int32Value);
    }
    else if(var.type & vmVariantTypeInt64){
        buffer_append_format(buffer, "%lld", InvokeTickArg,var.value.int64Value);
    }
}

vmVariant vmVariantToObject(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeObject,0};
    if(var.type == vmVariantTypeInt16){
        rs = vmNumberInt16Alloc(context, var.value.int16Value);
    }
    else if(var.type == vmVariantTypeInt32){
        rs = vmNumberInt32Alloc(context, var.value.int32Value);
    }
    else if(var.type == vmVariantTypeInt64){
        rs = vmNumberInt64Alloc(context, var.value.int64Value);
    }
    else if(var.type == vmVariantTypeDouble){
        rs = vmNumberDoubleAlloc(context, var.value.doubleValue);
    }
    else if(var.type == vmVariantTypeBoolean){
        rs = vmNumberBooleanAlloc(context, var.value.booleanValue);
    }
    else if(var.type == vmVariantTypeString){
        rs = vmStringAlloc(context, var.value.stringValue);
    }
    else if((var.type & vmVariantTypeObject) && var.value.objectValue){
        rs.type = var.type;
        rs.value.objectValue = var.value.objectValue;
        rs.vmClass = var.vmClass;
    }
    else{
        rs = vmObjectAlloc(context, (vmClass *)&vmObjectClass, NULL);
    }
    return rs;
}

void vmVariantTypeFor(vmVariant var1,vmVariant var2,vmVariantType * lowType,vmVariantType * highType, InvokeTickDeclare){
    vmVariantType type1 = var1.type & (vmVariantTypeInt16 | vmVariantTypeInt32 | vmVariantTypeInt64 | vmVariantTypeBoolean | vmVariantTypeDouble | vmVariantTypeString | vmVariantTypeObject);
    vmVariantType type2 = var2.type & (vmVariantTypeInt16 | vmVariantTypeInt32 | vmVariantTypeInt64 | vmVariantTypeBoolean | vmVariantTypeDouble | vmVariantTypeString | vmVariantTypeObject);
    if(((hint32)type1 - (hint32)type2) >0){
        * lowType = type2;
        * highType = type1;
    }
    else{
        * lowType = type1;
        * highType = type2;
    }
}


void vmVariantPrint(vmRuntimeContext context,vmVariant var,InvokeTickDeclare){
    hbuffer_t str = buffer_alloc(128, 128);
    vmVariantToString(context, var, str, InvokeTickArg);
    hlog("%s\n",buffer_to_str(str));
    buffer_dealloc(str);
}

hbool vmVariantEqual(vmRuntimeContext context,vmVariant var1,vmVariant var2,InvokeTickDeclare){
    vmVariantType lowType = vmVariantTypeVoid,highType = vmVariantTypeVoid;
    vmVariant rs = {vmVariantTypeBoolean,0};
    hbuffer_t str1 =buffer_alloc(128, 128);
    hbuffer_t str2 = buffer_alloc(128, 128);
    
	vmVariantTypeFor(var1, var2, &lowType, &highType, InvokeTickArg);

    if(lowType == vmVariantTypeObject && highType == vmVariantTypeObject){
        if(var1.value.objectValue && var2.value.objectValue){
            {
                vmVariantList vars = vmVariantListAlloc();
                vmVariantListAdd(vars, var2);
                rs = vmObjectInvoke(context,var1.value.objectValue->vmClass, var1.value.objectValue, context->systemKeys.functionEqualKey,vars);
                vmVariantListDealloc(vars);
            }
            
        }
        else{
            rs.value.booleanValue = var1.value.objectValue == var1.value.objectValue;
        }
    }
    else if(highType == vmVariantTypeString || highType == vmVariantTypeObject){
        buffer_clear(str1);
        buffer_clear(str2);
        vmVariantToString(context, var1, str1,InvokeTickArg);
        vmVariantToString(context, var2, str2,InvokeTickArg);
        rs.value.booleanValue = str_cmp(buffer_to_str(str1),buffer_to_str(str2)) ==0 ;
    }
    else{
        rs.value.booleanValue = vmVariantToDouble(context,var1,InvokeTickArg) == vmVariantToDouble(context,var2,InvokeTickArg);
    }
    buffer_dealloc(str1);
    buffer_dealloc(str2);
    return rs.value.booleanValue;
}
