//
//  h3dbone.h
//  hc3d
//
//  Created by zhang hailong on 12-12-3.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef H3DBONE_H
#define H3DBONE_H

// 骨骼

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
    
    struct _serial_list_t;
    
    typedef struct _c3d_bone_t{
        c3d_matrix3_t transform;
        float distance;             // 距父级距离
        float muscle;               // 肌肉位置，0 ~ 1 默认 0.5
        float elbow;                // 肘距 0～1 相对 distance 
        float size;                 // 影响大小， 顶点到骨骼距离最大值
        float minAffect;            // 影响范围最小 相对 distance, 默认 0
        float maxAffect;            // 影响范围最大 相对 distance, 默认 1
        struct{
            c3d_vector3_t minVector;    // 最小顶点
            c3d_vector3_t maxVector;    // 最大顶点
        } range;
        int parentIndex;
    } c3d_bone_t;
    
    typedef struct _c3d_bone_offset_t{
        c3d_matrix3_t transform;
        float distance;             // 距父级距离增量
        float muscle;               // 肌肉位置增量
        float muscleSwell;          // 肌肉膨胀增量
    } c3d_bone_offset_t;
    
    c3d_bone_offset_t c3d_bone_offset_identity();
    
    typedef struct _c3d_bone_context_t{
        struct _serial_list_t * bones;
        struct _data_t * bonesMap;
        struct _serial_list_t * positions;
        struct _serial_list_t * positions_anim;
        struct _serial_list_t * transforms;
        struct _serial_list_t * transforms_anim;
        struct _serial_list_t * distances;
        struct _serial_list_t * muscles;
        struct _serial_list_t * muscleSwells;
    } c3d_bone_context_t;
    
    typedef struct _c3d_bone_anim_frame_t{
        struct _serial_map_t * offsets;
        c3d_vector3_t translate;
        double duration;
    } c3d_bone_anim_frame_t;
    
    typedef struct _c3d_bone_anim_t{
        struct {
            struct _c3d_bone_anim_t * anim;
            double duration;
        } parent;
        struct _serial_list_t * frames;
        double duration;
    } c3d_bone_anim_t;
    
    void c3d_bone_context_cleanup(c3d_bone_context_t * context,InvokeTickDeclare);
    
    void c3d_bone_context_load(c3d_bone_context_t * context,hcchar * content,InvokeTickDeclare);
    
    void c3d_bone_context_load_file(c3d_bone_context_t * context,hcchar * filePath,InvokeTickDeclare);
    
    int c3d_bone_context_index_for(c3d_bone_context_t * context,hcchar * name,InvokeTickDeclare);
    
    void c3d_bone_context_positions(c3d_bone_context_t * context,c3d_vector3_t position,InvokeTickDeclare);

    void c3d_bone_context_anim(c3d_bone_context_t * context,c3d_bone_anim_t * anim,double current,InvokeTickDeclare);

    c3d_bone_offset_t c3d_bone_anim_offset(c3d_bone_anim_t * anim,double current,int index,InvokeTickDeclare);
   
    c3d_vector3_t c3d_bone_anim_translate(c3d_bone_anim_t * anim,double current,InvokeTickDeclare);
    
    void c3d_bone_anim_cleanup(c3d_bone_anim_t * anim,InvokeTickDeclare);
    
    void c3d_bone_anim_load(c3d_bone_context_t * context,c3d_bone_anim_t * anim,hcchar * content,InvokeTickDeclare);
    
    void c3d_bone_anim_load_file(c3d_bone_context_t * context,c3d_bone_anim_t * anim,hcchar * filePath,InvokeTickDeclare);
    
    c3d_bone_offset_t c3d_bone_offset_anim(c3d_bone_offset_t from,c3d_bone_offset_t to,double value,InvokeTickDeclare);
    
    
    void c3d_bone_anim_frame_add_offset(c3d_bone_anim_frame_t * frame,hint32,c3d_bone_offset_t * offset,InvokeTickDeclare);
                                
    
#ifdef __cplusplus
}
#endif


#endif
