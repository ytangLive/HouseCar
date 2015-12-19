//
//  hvm3d.c
//  hvm3d
//
//  Created by Zhang Hailong on 13-3-17.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hvm3d.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hlist.h"
#include "h3dcontext.h"
#include "hvm3d_render.h"
#include "hlog.h"
#include "hvm3d_task.h"
#include "hstr.h"


static hbool vmC3DNode_hidden_fun(struct _c3d_node_t * node,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    return vmNode->property.hidden;
}

static c3d_node_condition_t vmC3DNode_condition_fun(struct _c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    c3d_node_condition_t cond = c3d_node_condition_identity();
    
    cond.transform = vmNode->transform;
    cond.clips.x = cond.clips.y = 0;
    cond.clips.width = node->context->viewport.width;
    cond.clips.height = node->context->viewport.height;
    cond.bounds.x = cond.bounds.y = 0;
    cond.bounds.width = node->context->viewport.width;
    cond.bounds.height = node->context->viewport.height;
    
    if(vmNode->render && vmNode->render->condition){
        return ( * vmNode->render->condition)(vmNode->render,vmNode,condition,InvokeTickArg);
    }
    
    return c3d_node_condition_multiply(condition, cond);
}

static hint32 vmC3DNode_child_count_fun(struct _c3d_node_t * node,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    return list_count(vmNode->childs);
}

static struct _c3d_node_t * vmC3DNode_child_at_fun(struct _c3d_node_t * node,hint32 index,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    vmC3DNode * child = (vmC3DNode *) list_get(vmNode->childs, index);
    if(child){
        return &child->node;
    }
    return NULL;
}

static hbool vmC3DNode_draw_fun(struct _c3d_node_t * node,c3d_node_condition_t condition,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    
    if(vmNode->render && vmNode->render->draw){
        return (* vmNode->render->draw)(vmNode->render,vmNode,condition,InvokeTickArg);
    }
    
    return hbool_true;
}

static void vmC3DNode_connect_fun(struct _c3d_node_t * node,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    vmObjectInvoke(vmNode->runtimeContext, vmNode->base.vmClass, (vmObject *)vmNode, vmNode->uniqueKeys.connect, NULL);
}

static hbool vmC3DNode_receive_touches_fun(struct _c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    if(vmNode->render && vmNode->render->receiveTouches){
        return (* vmNode->render->receiveTouches)(vmNode->render,vmNode,condition,touches,InvokeTickArg);
    }
    return vmNode->property.receiveTouches;
}

static hbool vmC3DNode_touches_fun(struct _c3d_node_t * node,c3d_node_condition_t condition,c3d_touches_t * touches, InvokeTickDeclare){
    vmC3DNode * vmNode = C3DNODE_TO_VMC3DNODE(node);
    vmVariantList args = vmVariantListAlloc();
    vmVariant v ;
    vmC3DTouches * tchs;
    vmException * exp;
    
    v = vmObjectAlloc(vmNode->runtimeContext, &vmC3DTouchesClass, NULL);
    
    tchs = (vmC3DTouches *) v.value.objectValue;
    
    tchs->transform = vmNode->transform;
    tchs->target = vmNode;
    tchs->touches = * touches;
    
    vmVariantListAdd(args, v);
    
    v = vmObjectInvoke(vmNode->runtimeContext, vmNode->base.vmClass, (vmObject *)vmNode, vmNode->uniqueKeys.touches, args);
    
    vmVariantListDealloc(args);
    
    if(v.type & vmVariantTypeThrowable){
        if(vmRuntimeContextObjectIsKindClass(vmNode->runtimeContext, v.value.objectValue, (vmClass *) &vmExceptionClass)){
            exp = (vmException *) v.value.objectValue;
            hlog("vmC3DNode_touches_fun (%d)%s\n",exp->code,exp->error);
        }
    }
    
    return vmVariantToBoolean(vmNode->runtimeContext, v) || tchs->cancelBubble;
}


