//
//  hvmcompiler_exec.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_exec.h"
#include "hvmcompiler_class.h"
#include "hvmcompiler_tokenizer.h"
#include "htokenizer.h"

static vmCompilerMeta * vmCompilerExecExpression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);

static vmCompilerMeta * vmCompilerExecInvokes(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);


static hbool vmCompilerExecExtends(vmCompilerClassMeta * classMeta,htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    htokenizer_t * token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
    
    classMeta->superClass.location = token->range.begin;
    classMeta->superClass.length = token->range.end.p - token->range.begin.p + 1;
    
    return hbool_true;
}

static hbool vmCompilerExecProperty(vmCompilerClassMeta * classMeta,htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 index = 1;
    htokenizer_t * token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
    vmCompilerMetaOperator * op;
    vmCompilerMeta  * meta;
    
    if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)){
        index ++;
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeVarAssign, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
    }
    else{
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeVar, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
    }
    
    vmCompilerClassAddProperty(classMeta, op, InvokeTickArg);
    
    index ++;
    
    if(index < hobj_array_count(tokenizer->childs, InvokeTickArg)){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
           && token->range.begin.p == token->range.end.p && *token->range.begin.p == '='){
            
            index ++;
            if(index < hobj_array_count(tokenizer->childs, InvokeTickArg)){
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerValue), InvokeTickArg)){
                    meta = vmCompilerMetaNewWithNumberString(token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerString), InvokeTickArg)){
                    meta = vmCompilerMetaNewWithString(token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
            }
        }

    }
    
    return hbool_true;
}

static vmCompilerMeta * vmCompilerExecAction(htokenizer_t * tokenizer,hint32 index,hint32 length,hobj_array_t * errors,InvokeTickDeclare){
    
    vmCompilerMetaOperator * op;
    vmCompilerMeta * meta;
    hint32 i;
    htokenizer_t * token,* nextToken;
    
    if(length >0){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);

        meta = vmCompilerExecExpression(token, errors, InvokeTickArg);
        
        if(!meta){
            vmCompilerErrorSet(token->range.begin, "ExecAction");
            return NULL;
        }
        
        for(i=1;i<length;i++){
            
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + i, InvokeTickArg);
            
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                i ++;
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + i, InvokeTickArg);
            }
            
            if(i + 1 < length){
                nextToken = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + i + 1, InvokeTickArg);
                if(hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                    nextToken = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index + i +2, InvokeTickArg);
                }
            }
            else{
                nextToken = NULL;
            }
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                if(nextToken && hobj_is_kind((hobj_t *) nextToken, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * nextToken->range.begin.p == '('){
                    op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeFunction, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                    {
                        hint32 n,c = hobj_array_count(nextToken->childs, InvokeTickArg);
                        for(n=0;n<c;n++){
                            token = (htokenizer_t *) hobj_array_objectAt(nextToken->childs, n, InvokeTickArg);
                            if(!hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                                meta = vmCompilerExecExpression(token,errors,InvokeTickArg);
                                if(meta){
                                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                                }
                                else{
                                    vmCompilerErrorSet(token->range.begin, "ExecAction");
                                    return NULL;
                                }
                            }
                        }
                    }
                    meta = vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                    i ++;
                }
                else {
                    op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeProperty, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                    meta = vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                }
            }
            else if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->range.begin.p == '['){
                
                op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeProperty, token->range.begin, 0, InvokeTickArg);
                
                vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                
                meta = vmCompilerExecExpression(token,errors,InvokeTickArg);
                
                if(meta){
                    
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                    
                    meta = vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                    
                }
                else{
                    vmCompilerErrorSet(token->range.begin, "ExecAction");
                    
                    return NULL;
                }
                
            }
            else{
                
                vmCompilerErrorSet(token->range.begin, "ExecAction");
                
                return NULL;
            }
        }
        
        return meta;
    }
    
    return NULL;
}

#define VM_COMPILE_OP_TYPE_ONE_L    0x01
#define VM_COMPILE_OP_TYPE_ONE_R    0x02
#define VM_COMPILE_OP_TYPE_TOW      0x03
#define VM_COMPILE_OP_TYPE_THREE    0x04

#define VM_COMPILE_OP_TYPE_ONE_LL    0x05
#define VM_COMPILE_OP_TYPE_ONE_RR    0x06

