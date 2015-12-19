//
//  hevent.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-18.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HEVENT_H
#define HEVENT_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hmem.h"
#include "hserial_list.h"
    
#define EVENT_NAME_SIZE  128

    struct _ev_dispatch_t{
        struct _ev_dispatch_t * parent;
        struct _serial_list_t * listeners;
    };
    
    typedef struct _ev_dispatch_t ev_dispatch_t;
    
    typedef struct _ev_event_t{
        hchar READONLY name[EVENT_NAME_SIZE];
        ev_dispatch_t * READONLY target;
        hbool cancelBubble;
    }ev_event_t;
    
    void ev_dispatch_init(ev_dispatch_t * dispatch,InvokeTickDeclare);
    
    void ev_dispatch_destroy(ev_dispatch_t * dispatch,InvokeTickDeclare);
    
    ev_dispatch_t * ev_dispatch_alloc(InvokeTickDeclare);
    
    ev_dispatch_t * ev_dispatch_retain(ev_dispatch_t * dispatch,InvokeTickDeclare);
    
    void ev_dispatch_release(ev_dispatch_t * dispatch,InvokeTickDeclare);
    
    typedef hbool (*ev_dispatch_callback_t)(ev_dispatch_t * dispatch,ev_event_t * event,hany context,InvokeTickDeclare);
    
    void ev_dispatch_listener_add(ev_dispatch_t * dispatch,hcchar * name, ev_dispatch_callback_t callback,hany context,InvokeTickDeclare);
    
    void ev_dispatch_listener_remove(ev_dispatch_t * dispatch,hcchar * name,ev_dispatch_callback_t callback,hany context,InvokeTickDeclare);
    
    void ev_dispatch_fire(ev_dispatch_t * dispatch,ev_event_t * event,InvokeTickDeclare);
    
    ev_event_t * ev_event_alloc(hcchar * name,hint32 extraBytes, InvokeTickDeclare);
    
    ev_event_t * ev_event_retain(ev_event_t * event,InvokeTickDeclare);
    
    void ev_event_release(ev_event_t * event,InvokeTickDeclare);
    
    
    
#ifdef __cplusplus
}
#endif


#endif
