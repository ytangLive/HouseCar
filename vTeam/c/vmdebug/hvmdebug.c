//
//  hvmdebug.c
//  vmdebug
//
//  Created by Zhang Hailong on 13-3-17.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#include "hconfig.h"
#include "hvmdebug.h"
#include "hstream_tcp.h"

static void vmDebugDestroy (vmDebug  * obj,InvokeTickDeclare);

HOBJ_KEY_IMP(vmDebug)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmDebug)

HOBJ_CLASS_METHOD_IMP(destroy, vmDebugDestroy)

HOBJ_CLASS_METHOD_IMP_END(vmDebug)

HOBJ_CLASS_IMP_I(vmDebug, vmDebug, &hobj_class)


static void vmDebugDestroy (vmDebug  * obj,InvokeTickDeclare){
    hobj_method_t * method;
    
    if(obj->stream){
        stream_release(obj->stream, InvokeTickArg);
    }
    
    method = hobj_class_method(obj->base.clazz->superClass, HOBJ_KEY(destroy), InvokeTickArg);
    
    if(method){
        (* (hobj_method_destroy_impl_t) method->imp)((hobj_t *) obj,InvokeTickArg);
    }
}

vmDebug * vmDebugAlloc(hcchar * hostname,hint32 port,InvokeTickDeclare){
    vmDebug * debug = (vmDebug *) hobj_alloc(HOBJ_CLASS(vmDebug), InvokeTickArg);
    debug->stream = stream_tcp_alloc_with_host(hostname, port, InvokeTickArg);
    return debug;
}

StreamState vmDebugConnect(vmDebug * debug,hdouble timeout,InvokeTickDeclare){
    return stream_connect(debug->stream, timeout, InvokeTickArg);
}

StreamState vmDebugReadCommand(vmDebug * debug, vmRuntimeDebugCommand * command,hdouble timeout,InvokeTickDeclare){
    StreamState s = stream_has_data(debug->stream, timeout, InvokeTickArg);
    if(s == StreamStateOK){
        if( sizeof(vmRuntimeDebugCommand)
           == stream_read(debug->stream, command, sizeof(vmRuntimeDebugCommand), InvokeTickArg)){
            return StreamStateOK;
        }
        return StreamStateError;
    }
    return s;
}

StreamState vmDebugWriteCommand(vmDebug * debug, vmRuntimeDebugCommand * command,hdouble timeout,InvokeTickDeclare){
    StreamState s = stream_has_space(debug->stream, timeout, InvokeTickArg);
    if(s == StreamStateOK){
        if( sizeof(vmRuntimeDebugCommand)
           == stream_write(debug->stream, command, sizeof(vmRuntimeDebugCommand), InvokeTickArg)){
            return StreamStateOK;
        }
        return StreamStateError;
    }
    return s;
}
