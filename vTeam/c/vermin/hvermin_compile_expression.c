//
//  hvermin_compile_expression.c
//  C Library
//
//  Created by Hailong Zhang on 11-10-27.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_compile.h"
#include "hvermin_compile_expression.h"
#include "hlog.h"
#include "hstack.h"
#include "hlist.h"

#undef vmCompileExpressionString

typedef enum {
    vmCompileExpNodeTypeNone,vmCompileExpNodeTypeMeta
    ,vmCompileExpNodeTypeExp,vmCompileExpNodeTypeAction 
    ,vmCompileExpNodeTypeGroup,vmCompileExpNodeTypeFunction,vmCompileExpNodeTypeIndex
    ,vmCompileExpNodeTypeOperator
}vmCompileExpNodeType;


typedef struct{
    vmCompileObject base;
    vmCompileExpNodeType type;
    union{
        vmCompileObjectArray * groupValue;
        struct{
            hbuffer_t name;
            vmCompileObjectArray * arguments;
        }functionValue;
        vmCompileObjectArray * indexValue;
        hchar operatorValue[5];
        hbuffer_t actionValue;
        hbuffer_t expValue;
        vmCompileMeta * metaValue;
    }value;
}vmCompileExpNode;

static void _vmCompileExpNodeDealloc(vmCompileExpNode * node,InvokeTickDeclare){
   
    if(node->type == vmCompileExpNodeTypeMeta){
        vmCompileObjectRelease(node->value.metaValue);
    }
    else if(node->type == vmCompileExpNodeTypeExp){
        buffer_dealloc(node->value.expValue);
    }
    else if(node->type == vmCompileExpNodeTypeAction){
        buffer_dealloc(node->value.actionValue);
    }
    else if(node->type == vmCompileExpNodeTypeGroup){
        vmCompileObjectRelease(node->value.groupValue);
    }
    else if(node->type == vmCompileExpNodeTypeIndex){
        vmCompileObjectRelease(node->value.indexValue);
    }
    else if(node->type ==vmCompileExpNodeTypeFunction){
        vmCompileObjectRelease(node->value.functionValue.arguments);
        buffer_dealloc(node->value.functionValue.name);
        node->value.functionValue.name = NULL;
    }
}

static vmCompileExpNode * vmCompileExpNodeCreate(InvokeTickDeclare){
    return (vmCompileExpNode *)vmCompileObjectCreate(vmCompileExpNode, _vmCompileExpNodeDealloc);
}

static vmCompileExpNode * vmCompileExpNodeCreateWithExp(hcchar * exp,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeExp;
    node->value.expValue = buffer_alloc((hint32)strlen(exp) +1, 20);
    buffer_append_str(node->value.expValue, exp);
    return node;
}


static vmCompileExpNode * vmCompileExpNodeCreateWithAction(hcchar * action,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeAction;
    node->value.actionValue = buffer_alloc((hint32)strlen(action) +1, 20);
    buffer_append_str(node->value.actionValue, action);
    return node;
}

static vmCompileExpNode * vmCompileExpNodeCreateWithGroup(vmCompileObjectArray * array,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeGroup;
    node->value.groupValue = (vmCompileObjectArray *)vmCompileObjectRetain(array);
    return node;
}

static vmCompileExpNode * vmCompileExpNodeCreateWithIndex(vmCompileObjectArray * array,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeIndex;
    node->value.indexValue = (vmCompileObjectArray *)vmCompileObjectRetain(array);
    return node;
}

static vmCompileExpNode * vmCompileExpNodeCreateWithFunction(hcchar * name,vmCompileObjectArray * args,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeFunction;
    node->value.functionValue.name = buffer_alloc(32, 32);
    node->value.functionValue.arguments = (vmCompileObjectArray *)vmCompileObjectRetain(args);
    buffer_append_str(node->value.functionValue.name, name);
    
    return node;
}

static vmCompileExpNode * vmCompileExpNodeCreateWithOperator(hcchar * op,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeOperator;
    mem_strcpy(node->value.operatorValue, op);
    return node;
}

static vmCompileExpNode * vmCompileExpNodeCreateWithMeta(vmCompileMeta * meta,InvokeTickDeclare){
    vmCompileExpNode *node = vmCompileExpNodeCreate(InvokeTickArg);
    node->type = vmCompileExpNodeTypeMeta;
    node->value.metaValue = (vmCompileMeta *)vmCompileObjectRetain(meta);
    return node;
}



typedef struct{
    vmCompileObjectArray * list;
}vmCompileExpressionContext;

typedef hbool (*vmCompileExpressionScannerCallback)(vmCompileExpressionContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare);


static hbool vmCompileExpressionScanner(vmCompileExpressionContext * context,vmCompileExpressionScannerCallback callback,InvokeTickDeclare){
    hint32 c = vmCompileObjectArrayCount(context->list);
    hint32 i;
    hchar * p;
    hbool result = hbool_true;
    vmCompileExpNode * node;
    
    for(i=0;i<c;i++){
        node = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(node->type == vmCompileExpNodeTypeExp){
            p = (hchar *)buffer_to_str(node->value.expValue);
            while(*p != 0){
                result = (*callback)(context,p,NULL,InvokeTickArg);
                if(!result){
                    return result;
                }
                p++;
            }
        }
        else{
            result = (*callback)(context,NULL,node,InvokeTickArg);
            if(!result){
                return result;
            }
        }
    }
    
    result = (*callback)(context,NULL,NULL,InvokeTickArg);
    
    return result;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hbuffer_t buf;
    hint32 state;
    hint32 step;
}vmCompileExpressionClearContext;

static hbool vmCompileExpressionClearScannerCallback(vmCompileExpressionClearContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    
    vmCompileExpNode * tnode;
    
    if(p == NULL && node == NULL){
        if(buffer_length(context->buf)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->buf), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list,tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->buf);
        }
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        if(buffer_length(context->buf)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->buf), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->buf);
        }
        vmCompileObjectArrayAdd(context->list,node);
        return hbool_true;
    }
    
    if(context->step){
        context->step --;
        return hbool_true;
    }
    
    switch (context->state) {
        case 0:
            if(*p == '"'){
                context->state = 1;
                buffer_append(context->buf, p, 1);
            }
            else if(ISVAR(p)){
                context->step =3;
            }
            else if(!ISSPACECHAR(*p)){
                buffer_append(context->buf, p, 1);
            }
            break;
        case 1:
            if(*p == '\\'){
                context->state = 2;
                buffer_append(context->buf, p, 1);
            }
            else if(*p == '"'){
                context->state =0;
                buffer_append(context->buf, p, 1);
            }
            else{
                buffer_append(context->buf, p, 1);
            }
            break;
        case 2:
            context->state = 1;
            buffer_append(context->buf, p, 1);
            break;
        default:
            break;
    }
    
    return hbool_true;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hbuffer_t exp;
    hbuffer_t value;
    hint32 state;
}vmCompileExpressionStringValueContext;

