//
//  hvmdebug.h
//  vmdebug
//
//  Created by Zhang Hailong on 13-3-17.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef vmdebug_hvmdebug_h
#define vmdebug_hvmdebug_h


#ifdef __cplusplus
extern "C" {
#endif

#include "hobjc.h"
#include "hvermin_debug.h"
#include "hstream.h"
    
    
    struct _map_t;
    struct _list_t;
    
    typedef struct _vmDebug {
        hobj_t base;
        stream_t * stream;
    } vmDebug;
    
    vmDebug * vmDebugAlloc(hcchar * hostname,hint32 port,InvokeTickDeclare);
    
    StreamState vmDebugConnect(vmDebug * debug,hdouble timeout,InvokeTickDeclare);
    
    StreamState vmDebugReadCommand(vmDebug * debug, vmRuntimeDebugCommand * command,hdouble timeout,InvokeTickDeclare);
    
    StreamState vmDebugWriteCommand(vmDebug * debug, vmRuntimeDebugCommand * command,hdouble timeout,InvokeTickDeclare);
    
    typedef struct _vmDebugSource{
        struct _map_t * vms;
        struct _map_t * vmbs;
    } vmDebugSource;
    
    HOBJ_KEY_DEC(vmDebug)
    HOBJ_CLASS_DEC(vmDebug)
    
#ifdef __cplusplus
}
#endif


#endif
