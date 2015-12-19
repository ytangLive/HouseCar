//
//  hstream_tcp.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HSTREAM_TCP_H
#define HSTREAM_TCP_H

#ifdef __cplusplus
extern "C" {
#endif
   
#include "hstream.h"
#include "hmem.h"
    

    stream_t * stream_tcp_alloc(hsocket_t sock,InvokeTickDeclare);
    
    stream_t * stream_tcp_alloc_with_host(hcchar * hostname,hint32 port,InvokeTickDeclare);

#ifdef __cplusplus
}
#endif


#endif
