//
//  hvm3d_image.h
//  hvm3d
//
//  Created by Zhang Hailong on 13-3-25.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_image_h
#define hvm3d_hvm3d_image_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin.h"
#include "h3dcontext.h"
#include "h3dtexture.h"
#include "hvm3d.h"
    
    extern vmClassBase vmC3DImageClass;
    
    typedef struct _vmC3DImage{
        vmObject base;
        c3d_texture_t texture;
        struct {
            hint32 width;
            hint32 height;
            hint32 count;
            hfloat scale;
        } propertys;
        struct {
            vmUniqueKey width;
            vmUniqueKey height;
            vmUniqueKey count;
        } uniqueKeys;
    } vmC3DImage;
    
    extern c3d_texture_t vmC3DImageTexure(vmRuntimeContext vmContext, vmC3DContext * c3dContext,vmC3DImage * image,hint32 index,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
