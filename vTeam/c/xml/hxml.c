//
//  hxml.c
//  hxml
//
//  Created by zhang hailong on 13-3-6.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hxml.h"
#include "hxml_scanf.h"
#include "hdata_alloc.h"
#include "hlist.h"
#include "hext_obj.h"
#include "hbuffer.h"
#include "hlog.h"

typedef struct {
    hcchar * name;
    hcchar * ns;
} hxml_element_param_t;

static void hxml_element_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    hxml_element_param_t * param = (hxml_element_param_t *) arg;
    hxml_element_t * element = (hxml_element_t *) obj;
    
    element->ns = hdata_string_alloc(param->ns);
    element->name = hdata_string_alloc(param->name);
    
}

static void hxml_element_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    hxml_element_t * element = (hxml_element_t *) obj;
    hint32 i,c = list_count(element->childs);
    hxml_element_t * el;
    
    for(i=0;i<c;i++){
        el = hxml_element_child_at(element, i, InvokeTickArg);
        hxml_element_release(el, InvokeTickArg);
    }
    
    hdata_dealloc(element->ns);
    hdata_dealloc(element->name);
    hdata_dealloc(element->text);
    hdata_dealloc(element->attributes);
    list_dealloc(element->childs);
}

static ext_class_t hxml_element_class = {hxml_element_init,hxml_element_destroy,sizeof(hxml_element_t)};


hxml_element_t * hxml_element_alloc(hcchar * name,hcchar * ns, InvokeTickDeclare){
    hxml_element_param_t param = {name,ns};
    if(strcmp(name, "sectionirp") ==0){
        printf("");
    }
    return (hxml_element_t *) ext_obj_alloc(&param, &hxml_element_class);
}

hxml_element_t * hxml_element_retain(hxml_element_t * element,InvokeTickDeclare){
    return (hxml_element_t *) ext_obj_retain((hext_obj_t) element);
}

void hxml_element_release(hxml_element_t * element,InvokeTickDeclare){
    ext_obj_release((hext_obj_t) element);
}

hcchar * hxml_element_ns(hxml_element_t * element,hcchar * defaultValue,InvokeTickDeclare){
    return hdata_string(&hdata_class, element->ns, defaultValue);
}

hcchar * hxml_element_name(hxml_element_t * element,hcchar * defaultValue,InvokeTickDeclare){
    return hdata_string(&hdata_class, element->name, defaultValue);
}

hcchar * hxml_element_attr(hxml_element_t * element,hcchar * name,hcchar * defaultValue,InvokeTickDeclare){
    hdata_t v = hdata_object(&hdata_class, element->attributes, name);
    if(v){
        return hdata_string(&hdata_class, v, defaultValue);
    }
    return defaultValue;
}

void hxml_element_attr_set(hxml_element_t * element,hcchar * name,hcchar * value,InvokeTickDeclare){
    hdata_t v = hdata_string_alloc(value);
    if(element->attributes == NULL){
        element->attributes = hdata_object_alloc();
    }
    hdata_object_put(element->attributes, name, v);
}

void hxml_element_attr_set_format(hxml_element_t * element,hcchar * name,hcchar * format,InvokeTickDeclare,...){
    hbuffer_t value = buffer_alloc(128, 128);
    va_list va;
    
    va_start(va, __InvokeTick__);
    
    buffer_append_format_va_list(value, format, va);
    
    hxml_element_attr_set(element, name, buffer_to_str(value), InvokeTickArg);
    
    buffer_dealloc(value);
}

hcchar * hxml_element_text(hxml_element_t * element,hcchar * defalutValue,InvokeTickDeclare){
    return hdata_string(&hdata_class, element->text, defalutValue);
}

void hxml_element_text_set(hxml_element_t * element,hcchar * text,InvokeTickDeclare){
    hdata_t v = hdata_string_alloc(text);
    hdata_dealloc(element->text);
    element->text = v;
}

void hxml_element_text_set_format(hxml_element_t * element,hcchar * format,InvokeTickDeclare,...){
    hbuffer_t value = buffer_alloc(128, 128);
    va_list va;
    
    va_start(va, __InvokeTick__);
    
    buffer_append_format_va_list(value, format, va);
    
    hxml_element_text_set(element, buffer_to_str(value), InvokeTickArg);
    
    buffer_dealloc(value);

}

void hxml_element_child_add(hxml_element_t * element,hxml_element_t * child,InvokeTickDeclare){
    hxml_element_retain(child, InvokeTickArg);
    if(element->childs == NULL){
        element->childs = list_alloc(4, 20);
    }
    list_add(element->childs, child);
}

void hxml_element_child_insert(hxml_element_t * element,hxml_element_t * child,hint32 atIndex,InvokeTickDeclare){
    hxml_element_retain(child, InvokeTickArg);
    if(element->childs == NULL){
        element->childs = list_alloc(4, 20);
    }
    list_insert(element->childs, child, atIndex);
}

hint32 hxml_element_child_count(hxml_element_t * element,InvokeTickDeclare){
    return list_count(element->childs);
}

