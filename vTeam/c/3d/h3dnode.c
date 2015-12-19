//
//  h3dndoe.c
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "h3dnode.h"
#include "h3dcontext.h"
#include "hlog.h"

c3d_node_condition_t c3d_node_condition_identity(){
    c3d_node_condition_t condition;
    memset(&condition, 0, sizeof(c3d_node_condition_t));
    condition.transform = c3d_matrix4_identity;
    condition.alpha = 1.0;
    return condition;
}


c3d_node_condition_t c3d_node_condition_multiply(c3d_node_condition_t conditionLeft,c3d_node_condition_t conditionRight){
    
    c3d_node_condition_t condition;
    hfloat clipLeft,clipRight,clipTop,clipBottom;
    
    memset(&condition, 0, sizeof(c3d_node_condition_t));
    
    condition.transform = c3d_matrix4_multiply(conditionLeft.transform, conditionRight.transform);
    condition.alpha = conditionLeft.alpha * conditionRight.alpha;
    

    condition.bounds.x = conditionLeft.bounds.x + conditionRight.bounds.x;
    condition.bounds.y = conditionLeft.bounds.y + conditionRight.bounds.y;
    condition.bounds.width = conditionRight.bounds.width;
    condition.bounds.height = conditionRight.bounds.height;
    
    clipLeft = MAX(condition.bounds.x, conditionLeft.clips.x);
    clipTop = MAX(condition.bounds.y,conditionLeft.clips.y);
    
    clipRight = MIN(condition.bounds.x + condition.bounds.width, conditionLeft.clips.x + conditionLeft.clips.width);
    clipBottom = MIN(condition.bounds.y + condition.bounds.height,conditionLeft.clips.y + conditionLeft.clips.height);
    
    if(clipRight - clipLeft >0){
        condition.clips.x = clipLeft;
        condition.clips.width = clipRight - clipLeft;
    }
    
    if(clipBottom - clipTop >0){
        condition.clips.y = clipTop;
        condition.clips.height = clipBottom - clipTop;
    }
       
    condition.contentOffset = conditionRight.contentOffset;
    
    return condition;

}

void c3d_node_render(struct _c3d_context_t * context,c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare){
    if(!( * node->pClass->hidden)(node,InvokeTickArg)){
        c3d_node_condition_t t ;
        hint32 c ;
        hint32 i;
        c3d_node_t * n;

        context->zIndex ++;
        
        if(node->context != context){
            node->context = context;
            (*node->pClass->connect)(node,InvokeTickArg);
        }
        
        t = (* node->pClass->condition)(node,condition,InvokeTickArg) ;
        
        c = (* node->pClass->childCount)(node,InvokeTickArg);
        
        if((* node->pClass->draw)(node,t,InvokeTickArg)){
            for(i=0;i<c;i++){
                n = (* node->pClass->childAt)(node,i,InvokeTickArg);
                c3d_node_render(context,n,t,InvokeTickArg);
            }
        }
    }
}

hbool c3d_node_touches(struct _c3d_context_t * context,c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare){
    if(!( * node->pClass->hidden)(node,InvokeTickArg) ){
        c3d_node_condition_t t ;
        hint32 c ;
        hint32 i;
        c3d_node_t * n;
        
        if(node->context != context){
            node->context = context;
            (*node->pClass->connect)(node,InvokeTickArg);
        }
         
        t = (* node->pClass->condition)(node,condition,InvokeTickArg) ;

        if((* node->pClass->receiveTouches)(node,t,touches,InvokeTickArg)){
            
            c = (* node->pClass->childCount)(node,InvokeTickArg);
            
            for(i=c -1;i>=0;i--){
                n = (* node->pClass->childAt)(node,i,InvokeTickArg);
                if(c3d_node_touches(context,n,t,touches,InvokeTickArg)){
                    return hbool_true;
                }
            }
            
            return (* node->pClass->touches)(node,t,touches,InvokeTickArg);
        }
    }
    return hbool_false;
}

static void c3d_node_texture_draw(struct _c3d_context_t * context,c3d_node_t * node,c3d_node_condition_t condition, InvokeTickDeclare){
    if(!( * node->pClass->hidden)(node,InvokeTickArg)){
        c3d_node_condition_t t ;
        hint32 c ;
        hint32 i;
        c3d_node_t * n;
        
        context->zIndex ++;
        
        if(node->context != context){
            node->context = context;
            (*node->pClass->connect)(node,InvokeTickArg);
        }
        
        t = (* node->pClass->condition)(node,condition,InvokeTickArg) ;
        
        c = (* node->pClass->childCount)(node,InvokeTickArg);
        
        if((* node->pClass->draw)(node,t,InvokeTickArg)){
            for(i=0;i<c;i++){
                n = (* node->pClass->childAt)(node,i,InvokeTickArg);
                c3d_node_render(context,n,t,InvokeTickArg);
            }
        }
    }
}


