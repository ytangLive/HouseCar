//
//  h3dschedule.c
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hlist.h"
#include "hmem.h"
#include "h3dcontext.h"
#include "h3dprogram.h"
#include "h3dtexture.h"
#include "hdata_alloc.h"
#include "hmutex.h"
#include "h3dmodel.h"
#include "hbuffer.h"
#include "hext_obj.h"
#include "hlog.h"

static void c3d_task_event_frame_tick_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    
}

static hbool c3d_task_event_finish_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    return hbool_true;
}

static void c3d_task_event_release_fun(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_task_event_release((c3d_task_event_t *) task->target,InvokeTickArg);
}

static void c3d_task_event_retain_fun(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_task_event_retain((c3d_task_event_t *) task->target,InvokeTickArg);
}

static void c3d_task_event_start_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    ev_dispatch_fire(&context->dispatch, (ev_event_t *) task->target, InvokeTickArg);
}


static void c3d_task_event_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    c3d_task_event_t * task = (c3d_task_event_t *) obj;
    strncpy(task->event.name, (hcchar *) arg, sizeof(task->event.name));
    task->task.frameTick = c3d_task_event_frame_tick_fun;
    task->task.finish = c3d_task_event_finish_fun;
    task->task.release = c3d_task_event_release_fun;
    task->task.retain = c3d_task_event_retain_fun;
    task->task.start = c3d_task_event_start_fun;
    task->task.target = &task->event;
}

static void c3d_task_event_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    
}


static ext_class_t c3d_task_event_class = {c3d_task_event_init,c3d_task_event_destroy,sizeof(c3d_task_event_t)};

c3d_task_event_t * c3d_task_event_alloc(hcchar * name,InvokeTickDeclare){
    return (c3d_task_event_t *)ext_obj_alloc((hany)name, &c3d_task_event_class);
}

c3d_task_event_t * c3d_task_event_retain(c3d_task_event_t * task,InvokeTickDeclare){
    return (c3d_task_event_t *)ext_obj_retain((hext_obj_t)task);
}

void c3d_task_event_release(c3d_task_event_t * task,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)task);
}

void c3d_schedule_task_event(struct _c3d_context_t * context,c3d_task_event_t * task,InvokeTickDeclare){
    c3d_schedule_task(context, &task->task, InvokeTickArg);
}

void c3d_schedule_tick(struct _c3d_context_t * context,InvokeTickDeclare){
    if(!context->schedule.isPause){
        struct timeval tm;
        hdouble t;
        hlist_t l;
        hint32 i,c;
        c3d_task_t * task;
        gettimeofday(&tm, NULL);
        t = (hdouble)tm.tv_sec + (hdouble)tm.tv_usec / 1000000.0;
        if(context->schedule.tickTime == 0.0){
            context->schedule.tickTime = t;
        }
        else{
            context->schedule.current += t - context->schedule.tickTime;
            context->schedule.tickTime = t;
        }
        
        if(context->schedule.tasks){
            
            hmutex_lock(context->schedule.locker);
            l = list_clone(context->schedule.tasks);
            c = list_count(l);
            for(i=0;i<c;i++){
                task = list_get(l, i);
                (* task->retain)(context,task,InvokeTickArg);
            }
            hmutex_unlock(context->schedule.locker);
            
            c = list_count(l);

            for(i=0;i<c;i++){
                task = list_get(l, i);
                if(!task->taskIsStart){
                    (* task->start)(context,task,InvokeTickArg);
                    task->taskIsStart = hbool_true;
                }
                (* task->frameTick)(context,task,InvokeTickArg);
                if((* task->finish)(context,task,InvokeTickArg)){
                    hmutex_lock(context->schedule.locker);
                    list_remove(context->schedule.tasks, task);
                    (* task->release)(context,task,InvokeTickArg);
                    hmutex_unlock(context->schedule.locker);
                }
            }
            
            hmutex_lock(context->schedule.locker);
            c = list_count(l);
            for(i=0;i<c;i++){
                task = list_get(l, i);
                (* task->release)(context,task,InvokeTickArg);
            }
            list_dealloc(l);
            hmutex_unlock(context->schedule.locker);

        }
    }
}

