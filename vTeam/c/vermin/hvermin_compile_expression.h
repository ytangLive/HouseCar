//
//  hvermin_compile_expression.h
//  C Library
//
//  Created by Hailong Zhang on 11-10-27.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_COMPILE_EXPRESSION_H
#define _HVERMIN_COMPILE_EXPRESSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hvermin_compile.h"
#include "hbuffer.h" 
    
    hbool vmCompileExpressionString(hcchar * exp,vmCompileMeta ** meta,InvokeTickDeclare);
    
#define vmCompileExpressionString(a,b) vmCompileExpressionString((a),(b),InvokeTickArg)

#ifdef __cplusplus
}
#endif

#endif
