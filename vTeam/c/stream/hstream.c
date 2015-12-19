//
//  hstream.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-7.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hstream.h"
#include "hext_obj.h"
#include "hserial_list.h"
#include "hstr.h"
#include "hurl.h"
#include "hlog.h"
#include "hprocess.h"
#include "hbase64.h"

void stream_socket_close(hsocket_t sock){
    fd_set ds,ws;
    struct timeval timeo = {0, 0};
    hbyte buffer[1024];
    hint32 len,res;
    
    timeo.tv_usec = 100;
    
    while(hbool_true){
        FD_ZERO(&ws);
        FD_SET(sock, &ws);
        res = select(sock +1, NULL,&ws, NULL, &timeo);
        if(res == 0){
            continue;
        }
        if(res == -1){
            break;
        }
        shutdown(sock, SHUT_WR);
        break;
    }
    
    while(hbool_true){
        FD_ZERO(&ds);
        FD_SET(sock, &ds);
        res = select(sock +1, &ds,NULL, NULL, &timeo);
        if(res == 0){
            shutdown(sock, SHUT_RD);
            break;
        }
        if(res == -1){
            break;
        }
        else if(FD_ISSET(sock, &ds)){
            while((len = (hint32)recv(sock, buffer, sizeof(buffer), 0)) >0);
            shutdown(sock, SHUT_RD);
            break;
        }
    }
    
    closesocket(sock);
}

StreamState stream_socket_has_data(hsocket_t sock,hdouble timeout){
    StreamState state = StreamStateNone;
    hint32 res;
    fd_set rds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000000)};
    FD_ZERO(&rds);
    
    FD_SET(sock, &rds);
    
    res = select(sock + 1, &rds, NULL, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        if(errno == EINTR){
            state = StreamStateNone;
        }
        else{
            state = StreamStateError;
        }
    }
    else{
        if(FD_ISSET(sock, &rds)){
            state = StreamStateOK;
        }
    }
    
    return state;
}

StreamState stream_socket_has_space(hsocket_t sock ,hdouble timeout){
    StreamState state = StreamStateNone;
    hint32 res;
    fd_set wds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000000)};
    FD_ZERO(&wds);
    
    FD_SET(sock, &wds);
    
    res = select(sock + 1, NULL, &wds, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        if(errno == EINTR){
            state = StreamStateNone;
        }
        else{
            state = StreamStateError;
        }
    }
    else{
        if(FD_ISSET(sock, &wds)){
            state = StreamStateOK;
        }
    }
    
    return state;
}

hint32 stream_socket_read(hsocket_t sock, hany data,hint32 length){
    hint32 rs = (hint32)recv(sock,data,length,0);
    if(rs == -1){
        return 0;
    }
    return rs;
}

hint32 stream_socket_write(hsocket_t sock, hany data,hint32 length){
    hint32 rs = (hint32)send(sock,data,length,0);
    if(rs == -1){
        return 0;
    }
    return rs;
}


stream_t * stream_retain(stream_t * stream,InvokeTickDeclare){
    return (stream_t *)ext_obj_retain((hext_obj_t)stream);
}

void stream_release(stream_t * stream,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)stream);
}


StreamState stream_has_data(stream_t * stream,hdouble timeout,InvokeTickDeclare){
    if(stream->has_data){
        return (* stream->has_data)(stream,timeout);
    }
    return StreamStateError;
}

StreamState stream_has_space(stream_t * stream ,hdouble timeout,InvokeTickDeclare){
    if(stream->has_space){
        return (* stream->has_space)(stream,timeout);
    }
    return StreamStateError;
}

hint32 stream_read(stream_t * stream, hany data,hint32 length,InvokeTickDeclare){
    if(stream->read){
        return (* stream->read)(stream,data,length);
    }
    return 0;
}

hint32 stream_write(stream_t * stream, hany data,hint32 length,InvokeTickDeclare){
    if(stream->write){
        return (* stream->write)(stream,data,length);
    }
    return 0;
}

StreamState stream_connect(stream_t * stream,hdouble timeout,InvokeTickDeclare){
    if(stream->connect){
        return (* stream->connect)(stream,timeout);
    }
    return StreamStateError;
}

hbool stream_is_connected(stream_t * stream,InvokeTickDeclare){
    if(stream->is_connected){
        return (* stream->is_connected)(stream);
    }
    return hbool_true;
}

