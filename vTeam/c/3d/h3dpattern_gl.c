//
//  h3dpattern_gl.c
//  hc3d
//
//  Created by zhang hailong on 13-1-11.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dpattern.h"

#include "hserial_list.h"
#include "h3dtexture.h"
#include "h3dcontext.h"
#include "h3dprogram.h"

#ifdef __MACOS__

#ifdef __MACOSX__

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#elif defined __ANDROID__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

static void c3d_pattern_render_range(struct _c3d_context_t * context,C3DRenderMode mode,C3DPatternRenderAttribs attribs,c3d_pattern_light_t * light,c3d_pattern_t * pattern,c3d_pattern_element_range_t range, InvokeTickDeclare){
    
    c3d_pattern_material_t * pmtl;
    c3d_texture_t texture;
    
    c3d_pattern_element_t * element = (c3d_pattern_element_t *)pattern->elements->data;
    
    glEnableVertexAttribArray(attribs.position);
    glEnableVertexAttribArray(attribs.normal);
    glEnableVertexAttribArray(attribs.txtCoord);
    
    glVertexAttribPointer(attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(c3d_pattern_element_t), &element->vertex);
    
    glVertexAttribPointer(attribs.normal, 3, GL_FLOAT, GL_TRUE, sizeof(c3d_pattern_element_t), &element->normal);
    
    glVertexAttribPointer(attribs.txtCoord, 2, GL_FLOAT, GL_FALSE, sizeof(c3d_pattern_element_t), &element->txtcoord) ;
    
    if(light){
        glUniform4fv(attribs.light, sizeof(c3d_pattern_light_t), (GLfloat *)&light->location);
    }
    
    if(range.material){
        pmtl = (c3d_pattern_material_t *) serial_list_item_at(pattern->materials, range.material - 1);
        if(pmtl){
            huint32 textureId = 0;
            c3d_program_uniform_1i(attribs.mtl.is_map_d, 0);
            c3d_program_uniform_1i(attribs.mtl.is_map_Kd, 0);
            c3d_program_uniform_1i(attribs.mtl.is_map_Ks, 0);
            c3d_program_uniform_1i(attribs.mtl.is_map_Ka, 0);
            
            if(pmtl->map_d.texture[0] != '\0'){
                texture = c3d_context_texture(context, pmtl->map_d.texture, InvokeTickArg);
                if(texture.texture){
                    c3d_texture_active(texture, textureId);
                    c3d_program_uniform_1i(attribs.mtl.map_d, textureId);
                    c3d_program_uniform_1i(attribs.mtl.is_map_d, 1);
                    textureId ++;
                }
            }
            if(pmtl->map_Kd.texture[0] != '\0'){
                texture = c3d_context_texture(context, pmtl->map_Kd.texture, InvokeTickArg);
                if(texture.texture){
                    c3d_texture_active(texture, textureId);
                    c3d_program_uniform_1i(attribs.mtl.map_Kd, textureId);
                    c3d_program_uniform_1i(attribs.mtl.is_map_Kd, 1);
                    textureId ++;
                }
            }
            if(pmtl->map_Ka.texture[0] != '\0'){
                texture = c3d_context_texture(context, pmtl->map_Ka.texture, InvokeTickArg);
                if(texture.texture){
                    c3d_texture_active(texture, textureId);
                    c3d_program_uniform_1i(attribs.mtl.map_Ka, textureId);
                    c3d_program_uniform_1i(attribs.mtl.is_map_Ka, 1);
                    textureId ++;
                }
            }
            if(pmtl->map_Ks.texture[0] != '\0'){
                texture = c3d_context_texture(context, pmtl->map_Ks.texture, InvokeTickArg);
                if(texture.texture){
                    c3d_texture_active(texture, textureId);
                    c3d_program_uniform_1i(attribs.mtl.map_Ks, textureId);
                    c3d_program_uniform_1i(attribs.mtl.is_map_Ks, 1);
                    textureId ++;
                }
            }
            c3d_program_uniform_4f(attribs.mtl.Tf, pmtl->Tf);
            c3d_program_uniform_4f(attribs.mtl.Ka, pmtl->Ka);
            c3d_program_uniform_4f(attribs.mtl.Kd, pmtl->Kd);
            c3d_program_uniform_4f(attribs.mtl.Ks, pmtl->Ks);
            c3d_program_uniform_1f(attribs.mtl.Ns, pmtl->Ns / 1000.0);
            c3d_program_uniform_1f(attribs.mtl.d, pmtl->d);
        }
    }

    glDrawArrays(mode, range.location, range.length);
    
}

void c3d_pattern_render(struct _c3d_context_t * context,C3DRenderMode mode,C3DPatternRenderAttribs attribs,c3d_pattern_light_t * light,c3d_pattern_t * pattern,InvokeTickDeclare){
    
    hint32 i,c = serial_list_count(pattern->element_ranges);
    c3d_pattern_element_range_t * range;
    
    for(i=0;i<c;i++){
        range = (c3d_pattern_element_range_t *) serial_list_item_at(pattern->element_ranges, i);
        c3d_pattern_render_range(context,mode,attribs,light,pattern,* range,InvokeTickArg);
    }
    
}

void c3d_pattern_shadow_render(struct _c3d_context_t * context, C3DRenderMode mode,C3DPatternShadowRenderAttribs attribs,c3d_pattern_t * pattern,InvokeTickDeclare){
    
    c3d_pattern_element_t * element = (c3d_pattern_element_t *)pattern->elements->data;
    
    glEnableVertexAttribArray(attribs.position);
    glEnableVertexAttribArray(attribs.normal);
    
    glVertexAttribPointer(attribs.position, 3, GL_FLOAT, GL_FALSE, sizeof(c3d_pattern_element_t), &element->vertex);
    
    glVertexAttribPointer(attribs.normal, 3, GL_FLOAT, GL_TRUE, sizeof(c3d_pattern_element_t), &element->normal);
    
    glDrawArrays(mode, 0, pattern->elements->item_length);

}

