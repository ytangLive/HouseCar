//
//  hstream_process.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-30.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HSTREAM_PROCESS_H
#define HSTREAM_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hstream.h"
#include "hmem.h"
    
    
    stream_t * stream_process_alloc(hcchar * command,InvokeTickDeclare);
    
    stream_t * stream_process_alloc_with_io(hcchar * command,int inno,int outno,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
