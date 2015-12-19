//
//  hvmcompiler_expression.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_expression.h"

static hbool _vm_compiler_expression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);

static hbool _vm_compiler_combi_action(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg),index = 0,length = 0;
    htokenizer_t * token ;
    
    for(i=0;i<c;i++){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
            
            if( * token->range.begin.p != '.'){
                if(length >1){
                    htokenizer_comib(tokenizer, index, length, InvokeTickArg);
                    i = index;
                    c = c - length +1;
                }
                length = 0;
                index = 0;
            }
            else{
                if(length == 0){
                    length = 1;
                    index =  i;
                }
                else{
                    length ++;
                }
            }
        }
        else{
            
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)){
                if(* token->range.begin.p == '(' || * token->range.begin.p == '['){
                    if(! _vm_compiler_expression(token,errors,InvokeTickArg)){
                        return hbool_false;
                    }
                    
                    if(length == 0){
                        length = 1;
                        index =  i;
                    }
                    else{
                        length ++;
                    }
                }
                else{
                    vmCompilerErrorSet(token->range.begin,"expression action");
                    return hbool_false;
                }
            }
            else if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                if(length == 0){
                    length = 1;
                    index =  i;
                }
                else{
                    length ++;
                }
            }
            else{
                if(length >1){
                    htokenizer_comib(tokenizer, index, length, InvokeTickArg);
                    i = index;
                    c = c - length +1;
                }
                length = 0;
                index = 0;
            }
        }
        
    }
    
    if(length >1){
        htokenizer_comib(tokenizer, index, length, InvokeTickArg);
    }
    
    return hbool_true;
}

static hbool _vm_compiler_expression_operator_isop(hchar (*op)[HTOKENIZER_MAX_OPSIZE],hint32 opCount,hchar * bp,hchar * ep){
    hchar * p = bp,* opp;
    
    while(opCount>0){
        
        p = bp;
        opp =  * op;
        
        while(p <= ep){
            if(*p != *opp){
                break;
            }
            p ++;
            opp ++;
        }
        
        if(p > ep && *opp == '\0'){
            return hbool_true;
        }
        
        opCount --;
        op ++;
    }
    
    return hbool_false;
}

#define VM_COMPILE_OP_TYPE_ONE_LR   0x01
#define VM_COMPILE_OP_TYPE_ONE_R    0x02
#define VM_COMPILE_OP_TYPE_TOW      0x03
#define VM_COMPILE_OP_TYPE_THREE    0x04