static vmCompilerMeta *  vmCompilerExecOperator(htokenizer_t * tokenizer,hcchar * op,vmRuntimeOperatorType opType
                                     , hint32 type,hobj_array_t * errors,InvokeTickDeclare){
    
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    hint32 i = vm_compiler_tokenizer_operator_index_of(tokenizer, op, 0, InvokeTickArg);

    if(i != -1){

        if(type == VM_COMPILE_OP_TYPE_ONE_LL)
        {
            htokenizer_t * t1,*t2;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;
            hint32 n,nn;
            if(c == 2 && i==1){
                  
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs,0, InvokeTickArg);
                
                if(hobj_is_kind((hobj_t *) t1, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    op = vmCompilerMetaOperatorNew(opType, t1->range.begin, t1->range.end.p - t1->range.begin.p + 1, InvokeTickArg);

                    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                }
                else{
                    
                    nn =  hobj_array_count(t1->childs, InvokeTickArg);
                    
                    n = vm_compiler_tokenizer_operator_index_of(t1, ".", 0, InvokeTickArg);
                    
                    if(n != -1 && nn >2){
                        
                        t2 = (htokenizer_t *) hobj_array_last(t1->childs, InvokeTickArg);
                        
                        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                            
                            meta = vmCompilerExecAction(t1,0,nn -2,errors,InvokeTickArg);
                            
                            if(meta){

                                op = vmCompilerMetaOperatorNew(opType, t2->range.begin, t2->range.end.p - t2->range.begin.p + 1, InvokeTickArg);

                                vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                                
                                return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                            }
                            
                        }
                        
                    }
                    
                }
                
            }

        }
        
        if(type == VM_COMPILE_OP_TYPE_ONE_RR)
        {
            htokenizer_t * t1,*t2;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;
            hint32 n,nn;
            if(c == 2 && i==0){
                
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs,1, InvokeTickArg);
                
                if(hobj_is_kind((hobj_t *) t1, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    
                    op = vmCompilerMetaOperatorNew(opType, t1->range.begin, t1->range.end.p - t1->range.begin.p + 1, InvokeTickArg);
                    
                    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                }
                else{
                    
                    nn =  hobj_array_count(t1->childs, InvokeTickArg);
                    
                    n = vm_compiler_tokenizer_operator_index_of(t1, ".", 0, InvokeTickArg);
                    
                    if(n != -1 && nn >2){
                        
                        t2 = (htokenizer_t *) hobj_array_last(t1->childs, InvokeTickArg);
                        
                        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                            
                            meta = vmCompilerExecAction(t1,0,nn -2,errors,InvokeTickArg);
                            
                            if(meta){
                                
                                op = vmCompilerMetaOperatorNew(opType, t2->range.begin, t2->range.end.p - t2->range.begin.p + 1, InvokeTickArg);
                                
                                vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                                
                                return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                            }
                            
                        }
                        
                    }
                    
                }
                
            }
            
            
            
        }
        
        if(type == VM_COMPILE_OP_TYPE_ONE_L)
        {
            htokenizer_t * t1;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;
            if(c == 2 && i==1){
                
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                
                if(hobj_is_kind((hobj_t *)t1, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    
                    op = vmCompilerMetaOperatorNew(opType, t1->range.begin, t1->range.end.p - t1->range.begin.p +1, InvokeTickArg);
                    
                    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                }
                else{
                    meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                    if(meta){
                        op = vmCompilerMetaOperatorNew(opType, tokenizer->range.begin, 0, InvokeTickArg);
                        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                        
                        return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                    }
                }
                
            }
        }
        
        if(type == VM_COMPILE_OP_TYPE_ONE_R)
        {
            htokenizer_t * t1;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;
            if(c == 2 && i==0){
                
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
                
                if(hobj_is_kind((hobj_t *)t1, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                    
                    op = vmCompilerMetaOperatorNew(opType, t1->range.begin, t1->range.end.p - t1->range.begin.p +1, InvokeTickArg);
                    
                    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                }
                else{
                    meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                    if(meta){
                        op = vmCompilerMetaOperatorNew(opType, tokenizer->range.begin, 0, InvokeTickArg);
                        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                        
                        return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                    }
                }
                
            }
        }
        
        if(type == VM_COMPILE_OP_TYPE_TOW)
        {
            htokenizer_t * t1,*t2;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;
            if(c == 3 && i==1){
                
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                
                op = vmCompilerMetaOperatorNew(opType, tokenizer->range.begin, 0, InvokeTickArg);
                
                meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                if(meta){
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    return NULL;
                }
                
                meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
                
                if(meta){
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    return NULL;
                }
                
                return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                
            }
        }
        
        if(type == VM_COMPILE_OP_TYPE_THREE)
        {
            htokenizer_t * t1,*t2,* t3;
            vmCompilerMetaOperator * op;
            vmCompilerMeta * meta;

            if(c == 5 && i==1){
                
                t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                t3 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 4, InvokeTickArg);
                
                op = vmCompilerMetaOperatorNew(opType, tokenizer->range.begin, 0, InvokeTickArg);
                
                meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                if(meta){
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    return NULL;
                }
                
                meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
                
                if(meta){
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    return NULL;
                }
                
                meta = vmCompilerExecExpression(t3,errors,InvokeTickArg);
                
                if(meta){
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    return NULL;
                }
                
                return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                
            }
        }

    }
    
    return NULL;
}

static vmCompilerMeta * vmCompilerExecExpression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare);

