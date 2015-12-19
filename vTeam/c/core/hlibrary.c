/*
 *  hlibrary.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-13.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hlibrary.h"

hlibrary_t hlibrary_load(hcchar * name){
#ifdef __WIN32__
        return (hlibrary_t)LoadLibrary(name);
#else
	return (hlibrary_t) dlopen(name, RTLD_LAZY ); 
#endif
	
}

void hlibrary_unload(hlibrary_t library){
	if(library){
#ifndef __WIN32__
		dlclose(library); 
#endif
	}
}

hany hlibrary_proc(hlibrary_t library,hcchar * proc_name){
	if(library && proc_name){
		
#ifdef __WIN32__
                return GetProcAddress(library,proc_name);
#else
                return dlsym(library,proc_name);
#endif
	}
	return NULL;
}

