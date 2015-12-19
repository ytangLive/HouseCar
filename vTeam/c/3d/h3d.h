//
//  hc3d.h
//  hc3d
//
//  Created by zhang hailong on 12-11-9.
//
//

#ifndef H3D_H
#define H3D_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define C3D_TOUCHES_MAX 5
#define C3D_NAME_MAX    32
    
    typedef union _c3d_matrix2_t{
        struct
        {
            float m00, m01;
            float m10, m11;
        };
        float m[4];
    }c3d_matrix2_t;
    
    typedef union _c3d_matrix3_t{
        struct
        {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
        float m[9];
    }c3d_matrix3_t;
    
    extern const c3d_matrix3_t c3d_matrix3_identity;
    
    typedef union _c3d_matrix4_t
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        float m[16];
    } c3d_matrix4_t;

    typedef union _c3d_vector2_t
    {
        struct { float x, y; };
        struct { float s, t; };
        struct { float width, height; };
        float v[2];
    }c3d_vector2_t;
    
    typedef union _c3d_vector3_t
    {
        struct { float x, y, z; };
        struct { float r, g, b; };
        struct { float s, t, p; };
        float v[3];
    }c3d_vector3_t;
    
    typedef union _c3d_vector4_t
    {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        struct { float s, t, p, q; };
        struct {
            c3d_vector2_t origin;
            c3d_vector2_t size;
        };
        float v[4];
    }c3d_vector4_t ;
   
    typedef c3d_vector4_t c3d_color3d_t;
    
    struct _c3d_context_t;
    struct _c3d_task_t;

    
    typedef unsigned int c3d_program_t;
    typedef int c3d_location_t;
    struct _c3d_model_t;
    
    typedef void (*c3d_model_dealloc_fun_t)(struct _c3d_model_t * model);
    

    typedef struct _c3d_model_range_t{
        int offset;
        int length;
    }c3d_model_range_t;
    
    typedef struct _c3d_model_mtl_t{
        struct {
            c3d_vector3_t position;
            c3d_vector3_t size;
            char texture[C3D_NAME_MAX];
        } map_d;
        c3d_vector4_t Ka;
        c3d_vector4_t Kd;
        c3d_vector4_t Ks;
        float Ns;
    } c3d_model_mtl_t;
    
    typedef struct _c3d_model_group_t{
        char mtl[C3D_NAME_MAX];
        c3d_model_range_t range;
    }c3d_model_group_t;
    
    typedef struct _c3d_model_t{
        char * data;
        int length;
        int mask;
        c3d_vector3_t minVertex;
        c3d_vector3_t maxVertex;
        c3d_model_dealloc_fun_t dealloc;
        struct _data_t * groups;
        struct _data_t * mtls;
    }c3d_model_t;
    
    
    typedef struct _c3d_texture_t{
        unsigned int texture;
        int width;
        int height;
        float delay;
        int next;
    } c3d_texture_t;
    
    typedef struct _c3d_buffer_t{
        unsigned int frame;
        unsigned int texture;
        int width;
        int height;
    } c3d_buffer_t;
    
    typedef enum _C3DTouchState{
        C3DTouchStateNone,C3DTouchStateBegin,C3DTouchStateMove,C3DTouchStateCanceled,C3DTouchStateEnd
    }C3DTouchState;
    
    typedef struct _c3d_touch_t{
        long touchId;
        c3d_vector3_t coord;
    } c3d_touch_t;

    typedef struct _c3d_touches_t{
        c3d_touch_t touches[C3D_TOUCHES_MAX];
        C3DTouchState state;
        double timestamp;
        int length;
    }c3d_touches_t;
    
    extern const c3d_matrix4_t c3d_matrix4_identity;
    
    static __inline__ float c3d_vector3_length(c3d_vector3_t vector);
    static __inline__ float c3d_vector3_length2(c3d_vector3_t vector);
    static __inline__ float c3d_vector4_length(c3d_vector4_t vector);
    static __inline__ float c3d_vector4_length2(c3d_vector4_t vector);
    static __inline__ c3d_vector3_t c3d_vector3_normalize(c3d_vector3_t vector);
    static __inline__ c3d_vector3_t c3d_vector3_make(float x, float y, float z);
    
    extern c3d_matrix3_t cid_matrix3_invert_transpose(c3d_matrix3_t matrix);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_transpose(c3d_matrix3_t matrix);
    
    static __inline__ c3d_matrix3_t c3d_matrix4_to_matrix3(c3d_matrix4_t matrix4);
    
    static __inline__ c3d_vector3_t c3d_vector3_multiply_scalar(c3d_vector3_t vector, float value);
    static __inline__ c3d_vector3_t c3d_vector3_add(c3d_vector3_t vectorLeft, c3d_vector3_t vectorRight);
    static __inline__ c3d_vector3_t c3d_vector3_sub(c3d_vector3_t vectorLeft, c3d_vector3_t vectorRight);
    
    static __inline__ c3d_vector3_t c3d_vector3_anim(c3d_vector3_t from,c3d_vector3_t to,float value);
    
    static __inline__ c3d_vector3_t c3d_vector3_anim_offset(c3d_vector3_t from,c3d_vector3_t to,float value);
    
    static __inline__ c3d_vector3_t c3d_vector3_project(c3d_vector3_t a, c3d_vector3_t b,c3d_vector3_t c);
    
    static __inline__ float c3d_vector3_project_scalar(c3d_vector3_t a, c3d_vector3_t b,c3d_vector3_t c);
    
    static __inline__ float c3d_vector3_dot(c3d_vector3_t a, c3d_vector3_t b);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply_anim(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight,float value);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply_scalar(c3d_matrix3_t matrixLeft, float value);

    static __inline__ c3d_matrix3_t c3d_matrix3_add(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight);
    static __inline__ c3d_matrix3_t c3d_matrix3_sub(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_project(c3d_vector3_t v3);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_scale(float sx, float sy, float sz);
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation(float radians, float x, float y, float z);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_x(float radians);
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_y(float radians);
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_z(float radians);
    
    static __inline__ c3d_matrix3_t c3d_matrix3_scale(c3d_matrix3_t matrix, float sx, float sy, float sz);
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate(c3d_matrix3_t matrix, float radians, float x, float y, float z);
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_x(c3d_matrix3_t matrix, float radians);
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_y(c3d_matrix3_t matrix, float radians);
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_z(c3d_matrix3_t matrix, float radians);
    
    
    static __inline__ c3d_matrix4_t c3d_matrix4_multiply(c3d_matrix4_t matrixLeft, c3d_matrix4_t matrixRight);
    
    static __inline__ c3d_vector3_t c3d_matrix4_multiply_vector3(c3d_matrix4_t matrixLeft, c3d_vector3_t vectorRight,float v3);
    
    static __inline__ c3d_vector4_t c3d_matrix4_multiply_vector4(c3d_matrix4_t matrixLeft, c3d_vector4_t vectorRight);

    static __inline__ c3d_vector3_t c3d_matrix4_multiply_vector3_project(c3d_matrix4_t matrixLeft, c3d_vector3_t vectorRight);
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_project(c3d_vector3_t v3);
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_translation(float tx, float ty, float tz);
    static __inline__ c3d_matrix4_t c3d_matrix4_make_scale(float sx, float sy, float sz);
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation(float radians, float x, float y, float z);
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_x(float radians);
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_y(float radians);
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_z(float radians);
    
    static __inline__ c3d_matrix4_t c3d_matrix4_translate(c3d_matrix4_t matrix, float tx, float ty, float tz);
    static __inline__ c3d_matrix4_t c3d_matrix4_scale(c3d_matrix4_t matrix, float sx, float sy, float sz);
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate(c3d_matrix4_t matrix, float radians, float x, float y, float z);
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_x(c3d_matrix4_t matrix, float radians);
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_y(c3d_matrix4_t matrix, float radians);
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_z(c3d_matrix4_t matrix, float radians);

    static __inline__ c3d_matrix4_t c3d_matrix4_multiply_scalar(c3d_matrix4_t matrix4 ,float value);
    static __inline__ c3d_matrix4_t c3d_matrix4_add(c3d_matrix4_t matrixLeft ,c3d_matrix4_t matrixRight);
    static __inline__ c3d_matrix4_t c3d_matrix4_sub(c3d_matrix4_t matrixLeft ,c3d_matrix4_t matrixRight);
    
    static __inline__ c3d_matrix3_t c3d_quat_to_matrix3(c3d_vector4_t quat) ;
    
    static __inline__ c3d_vector4_t c3d_matrix3_to_quat(c3d_matrix3_t matrix3) ;
    
    static __inline__ c3d_matrix4_t c3d_matrix3_to_matrix4(c3d_matrix3_t matrix);
    
    static __inline__ c3d_matrix4_t c3d_matrix4_multiply_anim(c3d_matrix4_t matrixLeft, c3d_matrix4_t matrixRight,float value);

    static __inline__ c3d_vector4_t c3d_quat_to_axis(c3d_vector4_t q);
    
    static __inline__ c3d_vector4_t c3d_axis_to_quat(c3d_vector4_t a);
    
    static __inline__ float c3d_vector3_length(c3d_vector3_t vector)
    {
        return sqrt(vector.v[0] * vector.v[0] + vector.v[1] * vector.v[1] + vector.v[2] * vector.v[2]);
    }
    
    static __inline__ float c3d_vector3_length2(c3d_vector3_t vector)
    {
        return vector.v[0] * vector.v[0] + vector.v[1] * vector.v[1] + vector.v[2] * vector.v[2];
    }
    
    static __inline__ float c3d_vector4_length(c3d_vector4_t vector)
    {
        return sqrt(vector.v[0] * vector.v[0] + vector.v[1] * vector.v[1] + vector.v[2] * vector.v[2] + vector.v[3] * vector.v[3]);
    }
    
    static __inline__ float c3d_vector4_length2(c3d_vector4_t vector)
    {
        return vector.v[0] * vector.v[0] + vector.v[1] * vector.v[1] + vector.v[2] * vector.v[2] + vector.v[3] * vector.v[3];
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_normalize(c3d_vector3_t vector)
    {
        float scale = 1.0f / c3d_vector3_length(vector);
        c3d_vector3_t v = { vector.v[0] * scale, vector.v[1] * scale, vector.v[2] * scale };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_make(float x, float y, float z)
    {
        c3d_vector3_t v = { x, y, z };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_multiply_scalar(c3d_vector3_t vector, float value)
    {
        c3d_vector3_t v = { vector.v[0] * value,
            vector.v[1] * value,
            vector.v[2] * value };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_add(c3d_vector3_t vectorLeft, c3d_vector3_t vectorRight){
        c3d_vector3_t v = { vectorLeft.v[0] + vectorRight.v[0],
            vectorLeft.v[1] + vectorRight.v[1],
            vectorLeft.v[2] + vectorRight.v[2] };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_sub(c3d_vector3_t vectorLeft, c3d_vector3_t vectorRight){
        c3d_vector3_t v = { vectorLeft.v[0] - vectorRight.v[0],
            vectorLeft.v[1] - vectorRight.v[1],
            vectorLeft.v[2] - vectorRight.v[2] };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_anim(c3d_vector3_t from,c3d_vector3_t to,float value){
        c3d_vector3_t v = { from.x + (to.x - from.x) * value,
            from.y + (to.y - from.y) * value,
            from.z + (to.z - from.z) * value };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_anim_offset(c3d_vector3_t from,c3d_vector3_t to,float value){
        c3d_vector3_t v = { (to.x - from.x) * value,
            (to.y - from.y) * value,
            (to.z - from.z) * value };
        return v;
    }
    
    static __inline__ float c3d_vector3_dot(c3d_vector3_t a, c3d_vector3_t b){
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    static __inline__ float c3d_vector3_project_scalar(c3d_vector3_t a, c3d_vector3_t b,c3d_vector3_t c){
        c3d_vector3_t ab = c3d_vector3_sub(b, a);
        c3d_vector3_t ac = c3d_vector3_sub(c, a);
        return c3d_vector3_dot(ab,ac) / c3d_vector3_dot(ab,ab);
    }
    
    static __inline__ c3d_vector3_t c3d_vector3_project(c3d_vector3_t a, c3d_vector3_t b,c3d_vector3_t c){
        c3d_vector3_t ab = c3d_vector3_sub(b, a);
        c3d_vector3_t ac = c3d_vector3_sub(c, a);
        c3d_vector3_t v = c3d_vector3_add(a, c3d_vector3_multiply_scalar(ab, c3d_vector3_dot(ab,ac) / c3d_vector3_dot(ab,ab)) );
        return v;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix4_to_matrix3(c3d_matrix4_t matrix4){
        c3d_matrix3_t m3 = {
            matrix4.m00,matrix4.m01,matrix4.m02,
            matrix4.m10,matrix4.m11,matrix4.m12,
            matrix4.m20,matrix4.m21,matrix4.m22,
        };
        
        return m3;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_multiply(c3d_matrix4_t matrixLeft, c3d_matrix4_t matrixRight)
    {
        c3d_matrix4_t m;
        
        m.m[0]  = matrixLeft.m[0] * matrixRight.m[0]  + matrixLeft.m[4] * matrixRight.m[1]  + matrixLeft.m[8] * matrixRight.m[2]   + matrixLeft.m[12] * matrixRight.m[3];
        m.m[4]  = matrixLeft.m[0] * matrixRight.m[4]  + matrixLeft.m[4] * matrixRight.m[5]  + matrixLeft.m[8] * matrixRight.m[6]   + matrixLeft.m[12] * matrixRight.m[7];
        m.m[8]  = matrixLeft.m[0] * matrixRight.m[8]  + matrixLeft.m[4] * matrixRight.m[9]  + matrixLeft.m[8] * matrixRight.m[10]  + matrixLeft.m[12] * matrixRight.m[11];
        m.m[12] = matrixLeft.m[0] * matrixRight.m[12] + matrixLeft.m[4] * matrixRight.m[13] + matrixLeft.m[8] * matrixRight.m[14]  + matrixLeft.m[12] * matrixRight.m[15];
        
        m.m[1]  = matrixLeft.m[1] * matrixRight.m[0]  + matrixLeft.m[5] * matrixRight.m[1]  + matrixLeft.m[9] * matrixRight.m[2]   + matrixLeft.m[13] * matrixRight.m[3];
        m.m[5]  = matrixLeft.m[1] * matrixRight.m[4]  + matrixLeft.m[5] * matrixRight.m[5]  + matrixLeft.m[9] * matrixRight.m[6]   + matrixLeft.m[13] * matrixRight.m[7];
        m.m[9]  = matrixLeft.m[1] * matrixRight.m[8]  + matrixLeft.m[5] * matrixRight.m[9]  + matrixLeft.m[9] * matrixRight.m[10]  + matrixLeft.m[13] * matrixRight.m[11];
        m.m[13] = matrixLeft.m[1] * matrixRight.m[12] + matrixLeft.m[5] * matrixRight.m[13] + matrixLeft.m[9] * matrixRight.m[14]  + matrixLeft.m[13] * matrixRight.m[15];
        
        m.m[2]  = matrixLeft.m[2] * matrixRight.m[0]  + matrixLeft.m[6] * matrixRight.m[1]  + matrixLeft.m[10] * matrixRight.m[2]  + matrixLeft.m[14] * matrixRight.m[3];
        m.m[6]  = matrixLeft.m[2] * matrixRight.m[4]  + matrixLeft.m[6] * matrixRight.m[5]  + matrixLeft.m[10] * matrixRight.m[6]  + matrixLeft.m[14] * matrixRight.m[7];
        m.m[10] = matrixLeft.m[2] * matrixRight.m[8]  + matrixLeft.m[6] * matrixRight.m[9]  + matrixLeft.m[10] * matrixRight.m[10] + matrixLeft.m[14] * matrixRight.m[11];
        m.m[14] = matrixLeft.m[2] * matrixRight.m[12] + matrixLeft.m[6] * matrixRight.m[13] + matrixLeft.m[10] * matrixRight.m[14] + matrixLeft.m[14] * matrixRight.m[15];
        
        m.m[3]  = matrixLeft.m[3] * matrixRight.m[0]  + matrixLeft.m[7] * matrixRight.m[1]  + matrixLeft.m[11] * matrixRight.m[2]  + matrixLeft.m[15] * matrixRight.m[3];
        m.m[7]  = matrixLeft.m[3] * matrixRight.m[4]  + matrixLeft.m[7] * matrixRight.m[5]  + matrixLeft.m[11] * matrixRight.m[6]  + matrixLeft.m[15] * matrixRight.m[7];
        m.m[11] = matrixLeft.m[3] * matrixRight.m[8]  + matrixLeft.m[7] * matrixRight.m[9]  + matrixLeft.m[11] * matrixRight.m[10] + matrixLeft.m[15] * matrixRight.m[11];
        m.m[15] = matrixLeft.m[3] * matrixRight.m[12] + matrixLeft.m[7] * matrixRight.m[13] + matrixLeft.m[11] * matrixRight.m[14] + matrixLeft.m[15] * matrixRight.m[15];
        
        return m;
    }
    
    static __inline__ c3d_vector3_t c3d_matrix4_multiply_vector3(c3d_matrix4_t matrixLeft, c3d_vector3_t vectorRight ,float v3)
    {
        c3d_vector3_t v = { matrixLeft.m[0] * vectorRight.v[0] + matrixLeft.m[4] * vectorRight.v[1] + matrixLeft.m[8] * vectorRight.v[2] + matrixLeft.m[12] * v3,
            matrixLeft.m[1] * vectorRight.v[0] + matrixLeft.m[5] * vectorRight.v[1] + matrixLeft.m[9] * vectorRight.v[2] + matrixLeft.m[13] * v3,
            matrixLeft.m[2] * vectorRight.v[0] + matrixLeft.m[6] * vectorRight.v[1] + matrixLeft.m[10] * vectorRight.v[2] + matrixLeft.m[14] * v3 };
        return v;
    }
    
    static __inline__ c3d_vector4_t c3d_matrix4_multiply_vector4(c3d_matrix4_t matrixLeft, c3d_vector4_t vectorRight)
    {
        c3d_vector4_t v = { matrixLeft.m[0] * vectorRight.v[0] + matrixLeft.m[4] * vectorRight.v[1] + matrixLeft.m[8] * vectorRight.v[2] + matrixLeft.m[12] * vectorRight.v[3],
            matrixLeft.m[1] * vectorRight.v[0] + matrixLeft.m[5] * vectorRight.v[1] + matrixLeft.m[9] * vectorRight.v[2] + matrixLeft.m[13] * vectorRight.v[3],
            matrixLeft.m[2] * vectorRight.v[0] + matrixLeft.m[6] * vectorRight.v[1] + matrixLeft.m[10] * vectorRight.v[2] + matrixLeft.m[14] * vectorRight.v[3],
            matrixLeft.m[3] * vectorRight.v[0] + matrixLeft.m[7] * vectorRight.v[1] + matrixLeft.m[11] * vectorRight.v[2] + matrixLeft.m[15] * vectorRight.v[3] };
        return v;
    }
    
    static __inline__ c3d_vector3_t c3d_matrix4_multiply_vector3_project(c3d_matrix4_t matrixLeft, c3d_vector3_t vectorRight)
    {
        c3d_vector4_t v = {vectorRight.v[0],vectorRight.v[1],vectorRight.v[2],1.0f};
        c3d_vector4_t v4 = c3d_matrix4_multiply_vector4(matrixLeft, v);
        c3d_vector3_t v3 = {v4.v[0], v4.v[1], v4.v[2]};
        return c3d_vector3_multiply_scalar(v3, 1.0f / v4.v[3]);
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_translation(float tx, float ty, float tz)
    {
        c3d_matrix4_t m = c3d_matrix4_identity;
        m.m[12] = tx;
        m.m[13] = ty;
        m.m[14] = tz;
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_scale(float sx, float sy, float sz)
    {
        c3d_matrix4_t m = c3d_matrix4_identity;
        m.m[0] = sx;
        m.m[5] = sy;
        m.m[10] = sz;
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation(float radians, float x, float y, float z)
    {
        c3d_vector3_t v = c3d_vector3_normalize(c3d_vector3_make(x, y, z));
        float cos = cosf(radians);
        float cosp = 1.0f - cos;
        float sin = sinf(radians);
        
        c3d_matrix4_t m = { cos + cosp * v.v[0] * v.v[0],
            cosp * v.v[0] * v.v[1] + v.v[2] * sin,
            cosp * v.v[0] * v.v[2] - v.v[1] * sin,
            0.0f,
            cosp * v.v[0] * v.v[1] - v.v[2] * sin,
            cos + cosp * v.v[1] * v.v[1],
            cosp * v.v[1] * v.v[2] + v.v[0] * sin,
            0.0f,
            cosp * v.v[0] * v.v[2] + v.v[1] * sin,
            cosp * v.v[1] * v.v[2] - v.v[0] * sin,
            cos + cosp * v.v[2] * v.v[2],
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f };
        
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_x(float radians)
    {
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix4_t m = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos, sin, 0.0f,
            0.0f, -sin, cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
        
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_y(float radians)
    {
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix4_t m = { cos, 0.0f, -sin, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sin, 0.0f, cos, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
        
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_rotation_z(float radians)
    {
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix4_t m = { cos, sin, 0.0f, 0.0f,
            -sin, cos, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f };
        
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_translate(c3d_matrix4_t matrix, float tx, float ty, float tz)
    {
        c3d_matrix4_t m = { matrix.m[0], matrix.m[1], matrix.m[2], matrix.m[3],
            matrix.m[4], matrix.m[5], matrix.m[6], matrix.m[7],
            matrix.m[8], matrix.m[9], matrix.m[10], matrix.m[11],
            matrix.m[0] * tx + matrix.m[4] * ty + matrix.m[8] * tz + matrix.m[12],
            matrix.m[1] * tx + matrix.m[5] * ty + matrix.m[9] * tz + matrix.m[13],
            matrix.m[2] * tx + matrix.m[6] * ty + matrix.m[10] * tz + matrix.m[14],
            matrix.m[15] };
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_scale(c3d_matrix4_t matrix, float sx, float sy, float sz)
    {

        c3d_matrix4_t m = { matrix.m[0] * sx, matrix.m[1] * sx, matrix.m[2] * sx, matrix.m[3] * sx,
            matrix.m[4] * sy, matrix.m[5] * sy, matrix.m[6] * sy, matrix.m[7] * sy,
            matrix.m[8] * sz, matrix.m[9] * sz, matrix.m[10] * sz, matrix.m[11] * sz,
            matrix.m[12], matrix.m[13], matrix.m[14], matrix.m[15] };
        return m;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate(c3d_matrix4_t matrix, float radians, float x, float y, float z)
    {
        c3d_matrix4_t rm = c3d_matrix4_make_rotation(radians, x, y, z);
        return c3d_matrix4_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_x(c3d_matrix4_t matrix, float radians)
    {
        c3d_matrix4_t rm = c3d_matrix4_make_rotation_x(radians);
        return c3d_matrix4_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_y(c3d_matrix4_t matrix, float radians)
    {
        c3d_matrix4_t rm = c3d_matrix4_make_rotation_y(radians);
        return c3d_matrix4_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_rotate_z(c3d_matrix4_t matrix, float radians)
    {
        c3d_matrix4_t rm = c3d_matrix4_make_rotation_z(radians);
        return c3d_matrix4_multiply(matrix, rm);
    }

    static __inline__ c3d_matrix4_t c3d_matrix4_multiply_scalar(c3d_matrix4_t matrix4 ,float value){
        c3d_matrix4_t m4 = {
            matrix4.m00 * value,matrix4.m01 * value,matrix4.m02 * value,matrix4.m03 * value,
            matrix4.m10 * value,matrix4.m11 * value,matrix4.m12 * value,matrix4.m13 * value,
            matrix4.m20 * value,matrix4.m21 * value,matrix4.m22 * value,matrix4.m23 * value,
            matrix4.m30 * value,matrix4.m31 * value,matrix4.m32 * value,matrix4.m33 * value,
        };
        return m4;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_add(c3d_matrix4_t matrixLeft ,c3d_matrix4_t matrixRight){
        c3d_matrix4_t m4 = {
            matrixLeft.m00 + matrixRight.m00,matrixLeft.m01 + matrixRight.m01,matrixLeft.m02 + matrixRight.m02,matrixLeft.m03 + matrixRight.m03,
            matrixLeft.m10 + matrixRight.m10,matrixLeft.m11 + matrixRight.m11,matrixLeft.m12 + matrixRight.m12,matrixLeft.m13 + matrixRight.m13,
            matrixLeft.m20 + matrixRight.m20,matrixLeft.m21 + matrixRight.m21,matrixLeft.m22 + matrixRight.m22,matrixLeft.m23 + matrixRight.m23,
            matrixLeft.m30 + matrixRight.m30,matrixLeft.m31 + matrixRight.m31,matrixLeft.m32 + matrixRight.m32,matrixLeft.m33 + matrixRight.m33,
        };
        return m4;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_sub(c3d_matrix4_t matrixLeft ,c3d_matrix4_t matrixRight){
        c3d_matrix4_t m4 = {
            matrixLeft.m00 - matrixRight.m00,matrixLeft.m01 - matrixRight.m01,matrixLeft.m02 - matrixRight.m02,matrixLeft.m03 - matrixRight.m03,
            matrixLeft.m10 - matrixRight.m10,matrixLeft.m11 - matrixRight.m11,matrixLeft.m12 - matrixRight.m12,matrixLeft.m13 - matrixRight.m13,
            matrixLeft.m20 - matrixRight.m20,matrixLeft.m21 - matrixRight.m21,matrixLeft.m22 - matrixRight.m22,matrixLeft.m23 - matrixRight.m23,
            matrixLeft.m30 - matrixRight.m30,matrixLeft.m31 - matrixRight.m31,matrixLeft.m32 - matrixRight.m32,matrixLeft.m33 - matrixRight.m33,
        };
        return m4;
    }
    
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight){
        c3d_matrix3_t m;
        
        m.m[0] = matrixLeft.m[0] * matrixRight.m[0] + matrixLeft.m[3] * matrixRight.m[1] + matrixLeft.m[6] * matrixRight.m[2];
        m.m[3] = matrixLeft.m[0] * matrixRight.m[3] + matrixLeft.m[3] * matrixRight.m[4] + matrixLeft.m[6] * matrixRight.m[5];
        m.m[6] = matrixLeft.m[0] * matrixRight.m[6] + matrixLeft.m[3] * matrixRight.m[7] + matrixLeft.m[6] * matrixRight.m[8];
        
        m.m[1] = matrixLeft.m[1] * matrixRight.m[0] + matrixLeft.m[4] * matrixRight.m[1] + matrixLeft.m[7] * matrixRight.m[2];
        m.m[4] = matrixLeft.m[1] * matrixRight.m[3] + matrixLeft.m[4] * matrixRight.m[4] + matrixLeft.m[7] * matrixRight.m[5];
        m.m[7] = matrixLeft.m[1] * matrixRight.m[6] + matrixLeft.m[4] * matrixRight.m[7] + matrixLeft.m[7] * matrixRight.m[8];
        
        m.m[2] = matrixLeft.m[2] * matrixRight.m[0] + matrixLeft.m[5] * matrixRight.m[1] + matrixLeft.m[8] * matrixRight.m[2];
        m.m[5] = matrixLeft.m[2] * matrixRight.m[3] + matrixLeft.m[5] * matrixRight.m[4] + matrixLeft.m[8] * matrixRight.m[5];
        m.m[8] = matrixLeft.m[2] * matrixRight.m[6] + matrixLeft.m[5] * matrixRight.m[7] + matrixLeft.m[8] * matrixRight.m[8];
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_transpose(c3d_matrix3_t matrix)
    {
        c3d_matrix3_t m = { matrix.m[0], matrix.m[3], matrix.m[6],
            matrix.m[1], matrix.m[4], matrix.m[7],
            matrix.m[2], matrix.m[5], matrix.m[8] };
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply_scalar(c3d_matrix3_t matrixLeft, float value){
        c3d_matrix3_t m3 = {
            matrixLeft.m00 * value,matrixLeft.m01 * value,matrixLeft.m02 * value,
            matrixLeft.m10 * value,matrixLeft.m11 * value,matrixLeft.m12 * value,
            matrixLeft.m20 * value,matrixLeft.m21 * value,matrixLeft.m22 * value
        };
        return m3;
    }
    
    
    static __inline__ c3d_matrix3_t c3d_matrix3_add(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight)
    {
        c3d_matrix3_t m;
        
        m.m[0] = matrixLeft.m[0] + matrixRight.m[0];
        m.m[1] = matrixLeft.m[1] + matrixRight.m[1];
        m.m[2] = matrixLeft.m[2] + matrixRight.m[2];
        
        m.m[3] = matrixLeft.m[3] + matrixRight.m[3];
        m.m[4] = matrixLeft.m[4] + matrixRight.m[4];
        m.m[5] = matrixLeft.m[5] + matrixRight.m[5];
        
        m.m[6] = matrixLeft.m[6] + matrixRight.m[6];
        m.m[7] = matrixLeft.m[7] + matrixRight.m[7];
        m.m[8] = matrixLeft.m[8] + matrixRight.m[8];
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_sub(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight)
    {
        c3d_matrix3_t m;
        
        m.m[0] = matrixLeft.m[0] - matrixRight.m[0];
        m.m[1] = matrixLeft.m[1] - matrixRight.m[1];
        m.m[2] = matrixLeft.m[2] - matrixRight.m[2];
        
        m.m[3] = matrixLeft.m[3] - matrixRight.m[3];
        m.m[4] = matrixLeft.m[4] - matrixRight.m[4];
        m.m[5] = matrixLeft.m[5] - matrixRight.m[5];
        
        m.m[6] = matrixLeft.m[6] - matrixRight.m[6];
        m.m[7] = matrixLeft.m[7] - matrixRight.m[7];
        m.m[8] = matrixLeft.m[8] - matrixRight.m[8];
        
        return m;
    }
    
    static __inline__ c3d_vector3_t c3d_matrix3_multiply_vector3(c3d_matrix3_t matrixLeft, c3d_vector3_t vectorRight){
        c3d_vector3_t v = { matrixLeft.m[0] * vectorRight.v[0] + matrixLeft.m[3] * vectorRight.v[1] + matrixLeft.m[6] * vectorRight.v[2],
            matrixLeft.m[1] * vectorRight.v[0] + matrixLeft.m[4] * vectorRight.v[1] + matrixLeft.m[7] * vectorRight.v[2],
            matrixLeft.m[2] * vectorRight.v[0] + matrixLeft.m[5] * vectorRight.v[1] + matrixLeft.m[8] * vectorRight.v[2] };
        return v;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_scale(float sx, float sy, float sz){
        c3d_matrix3_t m = c3d_matrix3_identity;
        m.m[0] = sx;
        m.m[4] = sy;
        m.m[8] = sz;
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation(float radians, float x, float y, float z){
        c3d_vector3_t v3 = {x,y,z};
        c3d_vector3_t v = c3d_vector3_normalize(v3);
        float cos = cosf(radians);
        float cosp = 1.0f - cos;
        float sin = sinf(radians);
        
        c3d_matrix3_t m = { cos + cosp * v.v[0] * v.v[0],
            cosp * v.v[0] * v.v[1] + v.v[2] * sin,
            cosp * v.v[0] * v.v[2] - v.v[1] * sin,
            
            cosp * v.v[0] * v.v[1] - v.v[2] * sin,
            cos + cosp * v.v[1] * v.v[1],
            cosp * v.v[1] * v.v[2] + v.v[0] * sin,
            
            cosp * v.v[0] * v.v[2] + v.v[1] * sin,
            cosp * v.v[1] * v.v[2] - v.v[0] * sin,
            cos + cosp * v.v[2] * v.v[2] };
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_x(float radians){
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix3_t m = { 1.0f, 0.0f, 0.0f,
            0.0f, cos, sin,
            0.0f, -sin, cos };
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_y(float radians){
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix3_t m = { cos, 0.0f, -sin,
            0.0f, 1.0f, 0.0f,
            sin, 0.0f, cos };
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_rotation_z(float radians){
        float cos = cosf(radians);
        float sin = sinf(radians);
        
        c3d_matrix3_t m = { cos, sin, 0.0f,
            -sin, cos, 0.0f,
            0.0f, 0.0f, 1.0f };
        
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_scale(c3d_matrix3_t matrix, float sx, float sy, float sz){
        c3d_matrix3_t m = { matrix.m[0] * sx, matrix.m[1] * sx, matrix.m[2] * sx,
            matrix.m[3] * sy, matrix.m[4] * sy, matrix.m[5] * sy,
            matrix.m[6] * sz, matrix.m[7] * sz, matrix.m[8] * sz };
        return m;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate(c3d_matrix3_t matrix, float radians, float x, float y, float z){
        c3d_matrix3_t rm = c3d_matrix3_make_rotation(radians, x, y, z);
        return c3d_matrix3_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_x(c3d_matrix3_t matrix, float radians){
        c3d_matrix3_t rm = c3d_matrix3_make_rotation_x(radians);
        return c3d_matrix3_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_y(c3d_matrix3_t matrix, float radians){
        c3d_matrix3_t rm = c3d_matrix3_make_rotation_y(radians);
        return c3d_matrix3_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_rotate_z(c3d_matrix3_t matrix, float radians){
        c3d_matrix3_t rm = c3d_matrix3_make_rotation_z(radians);
        return c3d_matrix3_multiply(matrix, rm);
    }
    
    static __inline__ c3d_matrix3_t c3d_quat_to_matrix3(c3d_vector4_t quat) {
        float length = c3d_vector4_length2(quat);
        c3d_matrix3_t m3 = c3d_matrix3_identity;
        if (length == 0.0f)
        {
            m3.m00 = 0.0f; m3.m10 = 0.0f; m3.m20 = 0.0f;
            m3.m01 = 0.0f; m3.m11 = 0.0f; m3.m21 = 0.0f;
            m3.m02 = 0.0f; m3.m12 = 0.0f; m3.m22 = 0.0f;
        }
        else
        {
            float rlength;

            if (length != 1.0)
            {
                rlength = 2.0/length;
            }
            else
            {
                rlength = 2.0;
            }

            float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
            
            x2 = rlength* quat.x;
            y2 = rlength* quat.y;
            z2 = rlength* quat.z;
            
            xx = quat.x * x2;
            xy = quat.x * y2;
            xz = quat.x * z2;
            
            yy = quat.y * y2;
            yz = quat.y * z2;
            zz = quat.z * z2;
            
            wx = quat.w * x2;
            wy = quat.w * y2;
            wz = quat.w * z2;
            
            m3.m00 = 1.0 - (yy + zz);
            m3.m10 = xy - wz;
            m3.m20 = xz + wy;
            
            
            m3.m01 = xy + wz;
            m3.m11 = 1.0 - (xx + zz);
            m3.m21 = yz - wx;
            
            m3.m02 = xz - wy;
            m3.m12 = yz + wx;
            m3.m22 = 1.0 - (xx + yy);
        }
        return m3;
    }
    
    static __inline__ c3d_vector4_t c3d_matrix3_to_quat(c3d_matrix3_t m) {
        float s;
        c3d_vector4_t tq,q;
        int    i, j;
        
        tq.x = 1 + m.m00+m.m11+m.m22;
        tq.y = 1 + m.m00-m.m11-m.m22;
        tq.z = 1 - m.m00+m.m11-m.m22;
        tq.w = 1 - m.m00-m.m11+m.m22;
        
        j = 0;
        for(i=1;i<4;i++) j = (tq.v[i]>tq.v[j])? i : j;
        
        if (j==0)
        {
            q.w = tq.v[0];
            q.x = m.m12-m.m21;
            q.y = m.m20-m.m02;
            q.z = m.m01-m.m10;
        }
        else if (j==1)
        {
            q.w = m.m12-m.m21;
            q.x = tq.v[1];
            q.y = m.m01+m.m10;
            q.z = m.m20+m.m02;
        }
        else if (j==2)
        {
            q.w = m.m20-m.m02;
            q.x = m.m01+m.m10;
            q.y = tq.v[2];
            q.z = m.m12+m.m21;
        }
        else /* if (j==3) */
        {
            q.w = m.m01-m.m10;
            q.x = m.m20+m.m02;
            q.y = m.m12+m.m21;
            q.z = tq.v[3];
        }
        
        s = sqrt(0.25/tq.v[j]);
        q.w *= s;
        q.x *= s;
        q.y *= s;
        q.z *= s;
        
        return q;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_multiply_anim(c3d_matrix3_t matrixLeft, c3d_matrix3_t matrixRight,float value){
        c3d_vector4_t fv = c3d_matrix3_to_quat(matrixLeft);
        c3d_vector4_t tv = c3d_matrix3_to_quat(matrixRight);
        
        //float w = fv.w + (tv.w - fv.w) * value;
        
        float fw =  2.0f * acos(fv.w);
        float tw =  2.0f * acos(tv.w);
        if(fw > M_PI){
            fw = fw - 2.0f * M_PI;
        }
        if(tw > M_PI){
            tw = tw - 2.0f * M_PI;
        }
        float w = fw + (tw - fw) * value;
        if(w < 0){
            w = 2.0f * M_PI + w;
        }
        
        w = cos(w / 2 );
        
        
        
        c3d_vector4_t v = {
            fv.x + (tv.x - fv.x) * value,fv.y + (tv.y - fv.y) * value,fv.z + (tv.z - fv.z) * value,w
        };
        return c3d_quat_to_matrix3(v);
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix3_to_matrix4(c3d_matrix3_t matrix){
        c3d_matrix4_t m4 = {
            matrix.m00,matrix.m01,matrix.m02,0.0f,
            matrix.m10,matrix.m11,matrix.m12,0.0f,
            matrix.m20,matrix.m21,matrix.m22,0.0f,
            0.0f,0.0f,0.0f,1.0f
        };
        
        return m4;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_multiply_anim(c3d_matrix4_t matrixLeft, c3d_matrix4_t matrixRight,float value){
        c3d_matrix3_t m3 = c3d_matrix3_multiply_anim(c3d_matrix4_to_matrix3(matrixLeft),c3d_matrix4_to_matrix3(matrixRight),value);
        c3d_matrix4_t m4 = c3d_matrix3_to_matrix4(m3);
        m4.m30 = matrixLeft.m30 + (matrixRight.m30 - matrixLeft.m30) * value;
        m4.m31 = matrixLeft.m31 + (matrixRight.m31 - matrixLeft.m31) * value;
        m4.m32 = matrixLeft.m32 + (matrixRight.m32 - matrixLeft.m32) * value;
        return m4;
    }
    
    static __inline__ c3d_vector4_t c3d_quat_to_axis(c3d_vector4_t q){
        c3d_vector4_t v4;
        float scale = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z);
        v4.w = 2.0f * acos(q.w);
        if(scale ==0.0f){
            v4.x = v4.y = v4.z = 0.0f;
        }
        else{
            v4.x = q.x / scale;
            v4.y = q.y / scale;
            v4.z = q.y / scale;
        }
        return v4;
    }
    
    static __inline__ c3d_vector4_t c3d_axis_to_quat(c3d_vector4_t a){
        c3d_vector4_t v4;
        v4.w = cos(a.w / 2 );
        v4.x = a.x * sin(a.w / 2);
        v4.y = a.y * sin(a.w / 2);
        v4.z = a.z * sin(a.w / 2 );
        return v4;
    }
    
    static __inline__ c3d_matrix3_t c3d_matrix3_make_project(c3d_vector3_t v3){
        c3d_matrix3_t m3 = {
            1 - v3.x * v3.x,- v3.x * v3.y,- v3.x * v3.z,
            - v3.x * v3.y,1 - v3.y * v3.y, - v3.y * v3.z,
            - v3.x * v3.z,- v3.y * v3.z,1 - v3.z * v3.z
        };
        return m3;
    }
    
    static __inline__ c3d_matrix4_t c3d_matrix4_make_project(c3d_vector3_t v3){
        c3d_matrix4_t m4 = {
            1 - v3.x * v3.x,- v3.x * v3.y,- v3.x * v3.z,0.0f,
            - v3.x * v3.y,1 - v3.y * v3.y, - v3.y * v3.z,0.0f,
            - v3.x * v3.z,- v3.y * v3.z,1 - v3.z * v3.z,0.0f,
            0.0f,0.0f,0.0f,1.0f
        };
        return m4;
    }
    
#ifdef __cplusplus
}
#endif


#endif