static vmCompilerMeta * vmCompilerExecExpressionAssign(htokenizer_t * tokenizer,vmCompilerMeta * meta,hobj_array_t * errors,InvokeTickDeclare){
    
    vmCompilerMetaOperator * op;
    vmCompilerMeta * m;
    htokenizer_t * token;
    hint32 i,c;
    if(hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeAssign,tokenizer->range.begin,tokenizer->range.end.p - tokenizer->range.begin.p + 1,InvokeTickArg);
    }
    else{
        
        i = vm_compiler_tokenizer_operator_index_of(tokenizer, ".", 0, InvokeTickArg);
        
        if(i == -1){
            i = vm_compiler_tokenizer_group_index_of(tokenizer, '[', 0, InvokeTickArg);
        }
        
        if(i == -1){
            return NULL;
        }

        
        c =  hobj_array_count(tokenizer->childs, InvokeTickArg);
        
        token = (htokenizer_t *) hobj_array_last(tokenizer->childs, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            
            op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeAssign,token->range.begin,tokenizer->range.end.p - token->range.begin.p + 1,InvokeTickArg);
            
            if(c > 2){
                m = vmCompilerExecAction(tokenizer, 0, c - 2, errors, InvokeTickArg);
                if(m){
                    vmCompilerMetaOperatorAddCompilerMeta(op, m, InvokeTickArg);
                }
                else{
                    vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
                    return NULL;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
                return NULL;
            }
        }
        else if(hobj_is_kind((hobj_t *)token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
                && * token->range.begin.p == '['){
            
            op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeAssign,token->range.begin,0,InvokeTickArg);
            
            if(c > 1){
                m = vmCompilerExecAction(tokenizer, 0, c - 1, errors, InvokeTickArg);
                if(m){
                    vmCompilerMetaOperatorAddCompilerMeta(op, m, InvokeTickArg);
                }
                else{
                    vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
                    return NULL;
                }
            }
            else{
                vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
                return NULL;
            }
            
            m = vmCompilerExecExpression(token,errors,InvokeTickArg);
            
            if(m){
                vmCompilerMetaOperatorAddCompilerMeta(op, m, InvokeTickArg);
            }
            else{
                vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
                return NULL;
            }
        }
        else{
            vmCompilerErrorSet(token->range.begin, "ExecExpressionAssign");
            return NULL;
        }
        
    }
    
    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
    
    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
}


