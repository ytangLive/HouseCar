//
//  h3dtexture.h
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef H3DTEXTURE_H
#define H3DTEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "h3d.h"
    
    typedef enum{
        TexturePixelFormatRGBA,TexturePixelFormatBGRA
    }TexturePixelFormat;
    
    c3d_texture_t c3d_texture_create(hany pixels ,TexturePixelFormat format, hint32 width,hint32 height);
    
    c3d_texture_t c3d_texture_bind(c3d_texture_t texture,hany pixels,TexturePixelFormat format,hint32 width,hint32 height);
    
    void c3d_texture_delete(c3d_texture_t texture);
    
    /**
     textureId 0~31
     */
    void c3d_texture_active(c3d_texture_t texture,huint32 textureId);
    
    
#ifdef __cplusplus
}
#endif


#endif
