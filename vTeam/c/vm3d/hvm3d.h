//
//  hvm3d.h
//  hvm3d
//
//  Created by Zhang Hailong on 13-3-17.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_h
#define hvm3d_hvm3d_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hvermin.h"
#include "h3dnode.h"
#include "h3dcontext.h"
#include "hobjc_bundle.h"
#include "hlist.h"
    
    extern vmClassBase vmC3DTransformClass;
    
    extern vmClassBase vmC3DContextClass;
    
    extern vmClassBase vmC3DTouchesClass;
    
    extern vmClassBase vmC3DNodeClass;
    
    typedef struct _vmC3DTransform{
        vmObject base;
        c3d_matrix4_t transform;
        struct {
            vmUniqueKey toValue;
            vmUniqueKey translate;
            vmUniqueKey rotateX;
            vmUniqueKey rotateY;
            vmUniqueKey rotateZ;
            vmUniqueKey scale;
        } uniqueKeys;
    } vmC3DTransform;
    
    typedef struct _vmC3DContext{
        vmObject base;
        c3d_context_t ctx;
        vmObject * values;
        struct {
            hobj_t * res;
            hobj_t * tmp;
            hobj_t * lib;
            hobj_t * doc;
        } bundle;
        struct {
            vmUniqueKey width;
            vmUniqueKey height;
            vmUniqueKey loadProgram;
            vmUniqueKey unLoadProgram;
            vmUniqueKey schedule;
            vmUniqueKey cancelSchedule;
            vmUniqueKey time;
            vmUniqueKey getStringContent;
            vmUniqueKey getBytesContent;
            vmUniqueKey getFilePath;
            vmUniqueKey needRender;
            vmUniqueKey memoryWarning;
        } uniqueKeys;
    } vmC3DContext;
    
    struct _vmC3DRender;
    
    typedef struct _vmC3DNode{
        vmObject base;
        c3d_node_t node;
        hlist_t childs;
        struct _vmC3DRender * render;
        c3d_matrix4_t transform;
        struct {
            hbool hidden;
            hbool receiveTouches;
            c3d_matrix4_t transform;
            hfloat alpha;
            struct _vmC3DNode * parent;
        } property;
        struct {
            vmUniqueKey length;
            vmUniqueKey childAt;
            vmUniqueKey parent;
            vmUniqueKey removeFromParent;
            vmUniqueKey addChild;
            vmUniqueKey insertChild;
            vmUniqueKey hidden;
            vmUniqueKey connect;
            vmUniqueKey disconnect;
            vmUniqueKey receiveTouches;
            vmUniqueKey touches;
            vmUniqueKey transform;
            vmUniqueKey alpha;
            vmUniqueKey context;
            vmUniqueKey toFront;
            vmUniqueKey toBack;
            vmUniqueKey render;
        } uniqueKeys;
        vmRuntimeContext runtimeContext;
    } vmC3DNode;

    typedef struct _vmC3DTouches {
        vmObject base;
        c3d_touches_t touches;
        c3d_matrix4_t transform;
        vmC3DNode * target;
        hbool cancelBubble;
        struct {
            vmUniqueKey length;
            vmUniqueKey touchId;
            vmUniqueKey touchX;
            vmUniqueKey touchY;
            vmUniqueKey touchZ;
            vmUniqueKey timestamp;
            vmUniqueKey state;
            vmUniqueKey target;
            vmUniqueKey cancelBubble;
        } uniqueKeys;
    } vmC3DTouches;
    
    typedef struct _vmC3DRender{
        vmObject base;
        hbool ( * draw)(struct _vmC3DRender * render,vmC3DNode * node,c3d_node_condition_t condition,InvokeTickDeclare);
        c3d_node_condition_t ( * condition )(struct _vmC3DRender * render,vmC3DNode * node,c3d_node_condition_t condition,InvokeTickDeclare);
        hbool ( * receiveTouches) (struct _vmC3DRender * render,vmC3DNode * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare);
    } vmC3DRender;

#define C3DNODE_TO_VMC3DNODE(node)  (vmC3DNode *) ((hbyte *) (node) - sizeof(vmObject))

#define C3DCONTEXT_TO_VMC3DCONTEXT(ctx) (vmC3DContext *) ((hbyte *) (ctx) - sizeof(vmObject))
    
    
#ifdef __cplusplus
}
#endif



#endif
