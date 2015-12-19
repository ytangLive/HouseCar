//
//  hstream_pipe.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY
#include "hconfig.h"
#include "hstream_pipe.h"
#include "hext_obj.h"

typedef struct{
    stream_t base;
    hint32 pipe_in;
    hint32 pipe_out;
#ifdef __WIN32__
	HANDLE in_event;
	HANDLE out_event;
	FILE * in_file;
	FILE * out_file;
#endif
}stream_pipe_impl_t;

typedef struct{
    hint32 pipe_in;
    hint32 pipe_out;
}stream_pipe_impl_param_t;


static StreamState stream_pipe_has_data(stream_t * stream,hdouble timeout){
    stream_pipe_impl_t * impl = (stream_pipe_impl_t *)stream;
    StreamState state = StreamStateNone;
#ifdef __WIN32__
	DWORD rs = WaitForSingleObject(impl->in_event,(DWORD)(timeout * 1000));
	switch(rs){
        case WAIT_OBJECT_0:
            state = StreamStateOK;
            break;
        case WAIT_FAILED:
            state = StreamStateError;
            break;
        default:
            break;
	}
#else
    hint32 res;
    fd_set rds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    FD_ZERO(&rds);
    
    FD_SET(impl->pipe_in, &rds);
    
    res = select(impl->pipe_in + 1, &rds, NULL, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = StreamStateError;
    }
    else{
        if(FD_ISSET(impl->pipe_in, &rds)){
            state = StreamStateOK;
        }
    }
#endif
    return state;
}

static StreamState stream_pipe_has_space(stream_t * stream ,hdouble timeout){
    stream_pipe_impl_t * impl = (stream_pipe_impl_t *)stream;
    StreamState state = StreamStateNone;
#ifdef __WIN32__
	DWORD rs = WaitForSingleObject(impl->out_event,(DWORD)(timeout * 1000));
	switch(rs){
        case WAIT_OBJECT_0:
            state = StreamStateOK;
            break;
        case WAIT_FAILED:
            state = StreamStateError;
            break;
	}
#else
    hint32 res;
    fd_set wds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    FD_ZERO(&wds);
    
    FD_SET(impl->pipe_out, &wds);
    
    res = select(impl->pipe_out + 1, NULL, &wds, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = StreamStateError;
    }
    else{
        if(FD_ISSET(impl->pipe_out, &wds)){
            state = StreamStateOK;
        }
    }
#endif
    return state;
}

static hint32 stream_pipe_read(stream_t * stream, hany data,hint32 length){
    stream_pipe_impl_t * impl = (stream_pipe_impl_t *)stream;
#ifdef __WIN32__
	DWORD ret = 0;
	OVERLAPPED over = {0};
	over.hEvent = impl->in_event;
	if(!ReadFile((HANDLE)impl->in_file,data,length,&ret,&over)){
		ret = 0;
	}
	return ret;
#else
	return (hint32)read(impl->pipe_in, data, length);
#endif
}

static hint32 stream_pipe_write(stream_t * stream, hany data,hint32 length){
    stream_pipe_impl_t * impl = (stream_pipe_impl_t *)stream;
#ifdef __WIN32__
	OVERLAPPED over = {0};
	DWORD ret = 0;
	over.hEvent = impl->out_event;
	if(!WriteFile((HANDLE)impl->out_file,data,length,&ret,&over)){
		ret = 0;
	}
	return ret;
#else
    return (hint32)write(impl->pipe_out, data, length);
#endif
}


static void stream_pipe_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    stream_pipe_impl_t * impl =(stream_pipe_impl_t *)obj;
    stream_pipe_impl_param_t * param = (stream_pipe_impl_param_t *) arg;
    impl->pipe_in = param->pipe_in;
    impl->pipe_out = param->pipe_out;
    impl->base.has_data = stream_pipe_has_data;
    impl->base.has_space = stream_pipe_has_space;
    impl->base.read = stream_pipe_read;
    impl->base.write = stream_pipe_write;
#ifdef __WIN32__
	impl->in_event = CreateEvent(NULL,FALSE,FALSE,L"in_event");
	impl->out_event = CreateEvent(NULL,FALSE,FALSE,L"out_event");
	impl->in_file = fdopen(impl->pipe_in,"r");
	impl->out_file = fdopen(impl->pipe_out,"a");
#endif
}

static void stream_pipe_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    stream_pipe_impl_t * impl = (stream_pipe_impl_t *)obj;
#ifdef __WIN32__
	CloseHandle((HANDLE)impl->pipe_out);
	CloseHandle((HANDLE)impl->pipe_in);
	CloseHandle(impl->in_event);
	CloseHandle(impl->out_event);
	fclose(impl->in_file);
	fclose(impl->out_file);
#else
    close(impl->pipe_out);
    close(impl->pipe_in);
#endif
}

static ext_class_t stream_pipe_ext_class = {stream_pipe_ext_init,stream_pipe_ext_destroy,sizeof(stream_pipe_impl_t)};

stream_t * stream_pipe_alloc(hint32 pipe_in,hint32 pipe_out,InvokeTickDeclare){
    stream_pipe_impl_param_t param = {pipe_in,pipe_out};
    return (stream_t *)ext_obj_alloc(&param, &stream_pipe_ext_class);
}
