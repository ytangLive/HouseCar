//
//  h3drender.h
//  hc3d
//
//  Created by zhang hailong on 12-11-9.
//
//

#ifndef H3DRENDER_H
#define H3DRENDER_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
#include "h3dmodel.h"
    
    typedef struct _C3DRenderVertexAttribs{
        c3d_location_t position;
        c3d_location_t normal;
        c3d_location_t color;
        c3d_location_t txtCoord;
        c3d_location_t texture;
        struct{
            c3d_location_t Ka;
            c3d_location_t Kd;
            c3d_location_t Ks;
            c3d_location_t Ns;
        } mtl;
        struct {
            c3d_location_t positions;
            c3d_location_t translates;
            c3d_location_t rotates;
            c3d_location_t links;
            c3d_location_t hasBone;
        } bone;
    }C3DRenderVertexAttribs;
    
    typedef enum{
        C3DRenderModePoints = 0x0000
        ,C3DRenderModeLines = 0x0001
        ,C3DRenderModeLinePool = 0x0002
        ,C3DRenderModeLineStrip = 0x0003
        ,C3DRenderModeTriangles = 0x0004
        ,C3DRenderModeTriangleStrip = 0x0005
        ,C3DRenderModeTriangleFan = 0x0006
    }C3DRenderMode;
    
    void c3d_render(c3d_model_t * model,C3DRenderVertexAttribs attribs,hint32 mask);
    
    void c3d_render_group_animation(struct _c3d_context_t * context,c3d_model_t * fromModel,hcchar * fromGroup,C3DRenderVertexAttribs fromAttribs
                                    ,c3d_model_t * toModel,hcchar * toGroup, C3DRenderVertexAttribs toAttribs,float value,c3d_location_t valueUniform);
    
    void c3d_render_animation(c3d_model_t * fromModel,C3DRenderVertexAttribs fromAttribs,c3d_model_t * toModel, C3DRenderVertexAttribs toAttribs,float value,c3d_location_t valueUniform);
    
    void c3d_render_group(struct _c3d_context_t * context, c3d_model_t * model,C3DRenderVertexAttribs attribs,hcchar * group);
    
    void c3d_render_groups(struct _c3d_context_t * context, c3d_model_t * model,C3DRenderVertexAttribs attribs);
    
    void c3d_render_arrays(C3DRenderMode mode,hint32 offet,hint32 count);
    
    typedef unsigned int c3d_vertex_buffer_t;
    
    c3d_vertex_buffer_t c3d_vertex_buffer_create(hint32 size,hany data);
    
    void c3d_vertex_buffer_delete(c3d_vertex_buffer_t buffer);
    
    void c3d_vertex_buffer_bind(c3d_vertex_buffer_t buffer);
    
#ifdef __cplusplus
}
#endif


#endif
