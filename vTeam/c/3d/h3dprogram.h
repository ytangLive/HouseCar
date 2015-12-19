//
//  h3dprogram.h
//  hc3d
//
//  Created by hailong zhang on 12-11-25.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef H3DPROGRAM_H
#define H3DPROGRAM_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"

    
    c3d_program_t c3d_program_create(hcchar * vshSource,hcchar * fshSource);
    
    void c3d_program_delete(c3d_program_t program);
    
    void c3d_program_use(c3d_program_t program);
    
    c3d_location_t c3d_program_uniform_location(c3d_program_t program,hcchar * name);
    
    c3d_location_t c3d_program_attrib_location(c3d_program_t program,hcchar * name);
    
    void c3d_program_vertxt_attrib(c3d_location_t attrib,hint32 size,hany data,hint32 stride);
    
    void c3d_program_uniform_1f(c3d_location_t uniform,float value);
    
    void c3d_program_uniform_1fv(c3d_location_t uniform,float * value,hint32 count);
    
    void c3d_program_uniform_2f(c3d_location_t uniform,c3d_vector2_t value);
    
    void c3d_program_uniform_3f(c3d_location_t uniform,c3d_vector3_t value);
    
    void c3d_program_uniform_3fv(c3d_location_t uniform,c3d_vector3_t * value ,hint32 count);
    
    void c3d_program_uniform_4f(c3d_location_t uniform,c3d_vector4_t value);
    
    void c3d_program_uniform_1i(c3d_location_t uniform,int value);
    
    void c3d_program_uniform_1iv(c3d_location_t uniform,int * value,hint32 count);
    
    void c3d_program_uniform_4m(c3d_location_t uniform,c3d_matrix4_t value);

    void c3d_program_uniform_3m(c3d_location_t uniform,c3d_matrix3_t value);

    

#ifdef __cplusplus
}
#endif



#endif
