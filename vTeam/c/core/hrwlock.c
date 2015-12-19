//
//  hrwlock.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-6-20.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hrwlock.h"

#ifdef HAS_PTHREAD

hrwlock_t hrwlock_alloc(void){
    pthread_rwlock_t * rwlock = malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(rwlock, NULL);
	return (hrwlock_t)rwlock;
}

void hrwlock_dealloc(hrwlock_t hrwlock){
    if(hrwlock){
        pthread_rwlock_destroy((pthread_rwlock_t *)hrwlock);
    }
}

void hrwlock_rdlock(hrwlock_t hrwlock){
    pthread_rwlock_rdlock((pthread_rwlock_t *)hrwlock);
}

void hrwlock_wrlock(hrwlock_t hrwlock){
    pthread_rwlock_wrlock((pthread_rwlock_t *)hrwlock);
}

void hrwlock_unlock(hrwlock_t hrwlock){
    pthread_rwlock_unlock((pthread_rwlock_t *)hrwlock);
}

#else



#endif