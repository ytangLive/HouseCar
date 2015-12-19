//
//  hvmcompiler_build.h
//  vmcompiler
//
//  Created by Zhang Hailong on 13-3-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vmcompiler_hvmcompiler_build_h
#define vmcompiler_hvmcompiler_build_h

#ifdef __cplusplus
extern "C" {
#endif

#include "hvmcompiler.h"
#include "hinifile.h"
    
    hbool vmBuild(hcchar * project, hobj_array_t * errors, InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
