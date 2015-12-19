//
//  hstream_pipe.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HSTREAM_PIPE_H
#define HSTREAM_PIPE_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hstream.h"
#include "hmem.h"
    
    
    stream_t * stream_pipe_alloc(hint32 pipe_in,hint32 pipe_out,InvokeTickDeclare);
    
    
    
#ifdef __cplusplus
}
#endif


#endif
