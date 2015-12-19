//
//  hvermin_build.h
//  C Library
//
//  Created by Hailong Zhang on 11-11-1.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_BUILD_H
#define _HVERMIN_BUILD_H


#ifdef __cplusplus
extern "C" {
#endif
    
#include "hvermin_compile_scanner.h"
    
    vmScannerResult vmBuild(hcchar * files,hcchar * forDir,hcchar *outFile,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
