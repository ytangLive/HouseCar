//
//  hvermin_compile_scanner.h
//  C Library
//
//  Created by Hailong Zhang on 11-10-24.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_COMPILE_SCANNER_H
#define _HVERMIN_COMPILE_SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin_compile.h"
#include "hbuffer.h"
    

    typedef struct{
        hint32 state;
        hbuffer_t value;
        hint32 level;
        hint32 line;
        hint32 index;
    }vmScannerCompileBuffer;
    
    
    vmScannerFilterState vmScannerCompileBufferExpressionAppend(vmScannerContext * context,vmScannerCompileBuffer * buffer,hchar c,hchar endChar,InvokeTickDeclare);
    
    vmScannerFilterState vmScannerCompileBufferInvokeAppend(vmScannerContext * context,vmScannerCompileBuffer * buffer,hchar c,InvokeTickDeclare);
    
    void vmScannerCompileBufferClear(vmScannerCompileBuffer * buffer,InvokeTickDeclare);

#define vmScannerCompileBufferExpressionAppend(a,b,c,d) vmScannerCompileBufferExpressionAppend((a),(b),(c),(d),InvokeTickArg)
    
#define vmScannerCompileBufferInvokeAppend(a,b,c) vmScannerCompileBufferInvokeAppend((a),(b),(c),InvokeTickArg)
    
#define vmScannerCompileBufferClear(a) vmScannerCompileBufferClear((a),InvokeTickArg)
    
    typedef struct{
        vmScannerFilter base;
        hint32 state;
    }vmScannerCompileFilter;
    
    
    typedef struct{
        vmScannerCompileFilter base;
    }vmScannerNoteClearFilter;

    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t name;
        hbuffer_t type;
        vmScannerCompileBuffer value;
    }vmScannerPropertyFilter;

    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t name;
        vmScannerCompileBuffer args;
        vmScannerCompileBuffer invoke;
    }vmScannerFunctionFilter;

    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t name;
    }vmScannerExtendsFilter;

    typedef struct{
        vmScannerContext base;
        vmScannerNoteClearFilter noteClearFilter;
        vmScannerExtendsFilter extendsFilter;
        vmScannerPropertyFilter propertyFilter;
        vmScannerFunctionFilter functionFilter;
    }vmScannerClassContext;
    

    hbool vmScannerClassContextInit(vmScannerClassContext * context,InvokeTickDeclare);
    
    void vmScannerClassContextDestroy(vmScannerClassContext * context,InvokeTickDeclare);
    
#define vmScannerClassContextInit(a) vmScannerClassContextInit((a),InvokeTickArg)
    
#define vmScannerClassContextDestroy(a) vmScannerClassContextDestroy((a),InvokeTickArg)
    

    
    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t name;
        vmScannerCompileBuffer exp;
    }vmScannerVarFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp;
        vmScannerCompileBuffer invoke;
    }vmScannerIfFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp;
        vmScannerCompileBuffer invoke;
    }vmScannerElseIfFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer invoke;
    }vmScannerElseFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp;
        vmScannerCompileBuffer invoke;
    }vmScannerWhileFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp1;
        vmScannerCompileBuffer exp2;
        vmScannerCompileBuffer exp3;
        vmScannerCompileBuffer invoke;
    }vmScannerForFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp1;
        vmScannerCompileBuffer exp2;
        vmScannerCompileBuffer invoke;
    }vmScannerForInFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer invoke;
    }vmScannerTryFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t className;
        hbuffer_t name;
        vmScannerCompileBuffer invoke;
    }vmScannerCatchFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp;
    }vmScannerThrowFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
         vmScannerCompileBuffer invoke;
    }vmScannerFinallyFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
        vmScannerCompileBuffer exp;
    }vmScannerReturnFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
    }vmScannerBreakFilter;
    
    typedef struct{
        vmScannerCompileFilter base;
    }vmScannerContinueFilter;
    
    
    typedef struct{
        vmScannerCompileFilter base;
        hbuffer_t expression;
    }vmScannerStatementFilter;
    
    typedef struct _vmScannerInvokeContext{
        vmScannerContext base;
        vmScannerNoteClearFilter noteClearFilter;
        vmScannerVarFilter varFilter;
        vmScannerElseIfFilter elseIfFilter;
        vmScannerIfFilter ifFilter;
        vmScannerElseFilter elseFilter;
        vmScannerWhileFilter whileFilter;
        vmScannerForFilter  forFilter;
        vmScannerForInFilter forInFilter;
        vmScannerTryFilter tryFilter;
        vmScannerCatchFilter catchFilter;
        vmScannerThrowFilter throwFilter;
        vmScannerFinallyFilter finallyFilter;
        vmScannerReturnFilter returnFilter;
        vmScannerBreakFilter breakFilter;
        vmScannerContinueFilter continueFilter;
        vmScannerStatementFilter statementFilter;
    }vmScannerInvokeContext;
    
    
    hbool vmScannerInvokeContextInit(vmScannerInvokeContext * context,InvokeTickDeclare);
    
    void vmScannerInvokeContextDestroy(vmScannerInvokeContext * context,InvokeTickDeclare);
    
#define vmScannerInvokeContextInit(a) vmScannerInvokeContextInit((a),InvokeTickArg)
    
#define vmScannerInvokeContextDestroy(a) vmScannerInvokeContextDestroy((a),InvokeTickArg)
    
    

    
#ifdef __cplusplus
}
#endif

#endif
