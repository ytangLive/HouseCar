//
//  hvm3d_render.c
//  hvm3d
//
//  Created by zhang hailong on 13-3-18.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#include "hconfig.h"
#include "hvm3d_render.h"
#include "h3dnode.h"
#include "hvermin_runtime.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "h3dprogram.h"
#include "h3drender.h"
#include "h3dgraphics.h"
#include "h3dtexture.h"
#include "hvm3d_graphics.h"
#include "hlog.h"
#include "hvm3d_image.h"

static void vmC3DRenderDrawImpl(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);

static c3d_node_condition_t vmC3DRenderConditionImpl(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);

HOBJ_KEY_IMP(vmC3DRender)
HOBJ_KEY_IMP(draw)
HOBJ_KEY_IMP(condition)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmC3DRender)

HOBJ_CLASS_METHOD_IMP(draw, vmC3DRenderDrawImpl)

HOBJ_CLASS_METHOD_IMP(condition, vmC3DRenderConditionImpl)

HOBJ_CLASS_METHOD_IMP_END(vmC3DRender)

HOBJ_CLASS_IMP_I(vmC3DRender, vmC3DRender, &hobj_class)


vmC3DRender * vmC3DRenderAlloc(hobj_class_t * clazz, vmC3DNode * node,InvokeTickDeclare){
    if(hobj_class_is_kind(clazz, HOBJ_CLASS(vmC3DRender), InvokeTickArg)){
        vmC3DRender * r = (vmC3DRender *) hobj_alloc(clazz, InvokeTickArg);
        r->node = node;
        return r;
    }
    return NULL;
}


void vmC3DRenderDraw(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    hobj_method_t * method = hobj_method((hobj_t *) render, HOBJ_KEY(draw), InvokeTickArg);
    if(method){
        (* (vmC3DRenderDrawImp) method->imp)(render,condition,InvokeTickArg);
    }
}

c3d_node_condition_t vmC3DRenderCondition(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    hobj_method_t * method = hobj_method((hobj_t *) render, HOBJ_KEY(condition), InvokeTickArg);
    if(method){
        return (* (vmC3DRenderConditionImp) method->imp)(render,condition,InvokeTickArg);
    }
    return c3d_node_condition_multiply(condition, render->node->condition);
}

static void vmC3DRenderDrawImpl(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    
}

static c3d_node_condition_t vmC3DRenderConditionImpl(vmC3DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    return c3d_node_condition_multiply(condition, render->node->condition);
}

static void vmC2DRenderDrawImpl(vmC2DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);

static c3d_node_condition_t vmC2DRenderConditionImpl(vmC2DRender * render,c3d_node_condition_t condition,InvokeTickDeclare);

static void vmC2DRenderDestroyImpl(vmC2DRender * render,InvokeTickDeclare);

HOBJ_KEY_IMP(vmC2DRender)

HOBJ_CLASS_METHOD_IMP_BEGIN(vmC2DRender)

HOBJ_CLASS_METHOD_IMP(draw, vmC2DRenderDrawImpl)

HOBJ_CLASS_METHOD_IMP(condition, vmC2DRenderConditionImpl)

HOBJ_CLASS_METHOD_IMP(destroy, vmC2DRenderDestroyImpl)

HOBJ_CLASS_METHOD_IMP_END(vmC2DRender)

HOBJ_CLASS_IMP_I(vmC2DRender, vmC2DRender, HOBJ_CLASS(vmC3DRender))

typedef struct _C2DRenderVertxt{
    c3d_vector3_t position;
    c3d_vector3_t normal;
    c3d_vector2_t txtCood;
} C2DRenderVertxt;


