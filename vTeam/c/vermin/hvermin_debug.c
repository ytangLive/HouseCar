//
//  hvermin_debug.c
//  hcvermin
//
//  Created by Zhang Hailong on 13-3-17.
//
//

#include "hconfig.h"
#include "hvermin_debug.h"
#include "hext_obj.h"
#include "hstream_tcp.h"

typedef struct {
    hsocket_t sock;
    stream_t * stream;
    hint32 port;
} _vmRuntimeDebug;

static void vmRuntimeDebugInit(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    _vmRuntimeDebug * debug = (_vmRuntimeDebug *) obj;
    struct sockaddr_in addr ;
    socklen_t len = sizeof(struct sockaddr_in);
#ifdef O_NONBLOCK
    hint32 fl;
#endif

    debug->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(debug->sock != -1){
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons((hintptr)arg);
        addr.sin_addr.s_addr = INADDR_ANY;
        
        bind(debug->sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        
        listen(debug->sock, 0);
        
#ifdef O_NONBLOCK
        fl =  fcntl(debug->sock, F_GETFL) ;
        fcntl(debug->sock, F_SETFL, fl | O_NONBLOCK);
#endif
        getsockname(debug->sock, (struct sockaddr *) & addr, &len);
        
        debug->port = ntohs(addr.sin_port);
        
    }
}

static void vmRuntimeDebugDestroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    _vmRuntimeDebug * debug = (_vmRuntimeDebug *) obj;
    if(debug->sock != -1){
        stream_socket_close(debug->sock);
    }
    stream_release(debug->stream,InvokeTickArg);
}

static ext_class_t vmRuntimeDebugClass = {vmRuntimeDebugInit,vmRuntimeDebugDestroy,sizeof(_vmRuntimeDebug)};


vmRuntimeDebug vmRuntimeDebugAlloc(hint32 port,InvokeTickDeclare){
    return (vmRuntimeDebug) ext_obj_alloc((hany)(hintptr) port, &vmRuntimeDebugClass);
}

vmRuntimeDebug vmRuntimeDebugRetain(vmRuntimeDebug debug,InvokeTickDeclare){
    return (vmRuntimeDebug) ext_obj_retain((hext_obj_t) debug);
}

void vmRuntimeDebugRelease(vmRuntimeDebug debug,InvokeTickDeclare){
    ext_obj_release((hext_obj_t) debug);
}

hint32 vmRuntimeDebugGetPort(vmRuntimeDebug debug,InvokeTickDeclare){
    if(debug){
        _vmRuntimeDebug * dbg = (_vmRuntimeDebug *) debug;
        return dbg->port;
    }
    return 0;
}


void vmRuntimeDebugExec(vmRuntimeDebug debug,vmClassMeta * classMeta,vmRuntimeMetaOperator * op,InvokeTickDeclare){
    if(debug){
        _vmRuntimeDebug * dbg = (_vmRuntimeDebug *) debug;
        StreamState s;
        vmRuntimeDebugCommand command;
        vmRuntimeClassLibraryBytes * bytes = (vmRuntimeClassLibraryBytes* )((hbyte *) classMeta - classMeta->offset);
        memset(&command, 0, sizeof(command));
        
        if(dbg->stream){
            while(1){
                s = stream_has_space(dbg->stream, 0.2, InvokeTickArg);
                if(s == StreamStateOK){
                    
                    command.type = vmRuntimeDebugCommandTypeBreakpoint;
                    command.offset = (vmClassMetaOffset)( (hubyte *)op - (hubyte *)bytes );
                    memcpy(command.UDID, bytes->UDID, sizeof(command.UDID));
                    
                    stream_write(dbg->stream, &command, sizeof(command),InvokeTickArg);
                    
                    break;
                }
                else if(s == StreamStateError){
                    stream_release(dbg->stream, InvokeTickArg);
                    dbg->stream = NULL;
                    break;
                }
            }
        }
        
        if(dbg->stream){
            while (1) {
                s = stream_has_data(dbg->stream, 0.2, InvokeTickArg);
                if(s == StreamStateOK){
                    if(sizeof(command) == stream_read(dbg->stream, &command, sizeof(command), InvokeTickArg)){
                        if(command.type == vmRuntimeDebugCommandTypeContinue){
                            break;
                        }
                    }
                    else{
                        break;
                    }
                }
                else if(s == StreamStateError){
                    stream_release(dbg->stream, InvokeTickArg);
                    dbg->stream = NULL;
                    break;
                }
            }
        }
    }
}

StreamState vmRuntimeDebugAccept(vmRuntimeDebug debug,hdouble timeout,InvokeTickDeclare){
    if(debug){
        _vmRuntimeDebug * dbg = (_vmRuntimeDebug *) debug;
        StreamState s;
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        hsocket_t sock;
        if(dbg->stream == NULL){
            s = stream_socket_has_data(dbg->sock, timeout);
            if(s == StreamStateOK){
                sock = accept(dbg->sock, (struct sockaddr *) &addr, &len);
                if(sock != -1){
                    dbg->stream = stream_tcp_alloc(sock, InvokeTickArg);
                    return StreamStateOK;
                }
                return StreamStateError;
            }
            else if(s == StreamStateError){
                printf("%d\n",errno);
            }
            return s;
        }
        return StreamStateOK;
    }
    return StreamStateError;
}