static vmCompilerMeta * vmCompilerExecExpression(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    
    
    if(hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
        return vmCompilerMetaNewWithObjectKey(tokenizer->range.begin, tokenizer->range.end.p - tokenizer->range.begin.p + 1, InvokeTickArg);
    }
    else if(hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerString), InvokeTickArg)){
        return vmCompilerMetaNewWithString(tokenizer->range.begin, tokenizer->range.end.p - tokenizer->range.begin.p + 1, InvokeTickArg);
    }
    else if(hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerValue), InvokeTickArg)){
        if(htokenizer_equal_string(tokenizer, "null")){
            return vmCompilerMetaNew(InvokeTickArg);
        }
        if(htokenizer_equal_string(tokenizer, "false")){
            return vmCompilerMetaNewWithBoolean(hbool_false, InvokeTickArg);
        }
        if(htokenizer_equal_string(tokenizer, "true")){
            return vmCompilerMetaNewWithBoolean(hbool_true, InvokeTickArg);
        }
        return vmCompilerMetaNewWithNumberString(tokenizer->range.begin, tokenizer->range.end.p - tokenizer->range.begin.p + 1, InvokeTickArg);
    }
    else if(hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
            || hobj_is_kind((hobj_t *)tokenizer, HOBJ_CLASS(TokenizerCombi), InvokeTickArg)){
        {
            
            
            hint32 i;
            hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, ".", 0, InvokeTickArg);
            
            if(i != -1){
                return vmCompilerExecAction(tokenizer, 0, c, errors, InvokeTickArg);
            }
            
            i = vm_compiler_tokenizer_group_index_of(tokenizer, '[', 0, InvokeTickArg);
            
            if(i != -1){
                return vmCompilerExecAction(tokenizer, 0, c, errors, InvokeTickArg);
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "new", 0, InvokeTickArg);
            if(i != -1){
                {
                    htokenizer_t * t1,*t2;
                    vmCompilerMetaOperator * op;
                    vmCompilerMeta * meta;
                    hint32 j,n;
                    
                    if(c == 2 && i ==0){
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(t1->childs, 1, InvokeTickArg);
                        t1 = (htokenizer_t *) hobj_array_objectAt(t1->childs, 0, InvokeTickArg);
                        if(hobj_is_kind((hobj_t *) t1, HOBJ_CLASS(TokenizerName), InvokeTickArg)
                           && hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
                           && * t2->range.begin.p == '('){
                            op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeNew, t1->range.begin, t1->range.end.p - t1->range.begin.p + 1, InvokeTickArg);
                            n = hobj_array_count(t2->childs, InvokeTickArg);
                            for(j=0;j<n;j++){
                                t1 = (htokenizer_t *) hobj_array_objectAt(t2->childs, j, InvokeTickArg);
                                if(!hobj_is_kind((hobj_t *)t1, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                                    meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                                    if(meta){
                                        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                                    }
                                    else{
                                        return NULL;
                                    }
                                }
                            }
                            return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression new");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression new");
                    }
                }
                return NULL;
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "is", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    htokenizer_t * t1,*t2;
                    vmCompilerMetaOperator * op;
                    vmCompilerMeta * meta;
                    
                    if(c == 3 && i==1){
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeIs, tokenizer->range.begin, 0, InvokeTickArg);
                        
                        meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
                        
                        if(meta){
                            vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                        }
                        else{
                            return NULL;
                        }
                        
                        meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
                        
                        if(meta){
                            vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                        }
                        else{
                            return NULL;
                        }
                        
                        return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression is");
                    }
                }
                return NULL;
            }
            {
                vmCompilerMeta * meta = NULL;
                
                if((meta = vmCompilerExecOperator(tokenizer, "++", vmRuntimeOperatorTypeInc, VM_COMPILE_OP_TYPE_ONE_LL, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "++", vmRuntimeOperatorTypeBeforeInc, VM_COMPILE_OP_TYPE_ONE_RR, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "--", vmRuntimeOperatorTypeDec, VM_COMPILE_OP_TYPE_ONE_LL, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "--", vmRuntimeOperatorTypeBeforeDec, VM_COMPILE_OP_TYPE_ONE_RR, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "-", vmRuntimeOperatorTypeAntiNumber, VM_COMPILE_OP_TYPE_ONE_R, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "!", vmRuntimeOperatorTypeNot, VM_COMPILE_OP_TYPE_ONE_R, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "~", vmRuntimeOperatorTypeBitwiseComplement, VM_COMPILE_OP_TYPE_ONE_R, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "+", vmRuntimeOperatorTypeAdd, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "-", vmRuntimeOperatorTypeSub, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "*", vmRuntimeOperatorTypeMultiplication, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "/", vmRuntimeOperatorTypeDivision, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "%", vmRuntimeOperatorTypeModulus, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "&", vmRuntimeOperatorTypeBitwiseAnd, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "|", vmRuntimeOperatorTypeBitwiseOr, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "^", vmRuntimeOperatorTypeBitwiseExclusiveOr, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "<<", vmRuntimeOperatorTypeBitwiseShiftLeft, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, ">>", vmRuntimeOperatorTypeBitwiseShiftRight, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "===", vmRuntimeOperatorTypeAbsEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "!==", vmRuntimeOperatorTypeAbsNotEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "==", vmRuntimeOperatorTypeEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "!=", vmRuntimeOperatorTypeNotEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, ">", vmRuntimeOperatorTypeGreater, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, ">=", vmRuntimeOperatorTypeGreaterEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "<", vmRuntimeOperatorTypeLess, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "<=", vmRuntimeOperatorTypeLessEqual, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "&&", vmRuntimeOperatorTypeAnd, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "||", vmRuntimeOperatorTypeOr, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg))){
                    return meta;
                }
                
                if((meta = vmCompilerExecOperator(tokenizer, "?", vmRuntimeOperatorTypeIfElse, VM_COMPILE_OP_TYPE_THREE, errors, InvokeTickArg))){
                    return meta;
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression =");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression =");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "+=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "+=", vmRuntimeOperatorTypeAdd, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression +=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression +=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "-=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "-=", vmRuntimeOperatorTypeSub, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression -=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression -=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "*=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "*=", vmRuntimeOperatorTypeMultiplication, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression *=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression *=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "/=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "/=", vmRuntimeOperatorTypeDivision, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression /=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression /=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "%=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "%=", vmRuntimeOperatorTypeMultiplication, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression %=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression %=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "%=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "%=", vmRuntimeOperatorTypeMultiplication, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression %=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression %=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "<<=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "<<=", vmRuntimeOperatorTypeBitwiseShiftLeft, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression <<=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression <<=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, ">>=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, ">>=", vmRuntimeOperatorTypeBitwiseShiftRight, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression >>=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression >>=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "&=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "&=", vmRuntimeOperatorTypeBitwiseAnd, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression &=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression &=");
                    }
                }
            }
            
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "|=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "|=", vmRuntimeOperatorTypeBitwiseOr, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression |=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression |=");
                    }
                }
            }
            
            i = vm_compiler_tokenizer_operator_index_of(tokenizer, "^=", 0, InvokeTickArg);
            
            if(i != -1){
                {
                    if(c == 3 && i ==1){
                        htokenizer_t * t1,*t2;
                        vmCompilerMeta * meta;
                        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
                        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
                        
                        meta = vmCompilerExecOperator(tokenizer, "^=", vmRuntimeOperatorTypeBitwiseExclusiveOr, VM_COMPILE_OP_TYPE_TOW, errors, InvokeTickArg);
                        
                        if(meta){
                            return vmCompilerExecExpressionAssign(t1, meta, errors, InvokeTickArg);
                        }
                        else{
                            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression ^=");
                        }
                    }
                    else{
                        vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression ^=");
                    }
                }
            }
            if(((hobj_is_kind((hobj_t *) tokenizer, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
               && (* tokenizer->range.begin.p == '[' || * tokenizer->range.begin.p == '('))
                || hobj_is_kind((hobj_t *) tokenizer, HOBJ_CLASS(TokenizerCombi), InvokeTickArg))
               && hobj_array_count(tokenizer->childs, InvokeTickArg) ==1){
                return vmCompilerExecExpression((htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg), errors, InvokeTickArg);
            }
            vmCompilerErrorSet(tokenizer->range.begin, "ExecExpression");
        }
    }
    
    return NULL;
}