static c3d_node_class_t vmC3DNodeNodeClass = {
    vmC3DNode_draw_fun,
    vmC3DNode_hidden_fun,
    vmC3DNode_condition_fun,
    vmC3DNode_child_count_fun,
    vmC3DNode_child_at_fun,
    vmC3DNode_connect_fun,
    vmC3DNode_receive_touches_fun,
    vmC3DNode_touches_fun,
};

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DNodeInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DNode * node = (vmC3DNode *) object;
    vmVariant v = vmVariantListGet(args, 0);
   
    node->node.pClass = &vmC3DNodeNodeClass;
    
    node->property.hidden = hbool_false;
    node->property.receiveTouches = hbool_true;
    node->property.transform = c3d_matrix4_identity;
    node->property.alpha = 1.0;
    node->transform = c3d_matrix4_identity;
    
    node->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context,"length");
    node->uniqueKeys.childAt = vmRuntimeContextGetUniqueKey(context,"childAt");
    node->uniqueKeys.removeFromParent = vmRuntimeContextGetUniqueKey(context,"removeFromParent");
    node->uniqueKeys.addChild = vmRuntimeContextGetUniqueKey(context,"addChild");
    node->uniqueKeys.insertChild = vmRuntimeContextGetUniqueKey(context,"insertChild");
    node->uniqueKeys.hidden = vmRuntimeContextGetUniqueKey(context,"hidden");
    node->uniqueKeys.connect = vmRuntimeContextGetUniqueKey(context,"connect");
    node->uniqueKeys.disconnect = vmRuntimeContextGetUniqueKey(context,"disconnect");
    node->uniqueKeys.receiveTouches = vmRuntimeContextGetUniqueKey(context,"receiveTouches");
    node->uniqueKeys.transform = vmRuntimeContextGetUniqueKey(context,"transform");
    node->uniqueKeys.alpha = vmRuntimeContextGetUniqueKey(context,"alpha");
    node->uniqueKeys.context = vmRuntimeContextGetUniqueKey(context,"context");
    node->uniqueKeys.touches = vmRuntimeContextGetUniqueKey(context,"touches");
    node->uniqueKeys.parent = vmRuntimeContextGetUniqueKey(context,"parent");
    node->uniqueKeys.toFront = vmRuntimeContextGetUniqueKey(context, "toFront");
    node->uniqueKeys.toBack = vmRuntimeContextGetUniqueKey(context, "toBack");
    node->uniqueKeys.render = vmRuntimeContextGetUniqueKey(context, "render");
    
    node->runtimeContext = context;
    

    if(( v.type & vmVariantTypeObject ) && v.value.objectValue){
        node->render = (vmC3DRender *)vmObjectRetain(context, v.value.objectValue);
    }
    

    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DNodeDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DNode * node = (vmC3DNode *) object;
    hint32 i , c = list_count(node->childs);
    
    for(i=0;i<c;i++){
        vmObjectRelease(context, (vmObject *) list_get(node->childs, i));
    }
    
    list_dealloc(node->childs);

    if(node->render){
        vmObjectRelease(context, (vmObject *)node->render);
    }
   
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DNodeGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DNode * node = (vmC3DNode *) object;
    if(name == node->uniqueKeys.length){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = list_count(node->childs);
    }
    else if(name == node->uniqueKeys.hidden){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = node->property.hidden;
    }
    else if(name == node->uniqueKeys.parent){
        if(node->property.parent){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *)node->property.parent;
        }
    }
    else if(name == node->uniqueKeys.alpha){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = node->property.alpha;
    }
    else if(name == node->uniqueKeys.receiveTouches){
        rs.type = vmVariantTypeBoolean;
        rs.value.doubleValue = node->property.receiveTouches;
    }
    else if(name == node->uniqueKeys.transform){
        {
            vmC3DTransform * t ;
            rs = vmObjectAlloc(context, &vmC3DTransformClass, NULL);
            t = (vmC3DTransform *) rs.value.objectValue;
            t->transform = node->property.transform;
        }
    }
    else if(name == node->uniqueKeys.context){
        if(node->node.context){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *) C3DCONTEXT_TO_VMC3DCONTEXT(node->node.context);
        }
    }
    else if(name == node->uniqueKeys.render){
        if(node->render){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *) node->render;
        }
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DNodeSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DNode * node = (vmC3DNode *) object;
    
    if(name == node->uniqueKeys.hidden){
        node->property.hidden = vmVariantToBoolean(context,value);
    }
    else if(name == node->uniqueKeys.alpha){
        node->property.alpha = vmVariantToDouble(context,value);
    }
    else if(name == node->uniqueKeys.receiveTouches){
        node->property.receiveTouches = vmVariantToDouble(context,value);
    }
    else if(name == node->uniqueKeys.transform){
        {
            vmC3DTransform * t;
            if((value.type & vmVariantTypeObject) && value.value.objectValue
               && vmRuntimeContextObjectIsKindClass(context,value.value.objectValue,(vmClass *)&vmC3DTransformClass)){
                t = (vmC3DTransform *) value.value.objectValue;
                node->property.transform = t->transform;
            }
        }
    }
    else{
        return vmObjectClassSetPropertyCallback(context, clazz, object, name,value, InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DNodeInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DNode * node = (vmC3DNode *) object;
    
    if(node->uniqueKeys.addChild == name){
        {
            vmVariant v = vmVariantListGet(args, 0);
            vmC3DNode * child;
            if((v.type & vmVariantTypeObject) && v.value.objectValue
               && vmRuntimeContextObjectIsKindClass(context, v.value.objectValue, (vmClass *)&vmC3DNodeClass)){
                
                child = (vmC3DNode *) v.value.objectValue;

                if(child->property.parent != node){
                    if(node->childs == NULL){
                        node->childs = list_alloc(4, 4);
                    }
                    
                    list_add(node->childs, child);
                    vmObjectRetain(context, (vmObject *)child);
                    
                    if(child->property.parent){
                        list_remove(child->property.parent->childs, child);
                        vmObjectRelease(context, (vmObject *)child);
                    }
                    
                    child->property.parent = node;
                    
                    if(node->node.context){
                        if(child->node.context != node->node.context){
                            child->node.context = node->node.context;
                            (* node->node.pClass->connect)(&child->node,InvokeTickArg);
                        }
                        
                        node->node.context->needRender = hbool_true;
                    }
                }
            }
        }
        
    }
    else if(node->uniqueKeys.insertChild == name){
        {
            vmVariant v = vmVariantListGet(args, 0);
            vmC3DNode * child;
            if((v.type & vmVariantTypeObject) && v.value.objectValue
               && vmRuntimeContextObjectIsKindClass(context, v.value.objectValue,  (vmClass *)&vmC3DNodeClass)){
                
                child = (vmC3DNode *) v.value.objectValue;
                
                if(child->property.parent != node){
                    if(node->childs == NULL){
                        node->childs = list_alloc(4, 4);
                    }
                    
                    list_insert(node->childs, child,vmVariantToInt32(context, vmVariantListGet(args, 1)));
                    
                    vmObjectRetain(context, (vmObject *)child);
                    
                    if(child->property.parent){
                        list_remove(child->property.parent->childs, child);
                        vmObjectRelease(context, (vmObject *)child);
                    }
                    
                    child->property.parent = node;
                    
                    if(node->node.context){
                        if(child->node.context != node->node.context){
                            child->node.context = node->node.context;
                            (* node->node.pClass->connect)(&child->node,InvokeTickArg);
                        }
                        
                        node->node.context->needRender = hbool_true;
                    }
                }
            }
        }
        
    }
    else if(node->uniqueKeys.childAt == name){
        {
            vmVariant v = vmVariantListGet(args, 0);
            vmC3DNode * child = (vmC3DNode *) list_get(node->childs, vmVariantToInt32(context, v));
            
            if(child){
                rs.type = vmVariantTypeObject;
                rs.value.objectValue = (vmObject *) child;
            }
        }
        
    }
    else if(node->uniqueKeys.removeFromParent == name){
        {
            if(node->property.parent){
                list_remove(node->property.parent->childs, node);
                node->property.parent = NULL;
                if(node->node.context){
                    node->node.context->needRender = hbool_true;
                    vmObjectInvoke(context, node->base.vmClass, (vmObject *) node, node->uniqueKeys.disconnect, NULL);
                }
                vmObjectRelease(context, (vmObject *)node);
            }
        }
    }
    else if(node->uniqueKeys.toFront == name){
        if(node->property.parent){
            list_remove(node->property.parent->childs, node);
            list_add(node->property.parent->childs, node);
            node->node.context->needRender = hbool_true;
        }
    }
    else if(node->uniqueKeys.toBack == name){
        if(node->property.parent){
            list_remove(node->property.parent->childs, node);
            list_insert(node->property.parent->childs, node,0);
            node->node.context->needRender = hbool_true;
        }
    }
    else if(context->systemKeys.propertySetted == name){
        if(node->node.context){
            node->node.context->needRender = hbool_true;
        }
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmC3DNodePropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmC3DNode * node = (vmC3DNode *) object;
    vmUniqueKeyListAdd(names, node->uniqueKeys.length);
    vmUniqueKeyListAdd(names, node->uniqueKeys.hidden);
    vmUniqueKeyListAdd(names, node->uniqueKeys.parent);
    vmUniqueKeyListAdd(names, node->uniqueKeys.alpha);
    vmUniqueKeyListAdd(names, node->uniqueKeys.receiveTouches);
    vmUniqueKeyListAdd(names, node->uniqueKeys.transform);
    vmUniqueKeyListAdd(names, node->uniqueKeys.context);
    vmUniqueKeyListAdd(names, node->uniqueKeys.render);
}

vmClassBase vmC3DNodeClass = {
    vmClassTypeBase,
    sizeof(vmC3DNode),
    vmC3DNodeInitCallback,
    vmC3DNodeDestroyCallback,
    vmC3DNodeGetPropertyCallback,
    vmC3DNodeSetPropertyCallback,
    vmC3DNodePropertyNamesCallback,
    vmC3DNodeInvokeCallback,
};



/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTransformInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTransform * matrix = (vmC3DTransform *) object;
    
    matrix->transform = c3d_matrix4_identity;
    
    matrix->uniqueKeys.toValue = vmRuntimeContextGetUniqueKey(context,"toValue");
    matrix->uniqueKeys.translate = vmRuntimeContextGetUniqueKey(context,"translate");
    matrix->uniqueKeys.rotateX = vmRuntimeContextGetUniqueKey(context,"rotateX");
    matrix->uniqueKeys.rotateY = vmRuntimeContextGetUniqueKey(context,"rotateY");
    matrix->uniqueKeys.rotateZ = vmRuntimeContextGetUniqueKey(context,"rotateZ");
    matrix->uniqueKeys.scale = vmRuntimeContextGetUniqueKey(context,"scale");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTransformDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
       
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DTransformGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
        
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DTransformSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DTransformInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTransform * matrix = (vmC3DTransform *) object;
    
    if(name == matrix->uniqueKeys.translate){
        matrix->transform = c3d_matrix4_translate(matrix->transform
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 0))
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 1))
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 2)));
    }
    else if(name == matrix->uniqueKeys.rotateX){
        matrix->transform = c3d_matrix4_rotate_x(matrix->transform
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 0)) * M_PI / 180.0);
    }
    else if(name == matrix->uniqueKeys.rotateY){
        matrix->transform = c3d_matrix4_rotate_y(matrix->transform
                                                 , vmVariantToDouble(context, vmVariantListGet(args, 0)) * M_PI / 180.0);
    }
    else if(name == matrix->uniqueKeys.rotateZ){
        matrix->transform = c3d_matrix4_rotate_z(matrix->transform
                                                 , vmVariantToDouble(context, vmVariantListGet(args, 0)) * M_PI / 180.0);
    }
    else if(name == matrix->uniqueKeys.scale){
        matrix->transform = c3d_matrix4_scale(matrix->transform
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 0))
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 1))
                                                  , vmVariantToDouble(context, vmVariantListGet(args, 2)));
    }
    else if(name == matrix->uniqueKeys.toValue){
        {
            vmVariant v1 = vmVariantListGet(args, 0);
            vmVariant v2 = vmVariantListGet(args, 1);
            vmC3DTransform * t,* m4;
            if((v1.type & vmVariantTypeObject) && v1.value.objectValue
               && vmRuntimeContextObjectIsKindClass(context, v1.value.objectValue, (vmClass *)&vmC3DTransformClass)){
                t = (vmC3DTransform *) v1.value.objectValue;
                rs =  vmObjectAlloc(context, (vmClass *)&vmC3DTransformClass, NULL);
                m4 = (vmC3DTransform * ) rs.value.objectValue;
                m4->transform = c3d_matrix4_multiply_anim(matrix->transform, t->transform, vmVariantToDouble(context, v2));
            }
        }
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmC3DTransformPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){

}

