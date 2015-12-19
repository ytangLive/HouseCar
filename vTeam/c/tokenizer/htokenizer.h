//
//  htokenizer.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_htokenizer_h
#define hobjc_htokenizer_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_array.h"
    
#define HTOKENIZER_MAX_OPSIZE   8
    
    typedef struct _htokenizer_location_t{
        hchar * p;
        huint32 line;
        huint32 index;
    } htokenizer_location_t;
    
    typedef struct _htokenizer_range_t{
        htokenizer_location_t begin;
        htokenizer_location_t end;
    } htokenizer_range_t;
    
    typedef struct _htokenizer_t{
        hobj_t READONLY base;
        hcchar * READONLY ofString;
        htokenizer_range_t READONLY range;
        hobj_array_t * READONLY childs;
        struct _htokenizer_t * READONLY parent;
        hobj_t * READONLY userInfo;
    } htokenizer_t;
    
    htokenizer_t * htokenizer_alloc( hcchar * source,InvokeTickDeclare);
    
    htokenizer_t * htokenizer_new( hcchar * source,InvokeTickDeclare);
    
    void htokenizer_child_add(htokenizer_t * tokenizer,htokenizer_t * token,InvokeTickDeclare);
 
    void htokenizer_child_remove(htokenizer_t * tokenizer,htokenizer_t * token,InvokeTickDeclare);
    
    void htokenizer_user_info_set(htokenizer_t * tokenizer,hobj_t * userInfo,InvokeTickDeclare);
    
    hbool htokenizer_equal_string(htokenizer_t * tokenizer,hcchar * string);
    
    typedef struct _htokenizer_scanf_context_t{
        htokenizer_location_t begin;
        htokenizer_location_t prev;
        hint32 s;
        hchar op[HTOKENIZER_MAX_OPSIZE];
        hchar beginChar[8];     // ( [ {
        hchar endChar[8];       // ) ] }
        htokenizer_t * focusTokenizer;
    } htokenizer_scanf_context_t;
    
    typedef hbool (* htokenizer_scanf_t)(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    hbool htokenizer_scanf(htokenizer_t * tokenizer,htokenizer_scanf_t scanf,htokenizer_scanf_context_t * ctx,htokenizer_location_t * location,InvokeTickDeclare);
    
    HOBJ_CLASS_DEC(Tokenizer)
    
    HOBJ_KEY_DEC(Tokenizer)
    
    typedef struct _htokenizer_comment_t{
        htokenizer_t base;
    } htokenizer_comment_t;
    
    HOBJ_CLASS_DEC(TokenizerComment)
    
    HOBJ_KEY_DEC(TokenizerComment)
   
    hbool htokenizer_comment_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    typedef struct _htokenizer_string_t{
        htokenizer_t base;
    } htokenizer_string_t;
    
    HOBJ_CLASS_DEC(TokenizerString)
    
    HOBJ_KEY_DEC(TokenizerString)
    
    hbool htokenizer_string_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    typedef struct _htokenizer_value_t{
        htokenizer_t base;
    } htokenizer_value_t;
    
    HOBJ_CLASS_DEC(TokenizerValue)
    
    HOBJ_KEY_DEC(TokenizerValue)
    
    hbool htokenizer_value_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    typedef struct _htokenizer_name_t{
        htokenizer_t base;
    } htokenizer_name_t;
    
    HOBJ_CLASS_DEC(TokenizerName)
    
    HOBJ_KEY_DEC(TokenizerName)
    
    hbool htokenizer_name_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    typedef struct _htokenizer_operator_t{
        htokenizer_t base;
    } htokenizer_operator_t;
    
    HOBJ_CLASS_DEC(TokenizerOperator)
    
    HOBJ_KEY_DEC(TokenizerOperator)
    
    hbool htokenizer_operator_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);

    typedef struct _htokenizer_group_t{
        htokenizer_t base;
    } htokenizer_group_t;
    
    HOBJ_CLASS_DEC(TokenizerGroup)
    
    HOBJ_KEY_DEC(TokenizerGroup)
    
    hbool htokenizer_group_scanf(htokenizer_t * tokenizer,htokenizer_location_t * locaiton,htokenizer_t * token,htokenizer_scanf_context_t * ctx, InvokeTickDeclare);
    
    
    typedef struct _htokenizer_combi_t{
        htokenizer_t base;
    } htokenizer_combi_t;
    
    HOBJ_CLASS_DEC(TokenizerCombi)
    
    HOBJ_KEY_DEC(TokenizerCombi)

    htokenizer_combi_t * htokenizer_comib(htokenizer_t * tokenizer,hint32 index,hint32 length,InvokeTickDeclare);
    
    
    void htokenizer_log(htokenizer_t * tokenizer,hint32 level,InvokeTickDeclare);

    
#ifdef __cplusplus
}
#endif

#endif
