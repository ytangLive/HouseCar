//
//  hvm3d_graphics.c
//  hvm3d
//
//  Created by zhang hailong on 13-3-20.
//  Copyright (c) 2013年 9vteam. All rights reserved.
//

#include "hconfig.h"
#include "hvm3d_graphics.h"
#include "hvermin_runtime.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hbuffer.h"

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DGraphicsInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    vmC3DGraphics * graphics = (vmC3DGraphics *) object;
    
    graphics->uniqueKeys.drawText = vmRuntimeContextGetUniqueKey(context,"drawText");
    graphics->uniqueKeys.fillRect = vmRuntimeContextGetUniqueKey(context,"fillRect");
    graphics->uniqueKeys.textSize = vmRuntimeContextGetUniqueKey(context,"textSize");
    
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmC3DGraphicsDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmC3DGraphicsGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmC3DGraphicsSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs= {vmVariantTypeVoid,0};
    
    
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmC3DGraphicsInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    
    vmVariant rs= {vmVariantTypeVoid,0};
    
    vmC3DGraphics * graphics = (vmC3DGraphics *) object;

    if(graphics->uniqueKeys.drawText == name){
        {
            c3d_font_t f = vmVariantToFont(context, vmVariantListGet(args, 3), InvokeTickArg);
            c3d_color3d_t c = vmVariantToColor(context, vmVariantListGet(args, 4), InvokeTickArg);
            hbuffer_t text =buffer_alloc(128, 128);
            c3d_vector2_t p = {0.0f,0.0f};
            
            if(vmVariantListCount(args) >5){
                c.a = vmVariantToDouble(context, vmVariantListGet(args,5));
            }
            
            vmVariantToString(context, vmVariantListGet(args, 2), text);
            
            p.x = vmVariantToDouble(context, vmVariantListGet(args, 0));
            p.y = vmVariantToDouble(context, vmVariantListGet(args, 1));
            
            c3d_graphics_draw_text(graphics->ctx, p, buffer_to_str(text), c, f, InvokeTickArg);
            
            buffer_dealloc(text);
            
        }
        return rs;
    }
    else if(graphics->uniqueKeys.textSize == name){
        {
            c3d_font_t f = vmVariantToFont(context, vmVariantListGet(args, 1), InvokeTickArg);
            hbuffer_t text = buffer_alloc(128, 128);
            c3d_vector2_t v2;
            vmVariant v = {0};
            
            vmVariantToString(context, vmVariantListGet(args, 0), text);
            
            v2 = c3d_graphics_text_size(graphics->ctx, buffer_to_str(text), f, InvokeTickArg);
            
            buffer_dealloc(text);
            
            rs = vmDictionaryAlloc(context);
            
            v.type = vmVariantTypeDouble;
            v.value.doubleValue = v2.width * graphics->ctx->scale;
            
            vmDictionaryPut(context, rs.value.objectValue, "width", v);
            
            v.type = vmVariantTypeDouble;
            v.value.doubleValue = v2.height * graphics->ctx->scale;
            
            vmDictionaryPut(context, rs.value.objectValue, "height", v);
            
            
        }
        return rs;
    }
    else if(graphics->uniqueKeys.fillRect == name){
        {
            c3d_color3d_t c = vmVariantToColor(context, vmVariantListGet(args, 4), InvokeTickArg);
            
            if(vmVariantListCount(args) >5){
                c.a = vmVariantToDouble(context, vmVariantListGet(args,5));
            }
            
            c3d_graphics_fill_rect(graphics->ctx
                                   , vmVariantToInt32(context, vmVariantListGet(args, 0))
                                   , vmVariantToInt32(context, vmVariantListGet(args, 1))
                                   , vmVariantToInt32(context, vmVariantListGet(args, 2))
                                   , vmVariantToInt32(context, vmVariantListGet(args, 3))
                                   , c, InvokeTickArg);
        }
        return rs;
    }
    
    return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
}

/**
 
 Return : Void
 */
static void vmC3DGraphicsPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    
}

vmClassBase vmC3DGraphicsClass = {
    vmClassTypeBase,
    sizeof(vmC3DGraphics),
    vmC3DGraphicsInitCallback,
    vmC3DGraphicsDestroyCallback,
    vmC3DGraphicsGetPropertyCallback,
    vmC3DGraphicsSetPropertyCallback,
    vmC3DGraphicsPropertyNamesCallback,
    vmC3DGraphicsInvokeCallback,
};


c3d_color3d_t vmVariantToColor(vmRuntimeContext ctx, vmVariant value,InvokeTickDeclare){
    c3d_color3d_t c = {0.0f,0.0f,0.0f,1.0f};
    hint64 v = vmVariantToInt64(ctx, value);
    
    c.r = (float)((v & 0xff0000) >> 16) / 0xff;
    c.g = (float)((v & 0x00ff00) >> 8 ) / 0xff;
    c.b = (float)((v & 0x0000ff)) / 0xff ;
    
    return c;
}

c3d_font_t vmVariantToFont(vmRuntimeContext ctx, vmVariant value,InvokeTickDeclare){
    c3d_font_t font = {"","",14};
    hbuffer_t buf = buffer_alloc(64, 64);
    
    vmVariantToString(ctx, value,buf);
    
    sscanf(buffer_to_str(buf), "%fpx %31s",&font.size,font.family);
    
    if(strcmp(font.family, "bold") == 0){
        strcpy(font.family, "");
        strcpy(font.name, "bold");
    }
    else if(strcmp(font.family, "italic") ==0){
        strcpy(font.family, "");
        strcpy(font.name, "italic");
    }
    
    buffer_dealloc(buf);
    
    return font;
}

