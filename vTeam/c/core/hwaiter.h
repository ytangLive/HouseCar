/*
 * hwaiter.h
 *
 *  Created on: 2010-8-27
 *      Author: Administrator
 */

#ifndef HWAITER_H_
#define HWAITER_H_
#ifdef __cplusplus
extern "C" {
#endif

HANDLER(hwaiter_t)

hwaiter_t hwaiter_alloc(void);
void hwaiter_dealloc(hwaiter_t hwaiter);
void hwaiter_lock(hwaiter_t hwaiter);
void hwaiter_unlock(hwaiter_t hwaiter);
void hwaiter_wait(hwaiter_t hwaiter);
void hwaiter_join(hwaiter_t hwaiter);

#ifdef __cplusplus
}
#endif
	
#endif /* HWAITER_H_ */
