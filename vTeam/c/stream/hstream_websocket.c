//
//  hstream_websocket.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hstream_websocket.h"
#include "hext_obj.h"
#include "hdata_alloc.h"
#include "md5.h"
#include "sha1.h"
#include "hbase64.h"
#include "hlog.h"

typedef enum{
    StreamWebSocketTypeNone,StreamWebSocketTypeV1,StreamWebSocketTypeV2
}StreamWebSocketType;

typedef struct{
    stream_t base;
    hsocket_t sock;
    StreamWebSocketType type;
    hbuffer_t buffer;
    huint32 index;
    hbuffer_t base64;
}stream_websocket_t;

typedef struct{
    hsocket_t sock;
    StreamWebSocketType type;
}stream_websocket_param_t;

static StreamState stream_websocket_has_data(struct _stream_t * stream,hdouble timeout){
    stream_websocket_t * tcp = (stream_websocket_t *)stream;
    InvokeTickBegin
    return buffer_length(tcp->base64) >0 || stream_socket_has_data(tcp->sock, timeout);
}

static StreamState stream_websocket_has_space(struct _stream_t * stream,hdouble timeout){
    stream_websocket_t * tcp = (stream_websocket_t *)stream;
    return stream_socket_has_space(tcp->sock, timeout);
}

static hint32 stream_websocket_read(struct _stream_t * stream, hany data,hint32 length){
    stream_websocket_t * tcp = (stream_websocket_t *)stream;
    InvokeTickBegin
    
    if(tcp->type == StreamWebSocketTypeV1){
        {
            hint32 l;
            hubyte b = 0;
            StreamState s;
            if(buffer_length(tcp->base64) - tcp->index ==0){
                buffer_clear(tcp->base64);
                tcp->index = 0;
                buffer_clear(tcp->buffer);
                if(stream_socket_read(tcp->sock,&b,sizeof(b)) == sizeof(b) && b ==0){
                    while((s = stream_socket_has_data(tcp->sock, 0))){
                        if(s == StreamStateOK){
                            
                            if(stream_socket_read(tcp->sock,&b,sizeof(b)) == sizeof(b)){
                                if(b == 0xff){
                                    hbase64_decode(buffer_to_str(tcp->buffer), tcp->base64);
                                    buffer_clear(tcp->buffer);
                                    break;
                                }
                                else{
                                    buffer_append(tcp->buffer, &b, 1);
                                }
                            }
                            else{
                                return 0;
                            }

                        }
                        else if(s == StreamStateError){
                            return 0;
                        }
                    }
                }
                else{
                    return 0;
                }
            }
            if(buffer_length(tcp->base64)  - tcp->index ==0){
                return 0;
            }
            else{
                l = buffer_length(tcp->base64) - tcp->index;
                if(l >= length){
                    mem_memcpy(data, buffer_data(tcp->base64) + tcp->index, length);
                    tcp->index += length;
                    if(buffer_length(tcp->base64) - tcp->index ==0){
                        buffer_clear(tcp->base64);
                        tcp->index = 0;
                    }
                    return length;
                }
                else{
                    mem_memcpy(data, buffer_data(tcp->base64) + tcp->index, l);
                    buffer_clear(tcp->base64);
                    tcp->index = 0;
                    return l;
                }
            }
        }
    }
    else if(tcp->type == StreamWebSocketTypeV2){
        
    }
    
    return stream_socket_read(tcp->sock, data,length);
}

static hint32 stream_websocket_write(struct _stream_t * stream, hany data,hint32 length){
    stream_websocket_t * tcp = (stream_websocket_t *)stream;
    InvokeTickBegin
    if(tcp->type == StreamWebSocketTypeV1){
        {
            hubyte b = 0;
            buffer_clear(tcp->buffer);
            buffer_append(tcp->buffer, &b,1);
            hbase64_encode(data, length, tcp->buffer);
            b = 0xff;
            buffer_append(tcp->buffer, &b,1);
            if( stream_socket_write(tcp->sock, buffer_data(tcp->buffer),buffer_length(tcp->buffer)) == buffer_length(tcp->buffer)){
                buffer_clear(tcp->buffer);
                return length;
            }
            else{
                return 0;
            }
        }
    }
    
    return  stream_socket_write(tcp->sock, data, length);
}


