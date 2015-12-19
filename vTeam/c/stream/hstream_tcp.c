//
//  hstream_tcp.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hstream_tcp.h"
#include "hext_obj.h"
#include "hurl.h"
#include "hlog.h"
#include "hstr.h"

typedef struct{
    stream_t base;
    hsocket_t sock;
}stream_tcp_t;

static StreamState stream_tcp_has_data(struct _stream_t * stream,hdouble timeout){
    stream_tcp_t * tcp = (stream_tcp_t *)stream;
    return stream_socket_has_data(tcp->sock, timeout);
}

static StreamState stream_tcp_has_space(struct _stream_t * stream,hdouble timeout){
    stream_tcp_t * tcp = (stream_tcp_t *)stream;
    return stream_socket_has_space(tcp->sock, timeout);
}


static hint32 stream_tcp_read(struct _stream_t * stream, hany data,hint32 length){
    stream_tcp_t * tcp = (stream_tcp_t *)stream;
    return stream_socket_read(tcp->sock, data,length);
}

static hint32 stream_tcp_write(struct _stream_t * stream, hany data,hint32 length){
    stream_tcp_t * tcp = (stream_tcp_t *)stream;
    return stream_socket_write(tcp->sock, data,length);
}


static void stream_tcp_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    stream_tcp_t * impl =(stream_tcp_t *)obj;
    hsocket_t sock = (hsocket_t)(hintptr)arg;
#ifdef O_NONBLOCK
    hint32 fl;
    fl =  fcntl(sock, F_GETFL) ;
    fcntl(sock, F_SETFL, fl | O_NONBLOCK);
#endif
    impl->sock = sock;
    impl->base.has_data = stream_tcp_has_data;
    impl->base.has_space = stream_tcp_has_space;
    impl->base.read = stream_tcp_read;
    impl->base.write = stream_tcp_write;
}

static void stream_tcp_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    stream_tcp_t * tcp = (stream_tcp_t *)obj;
    stream_socket_close(tcp->sock);
}

static ext_class_t stream_tcp_ext_class = {stream_tcp_ext_init,stream_tcp_ext_destroy,sizeof(stream_tcp_t)};

stream_t * stream_tcp_alloc(hsocket_t sock,InvokeTickDeclare){
    return (stream_t *) ext_obj_alloc((hany)(hintptr)sock, &stream_tcp_ext_class);
}


typedef struct{
    stream_t base;
    hsocket_t sock;
    hchar * hostname;
    hint32 port;
}stream_host_impl_t;

typedef struct{
    hcchar * hostname;
    hint32 port;
}stream_host_impl_param_t;

static StreamState stream_host_has_data(stream_t * stream,hdouble timeout){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    if(impl->sock != -1){
        return stream_socket_has_data(impl->sock, timeout);
    }
    return StreamStateError;
}

static StreamState stream_host_has_space(stream_t * stream ,hdouble timeout){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    if(impl->sock != -1){
        return stream_socket_has_space(impl->sock, timeout);
    }
    return StreamStateError;
}

static hint32 stream_host_read(stream_t * stream, hany data,hint32 length){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    if(impl->sock != -1){
        return stream_socket_read(impl->sock, data,length);
    }
    return 0;
}

static hint32 stream_host_write(stream_t * stream, hany data,hint32 length){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    if(impl->sock != -1){
        return stream_socket_write(impl->sock, data,length);
    }
    return 0;
}

static hbool stream_host_is_connected(stream_t * stream){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    return impl->sock != -1;
}

static StreamState stream_host_connect(stream_t * stream,hdouble timeout){
    stream_host_impl_t * impl = (stream_host_impl_t *) stream;
    StreamState state = StreamStateNone;
    if(impl->sock != -1){
        state = stream_host_has_space(stream, timeout);
        if(state == StreamStateError){
            stream_socket_close(impl->sock);
            impl->sock = -1;
            state = StreamStateNone;
        }
    }
    
    if(impl->sock == -1){
        {
            hsocket_t sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            struct sockaddr_in addr ;
            hint32 res ;
#ifdef O_NONBLOCK
            hint32 fl;
#endif
            if(sock == -1){
                state = StreamStateError;
            }
            else{
                memset(&addr, 0, sizeof(struct sockaddr_in));
                addr.sin_family = AF_INET;
                addr.sin_port = 0;
                addr.sin_addr.s_addr = INADDR_ANY;
                
                res = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
                
                if(res != 0){
                    hlog("%s:%d socket bind error\n",impl->hostname,impl->port);
                    closesocket(sock);
                    state = StreamStateError;
                }
                else{
                    
#ifdef O_NONBLOCK
                    fl =  fcntl(sock, F_GETFL) ;
                    fcntl(sock, F_SETFL, fl | O_NONBLOCK);
#endif
                    addr.sin_port = htons(impl->port);
                    addr.sin_addr = url_resolv(impl->hostname);
                    res = connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
                    if(res == -1){
                        if(errno == EINPROGRESS || errno ==EINTR){
                            impl->sock = sock;
                            state = StreamStateNone;
                        }
                        else{
                            closesocket(sock);
                            hlog("errno: %d\n",errno);
                            state = StreamStateError;
                        }
                    }
                    else{
                        impl->sock = sock;
                        state = res == 0 ? StreamStateNone : StreamStateOK;
                    }
                }
            }
            
        }
    }
    return state;
}

static void stream_host_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    stream_host_impl_t * impl =(stream_host_impl_t *)obj;
    stream_host_impl_param_t * param = (stream_host_impl_param_t *)arg;
    impl->sock = -1;    
    str_cpy(&impl->hostname, param->hostname);
    impl->port = param->port;
    impl->base.has_data = stream_host_has_data;
    impl->base.has_space = stream_host_has_space;
    impl->base.read = stream_host_read;
    impl->base.write = stream_host_write;
    impl->base.is_connected = stream_host_is_connected;
    impl->base.connect = stream_host_connect;
}

static void stream_host_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    stream_host_impl_t * impl = (stream_host_impl_t *)obj;
    if(impl->sock != -1){
        stream_socket_close(impl->sock);
    }
    if(impl->hostname){
        mem_free(impl->hostname);
    }
}

static ext_class_t stream_host_ext_class = {stream_host_ext_init,stream_host_ext_destroy,sizeof(stream_host_impl_t)};

stream_t * stream_tcp_alloc_with_host(hcchar * hostname,hint32 port,InvokeTickDeclare){
    stream_host_impl_param_t param = {hostname,port};
    return (stream_t *) ext_obj_alloc(&param, &stream_host_ext_class);
}


