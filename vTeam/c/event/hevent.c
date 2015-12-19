//
//  hevent.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-18.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hevent.h"
#include "hext_obj.h"
#include "hstr.h"
#include "hserial_list.h"

static void ev_event_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    ev_event_t * impl = (ev_event_t *)obj;
    strncpy(impl->name, arg, sizeof(impl->name));
}

static void ev_event_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
   
}

static ext_class_t ev_event_ext_class = {ev_event_ext_init,ev_event_ext_destroy,sizeof(ev_event_t)};


ev_event_t * ev_event_alloc(hcchar * name,hint32 extraBytes, InvokeTickDeclare){
    return (ev_event_t *)ext_obj_alloc_with_extra(extraBytes, (hany)name, &ev_event_ext_class);
}

ev_event_t * ev_event_retain(ev_event_t * event,InvokeTickDeclare){
    return (ev_event_t *)ext_obj_retain((hext_obj_t)event);
}

void ev_event_release(ev_event_t * event,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)event);
}

typedef struct{
    hchar name[EVENT_NAME_SIZE];
    ev_dispatch_callback_t callback;
    hany context;
}ev_dispatch_listener_t;

void ev_dispatch_init(ev_dispatch_t * dispatch,InvokeTickDeclare){
    dispatch->listeners = serial_list_alloc(sizeof(ev_dispatch_listener_t), 0);
}

void ev_dispatch_destroy(ev_dispatch_t * dispatch,InvokeTickDeclare){
    serial_list_dealloc(dispatch->listeners);
}

static void ev_dispatch_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    ev_dispatch_t * impl = (ev_dispatch_t *)obj;
    ev_dispatch_init(impl, InvokeTickArg);
}

static void ev_dispatch_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    ev_dispatch_t * impl = (ev_dispatch_t *)obj;
    ev_dispatch_destroy(impl, InvokeTickArg);
}

static ext_class_t ev_dispatch_ext_class = {ev_dispatch_ext_init,ev_dispatch_ext_destroy,sizeof(ev_dispatch_t)};

ev_dispatch_t * ev_dispatch_alloc(InvokeTickDeclare){
    return (ev_dispatch_t *) ext_obj_alloc(NULL, &ev_dispatch_ext_class);
}

ev_dispatch_t * ev_dispatch_retain(ev_dispatch_t * dispatch,InvokeTickDeclare){
    return (ev_dispatch_t *) ext_obj_retain((hext_obj_t) dispatch);
}

void ev_dispatch_release(ev_dispatch_t * dispatch,InvokeTickDeclare){
    ext_obj_release((hext_obj_t) dispatch);
}

void ev_dispatch_listener_add(ev_dispatch_t * dispatch,hcchar * name, ev_dispatch_callback_t callback,hany context,InvokeTickDeclare){
    
    ev_dispatch_t * impl = (ev_dispatch_t *)dispatch;
    ev_dispatch_listener_t listener = {0};
    listener.callback = callback;
    listener.context = context;
    strncpy(listener.name, name, sizeof(listener.name));
    serial_list_item_add(impl->listeners, &listener);

}

void ev_dispatch_listener_remove(ev_dispatch_t * dispatch,hcchar * name,ev_dispatch_callback_t callback,hany context,InvokeTickDeclare){
    ev_dispatch_t * impl = (ev_dispatch_t *)dispatch;
    hint32 i;
    ev_dispatch_listener_t * listener;
    
    for(i=0;i<serial_list_count(impl->listeners);i++){
        listener = serial_list_item_at(impl->listeners, i);
        if((callback == NULL || listener->callback == callback)
           && (context == NULL || listener->context == context)){
            serial_list_remove_at(impl->listeners, i--);
        }
    }
}

void ev_dispatch_fire(ev_dispatch_t * dispatch,ev_event_t * event,InvokeTickDeclare){
    ev_dispatch_t * impl = (ev_dispatch_t *)dispatch, * parent = dispatch->parent;
    hint32 i,c;
    ev_dispatch_listener_t * listener;
    hserial_list_t lis =serial_list_alloc(sizeof(ev_dispatch_listener_t), 0);
    
    if(event->target == NULL){
        event->target = (ev_dispatch_t *)impl;
    }
    
    c = serial_list_count(impl->listeners);
    
    for(i=0;i<c;i++){
        listener = serial_list_item_at(impl->listeners, i);
        if(str_has_prefix(listener->name,event->name)){
            serial_list_item_add(lis, listener);
        }
    }
    
    c = serial_list_count(lis);
    
    for(i=0;i<c;i++){
        listener = serial_list_item_at(lis, i);
        if(! (* listener->callback)(dispatch,event,listener->context,InvokeTickArg)){
            break;
        }
    }
    
    serial_list_dealloc(lis);

    if(!event->cancelBubble && parent){
        ev_dispatch_fire(parent, event, InvokeTickArg);
    }
    
    
}





