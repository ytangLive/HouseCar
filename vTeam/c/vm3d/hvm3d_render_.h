//
//  hvm3d_render.h
//  hvm3d
//
//  Created by zhang hailong on 13-3-18.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_render_h
#define hvm3d_hvm3d_render_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hobjc.h"
#include "hvm3d.h"
    
    
    typedef struct _vmC3DRender{
        hobj_t base;
        vmC3DNode * node;
    } vmC3DRender;
    
    HOBJ_KEY_DEC(vmC3DRender)
    HOBJ_CLASS_DEC(vmC3DRender)
    
    HOBJ_KEY_DEC(draw)
    
    HOBJ_KEY_DEC(condition)
    
    vmC3DRender * vmC3DRenderAlloc(hobj_class_t * clazz, vmC3DNode * node,InvokeTickDeclare);
    
    void vmC3DRenderDraw(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);
    
    c3d_node_condition_t vmC3DRenderCondition(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);
    
    typedef void (*vmC3DRenderDrawImp)(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);
    
    typedef c3d_node_condition_t (*vmC3DRenderConditionImp)(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);
    
    typedef struct _C2DRenderProgramAttribs{
        c3d_location_t position;
        c3d_location_t normal;
        c3d_location_t txtCoord;
        c3d_location_t projectTransform;
        c3d_location_t normalTransform;
        c3d_location_t transform;
        c3d_location_t backgroundColor;
        c3d_location_t contents;
        c3d_location_t hasContents;
        c3d_location_t alpha;
        c3d_location_t flickX;
    } C2DRenderProgramAttribs;

    
    typedef struct _vmC2DRender{
        vmC3DRender base;
        c3d_texture_t texture;
        c3d_program_t program;
        C2DRenderProgramAttribs attribs;
        struct {
            vmUniqueKey width;
            vmUniqueKey height;
            vmUniqueKey left;
            vmUniqueKey top;
            vmUniqueKey contentLeft;
            vmUniqueKey contentTop;
            vmUniqueKey alpha;
            vmUniqueKey backgroundColor;
            vmUniqueKey backgroundAlpha;
            vmUniqueKey display;
            vmUniqueKey contents;
            vmUniqueKey contentsIndex;
            vmUniqueKey flickX;
            vmUniqueKey cornerRadius;
        } uniqueKeys;
    } vmC2DRender;
    
    HOBJ_KEY_DEC(vmC2DRender)
    HOBJ_CLASS_DEC(vmC2DRender)
    
#ifdef __cplusplus
}
#endif


#endif