static hbool vmCompilerExecInvokeVar(htokenizer_t * tokenizer,vmCompilerMetaOperator * op,hobj_array_t * errors,InvokeTickDeclare){
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token;
    hint32 s = 0;
    vmCompilerMetaOperator *opp = NULL;
    vmCompilerMeta * m;
    
    for(i=1;i<c;i++){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        switch (s) {
            case 0:
            {
                opp = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeVar, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
                m = vmCompilerMetaNewWithOperator(opp, InvokeTickArg);
                vmCompilerMetaOperatorAddCompilerMeta(op, m, InvokeTickArg);
                
                s = 1;
            }
                break;
            case 1:
            {
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                    if(* token->range.begin.p  == ','){
                        s = 0;
                        opp = NULL;
                    }
                    else if( * token->range.begin.p == '='){
                        s = 2;
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin, "ExecInvokeVar");
                        return hbool_false;
                    }
                }
            }
                break;
            case 2:
                
                if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)){
                    if(* token->range.begin.p  == ','){
                        s = 0;
                        opp = NULL;
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin, "ExecInvokeVar");
                        return hbool_false;
                    }
                }
                else{
                    m = vmCompilerExecExpression(token,errors,InvokeTickArg);
                    
                    if(m && opp){
                        vmCompilerMetaOperatorAddCompilerMeta(opp, m, InvokeTickArg);
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin, "ExecInvokeVar");
                        return hbool_false;
                    }
                }
                break;
            default:
                break;
        }
    }
    
    return hbool_true;
}

static vmCompilerMetaOperator * vmCompilerExecInvokeIf(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * t1,*t2;
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    
    if(c == 3){
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeIfElseIfElse, tokenizer->range.begin, 0, InvokeTickArg);
        
        meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
        
        if(!meta){
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
            return NULL;
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
                return NULL;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
                return NULL;
            }
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        return op;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
    }
    return NULL;
}

static vmCompilerMetaOperator * vmCompilerExecInvokeFor(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 i,f;
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    htokenizer_t * t1,*t2,*t3;
    if(c == 3){
        
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
        
        i = vm_compiler_tokenizer_operator_index_of(t1, "in", 0, InvokeTickArg);
        
        if(i == -1){
            
            op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeFor, tokenizer->range.begin, 0, InvokeTickArg);
            
            f = 0;
            
            while((i = vm_compiler_tokenizer_operator_index_of(t1, ";", f, InvokeTickArg)) != -1){
                
                f = i - f;
                
                if(f ==0){
                    meta = vmCompilerMetaNew(InvokeTickArg);
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                else{
                    meta = vmCompilerExecExpression((htokenizer_t *) hobj_array_objectAt(t1->childs, i-1, InvokeTickArg),errors,InvokeTickArg);
                    if(!meta){
                        vmCompilerErrorSet(tokenizer->range.begin, "InvokeFor");
                        return NULL;
                    }
                    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
                }
                
                f = i +1;
            }
            
            if(f < hobj_array_count(t1->childs, InvokeTickArg)){
                meta = vmCompilerExecExpression((htokenizer_t *) hobj_array_objectAt(t1->childs, f, InvokeTickArg),errors,InvokeTickArg);
                if(!meta){
                    vmCompilerErrorSet(tokenizer->range.begin, "InvokeFor");
                    return NULL;
                }
                vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
            }
            
            if(hobj_array_count(op->metas, InvokeTickArg) != 3){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeFor");
                return NULL;
            }
            
        }
        else if( i  >0 ){
            t3  = (htokenizer_t *) hobj_array_objectAt(t1->childs, i -1, InvokeTickArg);
            op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeForeach, t3->range.begin, t3->range.end.p - t3->range.begin.p + 1, InvokeTickArg);
            t3  = (htokenizer_t *) hobj_array_objectAt(t1->childs, i + 1, InvokeTickArg);
            meta = vmCompilerExecExpression(t3,errors,InvokeTickArg);
            
            if(!t3){
                vmCompilerErrorSet(t1->range.begin, "InvokeFor");
                return NULL;
            }
            
            if(!meta){
                vmCompilerErrorSet(t1->range.begin, "InvokeFor");
                return NULL;
            }
            
            vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
            
            
        }
        else{
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeFor");
            return NULL;
        }
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
                return NULL;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeIf");
                return NULL;
            }
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        return op;
    }
    else{
        
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeFor");
        return NULL;
    }
}

