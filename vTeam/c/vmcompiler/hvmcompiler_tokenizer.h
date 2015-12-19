//
//  hvmcompiler_tokenizer.h
//  vmcompiler
//
//  Created by zhang hailong on 13-3-13.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_tokenizer_h
#define vmcompiler_hvmcompiler_tokenizer_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "htokenizer.h"
#include "hvmcompiler.h"
    
    htokenizer_t * vm_compiler_tokenizer(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);
    
    void vm_compiler_tokenizer_comment_clear(htokenizer_t * tokenizer,InvokeTickDeclare);
    
    void vm_compiler_tokenizer_optimization(htokenizer_t * tokenizer,InvokeTickDeclare);
    
    hint32 vm_compiler_tokenizer_operator_index_of(htokenizer_t * tokenizer,hcchar * op,hint32 index,InvokeTickDeclare);
    
    hint32 vm_compiler_tokenizer_group_index_of(htokenizer_t * tokenizer,hchar beginChar,hint32 index,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif



#endif
