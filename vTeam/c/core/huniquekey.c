/*
 *  uniquekey.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hmap.h"
#include "huniquekey.h"

#undef unique_key

#undef unique_name

#undef unique_make_name

hmap_t g_unique_map = NULL;

static hmap_t unique_map(InvokeTickDeclare){
	if(g_unique_map ==NULL){
		g_unique_map = map_alloc( hash_code_str, equal_str);
	}
	return g_unique_map;
}	



unique_key_t unique_key(hcchar * name,InvokeTickDeclare){
	hmap_t map = unique_map(InvokeTickArg);
	hany key = map_get(map,(hany )name);
	if(key ==NULL){
		key = mem_malloc( (hint32)strlen(name)+1);
		strcpy((hchar *)key,name);
		map_put(unique_map(InvokeTickArg), key, key);
	}
	return (unique_key_t)key;
}

hcchar * unique_name(unique_key_t key,InvokeTickDeclare){
	return (hcchar *)key;
}

hcchar * unique_make_name(hcchar * name,InvokeTickDeclare){
	return (hcchar *)unique_key(name,InvokeTickArg);	
}

