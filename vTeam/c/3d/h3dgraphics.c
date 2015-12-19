//
//  h3dgraphics.c
//  hc3d
//
//  Created by zhang hailong on 12-12-28.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dgraphics.h"
#include "h3dcontext.h"

#include "hbuffer.h"
#include "hserial_list.h"


void c3d_graphics_fill_rect(struct _c3d_context_t * context,hint32 x,hint32 y,hint32 width,hint32 height,c3d_color3d_t color,InvokeTickDeclare){
    c3d_pixel_t * pixel = (c3d_pixel_t * )c3d_graphics_data(context,InvokeTickArg) ,*p;
    c3d_pixel_t px = {color.r * 0xff,color.g * 0xff,color.b * 0xff,color.a * 0xff};
    hint32 r,c;

    if(x <0){
        width = width + x;
        x = 0;
    }
    
    if(y <0){
        height = height + y;
        y = 0;
    }
    
    if(width > context->graphics.width){
        width = context->graphics.width;
    }
    if(height > context->graphics.height){
        height = context->graphics.height;
    }
    
    x = x / context->scale;
    y = y / context->scale;
    width = width / context->scale;
    height = height / context->scale;
    
    for(r = 0;r< height;r++){
        for(c = 0;c<width;c++){
            p = pixel + (r + x) * width + c +y;
            * p = px;
        }
    }
}