static void stream_websocket_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    stream_websocket_t * impl =(stream_websocket_t *)obj;
    stream_websocket_param_t * param = (stream_websocket_param_t *)arg;
    hsocket_t sock = param->sock;
#ifdef O_NONBLOCK
    hint32 fl;
    fl =  fcntl(sock, F_GETFL) ;
    fcntl(sock, F_SETFL, fl | O_NONBLOCK);
#endif
    impl->type = param->type;
    impl->sock = sock;
    impl->base.has_data = stream_websocket_has_data;
    impl->base.has_space = stream_websocket_has_space;
    impl->base.read = stream_websocket_read;
    impl->base.write = stream_websocket_write;
    impl->base64 = buffer_alloc(128, 1024);
    impl->buffer = buffer_alloc(128, 1024);
}

static void stream_websocket_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    stream_websocket_t * tcp = (stream_websocket_t *)obj;
    stream_socket_close(tcp->sock);
    buffer_dealloc(tcp->base64);
    buffer_dealloc(tcp->buffer);
}

static huint32 stream_websocket_key_decode(hchar * p,InvokeTickDeclare){
    hint32 count = 0;
    hdouble r = 0;
    while(*p != '\0'){
        if(*p >='0' && *p <= '9'){
            r = r * 10 + (*p - '0');
        }
        else if(*p == ' '){
            count ++;
        }
        p++;
    }
    return r / count;
}

static ext_class_t stream_tcp_ext_class = {stream_websocket_ext_init,stream_websocket_ext_destroy,sizeof(stream_websocket_t)};

