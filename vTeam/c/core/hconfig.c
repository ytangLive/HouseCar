/*
 *  hconfig.c
 *  C Library
 *
 *  Created by hailong zhang on 10-12-2.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"

void hclibrary_init(void){
    #ifdef HAS_PTHREAD
	signal(SIGPIPE,SIG_IGN);
    #endif
}
