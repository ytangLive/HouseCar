//
//  h3dcontext.h
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef H3DCONTEXT_H
#define H3DCONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "h3d.h"
#include "hevent.h"

#define C3D_EVENT_MEMORY_WARNING "MemoryWarning"
    
    typedef void (* c3d_task_frame_tick_fun_t)(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare);
    typedef hbool (* c3d_task_finish_fun_t)(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare);
    typedef void (* c3d_task_release_t)(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) ;
    typedef void (* c3d_task_retain_t)(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) ;
    typedef void (* c3d_task_start_fun_t)(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare);
    
    typedef struct _c3d_task_t{
        c3d_task_frame_tick_fun_t READONLY frameTick;
        c3d_task_finish_fun_t READONLY finish;
        c3d_task_release_t READONLY release;
        c3d_task_retain_t READONLY retain;
        c3d_task_start_fun_t READONLY start;
        hbool READONLY taskIsStart;
        hany target;
    } c3d_task_t;
    
    typedef struct _c3d_task_event_t{
        ev_event_t event;
        c3d_task_t task;
    } c3d_task_event_t;
    
    c3d_task_event_t * c3d_task_event_alloc(hcchar * name,InvokeTickDeclare);
    
    c3d_task_event_t * c3d_task_event_retain(c3d_task_event_t * task,InvokeTickDeclare);
    
    void c3d_task_event_release(c3d_task_event_t * task,InvokeTickDeclare);
    
    void c3d_schedule_task_event(struct _c3d_context_t * context,c3d_task_event_t * task,InvokeTickDeclare);
    
    typedef struct _c3d_schedule_t{
        hdouble READONLY tickTime;
        hdouble READONLY current;
        hbool READONLY isPause;
        struct _list_t * READONLY tasks;
        struct _mutex_t * locker;
    } c3d_schedule_t;
    
    void c3d_schedule_tick(struct _c3d_context_t * context,InvokeTickDeclare);
    
    void c3d_schedule_task(struct _c3d_context_t * context,c3d_task_t * task,InvokeTickDeclare);
    
    void c3d_schedule_task_cancel(struct _c3d_context_t * context,c3d_task_t * task,InvokeTickDeclare);
    
    void c3d_schedule_task_cancels(struct _c3d_context_t * context,hany target,InvokeTickDeclare);

    struct _buffer_t;
    struct _data_t;
    
    typedef hint32 (* c3d_context_resource_content_t)(struct _c3d_context_t * context,hcchar * filePath,struct _buffer_t * buffer,InvokeTickDeclare);
    
    typedef struct _c3d_context_t{
        ev_dispatch_t dispatch;
        struct {
            hint32 x;
            hint32 y;
            hint32 width;
            hint32 height;
        } viewport;
        struct {
            hchar path[PATH_MAX];
            c3d_context_resource_content_t content;
        } resources;
        struct {
            hany graphics;
            hany data;
            hint32 width;
            hint32 height;
        } graphics;
        c3d_matrix4_t transform;
        c3d_schedule_t schedule;
        struct _data_t * programs;
        struct _data_t * textures;
        struct _data_t * models;
        huint32 zIndex;
        hfloat scale;
        hbool needRender;
        hbool memoryWarning;
        c3d_buffer_t drawBuffer;
    } c3d_context_t;
    
    void c3d_context_cleanup(c3d_context_t * context,InvokeTickDeclare);
    
    hint32 c3d_context_resource_content(c3d_context_t * context,hcchar * filePath,struct _buffer_t * buffer,InvokeTickDeclare);
    
    c3d_program_t c3d_context_program(c3d_context_t * context,hcchar * name,InvokeTickDeclare);
    
    c3d_program_t c3d_context_program_load(c3d_context_t * context,hcchar * name,hcchar * vshSource,hcchar * fshSource,InvokeTickDeclare);
    
    void c3d_context_program_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare);
    
    
    c3d_texture_t c3d_context_texture(c3d_context_t * context,hcchar * name,InvokeTickDeclare);
    
    c3d_texture_t c3d_context_texture_at(c3d_context_t * context,hcchar * name,hint32 index,InvokeTickDeclare);
    
    void c3d_context_texture_set(c3d_context_t * context,hcchar * name, c3d_texture_t texture,InvokeTickDeclare);
    
    void c3d_context_texture_set_async(c3d_context_t * context,hcchar * name, c3d_texture_t texture,InvokeTickDeclare);
    
    void c3d_context_texture_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare);
    
    c3d_model_t c3d_context_model(c3d_context_t * context,hcchar * name,InvokeTickDeclare);
    
    void c3d_context_model_set(c3d_context_t * context,hcchar * name, c3d_model_t model,InvokeTickDeclare);
    
    void c3d_context_model_set_async(c3d_context_t * context,hcchar * name, c3d_model_t model,InvokeTickDeclare);
    
    void c3d_context_model_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare);

    c3d_vector3_t c3d_context_unproject(c3d_context_t * context,c3d_vector3_t position,InvokeTickDeclare);
    
    c3d_vector3_t c3d_context_project_viewport(c3d_context_t * context,c3d_vector3_t position,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif



#endif
