//
//  h3dgraphics.h
//  hc3d
//
//  Created by zhang hailong on 12-12-28.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef hc3d_h3dgraphics_h
#define hc3d_h3dgraphics_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
    
#define C3D_FONT_NAME_MAX   32
    
    typedef struct _c3d_font_t{
        hchar family[C3D_FONT_NAME_MAX];
        hchar name[C3D_FONT_NAME_MAX];
        hfloat size;
    } c3d_font_t;
    
    typedef struct _c3d_pixel_t{
        hubyte r;
        hubyte g;
        hubyte b;
        hubyte a;
    } c3d_pixel_t;
    
    extern void c3d_graphics_begin(struct _c3d_context_t * context,hint32 width,hint32 height,InvokeTickDeclare);
    
    extern hany c3d_graphics_data(struct _c3d_context_t * context,InvokeTickDeclare);
    
    extern void c3d_graphics_end(struct _c3d_context_t * context,InvokeTickDeclare);
    
    void c3d_graphics_fill_rect(struct _c3d_context_t * context,hint32 x,hint32 y,hint32 width,hint32 height,c3d_color3d_t color,InvokeTickDeclare);
    
    extern c3d_vector2_t c3d_graphics_text_size(struct _c3d_context_t * context,hcchar * text,c3d_font_t font,InvokeTickDeclare);
    
    extern void c3d_graphics_draw_text(struct _c3d_context_t * context,c3d_vector2_t position, hcchar * text,c3d_color3d_t textColor,c3d_font_t font,InvokeTickDeclare);
    
    
#ifdef __cplusplus
}
#endif

#endif
