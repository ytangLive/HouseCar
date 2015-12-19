//
//  hvm3d_render.c
//  hvm3d
//
//  Created by Zhang Hailong on 13-3-28.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#include "hconfig.h"
#include "hvm3d_render.h"
#include "hbuffer.h"
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
#include "h3dbuffer.h"

typedef struct _C2DRenderVertxt{
    c3d_vector3_t position;
    c3d_vector3_t normal;
    c3d_vector2_t txtCood;
} C2DRenderVertxt;

static hbool vmC2DRenderDraw(vmC2DRender * render,vmC3DNode * node,c3d_node_condition_t condition,InvokeTickDeclare){
    vmRuntimeContext ctx = node->runtimeContext;
    vmC3DContext * c3dContext = C3DCONTEXT_TO_VMC3DCONTEXT(node->node.context);
    vmVariant v;
    vmC3DImage * contents = NULL;
    hfloat width,height,flickX,cornerRadius,contentsLeft,contentsRight,contentsTop,contentsBottom;
    hint64 backgroundColor;
    hfloat backgroundAlpha;
    hbool display;
    hint32 contentsIndex = 0;
    c3d_texture_t txt;
    hbool rs = hbool_true;
    
    if(node->node.context->viewport.width > 0
       && node->node.context->viewport.height){
        
        width = render->propertys.width  / node->node.context->scale;
        
        height = render->propertys.height  / node->node.context->scale;
        
        backgroundColor = render->propertys.backgroundColor;
        
        backgroundAlpha = render->propertys.backgroundAlpha;
        
        display = render->propertys.display;

        flickX = render->propertys.flickX  / node->node.context->scale;
        
        contents = render->propertys.contents;
    
        contentsIndex = render->propertys.contentsIndex;
        
        cornerRadius = render->propertys.cornerRadius / node->node.context->scale;

        
        if(width >0 && height >0 && condition.clips.width >0 && condition.clips.height >0 ){
            
            float left = - 1.0;
            float bottom = - 1.0;
            float right = 1.0;
            float top = 1.0;
            
            float offsetLeft = condition.clips.x - condition.bounds.x ;
            float offsetTop = condition.clips.y - condition.bounds.y ;
  

            c3d_vector4_t contentsRect = {0,0,1.0f,1.0f};
            c3d_vector4_t v4;
            
            if(contents && contents->propertys.width >0 && contents->propertys.height >0){
                
                contentsLeft = render->propertys.contentsLeft / contents->propertys.width ;
                
                contentsRight = render->propertys.contentsRight / contents->propertys.width ;
                
                contentsTop = render->propertys.contentsTop / contents->propertys.height;
                
                contentsBottom = render->propertys.contentsBottom / contents->propertys.height ;
            
                
                contentsRect.origin.x += contentsLeft;
                contentsRect.origin.y += contentsTop;
                contentsRect.size.width -= contentsLeft + contentsRight;
                contentsRect.size.height -= contentsTop + contentsBottom;
                
                contentsRect.origin.x +=  contentsRect.size.width * offsetLeft / condition.bounds.width;
                contentsRect.origin.y +=   contentsRect.size.height * offsetTop / condition.bounds.height;
                contentsRect.size.width *= condition.clips.width  / condition.bounds.width;
                contentsRect.size.height *= condition.clips.height  / condition.bounds.height;
                
            }
            else{
                contentsRect.origin.x +=  offsetLeft / condition.bounds.width;
                contentsRect.origin.y +=  offsetTop / condition.bounds.height;
                contentsRect.size.width *= condition.clips.width / condition.bounds.width;
                contentsRect.size.height *= condition.clips.height  / condition.bounds.height;
            }
            
            
            right = -1.0 + 2.0 * (condition.clips.width  + offsetLeft) / condition.bounds.width;
            
            bottom = 1.0 - 2.0 * (condition.clips.height + offsetTop ) / condition.bounds.height;
           
            top = 1.0 - 2.0 * offsetTop / condition.bounds.height;
            
            left = - 1.0 + 2.0 * offsetLeft / condition.bounds.width;
            
            
            
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
            
            if(!render->program.program){
                render->program.program = c3d_context_program(node->node.context,buffer_to_str(render->program.key),InvokeTickRoot);
                render->program.attribs.position = c3d_program_attrib_location(render->program.program, "position");
                render->program.attribs.normal = c3d_program_attrib_location(render->program.program, "normal");
                render->program.attribs.txtCoord = c3d_program_attrib_location(render->program.program, "txtCoord");
                render->program.attribs.projectTransform = c3d_program_uniform_location(render->program.program, "projectTransform");
                render->program.attribs.normalTransform = c3d_program_uniform_location(render->program.program, "normalTransform");
                render->program.attribs.transform = c3d_program_uniform_location(render->program.program, "transform");
                render->program.attribs.backgroundColor = c3d_program_uniform_location(render->program.program, "backgroundColor");
                render->program.attribs.contents = c3d_program_uniform_location(render->program.program, "contents");
                render->program.attribs.hasContents = c3d_program_uniform_location(render->program.program, "hasContents");
                render->program.attribs.alpha = c3d_program_uniform_location(render->program.program, "alpha");
                render->program.attribs.flickX = c3d_program_uniform_location(render->program.program, "flickX");
                render->program.attribs.mask = c3d_program_uniform_location(render->program.program, "mask");
                render->program.attribs.hasMask = c3d_program_uniform_location(render->program.program, "hasVeins");
            }
            
            if(render->program.program){
                
                
                c3d_program_use(render->program.program);
                
                c3d_program_uniform_4m(render->program.attribs.projectTransform, node->node.context->transform);
                
                c3d_program_uniform_4m(render->program.attribs.transform, node->transform);
                c3d_program_uniform_3m(render->program.attribs.normalTransform, c3d_matrix4_to_matrix3(node->transform));
                
                
                v4.r = (float)((backgroundColor & 0xff0000) >> 16) / 0xff;
                v4.g = (float)((backgroundColor & 0x00ff00) >> 8 ) / 0xff;
                v4.b = (float)((backgroundColor & 0x0000ff)) / 0xff ;
                v4.a = backgroundAlpha;
                
                c3d_program_uniform_4f(render->program.attribs.backgroundColor, v4);
                c3d_program_uniform_1f(render->program.attribs.alpha, condition.alpha);
                c3d_program_uniform_1f(render->program.attribs.flickX, flickX / width);
                
                if(display){
                    
                    render->propertys.display = hbool_false;
                    
                    c3d_graphics_begin(node->node.context, condition.bounds.width + 0.99999, condition.bounds.height + 0.99999, InvokeTickArg);
                    
                    v = vmObjectAlloc(ctx, (vmClass *)&vmC3DGraphicsClass, NULL);
                    
                    if((v.type & vmVariantTypeObject) && v.value.objectValue){
                        
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
                                
                if(contents){
                    txt = vmC3DImageTexure(ctx, c3dContext,contents,contentsIndex, InvokeTickArg);
                    if(txt.texture){
                        c3d_program_uniform_1i( render->program.attribs.hasContents,1);
                        c3d_texture_active(txt, 0);
                        c3d_program_uniform_1i(render->program.attribs.contents, 0);
                    }
                    else{
                        c3d_program_uniform_1i( render->program.attribs.hasContents,0);
                    }
                }
                else if(render->texture.texture){
                    c3d_program_uniform_1i( render->program.attribs.hasContents,1);
                    c3d_texture_active(render->texture, 0);
                    c3d_program_uniform_1i(render->program.attribs.contents, 0);
                }
                else{
                    c3d_program_uniform_1i( render->program.attribs.hasContents,0);
                }
                
                if(render->propertys.mask){
                    txt = vmC3DImageTexure(ctx, c3dContext,render->propertys.mask,0, InvokeTickArg);
                    if(txt.texture){
                        c3d_program_uniform_1i( render->program.attribs.hasMask,1);
                        c3d_texture_active(txt, 1);
                        c3d_program_uniform_1i(render->program.attribs.mask, 1);
                    }
                    else{
                        c3d_program_uniform_1i( render->program.attribs.hasMask,0);
                    }
                }
                else{
                    c3d_program_uniform_1i( render->program.attribs.hasMask,0);
                }
            
                
                c3d_program_vertxt_attrib(render->program.attribs.position, 3,&vertxts[0].position, sizeof(C2DRenderVertxt));
                c3d_program_vertxt_attrib(render->program.attribs.normal, 3,&vertxts[0].normal, sizeof(C2DRenderVertxt));
                c3d_program_vertxt_attrib(render->program.attribs.txtCoord, 2,&vertxts[0].txtCood, sizeof(C2DRenderVertxt));
                
                c3d_render_arrays(C3DRenderModeTriangleFan, 0, 4);
                
            }
        }
    }
    
    return rs;
}

static c3d_node_condition_t vmC2DRenderCondition(vmC2DRender * render,vmC3DNode * node,c3d_node_condition_t condition,InvokeTickDeclare){
    
    c3d_context_t * c3dContext = node->node.context;
    hfloat left,top,width,height,contentLeft,contentTop;
    c3d_matrix4_t m4;
    c3d_node_condition_t rs;
    c3d_node_condition_t cond = c3d_node_condition_identity();
    
    cond.transform = node->transform;
    
    if(! render->c3dContext){
        render->c3dContext = c3dContext;
        c3dContext->needRender = hbool_true;
    }
    
    if(node->node.context->viewport.width > 0
       && node->node.context->viewport.height){
        
        left = render->propertys.left / node->node.context->scale;
        top = render->propertys.top / node->node.context->scale;
        contentLeft = render->propertys.contentLeft / node->node.context->scale;
        contentTop = render->propertys.contentTop / node->node.context->scale;
        width = render->propertys.width / node->node.context->scale;
        height = render->propertys.height / node->node.context->scale;
        
        cond.contentOffset.x = contentLeft;
        cond.contentOffset.y = contentTop;
        
        left = left - condition.contentOffset.x;
        top = top - condition.contentOffset.y;
        
        if(width >0 && height >0){
            
            if(left <0 ){
                cond.clips.x = 0;
                cond.clips.width = width + left;
            }
            else{
                cond.clips.x = left;
                cond.clips.width = width ;
            }
            
            if(cond.clips.width <0){
                cond.clips.width = 0;
            }
            
            if(top <0 ){
                cond.clips.y = 0;
                cond.clips.height = height + top;
            }
            else{
                cond.clips.y = top;
                cond.clips.height = height ;
            }
            
            if(cond.clips.height <0){
                cond.clips.height = 0;
            }
            
            cond.bounds.x = left;
            cond.bounds.y = top;
            cond.bounds.width = width;
            cond.bounds.height = height;
            
            m4 = c3d_matrix4_identity;
            
            m4 = c3d_matrix4_scale(m4, render->propertys.scale 
                                   , render->propertys.scale 
                                   , 1.0);
            
            
            cond.transform = c3d_matrix4_multiply( m4, node->property.transform);
            
            cond.alpha = node->property.alpha;
            
            
            rs =  c3d_node_condition_multiply(condition, cond);
            
            render->propertys.clipLeft = ( rs.clips.x - rs.bounds.x ) * node->node.context->scale;
            
            render->propertys.clipTop = ( rs.clips.y - rs.bounds.y ) * node->node.context->scale;
            
            render->propertys.clipWidth = rs.clips.width * node->node.context->scale;
            
            render->propertys.clipHeight = rs.clips.height * node->node.context->scale;
            
            
            
            node->transform = rs.transform;
    
            left = rs.bounds.x + rs.bounds.width / 2.0;
            top = rs.bounds.y + rs.bounds.height / 2.0;
            
            left = left / node->node.context->viewport.width;
            top = top/ node->node.context->viewport.height;
            
            left = left * 2.0 - 1.0;
            top = 1.0 - top * 2.0 ;
            
            left = left / node->node.context->transform.m00;
            top = top / node->node.context->transform.m11;
            
            node->transform = c3d_matrix4_translate(node->transform, left
                                              , top, node->node.context->zIndex);
            
            node->transform = c3d_matrix4_scale(node->transform, rs.bounds.width / node->node.context->viewport.width
                                          , rs.bounds.height / node->node.context->viewport.height , 1.0);
        }
        
        
    }

    return rs;
    
}

static hbool vmC2DRenderConditionReceiveTouches(vmC2DRender * render,vmC3DNode * node,c3d_node_condition_t condition,c3d_touches_t * touches,InvokeTickDeclare){
    if(node->property.receiveTouches){
        
        if(touches->state == C3DTouchStateBegin){
            
            hfloat x = (touches->touches->coord.x - node->transform.m30 * node->node.context->transform.m00) / node->transform.m00;
            hfloat y = (touches->touches->coord.y - node->transform.m31 * node->node.context->transform.m11) / node->transform.m11;
            hfloat minX = 2.0 * render->propertys.clipLeft / render->propertys.width  - 1.0;
            hfloat maxX = 2.0 * ( render->propertys.clipLeft + render->propertys.clipWidth ) / render->propertys.width - 1.0;
            hfloat minY = 2.0 * render->propertys.clipTop / render->propertys.height * 2.0 - 1.0;
            hfloat maxY = 2.0 * ( render->propertys.clipTop + render->propertys.clipHeight ) / render->propertys.height - 1.0;
            
            if( x >= minX && x <= maxX && y >= minY && y <= maxY){
                render->focusTouchId = touches->touches->touchId;
                return hbool_true;
            }
            return hbool_false;
        }
        else if(touches->state == C3DTouchStateMove){
            
            hint32 i;
            
            for(i=0;i<touches->length;i++){
                if(render->focusTouchId == touches->touches[i].touchId){
                    return hbool_true;
                }
            }
            
            return hbool_false;
        }
        else {
            hint32 i;
            
            for(i=0;i<touches->length;i++){
                if(render->focusTouchId == touches->touches[i].touchId){
                    render->focusTouchId = 0;
                    return hbool_true;
                }
            }
            
            return hbool_false;
        }
        
        return hbool_true;
    }
    return hbool_false;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC2DRenderInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC2DRender * render = (vmC2DRender *) object;
    vmVariant v = vmVariantListGet(args, 0);
    
    render->program.key = buffer_alloc(32,32);
    
    vmVariantToString(context,v,render->program.key);
    
    render->propertys.backgroundAlpha = 1.0;
    render->propertys.scale = 1.0;
    
    render->uniqueKeys.width = vmRuntimeContextGetUniqueKey(context,"width");
    render->uniqueKeys.height = vmRuntimeContextGetUniqueKey(context,"height");
    render->uniqueKeys.left = vmRuntimeContextGetUniqueKey(context,"left");
    render->uniqueKeys.top = vmRuntimeContextGetUniqueKey(context,"top");
    render->uniqueKeys.contentLeft = vmRuntimeContextGetUniqueKey(context,"contentLeft");
    render->uniqueKeys.contentTop = vmRuntimeContextGetUniqueKey(context,"contentTop");
    render->uniqueKeys.backgroundColor = vmRuntimeContextGetUniqueKey(context,"backgroundColor");
    render->uniqueKeys.backgroundAlpha = vmRuntimeContextGetUniqueKey(context,"backgroundAlpha");
    render->uniqueKeys.display = vmRuntimeContextGetUniqueKey(context,"display");
    render->uniqueKeys.mask = vmRuntimeContextGetUniqueKey(context,"mask");
    render->uniqueKeys.contents = vmRuntimeContextGetUniqueKey(context,"contents");
    render->uniqueKeys.contentsIndex = vmRuntimeContextGetUniqueKey(context,"contentsIndex");
    render->uniqueKeys.flickX = vmRuntimeContextGetUniqueKey(context,"flickX");
    render->uniqueKeys.cornerRadius = vmRuntimeContextGetUniqueKey(context,"cornerRadius");
    render->uniqueKeys.clipLeft = vmRuntimeContextGetUniqueKey(context,"clipLeft");
    render->uniqueKeys.clipTop = vmRuntimeContextGetUniqueKey(context,"clipTop");
    render->uniqueKeys.clipWidth = vmRuntimeContextGetUniqueKey(context,"clipWidth");
    render->uniqueKeys.clipHeight = vmRuntimeContextGetUniqueKey(context,"clipHeight");
    render->uniqueKeys.scale = vmRuntimeContextGetUniqueKey(context,"scale");
    render->uniqueKeys.contentsLeft = vmRuntimeContextGetUniqueKey(context,"contentsLeft");
    render->uniqueKeys.contentsTop = vmRuntimeContextGetUniqueKey(context,"contentsTop");
    render->uniqueKeys.contentsRight = vmRuntimeContextGetUniqueKey(context,"contentsRight");
    render->uniqueKeys.contentsBottom = vmRuntimeContextGetUniqueKey(context,"contentsBottom");
    render->uniqueKeys.isVisibleRect = vmRuntimeContextGetUniqueKey(context, "isVisibleRect");
    
    render->base.draw = (hbool (*)(struct _vmC3DRender * ,vmC3DNode * ,c3d_node_condition_t ,InvokeTickDeclare))vmC2DRenderDraw;
    render->base.condition = (c3d_node_condition_t ( *  )(struct _vmC3DRender * ,vmC3DNode * ,c3d_node_condition_t ,InvokeTickDeclare))vmC2DRenderCondition;
    render->base.receiveTouches = (hbool ( *  )(struct _vmC3DRender * ,vmC3DNode * ,c3d_node_condition_t ,c3d_touches_t * touches, InvokeTickDeclare))vmC2DRenderConditionReceiveTouches;
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC2DRenderDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC2DRender * render = (vmC2DRender *) object;
    
    buffer_dealloc(render->program.key);
    
    if(render->propertys.contents){
        vmObjectRelease(context,(vmObject *)render->propertys.contents);
    }
    
    if(render->propertys.mask){
        vmObjectRelease(context,(vmObject *)render->propertys.mask);
    }
    
 
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC2DRenderGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC2DRender * render = (vmC2DRender *) object;
    
    if(name == render->uniqueKeys.width){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.width;
    }
    else if(name == render->uniqueKeys.height){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.height;
    }
    else if(name == render->uniqueKeys.left){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.left;
    }
    else if(name == render->uniqueKeys.top){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.top;
    }
    else if(name == render->uniqueKeys.contentLeft){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentLeft;
    }
    else if(name == render->uniqueKeys.contentTop){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentTop;
    }
    else if(name == render->uniqueKeys.backgroundColor){
        rs.type = vmVariantTypeInt64;
        rs.value.int64Value = render->propertys.backgroundColor;
    }
    else if(name == render->uniqueKeys.backgroundAlpha){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.backgroundAlpha;
    }
    else if(name == render->uniqueKeys.display){
        rs.type = vmVariantTypeBoolean;
        rs.value.booleanValue = render->propertys.display;
    }
    else if(name == render->uniqueKeys.mask){
        if(render->propertys.mask){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *) render->propertys.mask;
        }
    }
    else if(name == render->uniqueKeys.contents){
        if(render->propertys.contents){
            rs.type = vmVariantTypeObject;
            rs.value.objectValue = (vmObject *) render->propertys.contents;
        }
    }
    else if(name == render->uniqueKeys.contentsIndex){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = render->propertys.contentsIndex;
    }
    else if(name == render->uniqueKeys.flickX){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.flickX;
    }
    else if(name == render->uniqueKeys.cornerRadius){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.cornerRadius;
    }
    else if(name == render->uniqueKeys.clipLeft){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = render->propertys.clipLeft;
    }
    else if(name == render->uniqueKeys.clipTop){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = render->propertys.clipTop;
    }
    else if(name == render->uniqueKeys.clipWidth){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = render->propertys.clipWidth;
    }
    else if(name == render->uniqueKeys.clipHeight){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = render->propertys.clipHeight;
    }
    else if(name == render->uniqueKeys.scale){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.scale;
    }
    else if(name == render->uniqueKeys.contentsLeft){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentsLeft;
    }
    else if(name == render->uniqueKeys.contentsTop){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentsTop;
    }
    else if(name == render->uniqueKeys.contentsRight){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentsRight;
    }
    else if(name == render->uniqueKeys.contentsBottom){
        rs.type = vmVariantTypeDouble;
        rs.value.doubleValue = render->propertys.contentsBottom;
    }
    else{
        return vmObjectClassGetPropertyCallback(context, clazz, object, name, InvokeTickArg);
    }
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC2DRenderSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC2DRender * render = (vmC2DRender *) object;
    
    if(render->c3dContext){
        render->c3dContext->needRender = hbool_true;
    }
    
    if(name == render->uniqueKeys.width){
        render->propertys.width = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.height){
        render->propertys.height = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.left){
        render->propertys.left = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.top){
        render->propertys.top = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.contentLeft){
        render->propertys.contentLeft = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.contentTop){
        render->propertys.contentTop = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.backgroundColor){
        render->propertys.backgroundColor = vmVariantToInt64(context,value);
    }
    else if(name == render->uniqueKeys.backgroundAlpha){
        render->propertys.backgroundAlpha = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.display){
        render->propertys.display = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.mask){
        
        if((value.type & vmVariantTypeObject) && vmRuntimeContextObjectIsKindClass(context,value.value.objectValue, (vmClass *) &vmC3DImageClass)){
            
            vmObjectRetain(context,value.value.objectValue);
            
            if(render->propertys.mask){
                vmObjectRelease(context,(vmObject *) render->propertys.mask);
            }
            
            render->propertys.mask = (vmC3DImage *)value.value.objectValue;
        }
        else{
            if(render->propertys.mask){
                vmObjectRelease(context,(vmObject *) render->propertys.mask);
            }
            
            render->propertys.mask = NULL;
        }
    }
    else if(name == render->uniqueKeys.contents){
        
        if((value.type & vmVariantTypeObject) && vmRuntimeContextObjectIsKindClass(context,value.value.objectValue, (vmClass *) &vmC3DImageClass)){
            
            vmObjectRetain(context,value.value.objectValue);
            
            if(render->propertys.contents){
                vmObjectRelease(context,(vmObject *) render->propertys.contents);
            }
            
            render->propertys.contents = (vmC3DImage *)value.value.objectValue;
        }
        else{
            if(render->propertys.contents){
                vmObjectRelease(context,(vmObject *) render->propertys.contents);
            }
            
            render->propertys.contents = NULL;
        }
    }
    else if(name == render->uniqueKeys.contentsIndex){
        render->propertys.contentsIndex = vmVariantToInt32(context,value);
    }
    else if(name == render->uniqueKeys.flickX){
        render->propertys.flickX = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.cornerRadius){
        render->propertys.cornerRadius = vmVariantToDouble(context,value);
    }
    else if(name == render->uniqueKeys.scale){
        render->propertys.scale = vmVariantToDouble(context, value);
    }
    else if(name == render->uniqueKeys.contentsLeft){
        render->propertys.contentsLeft = vmVariantToDouble(context, value);
    }
    else if(name == render->uniqueKeys.contentsTop){
        render->propertys.contentsTop = vmVariantToDouble(context, value);
    }
    else if(name == render->uniqueKeys.contentsRight){
        render->propertys.contentsRight = vmVariantToDouble(context, value);
    }
    else if(name == render->uniqueKeys.contentsBottom){
        render->propertys.contentsBottom = vmVariantToDouble(context, value);
    }
    else{
        return vmObjectClassSetPropertyCallback(context, clazz, object, name,value, InvokeTickArg);
    }
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC2DRenderInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC2DRender * render = (vmC2DRender *) object;
    
    if(name == render->uniqueKeys.isVisibleRect){
        {
            hfloat left = vmVariantToDouble(context, vmVariantListGet(args,0));
            hfloat top = vmVariantToDouble(context, vmVariantListGet(args,1));
            hfloat width = vmVariantToDouble(context, vmVariantListGet(args, 2));
            hfloat height = vmVariantToDouble(context, vmVariantListGet(args, 3));
            hfloat right = left + width;
            hfloat bottom = top + height;
            hfloat contentLeft = render->propertys.contentLeft;
            hfloat contentRight = contentLeft + render->propertys.width;
            hfloat contentTop = render->propertys.contentTop;
            hfloat contentBottom = contentTop + render->propertys.height;
            
            left = MAX(left, contentLeft);
            top = MAX(top,contentTop);
            right = MIN(right,contentRight);
            bottom = MIN(bottom,contentBottom);
            
            rs.type = vmVariantTypeBoolean;
            rs.value.booleanValue = right > left && bottom > top;
            
        }
        
        return rs;
    }
    
    return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
}

/**
 
 Return : Void
 */
static void vmC2DRenderPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmC2DRender * render = (vmC2DRender *) object;
    
    vmUniqueKeyListAdd(names, render->uniqueKeys.width);
    vmUniqueKeyListAdd(names, render->uniqueKeys.height);
    vmUniqueKeyListAdd(names, render->uniqueKeys.left);
    vmUniqueKeyListAdd(names, render->uniqueKeys.top);
    vmUniqueKeyListAdd(names, render->uniqueKeys.contentLeft);
    vmUniqueKeyListAdd(names, render->uniqueKeys.contentTop);
    vmUniqueKeyListAdd(names, render->uniqueKeys.backgroundColor);
    vmUniqueKeyListAdd(names, render->uniqueKeys.backgroundAlpha);
    vmUniqueKeyListAdd(names, render->uniqueKeys.display);
    vmUniqueKeyListAdd(names, render->uniqueKeys.mask);
    vmUniqueKeyListAdd(names, render->uniqueKeys.contents);
    vmUniqueKeyListAdd(names, render->uniqueKeys.contentsIndex);
    vmUniqueKeyListAdd(names, render->uniqueKeys.flickX);
    vmUniqueKeyListAdd(names, render->uniqueKeys.cornerRadius);
    vmUniqueKeyListAdd(names, render->uniqueKeys.clipLeft);
    vmUniqueKeyListAdd(names, render->uniqueKeys.clipTop);
    vmUniqueKeyListAdd(names, render->uniqueKeys.clipWidth);
    vmUniqueKeyListAdd(names, render->uniqueKeys.clipHeight);
    vmUniqueKeyListAdd(names, render->uniqueKeys.scale);
}

vmClassBase vmC2DRenderClass = {
    vmClassTypeBase,
    sizeof(vmC2DRender),
    vmC2DRenderInitCallback,
    vmC2DRenderDestroyCallback,
    vmC2DRenderGetPropertyCallback,
    vmC2DRenderSetPropertyCallback,
    vmC2DRenderPropertyNamesCallback,
    vmC2DRenderInvokeCallback,
};