void c3d_schedule_task(struct _c3d_context_t * context,c3d_task_t * task,InvokeTickDeclare){
    if(context->schedule.tasks == NULL){
        context->schedule.tasks = list_alloc(20, 20);
        context->schedule.locker = hmutex_alloc();
    }
    task->taskIsStart = hbool_false;
    hmutex_lock(context->schedule.locker);
    (* task->retain)(context,task,InvokeTickArg);
    list_add(context->schedule.tasks, task);
    hmutex_unlock(context->schedule.locker);
}

void c3d_schedule_task_cancel(struct _c3d_context_t * context,c3d_task_t * task,InvokeTickDeclare){
    hint32 i,c;
    c3d_task_t *t ;
    if(context->schedule.tasks){
        hmutex_lock(context->schedule.locker);
        c = list_count(context->schedule.tasks);
        for(i=0;i<c;i++){
            t = list_get(context->schedule.tasks, i);
            if(t == task){
                (* t->release)(context,t,InvokeTickArg);
                list_remove_at(context->schedule.tasks, i--);
            }
        }
        hmutex_unlock(context->schedule.locker);
    }
}

void c3d_schedule_task_cancels(struct _c3d_context_t * context,hany target,InvokeTickDeclare){
    hint32 i,c;
    c3d_task_t *t ;
    if(context->schedule.tasks){
        hmutex_lock(context->schedule.locker);
        c = list_count(context->schedule.tasks);
        for(i=0;i<c;i++){
            t = list_get(context->schedule.tasks, i);
            if(t->target == target){
                list_remove_at(context->schedule.tasks, i);
                i -- ;
                c --;
            }
        }
        hmutex_unlock(context->schedule.locker);
    }
}

static void c3d_context_cleanup_programs_callback(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
    c3d_program_t p = (c3d_program_t)hdata_int64(&hdata_class, value, 0);
    if(p){
        c3d_program_delete(p);
    }
}

static void c3d_context_cleanup_textures_callback(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
    c3d_texture_t t = {0,0,0};
    mem_memcpy(&t, hdata_bytes(&hdata_class, value), sizeof(c3d_texture_t));
    if(t.texture){
        c3d_texture_delete(t);
    }
}

void c3d_context_cleanup(c3d_context_t * context,InvokeTickDeclare){
    hint32 i,c;
    c3d_task_t *t ;
    if(context->schedule.tasks){
        c = list_count(context->schedule.tasks);
        for(i=0;i<c;i++){
            t = list_get(context->schedule.tasks, i);
            (* t->release)(context,t,InvokeTickArg);
        }
        list_dealloc(context->schedule.tasks);
        context->schedule.tasks = NULL;
        hmutex_dealloc(context->schedule.locker);
    }
    if(context->programs){
        hdata_object_each(&hdata_class, context->programs, c3d_context_cleanup_programs_callback, NULL);
    }
    if(context->textures){
        hdata_object_each(&hdata_class, context->programs, c3d_context_cleanup_textures_callback, NULL);
    }
}

c3d_program_t c3d_context_program(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    return (c3d_program_t) hdata_object_int64(&hdata_class, context->programs, name, 0) ;
}

c3d_program_t c3d_context_program_load(c3d_context_t * context,hcchar * name,hcchar * vshSource,hcchar * fshSource,InvokeTickDeclare){
    c3d_program_t r = (c3d_program_t)hdata_object_int64(&hdata_class, context->programs, name, 0);
    if(r == 0){
        r = c3d_program_create(vshSource, fshSource);
        if(r){
            if(context->programs == NULL){
                context->programs = hdata_object_alloc();
            }
            hdata_object_put(context->programs, name, hdata_int64_alloc(r));
        }
    }
    return r;
}

void c3d_context_program_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    c3d_program_t r = (c3d_program_t)hdata_object_int64(&hdata_class, context->programs, name, 0);
    if(r){
        c3d_program_delete(r);
        hdata_object_remove(context->programs, name);
    }
}

c3d_texture_t c3d_context_texture_at(c3d_context_t * context,hcchar * name,hint32 index,InvokeTickDeclare){
    if(index != 0){
        hbuffer_t key = buffer_alloc(64, 64);
        c3d_texture_t t ;
        buffer_append_format(key, "%s%d", InvokeTickArg,name,index);
        t = c3d_context_texture(context, buffer_to_str(key), InvokeTickArg);
        buffer_dealloc(key);
        return t;
    }
    return c3d_context_texture(context, name, InvokeTickArg);
}

