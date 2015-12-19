/*
 *  hlibrary.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-13.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HLIBRARY_H
#define HLIBRARY_H



#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hlibrary_t)
	
	hlibrary_t hlibrary_load(hcchar * name);
	
	void hlibrary_unload(hlibrary_t library);
	
	hany hlibrary_proc(hlibrary_t library,hcchar * proc_name);
	
	
#ifdef __cplusplus
}
#endif


#endif
