//
//  hvmcompiler_invokes.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_invokes.h"
#include "hserial_list.h"
#include "hvmcompiler_expression.h"
#include "hvmcompiler_tokenizer.h"

static hbool vm_compiler_invoke_var(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    hint32 focusIndex,s = 0,c = length + index;
    htokenizer_t * token ;

    s = 0;
    
    if(index >= c){
        vmCompilerErrorSet(tokenizer->range.begin,"invoke var");
        return hbool_false;
    }
    
    while(index < c){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        switch (s) {
            case 0:
            {
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    s = 1;
                }
                else{
                    vmCompilerErrorSet(token->range.begin,"invoke var");
                    return hbool_false;
                }
                
            }
                break;
            case 1:
                
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                    if(* token->range.begin.p == '='){
                        s = 2;
                        focusIndex = index + 1;
                    }
                    else if(* token->range.begin.p == ','){
                        s = 0;
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin,"invoke var");
                        return hbool_false;
                    }
                }
                else{
                    vmCompilerErrorSet(token->range.begin,"invoke var");
                    return hbool_false;
                }
                break;
            case 2:
                
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg) && * token->range.begin.p == ','){
                    if(index - focusIndex ==0){
                        vmCompilerErrorSet(token->range.begin,"invoke var");
                        return hbool_false;
                    }
                    else if(index - focusIndex == 1){
                        if(! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                        s = 0;
                    }
                    else{
                        if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, focusIndex,index - focusIndex, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                        
                        c = c - (index - focusIndex) + 1;
                        index = focusIndex + 1;
                        s = 0;
                    }
                }
                
                break;
                
            default:
                break;
        }
        
        index ++;
    }
    
    if(s == 2){
        if(index - focusIndex ==0){
            vmCompilerErrorSet(token->range.begin,"invoke var");
            return hbool_false;
        }
        else if(index - focusIndex == 1){
            if(! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg), errors, InvokeTickArg)){
                return hbool_false;
            }
        }
        else{
            if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, focusIndex,index - focusIndex, InvokeTickArg), errors, InvokeTickArg)){
                return hbool_false;
            }
        }
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_if(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    

    htokenizer_t * token ;
    if(length >0){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '('){
            
            if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                vmCompilerErrorSet(token->range.begin,"invoke if");
                return hbool_false;
            }
            
            if(length > 1){
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '{'){
                    if(! vm_compiler_invokes((htokenizer_t *) token, errors, InvokeTickArg)){
                        vmCompilerErrorSet(token->range.begin,"invoke if");
                        return hbool_false;
                    }
                }
                else{
                    if(length - 1 == 1){
                        if(! vm_compiler_expression((htokenizer_t *) token, errors, InvokeTickArg)){
                            vmCompilerErrorSet(token->range.begin,"invoke if");
                            return hbool_false;
                        }
                    }
                    else{
                        if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, index + 1,length - 1, InvokeTickArg), errors, InvokeTickArg)){
                            vmCompilerErrorSet(token->range.begin,"invoke if");
                            return hbool_false;
                        }
                    }
                }
            }
        }
        else{
            vmCompilerErrorSet(token->range.begin,"invoke if");
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke if");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_else(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token ;
    
    if(length > 0){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            if(htokenizer_equal_string(token,"if")){
                if(! vm_compiler_invoke_if(tokenizer,2, length -2,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin,"invoke else if");
                return hbool_false;
            }
        }
        else if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '{'){
            if(! vm_compiler_invokes((htokenizer_t *) token, errors, InvokeTickArg)){
                return hbool_false;
            }
        }
        else{
            if(length ==1){
                if(! vm_compiler_expression((htokenizer_t *) token, errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, index,length, InvokeTickArg), errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke else");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_for(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){

    htokenizer_t * token ,* t ;
    hint32 i,c,i1,i2;
   
    if(length > 0){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '('){
            
            c = hobj_array_count(token->childs, InvokeTickArg);
            
            i = vm_compiler_tokenizer_operator_index_of(token, "in", 0, InvokeTickArg);
            
            if(i == -1){
                
                i1 = vm_compiler_tokenizer_operator_index_of(token, ";", 0, InvokeTickArg);
                
                if(i1 == -1){
                    vmCompilerErrorSet(token->range.begin,"invoke for");
                    return hbool_false;
                }
                else if(i1 == 1){
                    if( ! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg), errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
                else if(i1 > 1){
                    
                    t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
                    
                    if(hobj_is_kind((hobj_t *) t, HOBJ_CLASS(TokenizerName), InvokeTickArg)
                       && htokenizer_equal_string(t, "var")){
                        if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(token, 1, i1 -1, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                        c = c - i1 + 2;
                        i1 = 2;
                    }
                    else{
                        if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(token, 0, i1, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                        c = c - i1 +1;
                        i1 = 1;
                    }
                }
                
                i2 = vm_compiler_tokenizer_operator_index_of(token, ";", i1 +1, InvokeTickArg);
                
                if(i2 == -1){
                    vmCompilerErrorSet(token->range.begin,"invoke for");
                    return hbool_false;
                }
                else if(i2 - i1 == 2){
                    if( ! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(token->childs, i1 +1, InvokeTickArg), errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
                else if(i2 - i1 > 2){
                    if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(token, i1 + 1, i2 - i1 - 1, InvokeTickArg), errors, InvokeTickArg)){
                        return hbool_false;
                    }
                    c = c - (i2 - i1 -1) + 1;
                    i2 = i1 +2;
                }
                
                if(c - i2 == 2){
                    if( ! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(token->childs, i2 +1, InvokeTickArg), errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
                else if(c - i2 > 2){
                    if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(token, i2 + 1, c - i2 - 1, InvokeTickArg), errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
                
            }
            else if(i == 0){
                vmCompilerErrorSet(token->range.begin,"invoke for");
                return hbool_false;
            }
            else if(i == 1){
                htokenizer_log(token, 5, InvokeTickArg);
                t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
            }
            else if(i == 2){
                t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) t, HOBJ_CLASS(TokenizerName), InvokeTickArg)
                   && htokenizer_equal_string(t, "var")){
                    
                    t = (htokenizer_t *) hobj_array_objectAt(token->childs, 1, InvokeTickArg);
                
                }
                else{
                    vmCompilerErrorSet(t->range.begin,"invoke for");
                    return hbool_false;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin,"invoke for");
                return hbool_false;
            }
            
            if(i == 1 || i ==2){
                
                if(hobj_is_kind((hobj_t *) t, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    
                    c = c - i - 1;
                    
                    if(c <=0 ){
                        vmCompilerErrorSet(t->range.begin,"invoke for");
                        return hbool_false;
                    }
                    else if(c == 1){
                        if( ! vm_compiler_expression((htokenizer_t *) hobj_array_objectAt(token->childs, i + 1, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                    else{
                        if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(token, i + 1, c, InvokeTickArg), errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                    
                }
                else{
                    vmCompilerErrorSet(t->range.begin,"invoke for");
                    return hbool_false;
                }
            }
            
            if(length == 2){
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)){
                    if(* token->range.begin.p == '{'){
                        if(! vm_compiler_invokes(token, errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                    else{
                        if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                }
                else{
                    if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
            }
            else if(length > 2){
                if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, index + 1,length -1, InvokeTickArg), errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin,"invoke for");
                return hbool_false;
            }
        }
        else{
            vmCompilerErrorSet(token->range.begin,"invoke for");
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke for");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_while(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token;
    
    if(length >0){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
           && * token->range.begin.p == '('){
            
            if( ! vm_compiler_expression(token, errors, InvokeTickArg) ){
                
                return hbool_false;
            }
            
            if(length == 2){
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)){
                    if(* token->range.begin.p == '{'){
                        if(! vm_compiler_invokes(token, errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                    else{
                        if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                            return hbool_false;
                        }
                    }
                }
                else{
                    if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                        return hbool_false;
                    }
                }
            }
            else if(length > 2){
                if(! vm_compiler_expression((htokenizer_t *) htokenizer_comib(tokenizer, index + 1,length -1, InvokeTickArg), errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin,"invoke while");
                return hbool_false;
            }
            
        }
        else{
            vmCompilerErrorSet(token->range.begin,"invoke while");
            return hbool_false;
        }
        
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke while");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_try(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token;
    
    if(length == 1){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
           && * token->range.begin.p == '{'){
            
            if(! vm_compiler_invokes(token, errors, InvokeTickArg)){
                return hbool_false;
            }
            
        }
        else{
            vmCompilerErrorSet(tokenizer->range.begin,"invoke try");
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke try");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_catch(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token,*t;
    hint32 c;
    
    if(length == 2){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
           && * token->range.begin.p == '('){
            
            c = hobj_array_count(token->childs, InvokeTickArg);
            
            if(c == 1){
                
                t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
                if(!hobj_is_kind((hobj_t *)t, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    vmCompilerErrorSet(t->range.begin,"invoke catch");
                    htokenizer_log(token, 100, InvokeTickArg);
                    return hbool_false;
                }
                
            }
            
            if(c == 2){
                t = (htokenizer_t *) hobj_array_objectAt(token->childs, 1, InvokeTickArg);
                if(!hobj_is_kind((hobj_t *)t, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    vmCompilerErrorSet(t->range.begin,"invoke catch");
                    return hbool_false;
                }

            }
            
            if(c< 1){
                vmCompilerErrorSet(token->range.begin,"invoke catch");
                return hbool_false;
            }
            
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
            
            if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
               && * token->range.begin.p == '{'){
                if(! vm_compiler_invokes(token, errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin,"invoke catch");
                return hbool_false;
            }
            
        }
        else{
            vmCompilerErrorSet(tokenizer->range.begin,"invoke catch");
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke catch");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_finally(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token;
    
    if(length == 1){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
           && * token->range.begin.p == '{'){
            
            if(! vm_compiler_invokes(token, errors, InvokeTickArg)){
                return hbool_false;
            }
            
        }
        else{
            vmCompilerErrorSet(tokenizer->range.begin,"invoke finally");
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke finally");
        return hbool_false;
    }
   
    return hbool_true;
}

static hbool vm_compiler_invoke_throw(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token;
    
    if(length > 0){
        
        if(length ==1 ){
            
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
            
            if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                return hbool_false;
            }
            
        }
        else{
            token = (htokenizer_t *) htokenizer_comib(tokenizer, index, length, InvokeTickArg);
            if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                return hbool_false;
            }
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke throw");
        return hbool_false;
    }

    return hbool_true;
}

static hbool vm_compiler_invoke_break(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    if(length == 0){
        
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke break");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_continue(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    if(length == 0){
        
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"invoke continue");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke_return(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token;
    
    if(length > 0){
        
        if(length ==1 ){
            
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
            
            if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                return hbool_false;
            }
            
        }
        else{
            token = (htokenizer_t *) htokenizer_comib(tokenizer, index, length, InvokeTickArg);
            if(! vm_compiler_expression(token, errors, InvokeTickArg)){
                return hbool_false;
            }
        }
    }
    
    return hbool_true;
}

static hbool vm_compiler_invoke(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token ;
    
    if(c > 0){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            if(htokenizer_equal_string(token,"var")){
                
                if(! vm_compiler_invoke_var(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }

            }
            else if(htokenizer_equal_string(token,"if")){
                if(! vm_compiler_invoke_if(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"else")){
                if(! vm_compiler_invoke_else(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"for")){
                
                if(! vm_compiler_invoke_for(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
                
            }
            else if(htokenizer_equal_string(token,"while")){
                if(! vm_compiler_invoke_while(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"try")){
                if(! vm_compiler_invoke_try(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"catch")){
                if(! vm_compiler_invoke_catch(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"finally")){
                if(! vm_compiler_invoke_finally(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"throw")){
                if(! vm_compiler_invoke_throw(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"break")){
                if(! vm_compiler_invoke_break(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"continue")){
                if(! vm_compiler_invoke_continue(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"return")){
                if(! vm_compiler_invoke_return(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                if(! vm_compiler_expression(tokenizer, errors, InvokeTickArg)){
                    return hbool_false;
                }
            }
        }
        else{
            if(! vm_compiler_expression(tokenizer, errors, InvokeTickArg)){
                return hbool_false;
            }
        }
    }
    
    return hbool_true;
}

hbool vm_compiler_invokes(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg),index = 0,length = 0;
    htokenizer_t * token ;
    
    for(i=0;i<c;i++){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
            
            if( * token->range.begin.p == ';'){
                if(length >1){
                    if( !vm_compiler_invoke((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                    i = index + 1;
                    c = c - length +1;
                }
                else if(length == 1){
                    if( !vm_compiler_invoke((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                }
                length = 0;
                index = 0;
            }
            else{
                if(length == 0){
                    index = i;
                }
                length ++;
            }
        }
        else{
            
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '{'){
                
                if(!vm_compiler_invokes(token,errors,InvokeTickArg)){
                   return hbool_false;
                }
                
                if(length == 0){
                    index = i;
                }
                length ++;
                
                if(length >1){
                   if( !vm_compiler_invoke((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
                       return hbool_false;
                   }
                   i = index ;
                   c = c - length +1;
                }
                else if(length == 1){
                   if( !vm_compiler_invoke((htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
                       return hbool_false;
                   }
                }
                length = 0;
                index = 0;
            }
            else if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerCombi), InvokeTickArg)){
                if(length >1){
                    if( !vm_compiler_invoke((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                    i = index;
                    c = c - length +1;
                }
                else if(length == 1){
                    if( !vm_compiler_invoke((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                }
                length = 1;
                index = i;
            }
            else{
                if(length == 0){
                    index = i;
                }
                length ++;
            }
        }
        
    }
    
    if(length >1){
        if( !vm_compiler_invoke((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
            return hbool_false;
        }
    }
    else if(length == 1){
        if( !vm_compiler_invoke((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
            return hbool_false;
        }
    }
    
    return hbool_true;
}

