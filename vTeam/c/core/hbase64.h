//
//  hbase64.h
//  C Library
//
//  Created by hailong zhang on 11-5-27.
//  Copyright 2011年 hailong.org. All rights reserved.
//


#ifndef HBASE64_H
#define HBASE64_H

#include "hmem.h"
#include "hbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	hbool hbase64_encode(hbyte * data,hint32 length,hbuffer_t buffer,InvokeTickDeclare);
    
	hbool hbase64_decode(hcchar * str,hbuffer_t buffer,InvokeTickDeclare);
    
    huint32 hbase64_decode_bytes(hchar * inData,huint32 inLength,hchar * outData ,huint32 outLength);
	
#define hbase64_encode(a,b,c) hbase64_encode((a),(b),(c),InvokeTickArg)
    
#define hbase64_decode(a,b) hbase64_decode((a),(b),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif


#endif
