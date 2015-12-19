//
//  hstream_websocket.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HSTREAM_WEBSOCKET_H
#define HSTREAM_WEBSOCKET_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hstream.h"
#include "hmem.h"
    
    
    stream_t * stream_websocket_alloc(hsocket_t sock,InvokeTickDeclare);
    
    
    
#ifdef __cplusplus
}
#endif

#endif