hxml_element_t * hxml_element_child_at(hxml_element_t * element, hint32 index,InvokeTickDeclare){
    return (hxml_element_t *) list_get(element->childs, index);
}

void hxml_element_child_clear(hxml_element_t * element,InvokeTickDeclare){
    hint32 i,c = list_count(element->childs);
    for(i=0;i<c;i++){
        hxml_element_release(list_get(element->childs, i), InvokeTickArg);
    }
    list_clear(element->childs);
}

void hxml_element_inner_xml_set(hxml_element_t * element,hcchar * innerXML,InvokeTickDeclare){
    hxml_element_child_clear(element,InvokeTickArg);
    hxml_element_inner_xml_add(element, innerXML, InvokeTickArg);
}

static hany hxml_element_scanf_element_new(struct _hxml_scanf_t * xml,hcchar * name,hcchar * ns, InvokeTickDeclare){
    return hxml_element_alloc(name, ns, InvokeTickArg);
}

static void hxml_element_scanf_element_attr_set(struct _hxml_scanf_t * xml,hany element,hcchar * key,hcchar * value,InvokeTickDeclare){
    hxml_element_attr_set((hxml_element_t *) element, key, value, InvokeTickArg);
}

static void hxml_element_scanf_element_child_add(struct _hxml_scanf_t * xml,hany element,hany child,InvokeTickDeclare){
    hxml_element_child_add((hxml_element_t *) element, (hxml_element_t *) child, InvokeTickArg);
}

static hbool hxml_element_scanf_element_is_empty(struct _hxml_scanf_t * xml,hany element,InvokeTickDeclare){
    return hxml_element_child_count((hxml_element_t *) element, InvokeTickArg) == 0;
}

static void hxml_element_scanf_element_text_set(struct _hxml_scanf_t * xml,hany element,hcchar * text,InvokeTickDeclare){
    hxml_element_text_set((hxml_element_t *)element, text, InvokeTickArg);
}

static hbool hxml_element_scanf_tag_has_children(struct _hxml_scanf_t * xml,hcchar * name,hcchar * ns,InvokeTickDeclare){
    return hbool_true;
}

static void hxml_element_scanf_element_release(struct _hxml_scanf_t * xml,hany element,InvokeTickDeclare){
    hxml_element_release((hxml_element_t *) element, InvokeTickArg);
}


static hxml_scanf_t hxml_element_scanf = {
    hxml_element_scanf_element_new,
    hxml_element_scanf_element_attr_set,
    hxml_element_scanf_element_child_add,
    hxml_element_scanf_element_is_empty,
    hxml_element_scanf_element_text_set,
    hxml_element_scanf_tag_has_children,
    hxml_element_scanf_element_release,
    HXMLScanfDecodeCaseModeNone,
};

void hxml_element_inner_xml_add(hxml_element_t * element,hcchar * innerXML,InvokeTickDeclare){
    hxml_scanf(&hxml_element_scanf,innerXML,element,InvokeTickArg);
}

static void hxml_element_print_echo_callback(hdata_t data,hcchar *key,hdata_t value,hany dest,InvokeTickDeclare){
    hchar * v = (hchar *)hdata_string(&hdata_class, value, NULL);
    hlog(" %s=\"",key);
    while(v && * v != '\0'){
        
        if(* v == '"'){
            hlog("\\\"");
        }
        else{
            hlog("%c",*v);
        }
        
        v ++;
    }
    hlog("\"");
}

static void _hxml_element_print_level(hint32 level,InvokeTickDeclare){
    while(level >0){
        hlog("\t");
        level --;
    }
}

static void _hxml_element_print(hxml_element_t * element,hint32 level,InvokeTickDeclare){
    hcchar * ns = hxml_element_ns(element, NULL, InvokeTickArg);
    hcchar * name = hxml_element_name(element, NULL, InvokeTickArg);
    hcchar * text = hxml_element_text(element, NULL, InvokeTickArg);
    hint32 i,c;
    if(ns && * ns != '\0'){
        hlog("<%s:%s",ns,name);
    }
    else{
        hlog("<%s",name);
    }
    hdata_object_each(&hdata_class, element->attributes, hxml_element_print_echo_callback, NULL);
    if(text && * text != '\0'){
        hlog(">%s",text);
        
        if(ns && * ns != '\0'){
            hlog("</%s:%s>",ns,name);
        }
        else{
            hlog("</%s>",name);
        }
    }
    else if((c = hxml_element_child_count(element, InvokeTickArg)) >0){
        hlog(">\n");
        
        for(i=0;i<c;i++){
            _hxml_element_print_level(level +1,InvokeTickArg);
            _hxml_element_print(hxml_element_child_at(element, i, InvokeTickArg), level +1, InvokeTickArg);
            hlog("\n");
        }
        
        _hxml_element_print_level(level,InvokeTickArg);
        
        if(ns && * ns != '\0'){
            hlog("</%s:%s>",ns,name);
        }
        else{
            hlog("</%s>",name);
        }
    }
    else{
        hlog(" />");
    }
}

void hxml_element_print(hxml_element_t * element,InvokeTickDeclare){
    _hxml_element_print(element,0,InvokeTickArg);
}
