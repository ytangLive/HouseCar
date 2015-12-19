//
//  h3drender.c
//  hc3d
//
//  Created by zhang hailong on 12-11-9.
//
//

#include "hconfig.h"
#include "h3drender.h"
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
#include "hdata.h"
#include "hdata_alloc.h"
#include "h3dcontext.h"
#include "h3dtexture.h"
#include "h3dprogram.h"


static hint32 c3d_render_bind(c3d_model_t * model,C3DRenderVertexAttribs attribs,hint32 mask){
    hint32 size = 0;
    hint32 offset = 0;
    
    if(model->mask & C3D_MODEL_MASK_VERTEXT){
        size += sizeof(c3d_model_vertex_t);
    }
    
    if(model->mask & C3D_MODEL_MASK_NORMAL){
        size += sizeof(c3d_model_normal_t);
    }
    
    if(model->mask & C3D_MODEL_MASK_COLOR){
        size += sizeof(c3d_model_color_t);
    }
    
    if(model->mask & C3D_MODEL_MASK_TXT_COORD){
        size += sizeof(c3d_model_txtcoord_t);
    }
    
    if((model->mask & C3D_MODEL_MASK_VERTEXT)){
        if((mask & C3D_MODEL_MASK_VERTEXT)){
            glVertexAttribPointer(attribs.position, sizeof(c3d_model_vertex_t) / sizeof(float), GL_FLOAT, GL_FALSE, size, (hubyte *)model->data + offset);
            glEnableVertexAttribArray(attribs.position);
        }
        offset += sizeof(c3d_model_vertex_t);
    }
    
    if((model->mask & C3D_MODEL_MASK_NORMAL)){
        if((mask & C3D_MODEL_MASK_NORMAL)){
            glVertexAttribPointer(attribs.normal, sizeof(c3d_model_normal_t) / sizeof(float), GL_FLOAT, GL_TRUE, size, (hubyte *)model->data + offset);
            glEnableVertexAttribArray(attribs.normal);
        }
        offset += sizeof(c3d_model_normal_t);
    }
    
    if((model->mask & C3D_MODEL_MASK_COLOR)){
        if((mask & C3D_MODEL_MASK_COLOR)){
            glVertexAttribPointer(attribs.color, sizeof(c3d_model_color_t) / sizeof(float), GL_UNSIGNED_BYTE, GL_FALSE, size, (hubyte *)model->data + offset);
            glEnableVertexAttribArray(attribs.color);
        }
        offset += sizeof(c3d_model_color_t);
    }
    
    
    if((model->mask & C3D_MODEL_MASK_TXT_COORD)){
        if((mask & C3D_MODEL_MASK_TXT_COORD)){
            glVertexAttribPointer(attribs.txtCoord, sizeof(c3d_model_txtcoord_t) / sizeof(float), GL_FLOAT, GL_FALSE, size, (hubyte *)model->data + offset);
            glEnableVertexAttribArray(attribs.txtCoord);
        }
        offset += sizeof(c3d_model_txtcoord_t);
    }
    
    return size;
}

void c3d_render(c3d_model_t * model,C3DRenderVertexAttribs attribs,hint32 mask){
    
    hint32 size = c3d_render_bind(model,attribs,mask);
    glDrawArrays(GL_TRIANGLES, 0, model->length / size );
}

static void _c3d_render_group(c3d_context_t * context,c3d_model_t * model,C3DRenderVertexAttribs attribs,c3d_model_group_t * pg){
    InvokeTickBegin
    hdata_t mtl;
    c3d_model_mtl_t * pmtl;
    c3d_texture_t texture;
    if(pg->mtl[0] != '\0'){
        mtl = hdata_object(&hdata_class, model->mtls, pg->mtl);
        if(mtl){
            pmtl = (c3d_model_mtl_t *) hdata_bytes(&hdata_class, mtl);
            if(pmtl->map_d.texture[0] != '0'){
                texture = c3d_context_texture(context, pmtl->map_d.texture, InvokeTickArg);
                if(texture.texture){
                    c3d_texture_active(texture, 0);
                    c3d_program_uniform_1i(attribs.texture, 0);
                }
            }
            c3d_program_uniform_4f(attribs.mtl.Ka, pmtl->Ka);
            c3d_program_uniform_4f(attribs.mtl.Kd, pmtl->Kd);
            c3d_program_uniform_4f(attribs.mtl.Ks, pmtl->Ks);
            c3d_program_uniform_1f(attribs.mtl.Ns, pmtl->Ns / 1000.0);
        }
    }

    c3d_render_bind(model,attribs,model->mask);
    
    glDrawArrays(GL_TRIANGLES, pg->range.offset, pg->range.length);

}

