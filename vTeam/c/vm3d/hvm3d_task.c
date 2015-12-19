//
//  hvm3d_task.c
//  hvm3d
//
//  Created by zhang hailong on 13-3-19.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hvm3d_task.h"
#include "hvermin_runtime.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hlog.h"
#include "hvm3d.h"

static void vmC3DTaskFrameTick(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    vmC3DTask * vmTask  = C3DTASK_TO_VMC3DTASK(task);
    vmC3DContext * ctx = C3DCONTEXT_TO_VMC3DCONTEXT(context);
    vmVariant v = {0};
    vmVariantList args = vmVariantListAlloc();
    
    v.type = vmVariantTypeObject;
    v.value.objectValue = (vmObject *)ctx;
    
    vmVariantListAdd(args, v);
    
    vmObjectInvoke(vmTask->runtimeContext, vmTask->base.vmClass, (vmObject *)vmTask, vmTask->uniqueKeys.frame,args);
    
    vmVariantListDealloc(args);
}

static hbool vmC3DTaskFinish(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    vmC3DTask * vmTask  = C3DTASK_TO_VMC3DTASK(task);
    vmC3DContext * ctx = C3DCONTEXT_TO_VMC3DCONTEXT(context);
    vmVariant v = {0} , rs;
    vmVariantList args = vmVariantListAlloc();
    
    v.type = vmVariantTypeObject;
    v.value.objectValue = (vmObject *)ctx;
    
    vmVariantListAdd(args, v);

    rs = vmObjectInvoke(vmTask->runtimeContext, vmTask->base.vmClass, (vmObject *)vmTask, vmTask->uniqueKeys.isFinish,args);
    
    vmVariantListDealloc(args);
    
    return vmVariantToBoolean(vmTask->runtimeContext, rs);
}

static void vmC3DTaskRelease(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    vmC3DTask * vmTask  = C3DTASK_TO_VMC3DTASK(task);
    vmObjectRelease(vmTask->runtimeContext, (vmObject *) vmTask);
}

static void vmC3DTaskRetain(struct _c3d_context_t * context , struct _c3d_task_t * task,InvokeTickDeclare) {
    vmC3DTask * vmTask  = C3DTASK_TO_VMC3DTASK(task);
    vmObjectRetain(vmTask->runtimeContext, (vmObject *) vmTask);
}

static void vmC3DTaskStart(struct _c3d_context_t * context,struct _c3d_task_t * task,InvokeTickDeclare){
    vmC3DTask * vmTask  = C3DTASK_TO_VMC3DTASK(task);
    vmC3DContext * ctx = C3DCONTEXT_TO_VMC3DCONTEXT(context);
    vmVariant v = {0} ;
    vmVariantList args = vmVariantListAlloc();
    
    v.type = vmVariantTypeObject;
    v.value.objectValue = (vmObject *)ctx;
    
    vmVariantListAdd(args, v);

    vmObjectInvoke(vmTask->runtimeContext, vmTask->base.vmClass, (vmObject *)vmTask, vmTask->uniqueKeys.start,args);
    
    vmVariantListDealloc(args);
}


/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTaskInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTask * task = (vmC3DTask *) object;
    
    task->task.finish = vmC3DTaskFinish;
    task->task.start = vmC3DTaskStart;
    task->task.frameTick = vmC3DTaskFrameTick;
    task->task.release = vmC3DTaskRelease;
    task->task.retain = vmC3DTaskRetain;
    
    task->uniqueKeys.start = vmRuntimeContextGetUniqueKey(context,"start");
    task->uniqueKeys.frame = vmRuntimeContextGetUniqueKey(context,"frame");
    task->uniqueKeys.isFinish = vmRuntimeContextGetUniqueKey(context,"isFinish");
    task->uniqueKeys.target = vmRuntimeContextGetUniqueKey(context,"target");
    task->runtimeContext = context;
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTaskDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DTaskGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTask * task = (vmC3DTask *) object;
    
    if(task->uniqueKeys.target == name){
        if(task->task.target){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *) task->task.target;
        }
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name,InvokeTickArg);
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DTaskSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    vmC3DTask * task = (vmC3DTask *) object;
    
    if(task->uniqueKeys.target == name){
        if(value.type & vmVariantTypeObject && value.value.objectValue){
            task->task.target = value.value.objectValue;
        }
        else{
            task->task.target = NULL;
        }
    }
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DTaskInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
}

/**
 
 Return : Void
 */
static void vmC3DTaskPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}

vmClassBase vmC3DTaskClass = {
    vmClassTypeBase,
    sizeof(vmC3DTask),
    vmC3DTaskInitCallback,
    vmC3DTaskDestroyCallback,
    vmC3DTaskGetPropertyCallback,
    vmC3DTaskSetPropertyCallback,
    vmC3DTaskPropertyNamesCallback,
    vmC3DTaskInvokeCallback,
};