c3d_texture_t c3d_context_texture(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    c3d_texture_t t = {0,0,0};
    hdata_t d = hdata_object(&hdata_class, context->textures, name);
    if(d){
        mem_memcpy(&t, hdata_bytes(&hdata_class, d), sizeof(c3d_texture_t));
    }
    return t ;
}

void c3d_context_texture_set(c3d_context_t * context,hcchar * name, c3d_texture_t texture,InvokeTickDeclare){
    hdata_t d = hdata_object(&hdata_class, context->textures, name);
    c3d_texture_t t = {0,0,0};
    assert(texture.texture);
    if(d){
        mem_memcpy(&t, hdata_bytes(&hdata_class, d), sizeof(c3d_texture_t));
        c3d_texture_delete(t);
    }
    if(context->textures == NULL){
        context->textures = hdata_object_alloc();
    }
    hdata_object_put(context->textures, name, hdata_bytes_alloc((hbyte *)&texture, sizeof(c3d_texture_t)));
}

typedef struct _c3d_texture_set_async_task_t{
    c3d_task_t base;
    hdata_t name;
    c3d_texture_t texture;
    hint32 retainCount;
} c3d_texture_set_async_task_t;

static void c3d_texture_set_async_task_frame_tick_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    
}

static hbool c3d_texture_set_async_task_finish_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    return hbool_true;
}

static void c3d_texture_set_async_task_release(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_texture_set_async_task_t * t = (c3d_texture_set_async_task_t *) task;
    if( -- t->retainCount ==0){
        hdata_dealloc(t->name);
        free(t);
    }
}

static void c3d_texture_set_async_task_retain(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_texture_set_async_task_t * t = (c3d_texture_set_async_task_t *) task;
    t->retainCount ++;
}

static void c3d_texture_set_async_task_start_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    c3d_texture_set_async_task_t * t = (c3d_texture_set_async_task_t *) task;
    c3d_context_texture_set(context,hdata_string(&hdata_class, t->name, NULL),t->texture,InvokeTickArg);
}

void c3d_context_texture_set_async(c3d_context_t * context,hcchar * name, c3d_texture_t texture,InvokeTickDeclare){
    if(name && texture.texture){
        c3d_texture_set_async_task_t * t = (c3d_texture_set_async_task_t *) malloc(sizeof(c3d_texture_set_async_task_t));
        mem_memset(t, 0, sizeof(c3d_texture_set_async_task_t));
        t->base.frameTick = c3d_texture_set_async_task_frame_tick_fun;
        t->base.finish = c3d_texture_set_async_task_finish_fun;
        t->base.release = c3d_texture_set_async_task_release;
        t->base.retain = c3d_texture_set_async_task_retain;
        t->base.start = c3d_texture_set_async_task_start_fun;
        t->name = hdata_string_alloc(name);
        t->texture = texture;
        t->retainCount = 1;
        c3d_schedule_task(context, &t->base, InvokeTickArg);
        c3d_texture_set_async_task_release(context,&t->base,InvokeTickArg);
    }
}

static void c3d_context_texture_sub_unload(c3d_context_t * context,hcchar * name,hint32 i, hbuffer_t buf, InvokeTickDeclare){
    c3d_texture_t t = {0};
    buffer_clear(buf);
    buffer_append_format(buf, "%s%d", InvokeTickArg,name,i);
    t = c3d_context_texture(context,buffer_to_str(buf),InvokeTickArg);
    if(t.texture){
        c3d_texture_delete(t);
        hdata_object_remove(context->textures, buffer_to_str(buf));
        if(t.next >0){
            c3d_context_texture_sub_unload(context,name,i +1 ,buf,InvokeTickArg);
        }
    }
}

void c3d_context_texture_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    c3d_texture_t r = c3d_context_texture(context,name,InvokeTickArg);
    hbuffer_t buf;
    if(r.texture){
        if(r.next >0){
            buf = buffer_alloc(C3D_NAME_MAX, 128);
            c3d_context_texture_sub_unload(context,name,1,buf,InvokeTickArg);
            buffer_dealloc(buf);
        }
        c3d_texture_delete(r);
        hdata_object_remove(context->textures, name);
    }
}

c3d_model_t c3d_context_model(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    c3d_model_t t = {0};
    hdata_t d = hdata_object(&hdata_class, context->models, name);
    if(d){
        mem_memcpy(&t, hdata_bytes(&hdata_class, d), sizeof(c3d_model_t));
    }
    return t ;
}