static vmCompilerMetaOperator * vmCompilerExecInvokeWhile(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    htokenizer_t * t1,*t2;
    if(c == 3){
        
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
        
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeWhile, tokenizer->range.begin, 0, InvokeTickArg);
        
        meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
        
        if(!meta){
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeWhile");
            return NULL;
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeWhile");
                return NULL;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeWhile");
                return NULL;
            }
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        return op;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeWhile");
        return NULL;
    }    
}

static vmCompilerMetaOperator * vmCompilerExecInvokeTry(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    htokenizer_t *t2;
    if(c == 2){
        
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeTryCatchFinally, tokenizer->range.begin, 0, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeTry");
                return NULL;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeTry");
                return NULL;
            }
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        
        return op;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeTry");
        return NULL;
    }
}

static vmCompilerMetaOperator * vmCompilerExecInvokeThrow(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMetaOperator *op;
    vmCompilerMeta * meta;
    htokenizer_t *t1;
    if(c == 2){

        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        
        op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeThrow, tokenizer->range.begin, 0, InvokeTickArg);
        
        meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
        
        if(!meta){
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeThrow");
            return NULL;
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        
        return op;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeThrow");
        return NULL;
    }
}

static vmCompilerMeta * vmCompilerExecInvokeBreak(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    vmCompilerMeta * meta;
    meta =  vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeBreak;
    return meta;
}

static vmCompilerMeta * vmCompilerExecInvokeContinue(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
   
    vmCompilerMeta * meta;
    meta =  vmCompilerMetaNew(InvokeTickArg);
    meta->type = vmRuntimeMetaTypeBreak;
    return meta;
}

static vmCompilerMeta * vmCompilerExecInvokeReturn(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMeta * meta;
    htokenizer_t *t1;
    if(c == 2){
        
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);

        meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
        
        if(!meta){
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeReturn");
            return NULL;
        }

        meta->type = meta->type | vmRuntimeMetaTypeReturn;
        
        return meta;
    }
    else{
        meta = vmCompilerMetaNew(InvokeTickArg);
        meta->type = meta->type | vmRuntimeMetaTypeReturn;
        return meta;
    }
}

static hbool vmCompilerExecInvokeCatch(htokenizer_t * tokenizer,vmCompilerMetaOperator * tryop,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMeta * meta;
    htokenizer_t * t1,*t2,*t3,*t4;
    if(c == 3){
        
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
        
        t3 = (htokenizer_t *) hobj_array_objectAt(t1->childs, 0, InvokeTickArg);
        t4 = (htokenizer_t *) hobj_array_objectAt(t1->childs, 1, InvokeTickArg);
        
        if(!t3 || !t4){
            vmCompilerErrorSet(tokenizer->range.begin, "InvokeCatch");
            return hbool_false;
        }
        
        meta = vmCompilerMetaNewWithString(t3->range.begin, t3->range.end.p - t3->range.begin.p + 1, InvokeTickArg);
        
        meta->type = meta->type | vmRuntimeMetaTypeCatch;
        
        vmCompilerMetaOperatorAddCompilerMeta(tryop, meta, InvokeTickArg);
        
        meta = vmCompilerMetaNewWithString(t4->range.begin, t4->range.end.p - t4->range.begin.p + 1, InvokeTickArg);
        
        meta->type = meta->type;
        
        vmCompilerMetaOperatorAddCompilerMeta(tryop, meta, InvokeTickArg);
        
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeCatch");
                return hbool_false;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeCatch");
                return hbool_false;
            }
        }
        
        vmCompilerMetaOperatorAddCompilerMeta(tryop, meta, InvokeTickArg);
        
        return hbool_true;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeCatch");
    }
    return hbool_false;
}

static hbool vmCompilerExecInvokeFinally(htokenizer_t * tokenizer,vmCompilerMetaOperator * tryop,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMeta * meta;
    htokenizer_t * t2;
    if(c == 2){
        
        t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
                
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeFinally");
                return hbool_false;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeFinally");
                return hbool_false;
            }
        }
        meta->type = meta->type | vmRuntimeMetaTypeFinally;
        
        vmCompilerMetaOperatorAddCompilerMeta(tryop, meta, InvokeTickArg);
        
        return hbool_true;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeFinally");
    }
    return hbool_false;
}

static hbool  vmCompilerExecInvokeElse(htokenizer_t * tokenizer,vmCompilerMetaOperator * ifop,hobj_array_t * errors,InvokeTickDeclare){
    hint32 c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    vmCompilerMeta * meta;
    htokenizer_t * t1,* t2;
    if(c >= 2){
        
        t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 1, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) t1, HOBJ_CLASS(TokenizerName), InvokeTickArg) && htokenizer_equal_string(t1, "if")){
            t1 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 2, InvokeTickArg);
            t2 = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 3, InvokeTickArg);
            
            meta = vmCompilerExecExpression(t1,errors,InvokeTickArg);
            
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeElse");
                return hbool_false;
            }
            
            vmCompilerMetaOperatorAddCompilerMeta(ifop, meta, InvokeTickArg);
        }
        else{
            
            t2 = t1;
            
            meta = vmCompilerMetaNewWithBoolean(hbool_true, InvokeTickArg);
            
            vmCompilerMetaOperatorAddCompilerMeta(ifop, meta, InvokeTickArg);
        }
        
        if(hobj_is_kind((hobj_t *) t2, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * t2->range.begin.p == '{'){
            
            meta = vmCompilerExecInvokes(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeElse");
                return hbool_false;
            }
        }
        else{
            meta = vmCompilerExecExpression(t2,errors,InvokeTickArg);
            
            if(!meta){
                vmCompilerErrorSet(tokenizer->range.begin, "InvokeElse");
                return hbool_false;
            }
        }
  
        vmCompilerMetaOperatorAddCompilerMeta(ifop, meta, InvokeTickArg);
        
        return hbool_true;
    }
    else{
        vmCompilerErrorSet(tokenizer->range.begin, "InvokeElse");
    }
    return hbool_false;
}

