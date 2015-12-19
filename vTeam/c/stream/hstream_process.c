//
//  hstream_process.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hstream_process.h"
#include "hprocess.h"
#include "hstr.h"
#include "hext_obj.h"

typedef struct{
    stream_t base;
    hchar * command;
    int inno;
    int outno;
    hprocess_t process;
}stream_process_impl_t;


static StreamState stream_process_has_data(stream_t * stream,hdouble timeout){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    if(impl->process){
        return (StreamState) process_has_data(impl->process, timeout);
    }
    return StreamStateError;
}

static StreamState stream_process_has_space(stream_t * stream ,hdouble timeout){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    if(impl->process){
        return (StreamState) process_has_space(impl->process, timeout);
    }
    return StreamStateError;
}

static hint32 stream_process_read(stream_t * stream, hany data,hint32 length){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    if(impl->process){
        return  process_read(impl->process, data,length);
    }
    return 0;
}

static hint32 stream_process_write(stream_t * stream, hany data,hint32 length){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    if(impl->process){
        return  process_write(impl->process, data,length);
    }
    return 0;
}

static hbool stream_process_is_connected(stream_t * stream){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    return impl->process != NULL;
}

static StreamState stream_process_connect(stream_t * stream,hdouble timeout){
    stream_process_impl_t * impl = (stream_process_impl_t *)stream;
    StreamState state = StreamStateNone;
    if(impl->process){
        state =  (StreamState) process_has_space(impl->process, timeout);
        if(state == StreamStateError){
            process_dealloc(impl->process);
            impl->process = NULL;
            state = StreamStateNone;
        }
    }
    if(impl->process == NULL){
        if(impl->inno && impl->outno){
            impl->process = process_alloc_with_io(impl->command, impl->inno, impl->outno);
        }
        else{
            impl->process = process_alloc(impl->command);
        }
        state =  (StreamState) process_has_space(impl->process, timeout);
    }
    return state;
}

typedef struct _stream_process_alloc_param_t{
    hcchar * command;
    int inno;
    int outno;
} stream_process_alloc_param_t;

static void stream_process_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    stream_process_alloc_param_t * param = (stream_process_alloc_param_t *) arg;
    stream_process_impl_t * impl =(stream_process_impl_t *)obj;
    str_cpy(&impl->command, param->command);
    impl->base.has_data = stream_process_has_data;
    impl->base.has_space = stream_process_has_space;
    impl->base.read = stream_process_read;
    impl->base.write = stream_process_write;
    impl->base.is_connected = stream_process_is_connected;
    impl->base.connect = stream_process_connect;
    impl->inno = param->inno;
    impl->outno = param->outno;
}

static void stream_process_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    stream_process_impl_t * impl = (stream_process_impl_t *)obj;
    if(impl->process){
        process_dealloc(impl->process);
    }
    mem_free(impl->command);
}

static ext_class_t stream_process_ext_class = {stream_process_ext_init,stream_process_ext_destroy,sizeof(stream_process_impl_t)};


stream_t * stream_process_alloc(hcchar * command,InvokeTickDeclare){
    stream_process_alloc_param_t param = {command,0,0};
    return (stream_t *) ext_obj_alloc((hany)&param, &stream_process_ext_class);
}

stream_t * stream_process_alloc_with_io(hcchar * command,int inno,int outno,InvokeTickDeclare){
    stream_process_alloc_param_t param = {command,inno,outno};
    return (stream_t *) ext_obj_alloc((hany)&param, &stream_process_ext_class);
}
