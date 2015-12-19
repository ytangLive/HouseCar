//
//  h3dbuffer.h
//  hc3d
//
//  Created by Zhang Hailong on 13-3-31.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hc3d_h3dbuffer_h
#define hc3d_h3dbuffer_h


#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
#include "h3dtexture.h"
    
    
    c3d_buffer_t c3d_buffer_create(hint32 width,hint32 height);
    
    void c3d_buffer_bind(c3d_buffer_t buffer);

    void c3d_buffer_delete(c3d_buffer_t buffer);
    
    void c3d_buffer_active(c3d_buffer_t texture,huint32 textureId);
    
#ifdef __cplusplus
}
#endif


#endif