static void vmC2DRenderDrawImpl(vmC2DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    vmC3DNode * node = render->base.node;
    vmRuntimeContext ctx = node->runtimeContext;
    vmC3DContext * c3dContext = C3DCONTEXT_TO_VMC3DCONTEXT(node->node.context);
    vmVariant v;
    vmC3DImage * contents = NULL;
    hfloat width,height,flickX,cornerRadius;
    hint64 backgroundColor;
    hfloat backgroundAlpha;
    hbool display;
    hint32 contentsIndex = 0;
    c3d_texture_t txt;
    
    if(node->node.context->viewport.width > 0
       && node->node.context->viewport.height){
         
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.width);
        width = vmVariantToDouble(ctx, v)  / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.height);
        height = vmVariantToDouble(ctx, v)  / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.backgroundColor);
        backgroundColor = vmVariantToInt64(ctx, v);
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.backgroundAlpha);
        backgroundAlpha = vmVariantToDouble(ctx, v);
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.display);
        display = vmVariantToBoolean(ctx, v);
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.flickX);
       
        flickX = vmVariantToDouble(ctx, v)  / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.contents);
        
        if( ( v.type & vmVariantTypeObject ) && v.value.objectValue ){
            if(vmRuntimeContextObjectIsKindClass(ctx, v.value.objectValue, (vmClass *)&vmC3DImageClass)){
                contents = (vmC3DImage *)v.value.objectValue;
            }
        }

        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.contentsIndex);
        
        contentsIndex = vmVariantToInt32(ctx, v);
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.cornerRadius);
        
        cornerRadius = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        if(width >0 && height >0 && condition.clips.width >0 && condition.clips.height >0 ){
            
            float left = - 1.0;
            float bottom = - 1.0;
            float right = 1.0;
            float top = 1.0;
            
        
            c3d_vector4_t contentsRect = {0.0f,0.0f,1.0f,1.0f};
            c3d_vector4_t v4;
            
            left = left / node->node.context->transform.m00;
            bottom = bottom / node->node.context->transform.m11;
            right = right / node->node.context->transform.m00;
            top = top / node->node.context->transform.m11;

            C2DRenderVertxt vertxts[4] = {
                { {left,top,0} , {0,0,1}, {contentsRect.origin.x,contentsRect.origin.y} },
                { {right,top,0} , {0,0,1}, {contentsRect.origin.x + contentsRect.size.width,contentsRect.origin.y} },
                { {right,bottom,0} , {0,0,1},{contentsRect.origin.x + contentsRect.size.width,contentsRect.origin.y + contentsRect.size.height}},
                { {left,bottom,0} , {0,0,1},{contentsRect.origin.x,contentsRect.origin.y + contentsRect.size.height} },
            };
            
            if(!render->program){
                render->program = c3d_context_program(node->node.context,"C2DRender",InvokeTickRoot);
                render->attribs.position = c3d_program_attrib_location(render->program, "position");
                render->attribs.normal = c3d_program_attrib_location(render->program, "normal");
                render->attribs.txtCoord = c3d_program_attrib_location(render->program, "txtCoord");
                render->attribs.projectTransform = c3d_program_uniform_location(render->program, "projectTransform");
                render->attribs.normalTransform = c3d_program_uniform_location(render->program, "normalTransform");
                render->attribs.transform = c3d_program_uniform_location(render->program, "transform");
                render->attribs.backgroundColor = c3d_program_uniform_location(render->program, "backgroundColor");
                render->attribs.contents = c3d_program_uniform_location(render->program, "contents");
                render->attribs.hasContents = c3d_program_uniform_location(render->program, "hasContents");
                render->attribs.alpha = c3d_program_uniform_location(render->program, "alpha");
                render->attribs.flickX = c3d_program_uniform_location(render->program, "flickX");
            }
            if(render->program){
                
                
                c3d_program_use(render->program);
                
                c3d_program_uniform_4m(render->attribs.projectTransform, node->node.context->transform);
                c3d_program_uniform_4m(render->attribs.transform, condition.transform);
                c3d_program_uniform_3m(render->attribs.normalTransform, c3d_matrix4_to_matrix3(condition.transform));

                
                v4.r = (float)((backgroundColor & 0xff0000) >> 16) / 0xff;
                v4.g = (float)((backgroundColor & 0x00ff00) >> 8 ) / 0xff;
                v4.b = (float)((backgroundColor & 0x0000ff)) / 0xff ;
                v4.a = backgroundAlpha;
                
                c3d_program_uniform_4f(render->attribs.backgroundColor, v4);
                c3d_program_uniform_1f(render->attribs.alpha, condition.alpha);
                //c3d_program_uniform_1f(attribs.flickX, flickX / width);
                
                if(display){
                    
                    v.type = vmVariantTypeBoolean;
                    v.value.booleanValue = hbool_false;
                    vmObjectSetProperty(ctx, node->base.vmClass, (vmObject *) node, vmRuntimeContextGetUniqueKey(ctx, "display"),v);
                
                    c3d_graphics_begin(node->node.context, condition.bounds.width + 0.99999, condition.bounds.height + 0.99999, InvokeTickArg);
                    
                    v = vmObjectAlloc(ctx, (vmClass *)&vmC3DGraphicsClass, NULL);
                    
                    if(v.type & vmVariantTypeObject && v.value.objectValue){
                        
                        {
                            vmVariantList args = vmVariantListAlloc();
                            
                            vmVariantListAdd(args, v);
                            
                            ((vmC3DGraphics *)v.value.objectValue)->ctx = node->node.context;
                            
                            vmObjectInvoke(ctx, node->base.vmClass, (vmObject *)node, vmRuntimeContextGetUniqueKey(ctx, "draw"), args);
                            
                            vmVariantListDealloc(args);
                        }
                        
                    }
                    
                    if(render->texture.texture){
                        render->texture = c3d_texture_bind(render->texture, c3d_graphics_data(node->node.context, InvokeTickArg), TexturePixelFormatRGBA
                                                                , node->node.context->graphics.width, node->node.context->graphics.height);
                    }
                    else{
                        render->texture = c3d_texture_create( c3d_graphics_data(node->node.context, InvokeTickArg), TexturePixelFormatRGBA
                                                                , node->node.context->graphics.width, node->node.context->graphics.height);
                    }
                    
                    c3d_graphics_end(node->node.context, InvokeTickArg);
                    
                }
                
                if(render->texture.texture){
                    c3d_program_uniform_1i( render->attribs.hasContents,1);
                    c3d_texture_active(render->texture, 0);
                    c3d_program_uniform_1i(render->attribs.contents, 0);
                }
                else{
                    if(contents){
                        txt = vmC3DImageTexure(ctx, c3dContext,contents,contentsIndex, InvokeTickArg);
                        if(txt.texture){
                            c3d_program_uniform_1i( render->attribs.hasContents,1);
                            c3d_texture_active(txt, 0);
                            c3d_program_uniform_1i(render->attribs.contents, 0);
                        }
                        else{
                            c3d_program_uniform_1i( render->attribs.hasContents,0);
                        }
                    }
                    else{
                        c3d_program_uniform_1i( render->attribs.hasContents,0);
                    }
                }
                
                c3d_program_vertxt_attrib(render->attribs.position, 3,&vertxts[0].position, sizeof(C2DRenderVertxt));
                c3d_program_vertxt_attrib(render->attribs.normal, 3,&vertxts[0].normal, sizeof(C2DRenderVertxt));
                c3d_program_vertxt_attrib(render->attribs.txtCoord, 2,&vertxts[0].txtCood, sizeof(C2DRenderVertxt));
                
                
                c3d_render_arrays(C3DRenderModeTriangleFan, 0, 4);

            }
        }
    }

}

