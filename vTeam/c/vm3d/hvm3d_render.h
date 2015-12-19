//
//  hvm3d_render.h
//  hvm3d
//
//  Created by Zhang Hailong on 13-3-28.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_render_h
#define hvm3d_hvm3d_render_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvm3d.h"
#include "hvm3d_image.h"
    
    struct _buffer_t;

    typedef struct _vmC2DRender{
        vmC3DRender base;
        c3d_texture_t texture;
        c3d_context_t * c3dContext;
        long focusTouchId;
        struct {
            struct _buffer_t * key;
            c3d_program_t program;
            struct {
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
                c3d_location_t mask;
                c3d_location_t hasMask;
            } attribs;
        } program;
        
        struct {
            vmUniqueKey width;
            vmUniqueKey height;
            vmUniqueKey left;
            vmUniqueKey top;
            vmUniqueKey contentLeft;
            vmUniqueKey contentTop;
            vmUniqueKey backgroundColor;
            vmUniqueKey backgroundAlpha;
            vmUniqueKey display;
            vmUniqueKey mask;
            vmUniqueKey contents;
            vmUniqueKey contentsIndex;
            vmUniqueKey contentsLeft;
            vmUniqueKey contentsTop;
            vmUniqueKey contentsRight;
            vmUniqueKey contentsBottom;
            vmUniqueKey flickX;
            vmUniqueKey cornerRadius;
            vmUniqueKey clipLeft;
            vmUniqueKey clipTop;
            vmUniqueKey clipWidth;
            vmUniqueKey clipHeight;
            vmUniqueKey scale;
            vmUniqueKey isVisibleRect;
        } uniqueKeys;
        
        struct {
            hfloat width;
            hfloat height;
            hfloat left;
            hfloat top;
            hfloat contentLeft;
            hfloat contentTop;
            huint32 backgroundColor;
            hfloat backgroundAlpha;
            hbool display;
            vmC3DImage * mask;
            vmC3DImage * contents;
            huint32 contentsIndex;
            hfloat flickX;
            hfloat cornerRadius;
            hint32 clipLeft;
            hint32 clipTop;
            hint32 clipWidth;
            hint32 clipHeight;
            hfloat scale;
            hfloat contentsLeft;
            hfloat contentsTop;
            hfloat contentsRight;
            hfloat contentsBottom;
            hbool cached;
        } propertys;
        
    } vmC2DRender;
    
    extern vmClassBase vmC2DRenderClass;
    
#ifdef __cplusplus
}
#endif



#endif