vmClassBase vmC3DTransformClass = {
    vmClassTypeBase,
    sizeof(vmC3DTransform),
    vmC3DTransformInitCallback,
    vmC3DTransformDestroyCallback,
    vmC3DTransformGetPropertyCallback,
    vmC3DTransformSetPropertyCallback,
    vmC3DTransformPropertyNamesCallback,
    vmC3DTransformInvokeCallback,
};


/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DContextInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DContext * ctx = (vmC3DContext *) object;
   
    ctx->uniqueKeys.width = vmRuntimeContextGetUniqueKey(context,"width");
    ctx->uniqueKeys.height = vmRuntimeContextGetUniqueKey(context,"height");
    ctx->uniqueKeys.loadProgram = vmRuntimeContextGetUniqueKey(context,"loadProgram");
    ctx->uniqueKeys.unLoadProgram = vmRuntimeContextGetUniqueKey(context,"unLoadProgram");
    ctx->uniqueKeys.schedule = vmRuntimeContextGetUniqueKey(context,"schedule");
    ctx->uniqueKeys.cancelSchedule = vmRuntimeContextGetUniqueKey(context,"cancelSchedule");
    ctx->uniqueKeys.time = vmRuntimeContextGetUniqueKey(context,"time");
    ctx->uniqueKeys.getStringContent = vmRuntimeContextGetUniqueKey(context,"getStringContent");
    ctx->uniqueKeys.getBytesContent = vmRuntimeContextGetUniqueKey(context,"getBytesContent");
    ctx->uniqueKeys.getFilePath = vmRuntimeContextGetUniqueKey(context,"getFilePath");
    ctx->uniqueKeys.needRender = vmRuntimeContextGetUniqueKey(context,"needRender");
    ctx->uniqueKeys.memoryWarning = vmRuntimeContextGetUniqueKey(context, "memoryWarning");
    
    ev_dispatch_init(&ctx->ctx.dispatch,InvokeTickArg);

    ctx->ctx.needRender = hbool_true;

    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DContextDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DContext * ctx = (vmC3DContext *) object;
    
    c3d_context_cleanup(&ctx->ctx, InvokeTickArg);
    
    ev_dispatch_destroy(&ctx->ctx.dispatch, InvokeTickArg);
    
    hobj_autorelease_pool_begin();
    
    hobj_release((hobj_t *) ctx->bundle.doc, InvokeTickArg);
    hobj_release((hobj_t *) ctx->bundle.res, InvokeTickArg);
    hobj_release((hobj_t *) ctx->bundle.tmp, InvokeTickArg);
    hobj_release((hobj_t *) ctx->bundle.lib, InvokeTickArg);
    
    if(ctx->values){
        vmObjectRelease(context, ctx->values);
    }
    
    hobj_autorelease_pool_end();
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DContextGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DContext * ctx = (vmC3DContext *) object;
    
    if(ctx->uniqueKeys.width == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = ctx->ctx.viewport.width * ctx->ctx.scale;
    }
    else if(ctx->uniqueKeys.height == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = ctx->ctx.viewport.height * ctx->ctx.scale;
    }
    else if(ctx->uniqueKeys.time == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = ctx->ctx.schedule.current;
    }
    else if(ctx->uniqueKeys.needRender == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = ctx->ctx.needRender;
    }
    else if(ctx->uniqueKeys.memoryWarning == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = ctx->ctx.memoryWarning;
    }
    else if(ctx->values){
        return vmObjectGetProperty(context, ctx->values->vmClass, ctx->values, name);
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DContextSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DContext * ctx = (vmC3DContext *) object;

    
    if(ctx->uniqueKeys.needRender == name){
        ctx->ctx.needRender = vmVariantToBoolean(context, value);
    }
    else if(ctx->uniqueKeys.memoryWarning == name){
        ctx->ctx.memoryWarning = vmVariantToBoolean(context, value);
    }
    else {
        if(ctx->values == NULL){
            vmVariant v = vmDictionaryAlloc(context);
            if(v.type & vmVariantTypeObject){
                ctx->values = vmObjectRetain(context, v.value.objectValue);
            }
        }
        if(ctx->values){
            return vmObjectSetProperty(context, ctx->values->vmClass, ctx->values, name, value);
        }
    }
    
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DContextInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DContext * ctx = (vmC3DContext *) object;
    
    if(ctx->uniqueKeys.loadProgram == name){
        {
            
            vmVariant v1 = vmVariantListGet(args, 0);
            vmVariant v2 = vmVariantListGet(args, 1);
            vmVariant v3 = vmVariantListGet(args, 2);
            hbuffer_t key = buffer_alloc(32, 32);
            hbuffer_t vshSource = buffer_alloc(128, 128);
            hbuffer_t fshSource = buffer_alloc(128, 128);
            
            vmVariantToString(context, v1, key);
            vmVariantToString(context, v2, vshSource);
            vmVariantToString(context, v3, fshSource);
            
            if(buffer_length(key) && buffer_length(vshSource) && buffer_length(vshSource)){
                if(c3d_context_program_load(&ctx->ctx, buffer_to_str(key), buffer_to_str(vshSource), buffer_to_str(fshSource),InvokeTickArg)){
                    rs.type = vmVariantTypeBoolean;
                    rs.value.booleanValue = hbool_true;
                }
            }
            
            buffer_dealloc(key);
            buffer_dealloc(vshSource);
            buffer_dealloc(fshSource);
            
        }
    }
    else if(ctx->uniqueKeys.unLoadProgram == name){
        {
            
            vmVariant v1 = vmVariantListGet(args, 0);
            hbuffer_t key = buffer_alloc(32, 32);
            
            vmVariantToString(context, v1, key);
            
            if(buffer_length(key)){
                c3d_context_program_unload(&ctx->ctx, buffer_to_str(key),InvokeTickArg);
            }
            
            buffer_dealloc(key);
            
        }
    }
    else if(ctx->uniqueKeys.schedule == name){
        {
            
            vmVariant v1 = vmVariantListGet(args, 0);
            vmC3DTask * t;
            
            if((v1.type & vmVariantTypeObject) && v1.value.objectValue
               && vmRuntimeContextObjectIsKindClass(context, v1.value.objectValue, (vmClass *)&vmC3DTaskClass)){
                t = (vmC3DTask *) v1.value.objectValue;
                c3d_schedule_task(&ctx->ctx, &t->task, InvokeTickArg);
                rs.type = vmVariantTypeBoolean;
                rs.value.booleanValue = hbool_true;
            }
            
        }
    }
    else if(ctx->uniqueKeys.cancelSchedule == name){
        {
            
            vmVariant v1 = vmVariantListGet(args, 0);
            vmC3DTask * t;
            
            if((v1.type & vmVariantTypeObject) && v1.value.objectValue){
                if(vmRuntimeContextObjectIsKindClass(context, v1.value.objectValue, (vmClass *)&vmC3DTaskClass)){
                    t = (vmC3DTask *) v1.value.objectValue;
                    c3d_schedule_task_cancel(&ctx->ctx, &t->task, InvokeTickArg);
                    rs.type = vmVariantTypeBoolean;
                    rs.value.booleanValue = hbool_true;
                }
                else{
                    c3d_schedule_task_cancels(&ctx->ctx, v1.value.objectValue, InvokeTickArg);
                    rs.type = vmVariantTypeBoolean;
                    rs.value.booleanValue = hbool_true;
                }
            }
            
        }
    }
    else if(ctx->uniqueKeys.getStringContent == name){
        {
            hbuffer_t path = buffer_alloc(128, 128);
            hobj_t * v = NULL;
            
            vmVariantToString(context, vmVariantListGet(args,0), path);
            
            if(str_has_prefix(buffer_to_str(path), "doc://")){
                v = hobj_bundle_getStringContent((hobj_bundle_t *)ctx->bundle.doc, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "tmp://")){
                v = hobj_bundle_getStringContent((hobj_bundle_t *)ctx->bundle.tmp, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "lib://")){
                v = hobj_bundle_getStringContent((hobj_bundle_t *)ctx->bundle.lib, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else{
                v = hobj_bundle_getStringContent((hobj_bundle_t *)ctx->bundle.res, buffer_to_str(path), InvokeTickArg);
            }

            
            if(v){
                rs = vmStringAlloc(context, hobj_value_stringValue((hobj_value_t *) v, InvokeTickArg));
            }
            
            buffer_dealloc(path);
        }
    }
    else if(ctx->uniqueKeys.getBytesContent == name){
        {
            hbuffer_t path = buffer_alloc(128, 128);
            hobj_t * v = NULL;
            
            vmVariantToString(context, vmVariantListGet(args,0), path);
            
            if(str_has_prefix(buffer_to_str(path), "doc://")){
                v = hobj_bundle_getBytesContent((hobj_bundle_t *)ctx->bundle.doc, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "tmp://")){
                v = hobj_bundle_getBytesContent((hobj_bundle_t *)ctx->bundle.tmp, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "lib://")){
                v = hobj_bundle_getBytesContent((hobj_bundle_t *)ctx->bundle.lib, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else{
                v = hobj_bundle_getBytesContent((hobj_bundle_t *)ctx->bundle.res, buffer_to_str(path), InvokeTickArg);
            }
            

            if(v){
                rs = vmDataAlloc(context, ((hobj_value_t *)v)->data, ((hobj_value_t *)v)->length);
            }
            
            buffer_dealloc(path);
        }
    }
    else if(ctx->uniqueKeys.getFilePath == name){
        {
            hbuffer_t path = buffer_alloc(128, 128);
            hobj_t * v = NULL;
            
            vmVariantToString(context, vmVariantListGet(args,0), path);
            
            if(str_has_prefix(buffer_to_str(path), "doc://")){
                v = hobj_bundle_getFilePath((hobj_bundle_t *)ctx->bundle.doc, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "tmp://")){
                v = hobj_bundle_getFilePath((hobj_bundle_t *)ctx->bundle.tmp, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else if(str_has_prefix(buffer_to_str(path), "lib://")){
                v = hobj_bundle_getFilePath((hobj_bundle_t *)ctx->bundle.lib, buffer_to_str(path) + 6, InvokeTickArg);
            }
            else{
                v = hobj_bundle_getFilePath((hobj_bundle_t *)ctx->bundle.res, buffer_to_str(path), InvokeTickArg);
            }
            
            
            if(v){
                rs = vmStringAlloc(context, hobj_value_stringValue((hobj_value_t *) v, InvokeTickArg));
            }
            
            buffer_dealloc(path);
        }
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmC3DContextPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmC3DContext * ctx = (vmC3DContext *) object;
    vmUniqueKeyListAdd(names, ctx->uniqueKeys.width);
    vmUniqueKeyListAdd(names, ctx->uniqueKeys.height);
}

vmClassBase vmC3DContextClass = {
    vmClassTypeBase,
    sizeof(vmC3DContext),
    vmC3DContextInitCallback,
    vmC3DContextDestroyCallback,
    vmC3DContextGetPropertyCallback,
    vmC3DContextSetPropertyCallback,
    vmC3DContextPropertyNamesCallback,
    vmC3DContextInvokeCallback,
};


/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTouchesInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTouches * touches = (vmC3DTouches *) object;
    
    touches->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context,"length");
    touches->uniqueKeys.touchId = vmRuntimeContextGetUniqueKey(context,"touchId");
    touches->uniqueKeys.touchX = vmRuntimeContextGetUniqueKey(context,"touchX");
    touches->uniqueKeys.touchY = vmRuntimeContextGetUniqueKey(context,"touchY");
    touches->uniqueKeys.touchZ = vmRuntimeContextGetUniqueKey(context,"touchZ");
    touches->uniqueKeys.timestamp = vmRuntimeContextGetUniqueKey(context,"timestamp");
    touches->uniqueKeys.state = vmRuntimeContextGetUniqueKey(context,"state");
    touches->uniqueKeys.target = vmRuntimeContextGetUniqueKey(context,"target");
    touches->uniqueKeys.cancelBubble = vmRuntimeContextGetUniqueKey(context,"cancelBubble");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DTouchesDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DTouchesGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTouches * touches = (vmC3DTouches *) object;
    
    if(touches->uniqueKeys.length == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = touches->touches.length;
    }
    else if(touches->uniqueKeys.state == name){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = touches->touches.state;
    }
    else if(touches->uniqueKeys.timestamp == name){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = touches->touches.timestamp;
    }
    else if(touches->uniqueKeys.target == name){
        rs.type = vmVariantTypeObject;
        rs.value.objectValue = (vmObject *)touches->target;
    }
    else if(touches->uniqueKeys.cancelBubble == name){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = touches->cancelBubble;
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }

    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DTouchesSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTouches * touches = (vmC3DTouches *) object;
    
    if(touches->uniqueKeys.cancelBubble == name){
        touches->cancelBubble = vmVariantToBoolean(context, value);
    }
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DTouchesInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DTouches * touches = (vmC3DTouches *) object;
    c3d_touch_t * touch = touches->touches.touches ;
    
    if(touches->uniqueKeys.touchX == name){
        touch = touches->touches.touches + vmVariantToInt32(context, vmVariantListGet(args, 0));
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = (touch->coord.x - touches->transform.m30 * touches->target->node.context->transform.m00) / touches->transform.m00;
    }
    else if(touches->uniqueKeys.touchY == name){
        touch = touches->touches.touches + vmVariantToInt32(context, vmVariantListGet(args, 0));
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = (touch->coord.y - touches->transform.m31 * touches->target->node.context->transform.m11) / touches->transform.m11 ;
    }
    else if(touches->uniqueKeys.touchZ == name){
        touch = touches->touches.touches + vmVariantToInt32(context, vmVariantListGet(args, 0));
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = (touch->coord.z - touches->transform.m32 * touches->target->node.context->transform.m22) / touches->transform.m22;
    }
    else if(touches->uniqueKeys.touchId == name){
        touch = touches->touches.touches + vmVariantToInt32(context, vmVariantListGet(args, 0));
        rs.type = vmVariantTypeInt64;
        rs.value.int64Value = touch->touchId;
    }
    else{
        return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : Void
 */
static void vmC3DTouchesPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmC3DTouches * touches = (vmC3DTouches *) object;
    vmUniqueKeyListAdd(names, touches->uniqueKeys.length);
}

vmClassBase vmC3DTouchesClass = {
    vmClassTypeBase,
    sizeof(vmC3DTouches),
    vmC3DTouchesInitCallback,
    vmC3DTouchesDestroyCallback,
    vmC3DTouchesGetPropertyCallback,
    vmC3DTouchesSetPropertyCallback,
    vmC3DTouchesPropertyNamesCallback,
    vmC3DTouchesInvokeCallback,
};
