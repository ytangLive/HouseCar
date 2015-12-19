/*
 *  hdebug.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-12.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HDEBUG_H
#define HDEBUG_H

#ifdef __cplusplus
extern "C" {
#endif
	
extern hint32 hdebug_random_asset;
extern huint32 hdebug_delay_second;

#ifdef _DEBUG


#define hdebug_asset (rand() % 100 >= hdebug_random_asset)	
#define hdebug_delay if(hdebug_delay_second>0) sleep(hdebug_delay_second);

#endif

#ifndef _DEBUG


#define hdebug_asset 1	
#define hdebug_delay 

#endif

#ifdef __cplusplus
}
#endif

#endif