static vmCompilerMeta * vmCompilerExecInvokes(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    vmCompilerMetaOperator * op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeInvoke, tokenizer->range.begin, 0, InvokeTickArg);
    vmCompilerMeta * meta , * nilMeta = vmCompilerMetaNew(InvokeTickArg);
    vmCompilerMetaOperator * ifop = NULL,* tryop = NULL, * top;
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token,* t;
    
    for(i=0;i<c;i++){
        
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        
        t = token;
        
        meta = NULL;
        top = NULL;
        
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerCombi), InvokeTickArg)){
            if(hobj_array_count(token->childs, InvokeTickArg) >0){
                t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0, InvokeTickArg);
            }
            else{
                meta = nilMeta;
            }
        }
        
        if( hobj_is_kind((hobj_t *) t, HOBJ_CLASS(TokenizerName), InvokeTickArg) ){
            
            if(htokenizer_equal_string(t,"var")){
                if(!vmCompilerExecInvokeVar(token,op,errors,InvokeTickArg)){
                    return NULL;
                }
                ifop = NULL;
                tryop = NULL;
            }
            else if(htokenizer_equal_string(t,"if")){
                ifop =  vmCompilerExecInvokeIf(token,errors,InvokeTickArg);
                if(!ifop){
                    return NULL;
                }
                meta = vmCompilerMetaNewWithOperator(ifop, InvokeTickArg);
                tryop = NULL;
            }
            else if(htokenizer_equal_string(t,"else")){
                tryop = NULL;
                
                if(ifop){
                    if(!vmCompilerExecInvokeElse(token,ifop,errors,InvokeTickArg)){
                        return NULL;
                    }
                }
                else{
                    vmCompilerErrorSet(t->range.begin, "Exec Invokes else");
                    return NULL;
                }
            }
            else if(htokenizer_equal_string(t,"for")){
                tryop = NULL;
                ifop = NULL;
                top =  vmCompilerExecInvokeFor(token,errors,InvokeTickArg);
                if(!top){
                    return NULL;
                }
                meta = vmCompilerMetaNewWithOperator(top, InvokeTickArg);
            }
            else if(htokenizer_equal_string(t,"while")){
                tryop = NULL;
                ifop = NULL;
                top =  vmCompilerExecInvokeWhile(token,errors,InvokeTickArg);
                if(!top){
                    return NULL;
                }
                meta = vmCompilerMetaNewWithOperator(top, InvokeTickArg);
            }
            else if(htokenizer_equal_string(t,"try")){
                ifop = NULL;
                tryop =  vmCompilerExecInvokeTry(token,errors,InvokeTickArg);
                if(!tryop){
                    return NULL;
                }
                meta = vmCompilerMetaNewWithOperator(tryop, InvokeTickArg);
            }
            else if(htokenizer_equal_string(t,"catch")){
                ifop = NULL;
                if(tryop){
                    if(!vmCompilerExecInvokeCatch(token,tryop,errors,InvokeTickArg)){
                        return NULL;
                    }
                }
                else{
                    vmCompilerErrorSet(t->range.begin, "Exec Invokes catch");
                    return NULL;
                }
            }
            else if(htokenizer_equal_string(t,"finally")){
                ifop = NULL;
                if(tryop){
                    if(!vmCompilerExecInvokeFinally(token,tryop,errors,InvokeTickArg)){
                        return NULL;
                    }
                }
                else{
                    vmCompilerErrorSet(t->range.begin, "Exec Invokes finally");
                    return NULL;
                }
            }
            else if(htokenizer_equal_string(t,"throw")){
                tryop = NULL;
                ifop = NULL;
                top =  vmCompilerExecInvokeThrow(token,errors,InvokeTickArg);
                if(!top){
                    return NULL;
                }
                meta = vmCompilerMetaNewWithOperator(top, InvokeTickArg);
            }
            else if(htokenizer_equal_string(t,"break")){
                tryop = NULL;
                ifop = NULL;
                meta =  vmCompilerExecInvokeBreak(token,errors,InvokeTickArg);
                if(!meta){
                    return NULL;
                }
            }
            else if(htokenizer_equal_string(t,"continue")){
                tryop = NULL;
                ifop = NULL;
                meta =  vmCompilerExecInvokeContinue(token,errors,InvokeTickArg);
                if(!meta){
                    return NULL;
                }
            }
            else if(htokenizer_equal_string(t,"return")){
                tryop = NULL;
                ifop = NULL;
                meta =  vmCompilerExecInvokeReturn(token,errors,InvokeTickArg);
                if(!meta){
                    return NULL;
                }
            }
            else{
                tryop = NULL;
                ifop = NULL;
                meta = vmCompilerExecExpression(token,errors,InvokeTickArg);
                if(!meta){
                    return NULL;
                }
            }
            
        }
        else{
            meta = vmCompilerExecExpression(token,errors,InvokeTickArg);
            if(!meta){
                return NULL;
            }
        }
        
        
        if(meta && meta->type != vmRuntimeMetaTypeVoid){
            vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        }

    }
    
    
    return vmCompilerMetaNewWithOperator(op, InvokeTickArg);
}

