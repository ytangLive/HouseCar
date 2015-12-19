//
//  hstream.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-7.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HSTREAM_H
#define HSTREAM_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hmem.h"

    struct _stream_t;
    
    typedef enum{
        StreamStateNone,StreamStateOK,StreamStateError
    }StreamState;
    
    typedef StreamState (* stream_has_data_t)(struct _stream_t * stream,hdouble timeout);
    typedef StreamState (* stream_has_space_t)(struct _stream_t * stream,hdouble timeout);
    typedef StreamState (* stream_connect_t)(struct _stream_t * stream,hdouble timeout);
    typedef hbool (* stream_is_connected_t)(struct _stream_t * stream);
    typedef hint32 (* stream_read_t)(struct _stream_t * stream, hany data,hint32 length);
    typedef hint32 (* stream_write_t)(struct _stream_t * stream, hany data,hint32 length);
    
    typedef struct _stream_t{
        stream_has_data_t READONLY has_data;
        stream_has_space_t READONLY has_space;
        stream_connect_t READONLY connect;
        stream_is_connected_t is_connected;
        stream_read_t READONLY read;
        stream_write_t READONLY write;
    }stream_t;
    
    stream_t * stream_retain(stream_t * stream,InvokeTickDeclare);
    
    void stream_release(stream_t * stream,InvokeTickDeclare);
    
    hbool stream_is_connected(stream_t * stream,InvokeTickDeclare);
    
    StreamState stream_connect(stream_t * stream,hdouble timeout,InvokeTickDeclare);
    
    StreamState stream_has_data(stream_t * stream,hdouble timeout,InvokeTickDeclare);
    
    StreamState stream_has_space(stream_t * stream ,hdouble timeout,InvokeTickDeclare);
    
    hint32 stream_read(stream_t * stream, hany data,hint32 length,InvokeTickDeclare);
    
    hint32 stream_write(stream_t * stream, hany data,hint32 length,InvokeTickDeclare);
    
    
    
    void stream_socket_close(hsocket_t sock);
    
    StreamState stream_socket_has_data(hsocket_t sock,hdouble timeout);
    
    StreamState stream_socket_has_space(hsocket_t sock ,hdouble timeout);
    
    hint32 stream_socket_read(hsocket_t sock, hany data,hint32 length);
    
    hint32 stream_socket_write(hsocket_t sock, hany data,hint32 length);
    
#ifdef __cplusplus
}
#endif


#endif