void c3d_render_group(c3d_context_t * context,c3d_model_t * model,C3DRenderVertexAttribs attribs,hcchar * group){
    
    InvokeTickBegin
    
    hdata_t g = hdata_object(&hdata_class, model->groups, group);
    c3d_model_group_t * pg;
    if(g){
    
        pg = (c3d_model_group_t *) hdata_bytes(&hdata_class, g);
        _c3d_render_group(context,model,attribs, pg);
    }
}

typedef struct {
    struct _c3d_context_t * context;
    c3d_model_t * model;
    C3DRenderVertexAttribs attribs;
} c3d_render_groups_echo_callback_param_t;


static void c3d_render_groups_echo_callback(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
    c3d_render_groups_echo_callback_param_t * p = (c3d_render_groups_echo_callback_param_t *) dest;
    c3d_model_group_t * pg = (c3d_model_group_t *) hdata_bytes(&hdata_class, value);
    _c3d_render_group(p->context,p->model,p->attribs, pg);
}

void c3d_render_groups(struct _c3d_context_t * context, c3d_model_t * model,C3DRenderVertexAttribs attribs){
    c3d_render_groups_echo_callback_param_t p = {context,model,attribs};
    InvokeTickBegin
    hdata_object_each(&hdata_class, model->groups, c3d_render_groups_echo_callback, &p);
}

void c3d_render_arrays(C3DRenderMode mode,hint32 offset,hint32 count){
    glDrawArrays(mode, offset, count);
}

void c3d_render_animation(c3d_model_t * fromModel,C3DRenderVertexAttribs fromAttribs,c3d_model_t * toModel, C3DRenderVertexAttribs toAttribs,float value,c3d_location_t valueUniform){
    float fromLength = fromModel->length / c3d_render_bind(fromModel,fromAttribs,fromModel->mask);
    float toLength = toModel->length / c3d_render_bind(toModel,toAttribs,toModel->mask);
    
    c3d_program_uniform_1f(valueUniform, value);

    glDrawArrays(GL_TRIANGLES, 0, fromLength > toLength ? toLength : fromLength);
}

void c3d_render_group_animation(struct _c3d_context_t * context,c3d_model_t * fromModel,hcchar * fromGroup,C3DRenderVertexAttribs fromAttribs
                                ,c3d_model_t * toModel,hcchar * toGroup, C3DRenderVertexAttribs toAttribs,float value,c3d_location_t valueUniform){
    
    InvokeTickBegin
    hdata_t g ;
    hdata_t mtl;
    c3d_model_group_t * pg;
    c3d_model_mtl_t * pmtl;
    c3d_texture_t texture;
    c3d_model_range_t range;
    
    g = hdata_object(&hdata_class, fromModel->groups, fromGroup);
    
    if(g){
        
        pg = (c3d_model_group_t *) hdata_bytes(&hdata_class, g);
        
        if(pg->mtl[0] != '\0'){
            mtl = hdata_object(&hdata_class, fromModel->mtls, pg->mtl);
            if(mtl){
                pmtl = (c3d_model_mtl_t *) hdata_bytes(&hdata_class, mtl);
                if(pmtl->map_d.texture[0] != '0'){
                    texture = c3d_context_texture(context, pmtl->map_d.texture, InvokeTickArg);
                    if(texture.texture){
                        c3d_texture_active(texture, 0);
                        c3d_program_uniform_1i(fromAttribs.texture, 0);
                    }
                }
            }
        }
        
        range = pg->range;

        g = hdata_object(&hdata_class, toModel->groups, fromGroup);
        
        if(g){
            
            pg = (c3d_model_group_t *) hdata_bytes(&hdata_class, g);
            
            if(pg->mtl[0] != '\0'){
                mtl = hdata_object(&hdata_class, toModel->mtls, pg->mtl);
                if(mtl){
                    pmtl = (c3d_model_mtl_t *) hdata_bytes(&hdata_class, mtl);
                    if(pmtl->map_d.texture[0] != '0'){
                        texture = c3d_context_texture(context, pmtl->map_d.texture, InvokeTickArg);
                        if(texture.texture){
                            c3d_texture_active(texture, 1);
                            c3d_program_uniform_1i(toAttribs.texture, 1);
                        }
                    }
                }
            }
            
            if(pg->range.offset > range.offset){
                range.offset = pg->range.offset;
            }
            if(pg->range.length < range.length){
                range.length = pg->range.length;
            }
            
            c3d_render_bind(fromModel,fromAttribs,fromModel->mask);
            c3d_render_bind(toModel,toAttribs,toModel->mask);
            
            c3d_program_uniform_1f(valueUniform, value);
            
            glDrawArrays(GL_TRIANGLES, range.offset, range.length);
            
        }

    }
    

}


c3d_vertex_buffer_t c3d_vertex_buffer_create(hint32 size,hany data){
    GLuint buf;
    
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    
    return buf;
}

void c3d_vertex_buffer_delete(c3d_vertex_buffer_t buffer){
    glDeleteBuffers(1, & buffer);
}

void c3d_vertex_buffer_bind(c3d_vertex_buffer_t buffer){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}
