//
//  hvmcompiler_tokenizer.c
//  vmcompiler
//
//  Created by zhang hailong on 13-3-13.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hvmcompiler_tokenizer.h"
#include "hlog.h"
#include "hserial_list.h"

/*
 
 ++
 --
 ===
 !==
 <<=
 >>=
 <<
 >>
 >=
 <=
 &&
 ||
 ==
 !=
 +=
 -=
 *=
 /=
 %=
 &=
 ^=
 |=
 
 +
 -
 *
 /
 %
 &
 |
 ^
 ~
 !
 >
 <
 =
 ? :
 
 
 */

htokenizer_t * vm_compiler_tokenizer(htokenizer_t * tokenizer,hobj_array_t * errors,InvokeTickDeclare){

    htokenizer_t * focusTokenizer = NULL;
    
    htokenizer_scanf_context_t ctx;
    htokenizer_location_t location;
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));

    if(htokenizer_scanf(tokenizer, htokenizer_comment_scanf, &ctx, &location, InvokeTickArg)){
        hlog("Comment scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"Comment scanf Error");
        return tokenizer;
    }

    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    if(htokenizer_scanf(tokenizer, htokenizer_string_scanf, &ctx, &location, InvokeTickArg)){
        hlog("String scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"String scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    if(htokenizer_scanf(tokenizer, htokenizer_value_scanf, &ctx, &location, InvokeTickArg)){
        hlog("Value scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"Value scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    if(htokenizer_scanf(tokenizer, htokenizer_name_scanf, &ctx, &location, InvokeTickArg)){
        hlog("Name scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"Name scanf Error");
        return tokenizer;
    }

    {
        hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
        htokenizer_t * token,* t;
        for(i=0;i<c;i++){
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
            if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerName), InvokeTickArg)){
                if(htokenizer_equal_string(token, "is")
                   || htokenizer_equal_string(token, "in")
                   || htokenizer_equal_string(token, "new")){
                    t = (htokenizer_t *) hobj_alloc(HOBJ_CLASS(TokenizerOperator), InvokeTickArg);
                    t->ofString = token->ofString;
                    t->range = token->range;
                    htokenizer_child_remove(tokenizer, token, InvokeTickArg);
                    htokenizer_child_add(tokenizer, t, InvokeTickArg);
                    hobj_release((hobj_t *) t, InvokeTickArg);
                }
            }
        }
    }
    
    hlog("is in new scanf OK\n");
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "++");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("++ scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"++ scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "--");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("-- scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"-- scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "===");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("=== scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"=== scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "!==");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("!== scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"!== scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, ">>=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(">>= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,">>= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "<<=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("<<= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"<<= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "<<");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("<< scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"<< scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ">>");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(">> scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,">> scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ">=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(">= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,">= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "<=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("<= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"<= scanf Error");
        return tokenizer;
    }

    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "&&");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("&& scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"&& scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "||");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("|| scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"|| scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));

    strcpy(ctx.op, "==");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("== scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"== scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "!=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("!= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"!= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "+=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("+= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"+= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "-=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("-= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"-= scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "*=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("*= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"*= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "/=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("/= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"/= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "%=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("%%= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"%= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "&=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("&= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"&= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "^=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("^= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"^= scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "|=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("|= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"|= scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "+");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("+ scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"+ scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "-");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("- scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"- scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "*");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("* scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"* scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    
    strcpy(ctx.op, "/");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("/ scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"/ scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "%");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("%% scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"% scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "&");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("& scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"& scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "|");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("| scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"| scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "^");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("^ scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"^ scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "~");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("~ scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"~ scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "!");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("! scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"! scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ">");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("> scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"> scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "<");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("< scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"< scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "=");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("= scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"= scanf Error");
        return tokenizer;
    }
    
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, "?");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("? scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"? scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ":");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(": scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,": scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ",");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(", scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,", scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ".");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog(". scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,". scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.op, ";");
    
    if(htokenizer_scanf(tokenizer, htokenizer_operator_scanf, &ctx, &location, InvokeTickArg)){
        hlog("; scanf OK\n");
    }
    else{
        vmCompilerErrorSet(location,"; scanf Error");
        return tokenizer;
    }
    
    memset(&ctx, 0, sizeof(htokenizer_scanf_context_t));
    memset(&location, 0, sizeof(htokenizer_location_t));
    
    strcpy(ctx.beginChar, "([{");
    strcpy(ctx.endChar, ")]}");

    focusTokenizer = (htokenizer_t *)hobj_copy((hobj_t *)tokenizer, InvokeTickArg);
    

    ctx.focusTokenizer = focusTokenizer;

    if(htokenizer_scanf(tokenizer, htokenizer_group_scanf, &ctx, &location, InvokeTickArg)){
        hlog("() [] {} scanf OK\n");
    }
    else{
        htokenizer_log(focusTokenizer, INT_MAX, InvokeTickArg);
        vmCompilerErrorSet(location,"() [] {} scanf Error");
        return tokenizer;
    }
    
    if(ctx.focusTokenizer != focusTokenizer){
    
        vmCompilerErrorSet(ctx.focusTokenizer->range.begin,"() [] {} scanf Error");

        return NULL;
    }
    
    return focusTokenizer;
}


void vm_compiler_tokenizer_comment_clear(htokenizer_t * tokenizer,InvokeTickDeclare){
    hint32 i,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token;
    for(i=0;i<c;i++){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerComment), InvokeTickArg)){
            hobj_array_removeAt(tokenizer->childs, i, InvokeTickArg);
            i --;
            c --;
        }
        else{
            vm_compiler_tokenizer_comment_clear(token,InvokeTickArg);
        }
    }
}

void vm_compiler_tokenizer_optimization(htokenizer_t * tokenizer,InvokeTickDeclare){
    htokenizer_t * token;
    hint32 i,c;
    while(hobj_is_kind((hobj_t *) tokenizer, HOBJ_CLASS(TokenizerCombi), InvokeTickArg) ){
        if(hobj_array_count(tokenizer->childs, InvokeTickArg) ==1){
            token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, 0, InvokeTickArg);
            if(token->range.begin.p == tokenizer->range.begin.p && token->range.end.p == tokenizer->range.end.p){
                hobj_retain((hobj_t *) token, InvokeTickArg);
                htokenizer_child_remove(tokenizer, token, InvokeTickArg);
                while(hobj_array_count(token->childs, InvokeTickArg) >0){
                    htokenizer_child_add(tokenizer,(htokenizer_t *)hobj_array_objectAt(token->childs, 0, InvokeTickArg), InvokeTickArg);
                }
                hobj_release((hobj_t *)token, InvokeTickArg);
                continue;
            }
        }
        break;
    }
    c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    for(i=0;i<c;i++){
        token = (htokenizer_t *)hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        if(hobj_is_kind((hobj_t * ) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
           && * token->range.begin.p == ';'){
            
            if(token->parent && hobj_is_kind((hobj_t * ) token->parent, HOBJ_CLASS(TokenizerGroup), InvokeTickArg) && * token->parent->range.begin.p == '('){
                
            }
            else{
                htokenizer_child_remove(tokenizer, token, InvokeTickArg);
                i --;
                c --;
            }
        }
        else{
            vm_compiler_tokenizer_optimization(token,InvokeTickArg);
        }
    }
}

hint32 vm_compiler_tokenizer_operator_index_of(htokenizer_t * tokenizer,hcchar * op,hint32 index,InvokeTickDeclare){
    hint32 i = index,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token ;
    for(i=index;i<c;i++){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerOperator), InvokeTickArg)
           && htokenizer_equal_string(token, op)){
            break;
        }
    }
    return i < c ? i : -1;
}

hint32 vm_compiler_tokenizer_group_index_of(htokenizer_t * tokenizer,hchar beginChar,hint32 index,InvokeTickDeclare){
    hint32 i = index,c = hobj_array_count(tokenizer->childs, InvokeTickArg);
    htokenizer_t * token ;
    for(i=index;i<c;i++){
        token = (htokenizer_t *) hobj_array_objectAt(tokenizer->childs, i, InvokeTickArg);
        if(hobj_is_kind((hobj_t *) token, HOBJ_CLASS(TokenizerGroup), InvokeTickArg)
           && * token->range.begin.p == beginChar){
            break;
        }
    }
    return i < c ? i : -1;
}