static hbool vmCompileExpressionStringValueScannerCallback(vmCompileExpressionStringValueContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    
    vmCompileExpNode * tnode;
    vmCompileMeta * tmeta;
    
    if(p == NULL && node == NULL){
        
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        if(buffer_length(context->value) !=0){
            return hbool_false;
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        vmCompileObjectArrayAdd(context->list,node);
        return hbool_true;
    }
    
    switch (context->state) {
        case 0:
            if( *p == '"'){
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                context->state =1 ;
            }
            else{
                buffer_append(context->exp, p, 1);
            }
            break;
        case 1:
            if( *p == '\\'){
                context->state =2;
            }
            else if(*p == '"'){
                tmeta = vmCompileMetaCreateWithString( buffer_to_str(context->value));
                tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                vmCompileObjectRelease(tmeta);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
                buffer_clear(context->value);
                context->state = 0;
            }
            else{
                buffer_append(context->value, p, 1);
            }
            break;
        case 2:
            if( *p == '\\'){
                buffer_append_str(context->value, "\\");
            }
            else if(*p == '\''){
                buffer_append_str(context->value, "\'");
            }
            else if(*p == '\"'){
                buffer_append_str(context->value, "\"");
            }
            else if(*p == 't'){
                buffer_append_str(context->value, "\t");
            }
            else if(*p == 'r'){
                buffer_append_str(context->value, "\r");
            }
            else if(*p == 'n'){
                buffer_append_str(context->value, "\n");
            }
            context->state =1;
            break;
        default:
            break;
    }
    
    return hbool_true;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hbuffer_t exp;
    hbuffer_t value;
    hint32 step;
    hint32 state;
    hbool hex;
}vmCompileExpressionValueContext;

static hbool vmCompileExpressionValueScannerCallback(vmCompileExpressionValueContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    vmCompileExpNode  * tnode;
    vmCompileMeta * tmeta ;
    
    if(p == NULL && node == NULL){
        
        if(buffer_length(context->value) ){
            tmeta = vmCompileMetaCreateWithNumberString( buffer_to_str(context->value));
            tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
            vmCompileObjectRelease(tmeta);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->value);
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        if(buffer_length(context->value) ){
            tmeta = vmCompileMetaCreateWithNumberString( buffer_to_str(context->value));
            tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
            vmCompileObjectRelease(tmeta);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->value);
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        vmCompileObjectArrayAdd(context->list,node);
        return hbool_true;
    }
    
    if(context->step >0){
        context->step --;
        return hbool_true;
    }
    
    switch (context->state) {
        case 0:
            if( ISFALSE(p)){
                context->step = 4;
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                tmeta = vmCompileMetaCreateWithBoolean( hbool_false);
                tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                vmCompileObjectRelease(tmeta);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
            }
            else if(ISTRUE(p)){
                context->step = 3;
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                tmeta = vmCompileMetaCreateWithBoolean( hbool_true);
                tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                vmCompileObjectRelease(tmeta);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
            }
            else if(ISNULL(p)){
                context->step = 3;
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                tmeta = vmCompileMetaCreate();
                tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                vmCompileObjectRelease(tmeta);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
            }
            else if(ISNUMBERFIRST(*p) && *p != '-'){
                context->state = 1;
                context->hex = p[1] == 'x';
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                buffer_append(context->value, p, 1);
            }
            else if(*p == '\''){
                if(p[1] == '\\' && p[3] =='\''){
                    context->step = 3;
                    
                    if(buffer_length(context->exp)){
                        tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                        vmCompileObjectArrayAdd(context->list, tnode);
                        vmCompileObjectRelease(tnode);
                        buffer_clear(context->exp);
                    }
                    tmeta = vmCompileMetaCreateWithInt16(p[2]);
                    tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                    vmCompileObjectRelease(tmeta);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else if(p[2] == '\''){
                    context->step = 2;
                    if(buffer_length(context->exp)){
                        tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                        vmCompileObjectArrayAdd(context->list, tnode);
                        vmCompileObjectRelease(tnode);
                        buffer_clear(context->exp);
                    }
                    tmeta = vmCompileMetaCreateWithInt16(p[1]);
                    tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                    vmCompileObjectRelease(tmeta);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    return hbool_false;
                }
            }
            else{
                buffer_append(context->exp, p, 1);
                if(!ISOP(*p)){
                    context->state = 2;
                }
            }
            break;
        case 1:
            if(ISNUMBER(*p) || (context->hex && (*p == 'x' || (*p >='A' && *p <='Z' ) || (*p >= 'a' && *p <= 'z' ) ) )){
                buffer_append(context->value, p, 1);
            }
            else{
                tmeta = vmCompileMetaCreateWithNumberString( buffer_to_str(context->value));
                tnode = vmCompileExpNodeCreateWithMeta(tmeta, InvokeTickArg);
                vmCompileObjectRelease(tmeta);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
                buffer_clear(context->value);
                context->state = 0;
                context->hex = hbool_false;
                buffer_append(context->exp, p, 1);
            }
            break;
        case 2:
            if(ISOP(*p)){
                buffer_append(context->exp, p, 1);
                context->state = 0;
            }
            else{
                buffer_append(context->exp, p, 1);
            }
            break;
        default:
            break;
    }
    
    return hbool_true;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hbuffer_t action;
    hbuffer_t exp;
    hint32 state;
}vmCompileExpressionActionContext;

static hbool vmCompileExpressionActionScannerCallback(vmCompileExpressionActionContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    vmCompileExpNode * tnode;
    
    if(p == NULL && node == NULL){
        
        if(buffer_length(context->action) ){
            tnode = vmCompileExpNodeCreateWithAction(buffer_to_str(context->action), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->action);
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        if(buffer_length(context->action) ){
            tnode = vmCompileExpNodeCreateWithAction(buffer_to_str(context->action), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->action);
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        vmCompileObjectArrayAdd(context->list,node);
        return hbool_true;
    }

    switch (context->state) {
        case 0:
            if(ISNAMEFIRST(*p)){
                buffer_append(context->action, p, 1);
                context->state ++;
            }
            else if(*p == '.'){
                buffer_append(context->action, p, 1);
                context->state = 2;
            }
            else if(ISOP(*p)){
                if(buffer_length(context->action) ){
                    tnode = vmCompileExpNodeCreateWithAction(buffer_to_str(context->action), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->action);
                }
                context->state = 3;
                buffer_append(context->exp, p, 1);
            }
            else{
                return hbool_false;
            }
            break;
        case 1:
            if(ISNAME(*p)){
                buffer_append(context->action, p, 1);
            }
            else if(*p == '.'){
                buffer_append(context->action, p, 1);
                context->state ++;
            }
            else if(ISOP(*p)){
                if(buffer_length(context->action) ){
                    tnode = vmCompileExpNodeCreateWithAction(buffer_to_str(context->action), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->action);
                }
                buffer_append(context->exp, p, 1);
                context->state = 3;
            }
            else{
                return hbool_false; 
            }
            break;
        case 2:
            if(ISNAMEFIRST(*p)){
                context->state = 1;
                buffer_append(context->action, p, 1);
            }
            else{
                return hbool_false;
            }
            break;
        case 3:
            if(ISOP(*p)){
                buffer_append(context->exp, p, 1);
            }
            else if(*p == '.'){
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                buffer_append(context->action, p, 1);
                context->state = 2;
            }
            else if(ISNAMEFIRST(*p)){
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(context->list, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                buffer_append(context->action, p, 1);
                context->state = 1;
            }
            else{
                return hbool_false;
            }
            break;
        default:
            break;
    }
    
    return hbool_true;
}

// () []
typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hstack_t groupStack;
    hstack_t charStack;
    hbuffer_t exp;
    hint32 state;
}vmCompileExpressionGroupContext;

static hbool vmCompileExpressionGroupScannerCallback(vmCompileExpressionGroupContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    vmCompileExpNode * tnode;
    vmCompileObjectArray * list1 = NULL;
    vmCompileObjectArray * list2 = NULL;
    
    if(p == NULL && node == NULL){
        if(stack_peek(context->groupStack) != NULL){
            return hbool_false;
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        list1 = (vmCompileObjectArray *)stack_peek(context->groupStack);
        if(list1 == NULL){
            list1 = context->list;
        }
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(list1, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        vmCompileObjectArrayAdd(list1,node);
        return hbool_true;
    }
    
    
    switch (context->state) {
        case 0:
            if(*p == '(' || *p == '['){
                list1 = (vmCompileObjectArray *)stack_peek(context->groupStack);
                if(list1 == NULL){
                    list1 = context->list;
                }
                
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(list1, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                
                list2 = vmCompileObjectArrayCreate();
                if(*p == '('){
                    tnode = vmCompileExpNodeCreateWithGroup(list2,InvokeTickArg);
                }
                else{
                    tnode = vmCompileExpNodeCreateWithIndex(list2,InvokeTickArg);
                }
                vmCompileObjectRelease(list2);
                vmCompileObjectArrayAdd(list1, tnode);
                vmCompileObjectRelease(tnode);
                
                stack_push(context->groupStack, list2);
                stack_push(context->charStack, (hany)(*p == '(' ? ')':']'));
            }
            else if(*p == ')' || *p == ']'){
                
                list1 = (vmCompileObjectArray *) stack_peek(context->groupStack);
                if(list1 == NULL){
                    return hbool_false;
                }
                
                if((hchar)(hintptr)stack_peek(context->charStack) !=  *p){
                    return hbool_false;
                }
                
                if(buffer_length(context->exp)){
                    tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                    vmCompileObjectArrayAdd(list1, tnode);
                    vmCompileObjectRelease(tnode);
                    buffer_clear(context->exp);
                }
                
                stack_pop(context->groupStack);
                stack_pop(context->charStack);
            }
            else{
                buffer_append(context->exp, p, 1);
            }
            break;
        default:
            break;
    }
    
    return hbool_true;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
    hbuffer_t exp;
    hbuffer_t op;
    hchar isop[5];
    hint32 index;
}vmCompileExpressionOperatorContext;

static hbool vmCompileExpressionOperatorScannerCallback(vmCompileExpressionOperatorContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    vmCompileExpNode * tnode;

    if(p == NULL && node == NULL){
        
        if(buffer_length(context->op)){
            buffer_append(context->exp, buffer_data(context->op), buffer_length(context->op));
            buffer_clear(context->op);
        }
        
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        
        context->index = 0;
        
        return hbool_true;
    }
    
    if(p == NULL && node != NULL){
        
        context->index = 0;
        
        if(buffer_length(context->op)){
            buffer_append(context->exp, buffer_data(context->op), buffer_length(context->op));
            buffer_clear(context->op);
        }
        
        if(buffer_length(context->exp)){
            tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->exp);
        }
        
        vmCompileObjectArrayAdd(context->list,node);
        return hbool_true;
    }
    
    if(context->isop[context->index] == *p){
        buffer_append(context->op, p, 1);
        context->index ++;
        if(context->isop[context->index] ==0){
            if(buffer_length(context->exp)){
                tnode = vmCompileExpNodeCreateWithExp(buffer_to_str(context->exp), InvokeTickArg);
                vmCompileObjectArrayAdd(context->list, tnode);
                vmCompileObjectRelease(tnode);
                buffer_clear(context->exp);
            }
            tnode = vmCompileExpNodeCreateWithOperator(buffer_to_str(context->op), InvokeTickArg);
            vmCompileObjectArrayAdd(context->list, tnode);
            vmCompileObjectRelease(tnode);
            buffer_clear(context->op);
            context->index = 0;
        }
    }
    else{
        if(buffer_length(context->op)){
            buffer_append(context->exp, buffer_data(context->op), buffer_length(context->op));
            buffer_clear(context->op);
        }
        buffer_append(context->exp, p, 1);
        context->index = 0;
    }
    
    
    return hbool_true;
}

typedef struct{
    vmCompileExpressionContext base;
    vmCompileObjectArray * list;
}vmCompileExpressionFunctionContext;

hbool vmCompileExpressionFunction(vmCompileExpressionContext * context,InvokeTickDeclare);

static hbool vmCompileExpressionFunctionScannerCallback(vmCompileExpressionFunctionContext * context,hchar * p,vmCompileExpNode * node,InvokeTickDeclare){
    vmCompileObjectArray * list = NULL;
    vmCompileExpNode * tnode;
    vmCompileExpNode * fnode1, * fnode2;
    
    if(p == NULL && node == NULL){

        return hbool_true;
    }
    
    if(p == NULL && node != NULL){


        if(node->type == vmCompileExpNodeTypeGroup){
            fnode1 =  (vmCompileExpNode *)vmCompileObjectArrayLast(context->list);
            fnode2 = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, vmCompileObjectArrayCount(context->list) -2);
            if(fnode1 && fnode1->type == vmCompileExpNodeTypeAction){
                list = node->value.groupValue;
                {
                    hint32 i,c;
                    vmCompileObjectArray * args = vmCompileObjectArrayCreate(), * tarray;
                    vmCompileExpNode * pnode;
                    vmCompileExpNode * before = NULL;
                    hbool rs = hbool_true;
                    hlist_t splitList = list_alloc(10, 10);
                    
                    list_split_str(splitList, buffer_to_str(fnode1->value.actionValue), ".");

                    if(fnode2 && fnode2->type == vmCompileExpNodeTypeOperator && mem_strcmp( fnode2->value.operatorValue,"new ") == 0 && list_count(splitList) != 1){
                        list_split_free(splitList);
                        list_dealloc(splitList);
                        vmCompileObjectRelease(args);
                        return hbool_false;
                    }
                    
                    c = vmCompileObjectArrayCount(list);
                    for(i=0;i<c;i++){
                        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(list, i);
                        
                        if(pnode->type == vmCompileExpNodeTypeOperator && strcmp(pnode->value.operatorValue, ",") ==0){
                            if(before){
                                vmCompileObjectArrayAdd(args, before);
                                vmCompileObjectRelease(before);
                                before = NULL;
                            }
                            else{
                                rs = hbool_false;
                                break;
                            }
                        }
                        else{
                            if(before){
                                vmCompileObjectArrayAdd(before->value.groupValue, pnode);
                            }
                            else{
                                tarray = vmCompileObjectArrayCreate();
                                before = vmCompileExpNodeCreateWithGroup(tarray, InvokeTickArg);
                                vmCompileObjectRelease(tarray);
                                vmCompileObjectArrayAdd(before->value.groupValue, pnode);
                            }
                        }
                    }
                    if(rs){
                        if(before){
                            vmCompileObjectArrayAdd(args, before);
                            vmCompileObjectRelease(before);
                            before = NULL;
                        }
                        tnode = vmCompileExpNodeCreateWithFunction((hcchar *)list_last(splitList), args, InvokeTickArg);
                        
                        c = list_count(splitList);
                        
                        if(c <= 1){
                            vmCompileObjectArrayRemoveLast(context->list);
                        }
                        else{
                            buffer_clear(fnode1->value.actionValue);
                            for(i=0;i<c-1;i++){
                                if(i != 0){
                                    buffer_append_str(fnode1->value.actionValue, ".");
                                }
                                buffer_append_str(fnode1->value.actionValue, (hcchar *)list_get(splitList, i));
                            }
                            
                        }
                        
                        list_split_free(splitList);
                        list_dealloc(splitList);
                        vmCompileObjectArrayAdd(context->list, tnode);
                        vmCompileObjectRelease(tnode);
                        vmCompileObjectRelease(args);
                    }
                    else{
                        if(before){
                            vmCompileObjectArrayAdd(args, before);
                            vmCompileObjectRelease(before);
                            before = NULL;
                        }
                        vmCompileObjectRelease(args);
                        list_split_free(splitList);
                        list_dealloc(splitList);
                        return rs;
                    }
                }
            }
            else{
                vmCompileObjectArrayAdd(context->list, node);
            }
        }
        else {
            vmCompileObjectArrayAdd(context->list, node);
        }
        
        return hbool_true;
    }

    return hbool_false;
}


static void vmCompileExpressionListPrint(vmCompileObjectArray * list,InvokeTickDeclare){
    hint32 c = vmCompileObjectArrayCount(list);
    hint32 i;
    vmCompileExpNode *node;
    
    for(i=0;i<c;i++){
        node = (vmCompileExpNode *)vmCompileObjectArrayGet(list, i);
        if(node->type == vmCompileExpNodeTypeExp){
            hlog("%s",buffer_to_str(node->value.expValue));
        }
        else if(node->type == vmCompileExpNodeTypeAction){
            hlog("%s",buffer_to_str(node->value.actionValue));
        }
        else if(node->type == vmCompileExpNodeTypeMeta){
            vmCompileMetaPrint(node->value.metaValue);
        }
        else if(node->type == vmCompileExpNodeTypeFunction){
            hlog(".@@");
            hlog("(");
            vmCompileExpressionListPrint(node->value.functionValue.arguments,InvokeTickArg);
            hlog(")");
        }
        else if(node->type == vmCompileExpNodeTypeOperator){
            hlog("%s",node->value.operatorValue);
        }
        else if(node->type == vmCompileExpNodeTypeGroup){
            hlog("(");
            vmCompileExpressionListPrint(node->value.groupValue,InvokeTickArg);
            hlog(")");
        }
        else if(node->type == vmCompileExpNodeTypeIndex){
            hlog("[");
            vmCompileExpressionListPrint(node->value.indexValue,InvokeTickArg);
            hlog("]");
        }
        else{
            assert(0);
        }
    }

}

hbool vmCompileExpressionFunction(vmCompileExpressionContext * context,InvokeTickDeclare){
    vmCompileExpressionFunctionContext argumentsContext;
    vmCompileExpressionContext cxt = {NULL};
    vmCompileExpNode * pnode, *pnode2;
    hbool result = hbool_true;
    hint32 i,c,j,n;

	argumentsContext.base = * context;
	argumentsContext.list = vmCompileObjectArrayCreate();

    result = vmCompileExpressionScanner(&argumentsContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionFunctionScannerCallback,InvokeTickArg);

    if(result){
        vmCompileObjectRelease(context->list);
        context->list = argumentsContext.list;
        argumentsContext.list = NULL;
        
        c = vmCompileObjectArrayCount(context->list);
        for(i=0;i<c;i++){
            pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
            if(pnode->type == vmCompileExpNodeTypeGroup){
                cxt.list = pnode->value.groupValue;
                result = vmCompileExpressionFunction(&cxt,InvokeTickArg);
                if(result){
                    pnode->value.groupValue = cxt.list;
                    cxt.list = NULL;
                }
                else{
                    cxt.list = NULL;
                    break;
                }
            }
            else if(pnode->type == vmCompileExpNodeTypeFunction){
                
                n = vmCompileObjectArrayCount(pnode->value.functionValue.arguments);
                for(j=0;j<n;j++){
                    pnode2 =  (vmCompileExpNode *)vmCompileObjectArrayGet(pnode->value.functionValue.arguments, j);
                    if(pnode2->type == vmCompileExpNodeTypeGroup){
                        cxt.list = pnode2->value.groupValue;
                        result = vmCompileExpressionFunction(&cxt,InvokeTickArg);
                        if(result){
                            pnode2->value.groupValue = cxt.list;
                            cxt.list = NULL;
                        }
                        else{
                            cxt.list = NULL;
                            break;
                        }
                    }
                    else{
                        result = hbool_false;
                        break;
                    }

                }
                
                if(!result){
                    break;
                }
                
            }
            else if(pnode->type == vmCompileExpNodeTypeIndex){
                cxt.list = pnode->value.indexValue;
                result = vmCompileExpressionFunction(&cxt,InvokeTickArg);
                if(result){
                    pnode->value.indexValue = cxt.list;
                    cxt.list = NULL;
                }
                else{
                    cxt.list = NULL;
                    break;
                }
            }
        }
    }

    if(argumentsContext.list){
        vmCompileObjectRelease(argumentsContext.list);
        argumentsContext.list = NULL;
    }
    
    return result;
}

static hbool vmCompileExpressionValidateNode(vmCompileExpNode * node,InvokeTickDeclare){
    hint32 c,i;
    vmCompileExpNode *pnode;
    if(node->type == vmCompileExpNodeTypeExp){
        return hbool_false;
    }
    if(node->type == vmCompileExpNodeTypeGroup){
        c = vmCompileObjectArrayCount(node->value.groupValue);
        for(i=0;i<c;i++){
            pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(node->value.groupValue, i);
            if(!vmCompileExpressionValidateNode(pnode,InvokeTickArg)){
                return hbool_false;
            }
        }
    }
    return hbool_true;
}

static hbool vmCompileExpressionValidate(vmCompileExpressionContext * context,InvokeTickDeclare){
    hint32 c,i;
    vmCompileExpNode *pnode;
    
    c = vmCompileObjectArrayCount(context->list);
    for(i=0;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(!vmCompileExpressionValidateNode(pnode,InvokeTickArg)){
            return hbool_false;
        }
    }
    return hbool_true;
}


hbool vmCompileExpressionListToMeta(vmCompileExpressionContext * context,vmCompileMeta ** meta,InvokeTickDeclare);

static hbool vmCompileExpressionActionToMeta(hcchar * action,vmCompileMeta * forMeta,vmCompileMeta ** outMeta,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    hlist_t list = list_alloc(10, 20);
    hint32 i = 0,c;
    hbool result = hbool_true;
    
    if(forMeta){
        meta = (vmCompileMeta *)vmCompileObjectRetain(forMeta);
    }
    
    list_split_str(list, action, ".");
    c = list_count(list);
    if(c >0){
        if(forMeta){
            i = 0;
        }
        else{
            meta = vmCompileMetaCreateWithObjectKey(list_get(list, 0));
            i = 1;
        }
        for(;i<c;i++){
            op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeProperty, list_get(list, i));
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
            meta = vmCompileMetaCreateWithOperator(op);
            vmCompileObjectRelease(op);
        }
    }
    list_split_free(list);
    list_dealloc(list);

    if(result){
        * outMeta = meta;
    }
    else{
        * outMeta = NULL;
        vmCompileObjectRelease(meta);
    }
    
    return result;
}

static hbool vmCompileExpressionIndexToMeta(vmCompileExpressionContext * context,vmCompileMeta * forMeta,vmCompileMeta ** outMeta,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    hbool result = hbool_true;
    
    assert(forMeta);

    result = vmCompileExpressionListToMeta(context,&meta,InvokeTickArg);

    if(result){
        op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeProperty, NULL);
        vmCompileMetaOperatorAddCompileMeta(op, forMeta);
        vmCompileMetaOperatorAddCompileMeta(op, meta);
        vmCompileObjectRelease(meta);
        * outMeta = vmCompileMetaCreateWithOperator(op);;
        vmCompileObjectRelease(op);
    }
    else{
        * outMeta = NULL;
    }
    
    return  result;
}

static hbool vmCompileExpressionFunctionToMeta(hcchar * name,vmCompileObjectArray * args,vmCompileMeta * forMeta,vmCompileMeta ** outMeta,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileExpressionContext context = {0};
    vmCompileMetaOperator * op;
    vmCompileExpNode * pnode;
    hbool result = hbool_true;
    hint32 i,c;
    assert(forMeta);
    
    op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeFunction, name);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta);
    
    c = vmCompileObjectArrayCount(args);
    
    for(i= 0;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(args, i);
        if(pnode->type == vmCompileExpNodeTypeGroup){
            context.list = pnode->value.groupValue;
            result = vmCompileExpressionListToMeta(&context,&meta,InvokeTickArg);
            pnode->value.groupValue = context.list;
            if(!result){
                break;
            }
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileMetaOperatorAddCompileMeta(op, pnode->value.metaValue);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        * outMeta = vmCompileMetaCreateWithOperator(op);
        vmCompileObjectRelease(op);
    }
    else{
        vmCompileObjectRelease(op);
        *outMeta = NULL;
    }
    
    return  result;
}

// new
static hbool vmCompileExpressionNewToMeta(hcchar * name,vmCompileObjectArray * args,vmCompileMeta ** outMeta,InvokeTickDeclare){
    vmCompileMeta *meta = NULL;
    vmCompileMetaOperator * op;
    vmCompileExpressionContext context = {0};
    vmCompileExpNode * pnode;
    hbool result = hbool_true;
    hint32 i,c;

    op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeNew, name);

    c = vmCompileObjectArrayCount(args);
    
    for(i= 0;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(args, i);
        if(pnode->type == vmCompileExpNodeTypeGroup){
            context.list = pnode->value.groupValue;
            result = vmCompileExpressionListToMeta(&context,&meta,InvokeTickArg);
            pnode->value.groupValue = context.list;
            if(!result){
                break;
            }
            vmCompileMetaOperatorAddCompileMeta(op, meta);
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileMetaOperatorAddCompileMeta(op, pnode->value.metaValue);
            pnode->type = vmCompileExpNodeTypeNone;
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        * outMeta = vmCompileMetaCreateWithOperator(op);
        vmCompileObjectRelease(op);
    }
    else{
        vmCompileObjectRelease(op);
        *outMeta = NULL;
    }
    
    return  result;
}

// =
static vmCompileMeta * vmCompileExpressionAssignToMeta(hcchar * name,vmCompileMeta * valueMeta,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeAssign , name);
    vmCompileMetaOperatorAddCompileMeta(op, valueMeta);
    meta = vmCompileMetaCreateWithOperator(op);
    vmCompileObjectRelease(op);
    return  meta;
}

// ! ++ -- -
static vmCompileMeta * vmCompileExpressionMetaOne(vmRuntimeOperatorType type,vmCompileMeta * forMeta,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    assert(forMeta);
    if(forMeta->type == vmRuntimeMetaTypeObject){
        op = vmCompileMetaOperatorCreate(type, buffer_to_str(forMeta->value.objectKey));
    }
    else if((type != vmRuntimeOperatorTypeNot && type != vmRuntimeOperatorTypeAntiNumber) && forMeta->type == vmRuntimeMetaTypeOperator && forMeta->value.operatorMeta->type == vmRuntimeOperatorTypeProperty 
            && buffer_length(forMeta->value.operatorMeta->uniqueKey) && vmCompileObjectArrayCount(forMeta->value.operatorMeta->compileMetas) ==1){
        
        op = vmCompileMetaOperatorCreate(type, buffer_to_str(forMeta->value.operatorMeta->uniqueKey));
        vmCompileMetaOperatorAddCompileMeta(op, (vmCompileMeta *)vmCompileObjectArrayLast(forMeta->value.operatorMeta->compileMetas));

    }
    else {
        op = vmCompileMetaOperatorCreate(type, NULL);
        vmCompileMetaOperatorAddCompileMeta(op, forMeta);
    }
    meta = vmCompileMetaCreateWithOperator(op);
    vmCompileObjectRelease(op);
    return  meta;
}

// * / % + - & | ^ << >>  > >=< <= == !=  && ||
static vmCompileMeta * vmCompileExpressionMetaTow(vmRuntimeOperatorType type,vmCompileMeta * forMeta1,vmCompileMeta * forMeta2,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    op = vmCompileMetaOperatorCreate(type, NULL);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta1);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta2);
    meta = vmCompileMetaCreateWithOperator(op);
    vmCompileObjectRelease(op);
    return  meta;
}

