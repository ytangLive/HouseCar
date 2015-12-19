/*
 *  uniquekey.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HUNIQUEKEY_H
#define HUNIQUEKEY_H


#ifdef __cplusplus
extern "C" {
#endif
	
	typedef hintptr unique_key_t; 
	
	unique_key_t unique_key(hcchar * name,InvokeTickDeclare);
	
	hcchar * unique_name(unique_key_t key,InvokeTickDeclare);
	
	hcchar * unique_make_name(hcchar * name,InvokeTickDeclare);
    
#define unique_key(a) unique_key((a),InvokeTickArg)
	
#define unique_name(a) unique_name((a),InvokeTickArg)
	
#define unique_make_name(a) unique_make_name((a),InvokeTickArg)
	
#ifdef __cplusplus
}
#endif


#endif
