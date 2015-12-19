//
//  hobjc_json.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_json_h
#define hobjc_hobjc_json_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc.h"
    
    hobj_t * hobj_json_decode(hcchar * json,InvokeTickDeclare);
    
    hobj_t * hobj_json_encode(hobj_t * obj,InvokeTickDeclare);
    
    hcchar * hobj_json_encode_c(hobj_t * obj,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif


#endif
