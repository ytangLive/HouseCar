//
//  hvmcompiler_class.h
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_class_h
#define vmcompiler_hvmcompiler_class_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "htokenizer.h"
#include "hvmcompiler.h"    

    hbool vm_compiler_class(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
