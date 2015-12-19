//
//  h3dnode.h
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef H3DNODE_H
#define H3DNODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "h3d.h"
    
    struct _c3d_node_t;
 
    typedef struct _c3d_node_condition_t{
        c3d_matrix4_t transform;
        float alpha;
        struct {
            hfloat x;
            hfloat y;
            hfloat width;
            hfloat height;
        } clips;
        struct {
            hfloat x;
            hfloat y;
            hfloat width;
            hfloat height;
        } bounds;
        struct {
            hfloat x;
            hfloat y;
        } contentOffset;
    }c3d_node_condition_t;
    
    c3d_node_condition_t c3d_node_condition_identity();
    
    c3d_node_condition_t c3d_node_condition_multiply(c3d_node_condition_t conditionLeft,c3d_node_condition_t conditionRight);
    
    typedef hbool (* c3d_node_hidden_fun_t)(struct _c3d_node_t * node,InvokeTickDeclare);
    
    typedef c3d_node_condition_t (* c3d_node_condition_fun_t)(struct _c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare);
    
    typedef hint32 (* c3d_node_child_count_fun_t)(struct _c3d_node_t * node,InvokeTickDeclare);
    
    typedef struct _c3d_node_t * (* c3d_node_child_at_fun_t)(struct _c3d_node_t * node,hint32 index,InvokeTickDeclare);
    
    typedef hbool (* c3d_node_draw_fun_t)(struct _c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare);
    
    typedef void (* c3d_node_connect_fun_t)(struct _c3d_node_t * node,InvokeTickDeclare);
    
    typedef hbool (* c3d_node_receive_touches_fun_t)(struct _c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare);
    
    typedef hbool (* c3d_node_touches_fun_t)(struct _c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches, InvokeTickDeclare);
    
    typedef struct _c3d_node_class_t{
        c3d_node_draw_fun_t draw;
        c3d_node_hidden_fun_t hidden;
        c3d_node_condition_fun_t condition;
        c3d_node_child_count_fun_t childCount;
        c3d_node_child_at_fun_t childAt;
        c3d_node_connect_fun_t connect;
        c3d_node_receive_touches_fun_t receiveTouches;
        c3d_node_touches_fun_t touches;
    } c3d_node_class_t;
    
    typedef struct _c3d_node_t{
        c3d_node_class_t * READONLY pClass;
        struct _c3d_context_t * READONLY context;
    } c3d_node_t;
    
    void c3d_node_render(struct _c3d_context_t * context,c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare);
    
    hbool c3d_node_touches(struct _c3d_context_t * context,c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
