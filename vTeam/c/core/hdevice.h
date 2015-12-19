//
//  hdevice.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-16.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HDEVICE_H
#define HDEVICE_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct{
        hubyte uniqueId[16];
    }device_t;
    
    device_t device(void);
    
#ifdef __cplusplus
}
#endif

#endif
