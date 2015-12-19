//
//  hvm3d_task.h
//  hvm3d
//
//  Created by zhang hailong on 13-3-19.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#ifndef hvm3d_hvm3d_task_h
#define hvm3d_hvm3d_task_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin.h"
#include "h3dnode.h"
#include "h3dcontext.h"

    extern vmClassBase vmC3DTaskClass;

    typedef struct _vmC3DTask{
        vmObject base;
        c3d_task_t task;
        struct {
            vmUniqueKey start;
            vmUniqueKey frame;
            vmUniqueKey isFinish;
            vmUniqueKey target;
        } uniqueKeys;
        vmRuntimeContext runtimeContext;
    } vmC3DTask;

#define C3DTASK_TO_VMC3DTASK(task)  (vmC3DTask *) ((hbyte *) (task) - sizeof(vmObject))

    
#ifdef __cplusplus
}
#endif



#endif