stream_t * stream_websocket_alloc(hsocket_t sock,InvokeTickDeclare){
    
    hint32 length;
    hchar buffer[2048] = {0};
    hdata_t headers;
    hbuffer_t name = buffer_alloc(128, 128),value = buffer_alloc(256, 256)
    ,method = buffer_alloc(32, 32),path = buffer_alloc(128, 128),protocol = buffer_alloc(32, 32);
    hint32 i;
    stream_websocket_param_t param = {sock,StreamWebSocketTypeNone};
    
    length = (hint32)recv( sock, buffer, sizeof(buffer), 0);
    
    headers = hdata_object_alloc();
    
    {
        hchar * p = buffer;
        hint32 s = 0;
        huint32 seg = 0;
        md5_state_t md5;
        SHA1Context sha1;
        md5_byte_t digest[16];
        
        buffer_clear(name);
        buffer_clear(value);
        buffer_clear(method);
        buffer_clear(path);
        buffer_clear(protocol);
        
        while(*p != '\0'){
            hlog("%c",*p);
            if(s == 0){
                if(*p == ' '){
                    s = 1;
                }
                else {
                    buffer_append(method, p, 1);
                }
            }
            else if(s == 1){
                if(*p == ' '){
                    s = 2;
                }
                else{
                    buffer_append(path, p, 1);
                }
            }
            else if(s == 2){
                if(*p == '\n'){
                    s = 3;
                }
                else if(*p == '\r'){
                    
                }
                else{
                    buffer_append(protocol, p, 1);
                }
            }
            else if(s == 3){
                if(*p == '\n'){
                    p ++;
                    break;
                }
                else if(*p == '\r'){
                    
                }
                else if(*p == ':'){
                    s = 4;
                    if(p[1] == ' '){
                        hlog(" ");
                        p ++;
                    }
                }
                else{
                    buffer_append(name, p, 1);
                }
            }
            else if(s == 4){
                if(*p == '\r'){
                    
                }
                else if(*p == '\n'){
                    if(buffer_length(name) == 0 && buffer_length(value) ==0){
                        break;
                    }
                    hdata_object_put(headers,buffer_to_str(name), hdata_string_alloc(buffer_to_str(value)));
                    buffer_clear(name);
                    buffer_clear(value);
                    s = 3;
                }
                else{
                    buffer_append(value, p, 1);
                }
            }
            
            p++;
        }
        
        if(hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key", NULL)){
            
            buffer_clear(value);
            buffer_append_str(value, hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key", NULL));
            buffer_append_str(value, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
            
            SHA1Reset(&sha1);
            
            SHA1Input(&sha1, (unsigned char *)buffer_to_str(value), buffer_length(value));
            
            SHA1Result(&sha1);
            
            buffer_clear(name);
            
            for(i=0;i<5;i++){
                sha1.Message_Digest[i] = htonl(sha1.Message_Digest[i]);
            }
            
            hbase64_encode((hany)sha1.Message_Digest, sizeof(sha1.Message_Digest), name);
            
            buffer_clear(value);
            
            buffer_append_str(value, "HTTP/1.1 101 Switching Protocols\r\n");
            buffer_append_str(value, "Upgrade: websocket\r\n");
            buffer_append_str(value, "Connection: Upgrade\r\n");
            buffer_append_format(value, "Sec-WebSocket-Accept: %s\r\n",InvokeTickArg, buffer_to_str(name));
            
            buffer_append_str(value, "\r\n");
            
            send(sock, buffer_data(value), buffer_length(value), 0);
            
            hlog("%s",buffer_to_str(value));
            
            param.type = StreamWebSocketTypeV2;
            
        }
        else if(hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key1", NULL) 
                && hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key2", NULL)){
            
            md5_init(&md5);
            buffer_clear(value);
            
            seg = stream_websocket_key_decode((hchar *)hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key1", NULL), InvokeTickArg);
            seg = htonl(seg);
            buffer_append(value, &seg, sizeof(seg));
            seg = stream_websocket_key_decode((hchar *)hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Key2", NULL), InvokeTickArg);
            seg = htonl(seg);
            buffer_append(value, &seg ,sizeof(seg));
            buffer_append(value, p ,8);
            
            md5_append(&md5,(hany)buffer_data(value),buffer_length(value));
            
            md5_finish(&md5, digest);
            
            buffer_clear(value);
            
            buffer_append_str(value, "HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
            buffer_append_str(value, "Upgrade: WebSocket\r\n");
            buffer_append_str(value, "Connection: Upgrade\r\n");
            buffer_append_format(value, "Sec-WebSocket-Origin: %s\r\n", InvokeTickArg,hdata_object_string(&hdata_class, headers, "Origin", ""));
            buffer_append_format(value, "Sec-WebSocket-Location: ws://%s%s\r\n",InvokeTickArg
                                 ,hdata_object_string(&hdata_class, headers, "Host", "")
                                 ,buffer_to_str(path));
            if(hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Protocol", NULL)){
                buffer_append_format(value, "Sec-WebSocket-Protocol: %s\r\n", InvokeTickArg,hdata_object_string(&hdata_class, headers, "Sec-WebSocket-Protocol", ""));
            }
            buffer_append_str(value, "\r\n");
            
            hlog("%s",buffer_to_str(value));
            
            buffer_append(value, digest, sizeof(digest));
            
            length = (hint32)send(sock, buffer_data(value), buffer_length(value), 0);
            
            param.type = StreamWebSocketTypeV1;

        }
    }
    
    hdata_dealloc(headers);
    
    buffer_dealloc(name);
    buffer_dealloc(value);
    buffer_dealloc(method);
    buffer_dealloc(path);
    buffer_dealloc(protocol);
    
    if(param.type != StreamWebSocketTypeNone){
        return (stream_t *) ext_obj_alloc(&param, &stream_tcp_ext_class);
    }
    return NULL;
}