static c3d_node_condition_t vmC2DRenderConditionImpl(vmC2DRender * render,c3d_node_condition_t condition,InvokeTickDeclare){
    
    vmC3DNode * node = render->base.node;
    vmRuntimeContext ctx = node->runtimeContext;
    vmVariant v;
    hfloat left,top,width,height,dx,dy,w,h,contentLeft,contentTop;
    c3d_matrix4_t m4;
    c3d_node_condition_t rs;
    
    if(node->node.context->viewport.width > 0
       && node->node.context->viewport.height){
        
        if(! render->uniqueKeys.alpha){
            render->uniqueKeys.alpha = vmRuntimeContextGetUniqueKey(ctx, "alpha");
        }
        
        if(! render->uniqueKeys.contentLeft){
            render->uniqueKeys.contentLeft = vmRuntimeContextGetUniqueKey(ctx, "contentLeft");
        }
        
        if(! render->uniqueKeys.contentTop){
            render->uniqueKeys.contentTop = vmRuntimeContextGetUniqueKey(ctx, "contentTop");
        }
        
        if(! render->uniqueKeys.height){
            render->uniqueKeys.height = vmRuntimeContextGetUniqueKey(ctx, "height");
        }
        
        if(! render->uniqueKeys.left){
            render->uniqueKeys.left = vmRuntimeContextGetUniqueKey(ctx, "left");
        }
        
        if(! render->uniqueKeys.top){
            render->uniqueKeys.top = vmRuntimeContextGetUniqueKey(ctx, "top");
        }
        
        if(! render->uniqueKeys.width){
            render->uniqueKeys.width = vmRuntimeContextGetUniqueKey(ctx, "width");
        }
        
        if(! render->uniqueKeys.backgroundColor){
            render->uniqueKeys.backgroundColor = vmRuntimeContextGetUniqueKey(ctx, "backgroundColor");
        }
        
        
        if(! render->uniqueKeys.backgroundAlpha){
            render->uniqueKeys.backgroundAlpha = vmRuntimeContextGetUniqueKey(ctx, "backgroundAlpha");
        }
        
        if(! render->uniqueKeys.display){
            render->uniqueKeys.display = vmRuntimeContextGetUniqueKey(ctx, "display");
        }
        
        if(! render->uniqueKeys.contents){
            render->uniqueKeys.contents = vmRuntimeContextGetUniqueKey(ctx, "contents");
        }
        
        if(! render->uniqueKeys.contentsIndex){
            render->uniqueKeys.contentsIndex = vmRuntimeContextGetUniqueKey(ctx, "contentsIndex");
        }
        
        if(! render->uniqueKeys.flickX){
            render->uniqueKeys.flickX = vmRuntimeContextGetUniqueKey(ctx, "flickX");
        }
        
        if(! render->uniqueKeys.cornerRadius){
            render->uniqueKeys.cornerRadius = vmRuntimeContextGetUniqueKey(ctx, "cornerRadius");
        }
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.left);
        left = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.top);
        top = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.contentLeft);
        contentLeft = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.contentTop);
        contentTop = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.width);
        width = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        v = vmObjectGetProperty(ctx, node->base.vmClass, (vmObject *) node, render->uniqueKeys.height);
        height = vmVariantToDouble(ctx, v) / node->node.context->scale;
        
        node->condition.contentOffset.x = contentLeft;
        node->condition.contentOffset.y = contentTop;
        
        left = left - condition.contentOffset.x;
        top = top - condition.contentOffset.y;
        
        if(width >0 && height >0){
            
            if(left <0 ){
                node->condition.clips.x = 0;
                node->condition.clips.width = width + left;
            }
            else{
                node->condition.clips.x = left;
                node->condition.clips.width = width ;
            }
            
            if(node->condition.clips.width <0){
                node->condition.clips.width = 0;
            }
            
            if(top <0 ){
                node->condition.clips.y = 0;
                node->condition.clips.height = height + top;
            }
            else{
                node->condition.clips.y = top;
                node->condition.clips.height = height ;
            }
            
            if(node->condition.clips.height <0){
                node->condition.clips.height = 0;
            }

            node->condition.bounds.x = left;
            node->condition.bounds.y = top;
            node->condition.bounds.width = width;
            node->condition.bounds.height = height;
            
            w = width;
            h = height;
            
            dx = condition.bounds.width / 2.0f;
            dy = condition.bounds.height / 2.0f;
            
            left = left + w / 2.0f;
            top = top + h / 2.0f ;
            
            left = (left - dx) / dx;
            top = ( dy - top) / dy;
        
            m4 = c3d_matrix4_identity;
            

            m4 = c3d_matrix4_translate(m4, left/ node->node.context->transform.m00 , top/ node->node.context->transform.m11, node->node.context->zIndex - condition.transform.m32);

            
            node->condition.bounds.scaleX = width / node->node.context->viewport.width;
            node->condition.bounds.scaleY = height / node->node.context->viewport.height;
            
            m4 = c3d_matrix4_scale(m4, node->condition.bounds.scaleX / condition.bounds.scaleX
                                   , node->condition.bounds.scaleY  / condition.bounds.scaleY
                                   , 1.0);
            

            node->condition.transform = c3d_matrix4_multiply( m4, node->property.transform);
            
            node->condition.alpha = node->property.alpha;
            
        }
    }

    rs =  c3d_node_condition_multiply(condition, node->condition);

    v.type = vmVariantTypeInt32;
    v.value.int32Value = ( node->condition.clips.x - node->condition.bounds.x ) * node->node.context->scale;
    
    vmObjectSetProperty(ctx, node->base.vmClass, (vmObject *)node, vmRuntimeContextGetUniqueKey(ctx, "clipLeft"), v);
    
    v.type = vmVariantTypeInt32;
    v.value.int32Value = ( node->condition.clips.y - node->condition.bounds.y ) * node->node.context->scale;
    
    vmObjectSetProperty(ctx, node->base.vmClass, (vmObject *)node, vmRuntimeContextGetUniqueKey(ctx, "clipTop"), v);
    
    v.type = vmVariantTypeInt32;
    v.value.int32Value = rs.clips.width * node->node.context->scale;
    
    vmObjectSetProperty(ctx, node->base.vmClass, (vmObject *)node, vmRuntimeContextGetUniqueKey(ctx, "clipWidth"), v);
    
    v.type = vmVariantTypeInt32;
    v.value.int32Value = rs.clips.height * node->node.context->scale;
    
    vmObjectSetProperty(ctx, node->base.vmClass, (vmObject *)node, vmRuntimeContextGetUniqueKey(ctx, "clipHeight"), v);
    
    return rs;
}

void vmC2DRenderDestroyImpl(vmC2DRender * render,InvokeTickDeclare){
    hobj_method_t * method;
    
    if(render->texture.texture){
        c3d_texture_delete(render->texture);
    }
    
    method = hobj_class_method(hobj_vmC2DRender_class.superClass, HOBJ_KEY(destroy), InvokeTickArg);
    
    if(method){
        ( * (hobj_method_destroy_impl_t ) method->imp)( (hobj_t* ) render,InvokeTickArg);
    }

}