static hbool _vm_compiler_expression_operator(htokenizer_t * tokenizer,hchar (*op)[HTOKENIZER_MAX_OPSIZE],hint32 opCount,hint32 type,hobj_array_t * errors,InvokeTickDeclare){
    hint32 i,c;
    htokenizer_t * token , *prevToken = NULL,* nextToken = NULL;
    
    c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    
    for(i=0;i<c;i++){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        if(i +1 <c){
            nextToken = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i + 1, InvokeTickArg);
        }
        else{
            nextToken = NULL;
        }
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
            
            if(_vm_compiler_expression_operator_isop(op,opCount,token->range.begin.p,token->range.end.p)){
                
                switch (type) {
                    case VM_COMPILE_OP_TYPE_ONE_LR:
                    {
                        if(prevToken && !hobj_is_kind((hobj_t *) prevToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                            prevToken = (htokenizer_t *)htokenizer_comib(tokenizer, i -1, 2, InvokeTickArg);
                            c = c -1;
                            i = i -1;
                            continue;
                        }
                        
                        if(nextToken && !hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                            prevToken = (htokenizer_t *)htokenizer_comib(tokenizer, i, 2, InvokeTickArg);
                            c = c -1;
                            continue;
                        }
                        
                        vmCompilerErrorSet(token->range.begin,"expression operator");
                        return hbool_false;
                    }
                        break;
                    case VM_COMPILE_OP_TYPE_ONE_R:
                    {
                        if( token->range.begin.p == token->range.end.p && * token->range.begin.p == '-'){
                            if(prevToken && !hobj_is_kind((hobj_t *) prevToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                                break;
                            }
                        }
                        if(nextToken && !hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                            prevToken = (htokenizer_t *)htokenizer_comib(tokenizer, i, 2, InvokeTickArg);
                            c = c -1;
                            continue;
                        }
                        
                        vmCompilerErrorSet(token->range.begin,"expression operator");
                        return hbool_false;
                    }
                        break;
                    case VM_COMPILE_OP_TYPE_TOW:
                    {
                        if(prevToken && nextToken
                           && !hobj_is_kind((hobj_t *) prevToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
                           && !hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                            
                            prevToken = (htokenizer_t *)htokenizer_comib(tokenizer, i-1, 3, InvokeTickArg);
                            c = c -2;
                            i = i -1;
                            continue;
                        }
                        
                        vmCompilerErrorSet(token->range.begin,"expression operator");
                        return hbool_false;
                    }
                        break;
                    case VM_COMPILE_OP_TYPE_THREE:
                    {
                        if(prevToken && nextToken
                           && !hobj_is_kind((hobj_t *) prevToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
                           && !hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                            
                            if(i + 3 <c){
                                prevToken = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i + 2, InvokeTickArg);
                                nextToken = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i + 3, InvokeTickArg);
                                
                                if(prevToken && nextToken
                                   && hobj_is_kind((hobj_t *) prevToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
                                   && * prevToken->range.begin.p == ':'
                                   && !hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                                    prevToken = (htokenizer_t *)htokenizer_comib(tokenizer, i-1, 5, InvokeTickArg);
                                    c = c -4;
                                    i = i -1;
                                    continue;
                                }
                            }
                        }
                        vmCompilerErrorSet(token->range.begin,"expression operator");
                        return hbool_false;
                    }
                        break;
                    default:
                        break;
                }
                
            }
            
        }
        
        prevToken = token;
    }
    
    return hbool_true;
}


static hbool _vm_compiler_expression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    static hchar ops[][HTOKENIZER_MAX_OPSIZE] = {
        "++","--","-",
        "!","~",
        "*","/","%",
        "+","-",
        "<<",">>",
        ">",">=","<","<=",
        "==","!=","===","!==",
        "&",
        "^",
        "|",
        "&&",
        "||",
        
        "?",
        
        "=","/=","*=","%=","+=","-=","<<=",">>=","&=","^=","|=",
        
        "is",
        "new",
    };
    

    if(!_vm_compiler_combi_action(tokenizer,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    //htokenizer_log(tokenizer, InvokeTickArg);
    
    // new 
    if(!_vm_compiler_expression_operator(tokenizer,ops + 38,1
                                         ,VM_COMPILE_OP_TYPE_ONE_R,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // ++ --
    if(!_vm_compiler_expression_operator(tokenizer,ops,2
                                                ,VM_COMPILE_OP_TYPE_ONE_LR,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    //htokenizer_log(tokenizer, InvokeTickArg);
    
    // -
    if(!_vm_compiler_expression_operator(tokenizer,ops +2,1
                                                ,VM_COMPILE_OP_TYPE_ONE_R,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    //htokenizer_log(tokenizer, InvokeTickArg);
    
    // ! ~
    if(!_vm_compiler_expression_operator(tokenizer,ops +3,2
                                                ,VM_COMPILE_OP_TYPE_ONE_R,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // * / %
    if(!_vm_compiler_expression_operator(tokenizer,ops +5,3
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // + -
    if(!_vm_compiler_expression_operator(tokenizer,ops +8,2
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // << >>
    if(!_vm_compiler_expression_operator(tokenizer,ops +10,2
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // > >= < <=
    if(!_vm_compiler_expression_operator(tokenizer,ops +12,4
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // == != === !==
    if(!_vm_compiler_expression_operator(tokenizer,ops +16,4
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // &
    if(!_vm_compiler_expression_operator(tokenizer,ops +20,1
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // ^
    if(!_vm_compiler_expression_operator(tokenizer,ops +21,1
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // |
    if(!_vm_compiler_expression_operator(tokenizer,ops +22,1
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // is
    if(!_vm_compiler_expression_operator(tokenizer,ops +37,1
                                        ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // &&
    if(!_vm_compiler_expression_operator(tokenizer,ops +23,1
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // ||
    if(!_vm_compiler_expression_operator(tokenizer,ops +24,1
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    // ? :
    if(!_vm_compiler_expression_operator(tokenizer,ops +25,1
                                                ,VM_COMPILE_OP_TYPE_THREE,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    //  "=","/=","*=","%=","+=","-=","<<=",">>=","&=","^=","|=",
    if(!_vm_compiler_expression_operator(tokenizer,ops +26,11
                                                ,VM_COMPILE_OP_TYPE_TOW,errors,InvokeTickArg)){
        return hbool_false;
    }
    
    {
        //htokenizer_log(tokenizer, InvokeTickArg);
        hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
        hint32 s = 0;
        htokenizer_t * token = NULL;
        
        for(i=0;i<c;i++){
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                if( * token->range.begin.p == ','){
                    if(s == 0){
                        vmCompilerErrorSet(token->range.begin,"expression ,");
                        return hbool_false;
                    }
                    s = 0;
                }
                else{
                    vmCompilerErrorSet(token->range.begin,"expression ,");
                    return hbool_false;
                }
            }
            else{
                if(s == 1){
                    vmCompilerErrorSet(token->range.begin,"expression ,");
                    return hbool_false;
                }
                
                s = 1;
            }
        }
        if(token && s == 0){
            vmCompilerErrorSet(token->range.begin,"");
            return hbool_false;
        }
    }
    
    return hbool_true;
}

hbool vm_compiler_expression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    return _vm_compiler_expression(tokenizer, errors, InvokeTickArg);
}



