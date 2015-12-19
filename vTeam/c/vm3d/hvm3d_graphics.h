//
//  hvm3d_graphics.h
//  hvm3d
//
//  Created by zhang hailong on 13-3-20.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_graphics_h
#define hvm3d_hvm3d_graphics_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin.h"
#include "h3dnode.h"
#include "h3dcontext.h"
#include "h3dgraphics.h"
    
    extern vmClassBase vmC3DGraphicsClass;
    
    typedef struct _vmC3DGraphics{
        vmObject base;
        c3d_context_t * ctx;
        struct {
            vmUniqueKey drawText;
            vmUniqueKey fillRect;
            vmUniqueKey textSize;
        } uniqueKeys;
    } vmC3DGraphics;
    
    c3d_color3d_t vmVariantToColor(vmRuntimeContext ctx, vmVariant value,InvokeTickDeclare);
    
    c3d_font_t vmVariantToFont(vmRuntimeContext ctx, vmVariant value,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif



#endif
