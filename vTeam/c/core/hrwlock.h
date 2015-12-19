//
//  hrwlock.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-6-20.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HRWLOCK_H
#define HRWLOCK_H

#ifdef __cplusplus
extern "C" {
#endif
    
    HANDLER(hrwlock_t)
    
    hrwlock_t hrwlock_alloc(void);
    void hrwlock_dealloc(hrwlock_t hrwlock);
    void hrwlock_rdlock(hrwlock_t hrwlock);
    void hrwlock_wrlock(hrwlock_t hrwlock);
    void hrwlock_unlock(hrwlock_t hrwlock);

    
#ifdef __cplusplus
}
#endif

#endif