static hbool vmCompilerExecFunction(vmCompilerClassMeta * classMeta,htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){
    hint32 index = 1,i,c;
    htokenizer_t * token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg),*t;
    vmCompilerMetaOperator * op;
    vmCompilerMeta  * meta;
    
    op = vmCompilerMetaOperatorNew(vmRuntimeOperatorTypeFunction, token->range.begin, token->range.end.p - token->range.begin.p + 1, InvokeTickArg);
    
    vmCompilerClassAddFunction(classMeta, op, InvokeTickArg);
    
    index ++;
    
    token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
    
    c = hobj_array_count(token->childs, InvokeTickArg);
    
    for(i=0;i<c;i++){
        t = (htokenizer_t *) hobj_array_objectAt(token->childs, i, InvokeTickArg);
        
        if(hobj_is_kind((hobj_t *) t, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
            meta = vmCompilerMetaNewWithArg(t->range.begin, t->range.end.p - t->range.begin.p + 1, InvokeTickArg);
            vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
        }
        
    }
    
    index ++;
    
    token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, index, InvokeTickArg);
    
    meta = vmCompilerExecInvokes(token,errors,InvokeTickArg);
    
    if(meta == NULL){
        vmCompilerErrorSet(token->range.begin, "ExecFunction");
        return hbool_false;
    }
    
    vmCompilerMetaOperatorAddCompilerMeta(op, meta, InvokeTickArg);
    
    return hbool_true;
}

vmCompilerClassMeta * vmCompilerClassExec(hcchar * source,hobj_array_t * errors,InvokeTickDeclare){
    
    htokenizer_t * sourceTokenizer = htokenizer_new(source, InvokeTickArg);
    htokenizer_t * token,* t;
    htokenizer_t * tokenizer;
    vmCompilerClassMeta * classMeta;
    hint32 i,c;
    
    tokenizer = vm_compiler_tokenizer(sourceTokenizer, errors, InvokeTickArg);
    
    if(tokenizer != sourceTokenizer){
        
        vm_compiler_tokenizer_comment_clear(tokenizer,InvokeTickArg);
        
        htokenizer_log(tokenizer, INT_MAX, InvokeTickArg);
        
        if(vm_compiler_class(tokenizer, errors, InvokeTickArg)){
            
            vm_compiler_tokenizer_optimization(tokenizer, InvokeTickArg);
            
            htokenizer_log(tokenizer, INT_MAX, InvokeTickArg);
            
            classMeta = vmCompilerClassMetaNew(InvokeTickArg);
            
            c = hobj_array_count(tokenizer->childs, InvokeTickArg);
            
            for(i=0;i<c;i++){
                
                token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
                
                if(hobj_array_count(token->childs, InvokeTickArg) >0){
                    
                    t = (htokenizer_t *) hobj_array_objectAt(token->childs, 0,InvokeTickArg);
                    
                    if(hobj_is_kind((hobj_t *)t, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                        
                        if(htokenizer_equal_string(t, "extends")){
                            if(! vmCompilerExecExtends(classMeta,token,errors,InvokeTickArg)){
                                return NULL;
                            }
                        }
                        else if(htokenizer_equal_string(t, "var")){
                            if(! vmCompilerExecProperty(classMeta,token,errors,InvokeTickArg)){
                                return NULL;
                            }
                        }
                        else if(htokenizer_equal_string(t, "function")){
                            if(! vmCompilerExecFunction(classMeta,token,errors,InvokeTickArg)){
                                return NULL;
                            }
                        }
                        else{
                            vmCompilerErrorSet(token->range.begin, "Compiler Exc Class");
                            return NULL;
                        }
                    }
                    else{
                        vmCompilerErrorSet(token->range.begin, "Compiler Exc Class");
                        
                        return NULL;
                    }
                    
                }
                else{
                    
                    vmCompilerErrorSet(token->range.begin, "Compiler Exc Class");
                    
                    return NULL;
                }
                
            }
            
            return classMeta;
            
        }
        
    }
    else{
        htokenizer_log(tokenizer, INT_MAX, InvokeTickArg);
    }
    
    return NULL;
    
}
