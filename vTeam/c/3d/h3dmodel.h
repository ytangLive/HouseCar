//
//  h3dmodel.h
//  hc3d
//
//  Created by Zhang Hailong on 12-11-10.
//
//

#ifndef H3DMODEL_H
#define H3DMODEL_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
#include "h3dbone.h"
    
#define C3D_MODEL_MASK_VERTEXT       0x0001
#define C3D_MODEL_MASK_NORMAL        0x0002
#define C3D_MODEL_MASK_COLOR         0x0004
#define C3D_MODEL_MASK_TXT_COORD     0x0008
    
    typedef c3d_vector3_t c3d_model_vertex_t;
    typedef c3d_vector3_t c3d_model_normal_t;
    typedef c3d_vector3_t c3d_model_txtcoord_t;
    typedef c3d_vector4_t c3d_model_color_t;
    
    c3d_model_t c3d_model_load_obj(struct _c3d_context_t * context, hcchar * objFile,hcchar * mtlFile,float scale,c3d_bone_context_t * boneContext);
    
    void c3d_model_delete(c3d_model_t model);
    
#ifdef __cplusplus
}
#endif


#endif
