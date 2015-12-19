//
//  h3dpattern.h
//  hc3d
//
//  Created by zhang hailong on 13-1-10.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hc3d_h3dpattern_h
#define hc3d_h3dpattern_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
#include "h3dbone.h"
#include "h3drender.h"
    
    struct _serial_list_t ;
    struct _serial_map_t;
    
    typedef struct _c3d_pattern_material_t{
        struct {
            char texture[C3D_NAME_MAX];
        } map_d;
        struct {
            char texture[C3D_NAME_MAX];
        } map_Kd;
        struct {
            char texture[C3D_NAME_MAX];
        } map_Ka;
        struct {
            char texture[C3D_NAME_MAX];
        } map_Ks;
        c3d_vector4_t Ka;
        c3d_vector4_t Kd;
        c3d_vector4_t Ks;
        c3d_vector4_t Tf;
        float d;
        float Ns;
    } c3d_pattern_material_t;
    
    typedef struct _c3d_pattern_bone_t{
        unsigned int link;
        float weight;
    } c3d_pattern_bone_t;
    
    typedef struct _c3d_pattern_element_t{
        c3d_vector3_t vertex;
        c3d_vector3_t normal;
        c3d_vector2_t txtcoord;
        unsigned int vIndex;
        unsigned int vnIndex;
    } c3d_pattern_element_t;
    
    typedef struct _c3d_pattern_element_range_t{
        unsigned int material;
        unsigned int location;
        unsigned int length;
    } c3d_pattern_element_range_t;
    
    
    typedef struct _c3d_pattern_t{
        
        struct _serial_list_t * elements;
        
        struct _serial_list_t * materials;
        
        struct _serial_list_t * element_ranges;
        
        struct _serial_list_t * bones;
        
        struct _serial_list_t * vertexs;
        
        struct _serial_list_t * normals;
        
        c3d_vector3_t minVertex;
        c3d_vector3_t maxVertex;
        
        hfloat muscleScale;
        
    } c3d_pattern_t;
    
    c3d_pattern_t c3d_pattern_load(const char * objSource,const char * mtlSource,float scale,c3d_vector3_t offset, InvokeTickDeclare);
    
    c3d_pattern_t c3d_pattern_load_file(const char * objFilePath,const char * mtlFilePath,float scale,c3d_vector3_t offset,InvokeTickDeclare);
    
    void c3d_pattern_bone_bind(c3d_pattern_t * pattern,const char * boneSource, c3d_bone_context_t * boneContext,InvokeTickDeclare);
    
    void c3d_pattern_bone_bind_file(c3d_pattern_t * pattern,const char * boneFilePath, c3d_bone_context_t * boneContext,InvokeTickDeclare);
    
    void c3d_pattern_bone_bind_auto(c3d_pattern_t * pattern, c3d_bone_context_t * boneContext,InvokeTickDeclare);
    
    void c3d_pattern_bond_apply(c3d_pattern_t * pattern,c3d_bone_context_t * boneConetxt,InvokeTickDeclare);
    
    void c3d_pattern_delete(c3d_pattern_t * pattern,InvokeTickDeclare);
    
    typedef struct _C3DPatternRenderAttribs{
        c3d_location_t position;
        c3d_location_t normal;
        c3d_location_t txtCoord;
        struct{
            c3d_location_t Ka;
            c3d_location_t Kd;
            c3d_location_t Ks;
            c3d_location_t Ns;
            c3d_location_t Tf;
            c3d_location_t d;
            c3d_location_t map_d;
            c3d_location_t map_Ka;
            c3d_location_t map_Kd;
            c3d_location_t map_Ks;
            c3d_location_t is_map_d;
            c3d_location_t is_map_Ka;
            c3d_location_t is_map_Kd;
            c3d_location_t is_map_Ks;
        } mtl;
        c3d_location_t projectTransform;
        c3d_location_t transform;
        c3d_location_t normalTransform;
        c3d_location_t light;
        c3d_location_t alpha;
    } C3DPatternRenderAttribs;
    
#define C3D_MAX_PATTERN_LIGHT_SIZE  4
    
    typedef struct _c3d_light_t{
        c3d_vector4_t location[C3D_MAX_PATTERN_LIGHT_SIZE];
    } c3d_pattern_light_t;
    
    void c3d_pattern_light_load(c3d_pattern_light_t * light,const char * lightSource,InvokeTickDeclare);
    
    void c3d_pattern_light_load_file(c3d_pattern_light_t * light,const char * lightFile,InvokeTickDeclare);
    
    void c3d_pattern_render(struct _c3d_context_t * context, C3DRenderMode mode,C3DPatternRenderAttribs attribs,c3d_pattern_light_t * light,c3d_pattern_t * pattern,InvokeTickDeclare);
    
    typedef struct _C3DPatternShadowRenderAttribs{
        c3d_location_t position;
        c3d_location_t normal;
        c3d_location_t projectTransform;
        c3d_location_t transform;
        c3d_location_t normalTransform;
        c3d_location_t shadowColor;
        c3d_location_t shadowVector;
        c3d_location_t shadowPosition;
    } C3DPatternShadowRenderAttribs;
    
    void c3d_pattern_shadow_render(struct _c3d_context_t * context, C3DRenderMode mode,C3DPatternShadowRenderAttribs attribs,c3d_pattern_t * pattern,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
