//
//  hvermin_runtime_variant.h
//  C Library
//
//  Created by Hailong Zhang on 11-11-1.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_RUNTIME_VARIANT_H
#define _HVERMIN_RUNTIME_VARIANT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hvermin.h"
#include "hbuffer.h"
    
#define vmVariantTypeMask   (vmVariantTypeInt16 | vmVariantTypeInt32 | vmVariantTypeInt64 | vmVariantTypeBoolean | vmVariantTypeDouble | vmVariantTypeObject | vmVariantTypeThrowable)
    
    hdouble vmVariantToDouble(vmRuntimeContext context, vmVariant var,InvokeTickDeclare);
    
    hint64 vmVariantToInt64(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    hint32 vmVariantToInt32(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    hint16 vmVariantToInt16(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    hbool vmVariantToBoolean(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    void vmVariantToString(vmRuntimeContext context,vmVariant var,hbuffer_t buffer,InvokeTickDeclare);
    
    void vmVariantTypeFor(vmVariant var1,vmVariant var2,vmVariantType * lowType,vmVariantType * highType, InvokeTickDeclare);

    vmVariant vmVariantToObject(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    void vmVariantPrint(vmRuntimeContext context,vmVariant var,InvokeTickDeclare);
    
    hbool vmVariantEqual(vmRuntimeContext context,vmVariant var1,vmVariant var2,InvokeTickDeclare);
    
#define vmVariantToDouble(a,b) vmVariantToDouble((a),(b),InvokeTickArg)
    
#define vmVariantToInt64(a,b) vmVariantToInt64((a),(b),InvokeTickArg)
    
#define vmVariantToInt32(a,b) vmVariantToInt32((a),(b),InvokeTickArg)
    
#define vmVariantToInt16(a,b) vmVariantToInt16((a),(b),InvokeTickArg)
    
#define vmVariantToBoolean(a,b) vmVariantToBoolean((a),(b),InvokeTickArg)
    
#define vmVariantToString(a,b,c) vmVariantToString((a),(b),(c),InvokeTickArg)

#define vmVariantTypeFor(a,b,c,d) vmVariantTypeFor((a),(b),(c),(d),InvokeTickArg)
    
#define vmVariantToObject(a,b) vmVariantToObject((a),(b),InvokeTickArg)

#define vmVariantPrint(a,b) vmVariantPrint((a),(b),InvokeTickArg)
    
#define vmVariantEqual(a,b,c) vmVariantEqual((a),(b),(c),InvokeTickArg)
                         
#ifdef __cplusplus
}
#endif

#endif
