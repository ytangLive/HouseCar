//
//  hvmcompiler_class.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_class.h"
#include "hvmcompiler_invokes.h"
#include "hvmcompiler_expression.h"
#include "hserial_list.h"


static hbool vm_compiler_class_extends(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    htokenizer_t * token ;
    
    if(length == 1){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
        if( ! hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            vmCompilerErrorSet(token->range.begin,"class extends");
            
            return hbool_false;
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"class extends");
        return hbool_false;
    }
    
    return hbool_true;
}

static hbool vm_compiler_class_property(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * token ;
    
    if(length >0 && length <= 3){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if( hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)){
            if(* token->range.begin.p == '('){
                
                if(hobj_array_count(token->childs, InvokeTickArg) == 1){
                    
                    token = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
                    
                    if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)
                       && htokenizer_equal_string(token, "assign")){
                        
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin,"class property");
                        
                        return hbool_false;
                    }
                    
                }
                else{
                    
                    vmCompilerErrorSet(token->range.begin,"class property");
                    
                    return hbool_false;
                }
                
            }
            else{
                vmCompilerErrorSet(token->range.begin,"class property");
                
                return hbool_false;
            }
            
            if(length > 1){
                
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index +1, InvokeTickArg);
                
                index ++;
                length --;
            }
            else{
                vmCompilerErrorSet(token->range.begin,"class property");
                return hbool_false;
            }
            
        }
        
        if( ! hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            vmCompilerErrorSet(token->range.begin,"class property");
            
            return hbool_false;
        }
        
        if(length > 1){
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
            
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
               && token->range.begin.p == token->range.end.p && * token->range.begin.p == '='){
                
                if(length > 2){
                    
                    token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 2, InvokeTickArg);
                    
                    if(!hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerString), InvokeTickArg)
                       && !hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerValue), InvokeTickArg)){
                        
                        vmCompilerErrorSet(token->range.begin,"class property");
                        
                        return hbool_false;
                    }
                                        
                }
                else{
                    vmCompilerErrorSet(token->range.begin,"class property");
                    
                    return hbool_false;
                }
                
            }
            else{
                vmCompilerErrorSet(token->range.begin,"class property");
                
                return hbool_false;
            }
        }
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"class property");
        return hbool_false;
    }
    return hbool_true;
}

static hbool vm_compiler_class_function_arguments(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){

    hint32 s = 0,i,c = hobj_array_count(tokenizer->childs,InvokeTickArg);
    htokenizer_t * token = NULL;
    
    for(i=0;i<c;i++){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        if( hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            if(s == 1){
                vmCompilerErrorSet(token->range.begin,"class function arguments");
                return hbool_false;
            }
            
            s = 1;
            
        }
        else if( hobj_is_kind((hobj_t *) token,HOBJ_CLASS(TokenizerOperator), InvokeTickArg) && token->range.begin.p == token->range.end.p && * token->range.begin.p == ','){
           
            if(s == 0){
                vmCompilerErrorSet(token->range.begin,"class function arguments");
                return hbool_false;
            }
            
            s = 0;
        }
        else{
            vmCompilerErrorSet(token->range.begin,"class function arguments");
            
            return hbool_false;
        }
        
    }
    
    return token == NULL || s == 1;
}

static hbool vm_compiler_class_function(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    htokenizer_t * token ;
    
    if(length == 3){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if( ! hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            vmCompilerErrorSet(token->range.begin,"class function");
            
            return hbool_false;
        }
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 1, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '('){
            
            if(! vm_compiler_class_function_arguments(token,errors,InvokeTickArg)){
                return hbool_false;
            }
        }
        else{
            vmCompilerErrorSet(token->range.begin,"class function");
            
            return hbool_false;
        }
        
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + 2, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '{'){
            
            if(! vm_compiler_invokes(token,errors,InvokeTickArg)){
                return hbool_false;
            }
            
        }
        else{
            vmCompilerErrorSet(token->range.begin,"class function");
            
            return hbool_false;
        }

    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin,"class function");
        return hbool_false;
    }
    return hbool_true;
}

static hbool vm_compiler_class_command(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token ;
    
    if(c > 0){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            if(htokenizer_equal_string(token,"extends")){
                
                if(! vm_compiler_class_extends(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
                
            }
            else if(htokenizer_equal_string(token,"var")){
                if(! vm_compiler_class_property(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else if(htokenizer_equal_string(token,"function")){
                if(! vm_compiler_class_function(tokenizer,1, c -1,errors,InvokeTickArg)){
                    return hbool_false;
                }
            }
            else{
                
                vmCompilerErrorSet(token->range.begin,"class");
                
                return hbool_false;
            }
        }
        else{
            vmCompilerErrorSet(token->range.begin,"class");
            return hbool_false;
        }
    }
    
    return hbool_true;
}

hbool vm_compiler_class(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg),index = 0,length = 0;
    htokenizer_t * token ;
    
    for(i=0;i<c;i++){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
            
            if( * token->range.begin.p == ';'){
                if(length >1){
                    if( !vm_compiler_class_command((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                    i = index;
                    c = c - length +1;
                }
                else if(length == 1){
                    if( !vm_compiler_class_command((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
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
                    if( !vm_compiler_class_command((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
                        return hbool_false;
                    }
                    i = index;
                    c = c - length +1;
                }
                else if(length == 1){
                    if( !vm_compiler_class_command((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
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
        
    }
    
    if(length >1){
        if( !vm_compiler_class_command((htokenizer_t *)htokenizer_comib(tokenizer, index, length, InvokeTickArg),errors,InvokeTickArg)){
            return hbool_false;
        }
    }
    else if(length == 1){
        if( !vm_compiler_class_command((htokenizer_t *)(htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),errors,InvokeTickArg)){
            return hbool_false;
        }
    }
    
    return hbool_true;

}