void c3d_context_model_set(c3d_context_t * context,hcchar * name, c3d_model_t model,InvokeTickDeclare){
    hdata_t d = hdata_object(&hdata_class, context->models, name);
    c3d_model_t t = {0};
    if(d){
        mem_memcpy(&t, hdata_bytes(&hdata_class, d), sizeof(c3d_model_t));
        c3d_model_delete(t);
    }
    if(context->models == NULL){
        context->models = hdata_object_alloc();
    }
    hdata_object_put(context->models, name, hdata_bytes_alloc((hbyte *)&model, sizeof(c3d_model_t)));
}

typedef struct _c3d_model_set_async_task_t{
    c3d_task_t base;
    hdata_t name;
    c3d_model_t model;
    hint32 retainCount;
} c3d_model_set_async_task_t;

static void c3d_model_set_async_task_frame_tick_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    
}

static hbool c3d_model_set_async_task_finish_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    return hbool_true;
}

static void c3d_model_set_async_task_release(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_model_set_async_task_t * t = (c3d_model_set_async_task_t *) task;
    if( -- t->retainCount ==0){
        hdata_dealloc(t->name);
        free(t);
    }
}

static void c3d_model_set_async_task_retain(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    c3d_model_set_async_task_t * t = (c3d_model_set_async_task_t *) task;
    t->retainCount ++;
}

static void c3d_model_set_async_task_start_fun(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    c3d_model_set_async_task_t * t = (c3d_model_set_async_task_t *) task;
    c3d_context_model_set(context,hdata_string(&hdata_class, t->name, NULL),t->model,InvokeTickArg);
}

void c3d_model_texture_set_async(c3d_context_t * context,hcchar * name, c3d_model_t model,InvokeTickDeclare){
    if(name){
        c3d_model_set_async_task_t * t = (c3d_model_set_async_task_t *) malloc(sizeof(c3d_model_set_async_task_t));
        mem_memset(t, 0, sizeof(c3d_model_set_async_task_t));
        t->base.frameTick = c3d_model_set_async_task_frame_tick_fun;
        t->base.finish = c3d_model_set_async_task_finish_fun;
        t->base.release = c3d_model_set_async_task_release;
        t->base.retain = c3d_model_set_async_task_retain;
        t->base.start = c3d_model_set_async_task_start_fun;
        t->name = hdata_string_alloc(name);
        t->model = model;
        t->retainCount = 1;
        c3d_schedule_task(context, &t->base, InvokeTickArg);
        c3d_model_set_async_task_release(context,&t->base,InvokeTickArg);
    }
}

void c3d_context_model_unload(c3d_context_t * context,hcchar * name,InvokeTickDeclare){
    c3d_model_t r = c3d_context_model(context,name,InvokeTickArg);
    if(r.data){
        c3d_model_delete(r);
        hdata_object_remove(context->models, name);
    }
}

hint32 c3d_context_resource_content(c3d_context_t * context,hcchar * filePath,struct _buffer_t * buffer,InvokeTickDeclare){
    hchar path[PATH_MAX] = {0};
    strcat(path, context->resources.path);
    strcat(path, filePath);
    
    if(context->resources.content){
        return (* context->resources.content)(context,path,buffer,InvokeTickArg);
    }
    
    {
        FILE * f = fopen(path, "r");
        hbyte buf[1024];
        hint32 rs = 0;
        hint32 length;
        if(f){
            while(!feof(f) && (length = fread(buf, 1, sizeof(buf), f)) >0){
                buffer_append(buffer, buf, length);
                rs += length;
            }
            fclose(f);
        }
        return rs;
    }
}

c3d_vector3_t c3d_context_unproject(c3d_context_t * context,c3d_vector3_t position,InvokeTickDeclare){
    c3d_vector3_t v3;
    v3.x = position.x / context->transform.m00 - context->transform.m30;
    v3.y = position.y / context->transform.m11 - context->transform.m31;
    v3.z = position.z / context->transform.m22 - context->transform.m32;
    return v3;
}

c3d_vector3_t c3d_context_project_viewport(c3d_context_t * context,c3d_vector3_t position,InvokeTickDeclare){
    c3d_vector3_t v3;
    hfloat dx = context->viewport.width / 2.0;
    hfloat dy = context->viewport.height / 2.0;
    v3.x = position.x * context->transform.m00 * dx + dx;
    v3.y = position.y * context->transform.m11 * dy + dy;
    v3.z = position.z * context->transform.m22;
    return v3;
}


