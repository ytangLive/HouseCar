//
//  hvmxml.c
//  hcvermin
//
//  Created by zhang hailong on 13-3-7.
//
//
#include "hconfig.h"
#include "hvmxml.h"
#include "hxml.h"
#include "hbuffer.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hdata_alloc.h"

typedef struct{
    vmObject base;
    hxml_element_t * element;
    struct{
        vmUniqueKey innerXML;
        vmUniqueKey length;
        vmUniqueKey childAt;
        vmUniqueKey tagName;
        vmUniqueKey tagNamespace;
        vmUniqueKey textContent;
    }uniqueKeys;
} vmXML;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmXMLClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    
    vmXML * xml = (vmXML *) object;
    vmVariant rs=  {vmVariantTypeVoid,0};
    hbuffer_t bName ,bNs;
    vmVariant name = vmVariantListGet(args, 0);
    vmVariant ns = vmVariantListGet(args, 1);
    
    if(name.type == vmVariantTypeThrowable){
        xml->element = hxml_element_retain((hxml_element_t *)name.value.objectValue, InvokeTickArg);
    }
    else{
        
        bName = buffer_alloc(128, 128);
        bNs = buffer_alloc(32, 32);
        
        vmVariantToString(context, name, bName);
        vmVariantToString(context, ns, bNs);
        
        xml->element = hxml_element_alloc(buffer_to_str(bName), buffer_to_str(bNs), InvokeTickArg);
        
        buffer_dealloc(bName);
        buffer_dealloc(bNs);
    }

    xml->uniqueKeys.innerXML = vmRuntimeContextGetUniqueKey(context, "innerXML");
    xml->uniqueKeys.length = vmRuntimeContextGetUniqueKey(context, "length");
    xml->uniqueKeys.childAt = vmRuntimeContextGetUniqueKey(context, "childAt");
    xml->uniqueKeys.tagName = vmRuntimeContextGetUniqueKey(context, "tagName");
    xml->uniqueKeys.tagNamespace = vmRuntimeContextGetUniqueKey(context, "tagNamespace");
    xml->uniqueKeys.textContent = vmRuntimeContextGetUniqueKey(context, "textContent");
    
    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmXMLClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmXML * xml = (vmXML *) object;
    hxml_element_release(xml->element, InvokeTickArg);
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmXMLClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmXML * xml = (vmXML *) object;
    
    if(name == xml->uniqueKeys.innerXML){
        
    }
    else if(name == xml->uniqueKeys.length){
        rs.type = vmVariantTypeInt32;
        rs.value.int32Value = hxml_element_child_count(xml->element, InvokeTickArg);
    }
    else if(name == xml->uniqueKeys.tagName){
        {
            hcchar * v = hxml_element_name(xml->element,NULL,InvokeTickArg);
            if(v){
                rs = vmStringAlloc(context,v );
            }
        }
    }
    else if(name == xml->uniqueKeys.tagNamespace){
        {
            hcchar * v = hxml_element_ns(xml->element,NULL,InvokeTickArg);
            if(v){
                rs = vmStringAlloc(context,v );
            }
        }
    }
    else if(name == xml->uniqueKeys.textContent){
        {
            hcchar * v = hxml_element_text(xml->element,NULL,InvokeTickArg);
            if(v){
                rs = vmStringAlloc(context,v );
            }
        }
    }
    else{
        {
            hcchar * n = vmRuntimeContextUniqueKeyToString(context, name);
            hcchar * v = hxml_element_attr(xml->element, n, NULL, InvokeTickArg);
            
            if(v){
                rs = vmStringAlloc(context, v);
            }
        }
    }
    
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmXMLClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmXML * xml = (vmXML *) object;
    
    if(name == xml->uniqueKeys.innerXML){
        {
            hbuffer_t v = buffer_alloc(128, 128);
            
            vmVariantToString(context, value, v);
            
            hxml_element_inner_xml_set(xml->element, buffer_to_str(v), InvokeTickArg);
            
            buffer_dealloc(v);
        }
    }
    else if(name == xml->uniqueKeys.length){
        
    }
    else if(name == xml->uniqueKeys.tagName){

    }
    else if(name == xml->uniqueKeys.tagNamespace){

    }
    else if(name == xml->uniqueKeys.textContent){
        {
            hbuffer_t v = buffer_alloc(128, 128);
            vmVariantToString(context, value, v);
            
            hxml_element_text_set(xml->element, buffer_to_str(v), InvokeTickArg);
            
            buffer_dealloc(v);
        }
    }
    else{
        {
            hbuffer_t v = buffer_alloc(128, 128);
            hcchar * n = vmRuntimeContextUniqueKeyToString(context, name);
           
            vmVariantToString(context, value, v);
            
            hxml_element_attr_set(xml->element, n, buffer_to_str(v), InvokeTickArg);
            
            buffer_dealloc(v);
        }
    }
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmXMLClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmXML * xml = (vmXML *) object;
    
    if(xml->uniqueKeys.childAt == name){
        {
            vmVariant v = vmVariantListGet(args, 0);
            hint32 index = vmVariantToInt32(context, v);
            hxml_element_t * element;
            vmVariantList vars;
            
            if(index >=0 && index< hxml_element_child_count(xml->element, InvokeTickArg)){
                element = hxml_element_child_at(xml->element, index, InvokeTickArg);
                
                vars = vmVariantListAlloc();
                v.type = vmVariantTypeThrowable;
                v.value.objectValue = (vmObject *) element;
                
                vmVariantListAdd(vars, v);
                
                rs = vmObjectAlloc(context, &vmXMLClass, vars);
                
                vmVariantListDealloc(vars);
            }
        }
        
        return rs;
    }
    return vmObjectClassInvokeCallback(context, clazz, object, name, args,InvokeTickArg);
}

typedef struct {
    vmRuntimeContext context;
    vmUniqueKeyList names;
} vmXMLClassPropertyNamesCallbackAttributeParam;

static void vmXMLClassPropertyNamesCallbackAttribute(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
    vmXMLClassPropertyNamesCallbackAttributeParam * param = (vmXMLClassPropertyNamesCallbackAttributeParam *) dest;
    vmUniqueKey uk = vmRuntimeContextGetUniqueKey(param->context, key);
    vmUniqueKeyListAdd(param->names, uk);
}
/**
 
 Return : Void
 */
static void vmXMLClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){
    vmXML * xml = (vmXML *) object;
    vmXMLClassPropertyNamesCallbackAttributeParam param = {context,names};
    if(xml->element){
        hdata_object_each(&hdata_class, xml->element->attributes, vmXMLClassPropertyNamesCallbackAttribute, &param);
    }
}


vmClassBase vmXMLClass = {vmClassTypeBase,sizeof(vmXML),vmXMLClassInitCallback,vmXMLClassDestroyCallback,vmXMLClassGetPropertyCallback,vmXMLClassSetPropertyCallback,vmXMLClassPropertyNamesCallback,vmXMLClassInvokeCallback};