// ? :
static vmCompileMeta * vmCompileExpressionMetaThree(vmRuntimeOperatorType type,vmCompileMeta * forMeta1,vmCompileMeta * forMeta2,vmCompileMeta * forMeta3,InvokeTickDeclare){
    vmCompileMeta * meta = NULL;
    vmCompileMetaOperator * op;
    op = vmCompileMetaOperatorCreate(type, NULL);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta1);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta2);
    vmCompileMetaOperatorAddCompileMeta(op, forMeta3);
    meta = vmCompileMetaCreateWithOperator(op);
    vmCompileObjectRelease(op);
    return  meta;
}



hbool vmCompileExpressionUnionAGFI(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP1(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP2(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP3(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP4(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP5(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP6(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP7(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP8(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP9(vmCompileExpressionContext * context, InvokeTickDeclare);
hbool vmCompileExpressionUnionOP10(vmCompileExpressionContext * context, InvokeTickDeclare);

hbool vmCompileExpressionListToMeta(vmCompileExpressionContext * context ,vmCompileMeta ** meta,InvokeTickDeclare){
    
    hbool result = hbool_true;
    vmCompileExpNode * pnode;
    
    
    hlog("\n\nListToMeta  ... \n\n");
    
    
    if(result){
        result = vmCompileExpressionUnionAGFI(context,InvokeTickArg);
    }
    
    if(result){
        result = vmCompileExpressionUnionOP1(context,InvokeTickArg);
    }
    else{
        hlog("UnionAGF\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP2(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP1\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP3(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP2\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP4(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP3\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP5(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP4\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP6(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP5\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP7(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP6\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP8(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP7\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP9(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP8\n");
    }
    
    if(result){
        result = vmCompileExpressionUnionOP10(context,InvokeTickArg);
    }
    else{
        hlog("UnionOP9\n");
    }
    
    if(result ){
        if(vmCompileObjectArrayCount(context->list) == 1){
            pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, 0);
            if(pnode->type == vmCompileExpNodeTypeMeta){
                * meta =(vmCompileMeta *) vmCompileObjectRetain(pnode->value.metaValue);
            }
            else{
                * meta  = NULL;
                result = hbool_false;
            }
        }
        else if(vmCompileObjectArrayCount(context->list) == 0){
            * meta = vmCompileMetaCreate();
        }
        else{
            * meta  = NULL;
            vmCompileExpressionListPrint(context->list,InvokeTickArg);
            result = hbool_false;
        }
    }
    else{
        * meta  = NULL;
        hlog("UnionOP10\n");
    }
    
    return result;
}

//vmCompileExpNodeTypeAction , vmCompileExpNodeTypeGroup, vmCompileExpNodeTypeFunction, vmCompileExpNodeTypeIndex, => vmCompileExpNodeTypeMeta

hbool vmCompileExpressionUnionAGFI(vmCompileExpressionContext * context, InvokeTickDeclare){
    
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileMeta * meta = NULL;
    vmCompileExpressionContext ctx = {0};
    hbool result = hbool_true;
    
    hlog("\n");
    
    vmCompileExpressionListPrint(context->list, InvokeTickArg);
    
    hlog("\n");
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeAction){
            fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
            if(fnode){
                if(fnode->type == vmCompileExpNodeTypeMeta){
                    result = vmCompileExpressionActionToMeta(buffer_to_str(pnode->value.actionValue), fnode->value.metaValue, &meta, InvokeTickArg);
                    if(result){
                        vmCompileObjectArrayRemoveLast(list);
                        tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                        vmCompileObjectRelease(meta);
                        vmCompileObjectArrayAdd(list, tnode);
                        vmCompileObjectRelease(tnode);
                    }
                    else{
                        hlog("ActionToMeta %s \n",buffer_to_str(pnode->value.actionValue));
                        break;
                    }
                }
                else if(fnode->type ==  vmCompileExpNodeTypeOperator){
                    result = vmCompileExpressionActionToMeta(buffer_to_str(pnode->value.actionValue), NULL, &meta, InvokeTickArg);
                    if(result){
                        tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                        vmCompileObjectRelease(meta);
                        vmCompileObjectArrayAdd(list, tnode);
                        vmCompileObjectRelease(tnode);
                    }
                    else{
                        hlog("ActionToMeta %s \n",buffer_to_str(pnode->value.actionValue));
                        break;
                    }
                }
                else{
                    result = hbool_false;
                }
            }
            else{
                result = vmCompileExpressionActionToMeta(buffer_to_str(pnode->value.actionValue), NULL, &meta, InvokeTickArg);
                if(result){
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    hlog("ActionToMeta %s \n",buffer_to_str(pnode->value.actionValue));
                    break;
                }
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeGroup){
            ctx.list = pnode->value.groupValue;
            result = vmCompileExpressionListToMeta(&ctx, &meta, InvokeTickArg);
            pnode->value.groupValue = ctx.list;
            if(result){
                tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                vmCompileObjectRelease(meta);
                vmCompileObjectArrayAdd(list, tnode);
                vmCompileObjectRelease(tnode);
            }
            else{
                hlog("ListToMeta error \n");
                vmCompileExpressionListPrint(pnode->value.groupValue, InvokeTickArg);
                
                hlog("\n");
                break;
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeFunction){
            fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
            if(fnode && fnode->type == vmCompileExpNodeTypeMeta){
                result = vmCompileExpressionFunctionToMeta(buffer_to_str(pnode->value.functionValue.name), pnode->value.functionValue.arguments, fnode->value.metaValue, &meta, InvokeTickArg);
                if(result){
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    hlog("FunctionToMeta %s \n",buffer_to_str(pnode->value.functionValue.name));
                    break;
                }
            }
            else if(fnode && fnode->type == vmCompileExpNodeTypeOperator && mem_strcmp(fnode->value.operatorValue,"new ") ==0){
                result = vmCompileExpressionNewToMeta(buffer_to_str(pnode->value.functionValue.name), pnode->value.functionValue.arguments, &meta, InvokeTickArg);
                if(result){
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    hlog("new FunctionToMeta %s \n",buffer_to_str(pnode->value.functionValue.name));
                    break;
                }
            }
            else{
                result = hbool_false;
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeIndex){
            fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
            if(fnode && fnode->type == vmCompileExpNodeTypeMeta){
                ctx.list = pnode->value.indexValue;
                result = vmCompileExpressionIndexToMeta( &ctx, fnode->value.metaValue, &meta, InvokeTickArg);
                pnode->value.indexValue = ctx.list;
                if(result){
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    hlog("IndexToMeta  \n");
                    break;
                }
            }
            else{
                result = hbool_false;
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeOperator || pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list,pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
}

// ++ -- 
hbool vmCompileExpressionUnionOP1(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
           if( mem_strcmp(pnode->value.operatorValue,"++") ==0 || mem_strcmp(pnode->value.operatorValue,"--") ==0){
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                if(fnode && fnode->type == vmCompileExpNodeTypeMeta){
                    
                    if( mem_strcmp(pnode->value.operatorValue,"++") ==0){
                        meta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeInc, fnode->value.metaValue,InvokeTickArg);
                    }
                    else{
                        meta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeDec, fnode->value.metaValue,InvokeTickArg);
                    }

                    vmCompileObjectArrayRemoveLast(list);
                    
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                }
                else{
                    nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                    if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                        if( mem_strcmp(pnode->value.operatorValue,"++") ==0){
                            meta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeBeforeInc, nnode->value.metaValue,InvokeTickArg);
                        }
                        else{
                             meta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeBeforeDec, nnode->value.metaValue,InvokeTickArg);
                        }
                        tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                        vmCompileObjectRelease(meta);
                        vmCompileObjectArrayAdd(list, tnode);
                        vmCompileObjectRelease(tnode);
                        i ++;
                    }
                    else{
                        result = hbool_false;
                        break; 
                    }
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;

}

// ! ~ -
hbool vmCompileExpressionUnionOP2(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode1;
    vmCompileExpNode * fnode2;
    vmCompileMeta * meta = NULL,*tmeta;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            
            fnode1  = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
            
            if(fnode1){
                pnode->type = vmCompileExpNodeTypeNone;
                meta = (vmCompileMeta *)vmCompileObjectRetain(pnode->value.metaValue);
                
                while(fnode1){
                    if(fnode1->type == vmCompileExpNodeTypeOperator){
                        if(mem_strcmp(fnode1->value.operatorValue,"!") ==0){
                            tmeta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeNot, meta, InvokeTickArg);
                            vmCompileObjectRelease(meta);
                            meta = tmeta;
                            vmCompileObjectArrayRemoveLast(list);
                            fnode1 = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                        }
                        else if(mem_strcmp(fnode1->value.operatorValue,"~") ==0){
                            tmeta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeBitwiseComplement, meta, InvokeTickArg);
                            vmCompileObjectRelease(meta);
                            meta = tmeta;
                            vmCompileObjectArrayRemoveLast(list);
                            fnode1 = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                        }
                        else if(mem_strcmp(fnode1->value.operatorValue,"-") ==0){
                            fnode2 = (vmCompileExpNode *)vmCompileObjectArrayGet(list, vmCompileObjectArrayCount(list) -2);
                            if(!fnode2 || fnode2->type == vmCompileExpNodeTypeOperator){
                                tmeta = vmCompileExpressionMetaOne(vmRuntimeOperatorTypeAntiNumber, meta, InvokeTickArg);
                                vmCompileObjectRelease(meta);
                                meta = tmeta;
                                vmCompileObjectArrayRemoveLast(list);
                                fnode1 = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                            }
                            else{
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                    else{
                        break;
                    }
                }
                
                tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                vmCompileObjectRelease(meta);
                vmCompileObjectArrayAdd(list, tnode);
                vmCompileObjectRelease(tnode);
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}


// * / %
hbool vmCompileExpressionUnionOP3(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"*") ==0 || mem_strcmp(pnode->value.operatorValue,"/") ==0 || mem_strcmp(pnode->value.operatorValue,"%") ==0){
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    if( mem_strcmp(pnode->value.operatorValue,"*") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeMultiplication, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"/") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeDivision, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"%") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeModulus, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// + -
hbool vmCompileExpressionUnionOP4(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"+") ==0 || mem_strcmp(pnode->value.operatorValue,"-") ==0 ){
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode =  (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    if( mem_strcmp(pnode->value.operatorValue,"+") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeAdd, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"-") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeSub, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// & | ^ << >>
hbool vmCompileExpressionUnionOP5(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta  *meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"&") ==0 || mem_strcmp(pnode->value.operatorValue,"|") ==0 
               ||  mem_strcmp(pnode->value.operatorValue,"^") ==0 || mem_strcmp(pnode->value.operatorValue,">>") ==0 
               || mem_strcmp(pnode->value.operatorValue,"<<") ==0){
                
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    if( mem_strcmp(pnode->value.operatorValue,"&") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeBitwiseAnd, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"|") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeBitwiseOr, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"^") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeBitwiseExclusiveOr, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,">>") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeBitwiseShiftRight, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else{
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeBitwiseShiftLeft, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }

                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list,pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// > >=< <= == != === !== is
hbool vmCompileExpressionUnionOP6(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,">") ==0 || mem_strcmp(pnode->value.operatorValue,">=") ==0 
               || mem_strcmp(pnode->value.operatorValue,"<") ==0 || mem_strcmp(pnode->value.operatorValue,"<=") ==0 
               || mem_strcmp(pnode->value.operatorValue,"==") ==0 || mem_strcmp(pnode->value.operatorValue,"!=") ==0
               || mem_strcmp(pnode->value.operatorValue,"===") ==0 || mem_strcmp(pnode->value.operatorValue,"!==") ==0
               || mem_strcmp(pnode->value.operatorValue," is ") ==0){
                
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    vmCompileExpressionListPrint(list, InvokeTickArg);
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    if( mem_strcmp(pnode->value.operatorValue,">") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeGreater, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,">=") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeGreaterEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"<") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeLess, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"<=") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeLessEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"==") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"!=") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeNotEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"===") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeAbsEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue,"!==") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeAbsNotEqual, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }
                    else if( mem_strcmp(pnode->value.operatorValue," is ") ==0){
                        meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeIs, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    }

                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// &&
hbool vmCompileExpressionUnionOP7(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"&&") ==0 ){
                
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeAnd, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// ||
hbool vmCompileExpressionUnionOP8(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"||") ==0 ){
                
                fnode =(vmCompileExpNode *) vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    meta = vmCompileExpressionMetaTow(vmRuntimeOperatorTypeOr, fnode->value.metaValue,nnode->value.metaValue,InvokeTickArg);
                    vmCompileObjectArrayRemoveLast(list);
                    tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                    vmCompileObjectRelease(meta);
                    vmCompileObjectArrayAdd(list, tnode);
                    vmCompileObjectRelease(tnode);
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
                
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// ? :
hbool vmCompileExpressionUnionOP9(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode1;
    vmCompileExpNode * nnode2;
    vmCompileExpNode * nnode3;
    vmCompileMeta * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"?") ==0 ){
                
                fnode =  (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode1 = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                if(!(nnode1 && nnode1->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                nnode2 = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+2);
                if(!(nnode2 && nnode2->type == vmCompileExpNodeTypeOperator && mem_strcmp(nnode2->value.operatorValue,":") ==0 )){
                    result = hbool_false;
                    break;
                }
                
                nnode3 = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+3);
                if(!(nnode1 && nnode1->type == vmCompileExpNodeTypeMeta)){
                    result = hbool_false;
                    break;
                }
                
                meta = vmCompileExpressionMetaThree(vmRuntimeOperatorTypeIfElse,fnode->value.metaValue,nnode1->value.metaValue,nnode3->value.metaValue,InvokeTickArg);
                
                
                vmCompileObjectArrayRemoveLast(list);
                
                tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                
                vmCompileObjectRelease(meta);
                
                vmCompileObjectArrayAdd(list, tnode);
                
                vmCompileObjectRelease(tnode);
                
                i += 3;
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
                
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

// =
hbool vmCompileExpressionUnionOP10(vmCompileExpressionContext * context, InvokeTickDeclare){
    hint32 i,c;
    vmCompileObjectArray * list = vmCompileObjectArrayCreate();
    vmCompileExpNode * tnode;
    vmCompileExpNode * pnode ;
    vmCompileExpNode * fnode;
    vmCompileExpNode * nnode;
    vmCompileMeta  * meta = NULL;
    hbool result = hbool_true;
    
    c = vmCompileObjectArrayCount(context->list);
    
    for(i=0 ;i<c;i++){
        pnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i);
        if(pnode->type == vmCompileExpNodeTypeOperator){
            if( mem_strcmp(pnode->value.operatorValue,"=") ==0 ){
                
                fnode = (vmCompileExpNode *)vmCompileObjectArrayLast(list);
                
                if(!(fnode && fnode->type == vmCompileExpNodeTypeMeta 
                     && ( fnode->value.metaValue->type == vmRuntimeMetaTypeObject 
                         || (fnode->value.metaValue->type == vmRuntimeMetaTypeOperator 
                                && fnode->value.metaValue->value.operatorMeta->type == vmRuntimeOperatorTypeProperty)
                         ))){
                         
                    result = hbool_false;
                    break;
                }
                
                nnode = (vmCompileExpNode *)vmCompileObjectArrayGet(context->list, i+1);
                
                if(nnode && nnode->type == vmCompileExpNodeTypeMeta){
                    if(fnode->value.metaValue->type == vmRuntimeMetaTypeObject){
                        meta = vmCompileExpressionAssignToMeta(buffer_to_str(fnode->value.metaValue->value.objectKey),nnode->value.metaValue,InvokeTickArg);
                        vmCompileObjectArrayRemoveLast(list);
                        tnode = vmCompileExpNodeCreateWithMeta(meta, InvokeTickArg);
                        vmCompileObjectRelease(meta);
                        vmCompileObjectArrayAdd(list, tnode);
                        vmCompileObjectRelease(tnode);
                    }
                    else{
                        fnode->value.metaValue->value.operatorMeta->type = vmRuntimeOperatorTypeAssign;
                        vmCompileMetaOperatorAddCompileMeta(fnode->value.metaValue->value.operatorMeta, nnode->value.metaValue);
                    }
 
                    i++;
                }
                else{
                    result = hbool_false;
                    break;
                }
            }
            else{
                vmCompileObjectArrayAdd(list, pnode);
                
            }
        }
        else if(pnode->type == vmCompileExpNodeTypeMeta){
            vmCompileObjectArrayAdd(list, pnode);
        }
        else{
            result = hbool_false;
            break;
        }
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = list;
    }
    else{
        vmCompileObjectRelease(list);
    }
    
    return result;
    
}

static hbool vmCompileExpression(vmCompileExpressionContext * context,vmCompileMeta ** meta,InvokeTickDeclare){
    vmCompileExpressionClearContext clearContext;
    vmCompileExpressionStringValueContext stringValueContext;
    vmCompileExpressionValueContext valueContext;
    vmCompileExpressionActionContext actionContext;
    vmCompileExpressionOperatorContext operatorContext;
    vmCompileExpressionGroupContext groupContext ;

    hbool result = hbool_true;

	clearContext.base = * context;
	clearContext.list = vmCompileObjectArrayCreate();
	clearContext.buf = buffer_alloc(128,128);
	clearContext.state = 0;
	clearContext.step = 0;
    
	stringValueContext.base = * context;
	stringValueContext.list = vmCompileObjectArrayCreate();
	stringValueContext.exp = buffer_alloc(128,128);
	stringValueContext.value = buffer_alloc(128,128);
	stringValueContext.state = 0;

	valueContext.base = * context;
	valueContext.exp = buffer_alloc(128,128);
	valueContext.list = vmCompileObjectArrayCreate();
	valueContext.value = buffer_alloc(128,128);
	valueContext.state = 0;
	valueContext.step = 0;
    valueContext.hex = hbool_false;

	actionContext.base = * context;
	actionContext.list = vmCompileObjectArrayCreate();
	actionContext.exp = buffer_alloc(128,128);
	actionContext.action = buffer_alloc(128,128);
	actionContext.state = 0;
	
	operatorContext.base = * context;
	operatorContext.list = vmCompileObjectArrayCreate();
	operatorContext.exp = buffer_alloc(128,128);
	operatorContext.op = buffer_alloc(128,128);
	operatorContext.index = 0;
	mem_strcpy(operatorContext.isop, "");

	groupContext.base = * context;
	groupContext.list = vmCompileObjectArrayCreate();
	groupContext.exp = buffer_alloc(128,128);
	groupContext.charStack = stack_alloc();
	groupContext.groupStack = stack_alloc();
	groupContext.state = 0;

    if(result){
        
        stringValueContext.base = *context;
        
        result = vmCompileExpressionScanner(&stringValueContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionStringValueScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = stringValueContext.list;
        stringValueContext.list = NULL;
        
        hlog("\nString Value\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "new "); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();        
        
        hlog("\nOP new\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, " is "); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();        
        
        hlog("\nOP is \n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        clearContext.base = *context;
        result = vmCompileExpressionScanner(&clearContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionClearScannerCallback,InvokeTickArg);
    }

    if(result){
        vmCompileObjectRelease(context->list);
        context->list = clearContext.list;
        clearContext.list = NULL;
        
        hlog("\nClear\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        valueContext.base = *context;
        result = vmCompileExpressionScanner(&valueContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionValueScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = valueContext.list;
        valueContext.list = NULL;
       
        hlog("\nValue\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        actionContext.base = *context;
        result = vmCompileExpressionScanner(&actionContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionActionScannerCallback,InvokeTickArg);
        
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = actionContext.list;
        actionContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nAction\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, ","); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP,\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "++"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    } 
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP++\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "--"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP--\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "+"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    } 
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP+\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "-"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP-\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "*"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP*\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "/"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP/\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "%"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP%%\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "&&"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP&&\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "||"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
        
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP||\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "~"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP~\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "^"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP^\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "&"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP&\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "|"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP|\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "==="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP===\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "!=="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP!==\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "=="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }

    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP!==\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "!="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP!=\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "!"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP!\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, ">>"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP>>\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "<<"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP<<\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "<="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP<=\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, ">="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP>=\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "<"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP<\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, ">"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP>\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "="); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP=\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, "?"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = vmCompileObjectArrayCreate();
        
        hlog("\nOP?\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        operatorContext.base = * context;
        mem_strcpy(operatorContext.isop, ":"); 
        
        result = vmCompileExpressionScanner(&operatorContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionOperatorScannerCallback,InvokeTickArg);
    }
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = operatorContext.list;
        operatorContext.list = NULL;
        
        hlog("\nOP:\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        groupContext.base = *context;
        result = vmCompileExpressionScanner(&groupContext.base,(vmCompileExpressionScannerCallback)vmCompileExpressionGroupScannerCallback,InvokeTickArg);
    } 
    
    if(result){
        vmCompileObjectRelease(context->list);
        context->list = groupContext.list;
        groupContext.list = NULL;

        hlog("\nGroup\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        result = vmCompileExpressionFunction(context,InvokeTickArg);
        
    }
    
    if(result){
        
        hlog("\nFunction\n");
        
        vmCompileExpressionListPrint(context->list,InvokeTickArg);
        
        hlog("\n\n");
        
        result = vmCompileExpressionListToMeta(context, meta, InvokeTickArg);
        
    }
    
    if(result){
        
        hlog("\nExpression\n");
        
        hlog("\n\n");
        
        hlog("\nOK\n");
    }

    if(clearContext.list){
        vmCompileObjectRelease(clearContext.list);
    }
    if(clearContext.buf){
        buffer_dealloc(clearContext.buf);
    }
    
    if(stringValueContext.list){
        vmCompileObjectRelease(stringValueContext.list);
    }
    
    if(stringValueContext.exp){
        buffer_dealloc(stringValueContext.exp);
    }
    
    if(stringValueContext.value){
        buffer_dealloc(stringValueContext.value);
    }
     
    if(valueContext.list){
        vmCompileObjectRelease(valueContext.list);
    }
    if(valueContext.exp){
        buffer_dealloc(valueContext.exp);
    }
    if(valueContext.value){
        buffer_dealloc(valueContext.value);
    }
    
    if(actionContext.list){
        vmCompileObjectRelease(actionContext.list);
    }
    if(actionContext.exp){
        buffer_dealloc(actionContext.exp);
    }
    if(actionContext.action){
        buffer_dealloc(actionContext.action);
    }

    if(operatorContext.list){
        vmCompileObjectRelease(operatorContext.list);
    }
    if(operatorContext.exp){
        buffer_dealloc(operatorContext.exp);
    }
    if(operatorContext.op){
        buffer_dealloc(operatorContext.op);
    }
    
    if(groupContext.list){
        vmCompileObjectRelease(groupContext.list);
    }
    
    if(groupContext.exp){
        buffer_dealloc(groupContext.exp);
    }
    
    if(groupContext.charStack){
        stack_dealloc(groupContext.charStack);
    }
    
    if(groupContext.groupStack){
        stack_dealloc(groupContext.groupStack);
    }

    
    return result;
}

hbool vmCompileExpressionString(hcchar * exp,vmCompileMeta ** meta,InvokeTickDeclare){
    vmCompileExpNode * node = vmCompileExpNodeCreateWithExp(exp, InvokeTickArg);
    vmCompileExpressionContext context = {vmCompileObjectArrayCreate()};
	hbool rs = hbool_false;
    vmCompileObjectArrayAdd(context.list, node);
    vmCompileObjectRelease(node);
    
	rs = vmCompileExpression(&context,meta,InvokeTickArg);
    
    if(rs){
        vmCompileMetaPrint(* meta);
    }
    
    vmCompileObjectRelease(context.list);
    
    
    return rs;
}
