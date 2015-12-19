//
//  hvermin_debug.h
//  hcvermin
//
//  Created by Zhang Hailong on 13-3-17.
//
//

#ifndef hcvermin_hvermin_debug_h
#define hcvermin_hvermin_debug_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hstream.h"
#include "hvermin_runtime.h"
    
    
    typedef struct {
        hubyte type;
        union {
            struct{
                hubyte UDID[16];
                vmClassMetaOffset offset;
            };
        };
    } vmRuntimeDebugCommand;
    
    typedef enum {
        vmRuntimeDebugCommandTypeNone,vmRuntimeDebugCommandTypeBreakpoint,vmRuntimeDebugCommandTypeContinue
    }vmRuntimeDebugCommandType;
    
    vmRuntimeDebug vmRuntimeDebugAlloc(hint32 port,InvokeTickDeclare);
    
    vmRuntimeDebug vmRuntimeDebugRetain(vmRuntimeDebug debug,InvokeTickDeclare);
    
    void vmRuntimeDebugRelease(vmRuntimeDebug debug,InvokeTickDeclare);
    
    void vmRuntimeDebugExec(vmRuntimeDebug debug,vmClassMeta * classMeta,vmRuntimeMetaOperator * op,InvokeTickDeclare);
    
    hint32 vmRuntimeDebugGetPort(vmRuntimeDebug debug,InvokeTickDeclare);
    
    StreamState vmRuntimeDebugAccept(vmRuntimeDebug debug,hdouble timeout,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
