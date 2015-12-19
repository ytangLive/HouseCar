//
//  hvmcompiler_exec.h
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_exec_h
#define vmcompiler_hvmcompiler_exec_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvmcompiler.h"
    
    
    vmCompilerClassMeta * vmCompilerClassExec(hcchar * source,hobj_array_t * errors,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
