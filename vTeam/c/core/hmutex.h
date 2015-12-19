/*
 * hmutex.h
 *
 *  Created on: 2010-8-27
 *      Author: zhanghailong
 */

#ifndef HMUTEX_H_
#define HMUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif
	
    typedef struct _mutex_t{
        long unused;
    } * hmutex_t;

hmutex_t hmutex_alloc(void);
void hmutex_dealloc(hmutex_t hmutex);
void hmutex_lock(hmutex_t hmutex);
void hmutex_unlock(hmutex_t hmutex);

#ifdef __cplusplus
}
#endif
#endif /* HMUTEX_H_ */
