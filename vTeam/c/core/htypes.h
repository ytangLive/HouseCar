/*
 *  htypes.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-5.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef HTYPES_H
#define HTYPES_H


#ifdef __cplusplus
extern "C" {
#endif
	
typedef char hbyte;
typedef unsigned char hubyte;
typedef short hword;
typedef unsigned short huword;
typedef int hdword;
typedef unsigned int hudword;
typedef char hchar;
typedef unsigned char huchar;
typedef const char hcchar;
typedef const unsigned char hcuchar;
typedef short hint16;
typedef unsigned short huint16;
typedef int hint32;
typedef unsigned int huint32;
typedef long long hint64;
typedef unsigned long long huint64;
typedef long hintptr;
typedef unsigned long huintptr;
typedef float hfloat;
typedef double hdouble;
typedef void * hany;
typedef const void * hcany;
typedef void hvoid;

typedef int hbool;
typedef unsigned short hwchar;

#define hbool_true	1
#define hbool_false	0



#define HINT16_BYTES	2
#define HINT32_BYTES	4
#define HINT64_BYTES	8
#define HFLOAT_BYTES	4
#define HDOUBLE_BYTES	8
#define HBOOL_BYTES		1
#define HPTR_BYTES		8
#define HTYPE_MAX_BYTES	8
#define HBYTE_BYTES		1

#define TOPTR(v)  	((hany)(hintptr)(v))
#define FROMPTR(p)	((hintptr)(p))

hint32 type_int16_to_bytes(hbyte * bytes,hint16 v);
hint32 type_int32_to_bytes(hbyte * bytes,hint32 v);
hint32 type_int64_to_bytes(hbyte * bytes,hint64 v);
hint32 type_float_to_bytes(hbyte * bytes,hfloat v);
hint32 type_double_to_bytes(hbyte * bytes,hdouble v);
hint32 type_bool_to_bytes(hbyte * bytes,hbool v);
hint32 type_byte_to_bytes(hbyte * bytes,hbyte v);

hint16 type_int16_from_bytes(hbyte * bytes);
hint32 type_int32_from_bytes(hbyte * bytes);
hint64 type_int64_from_bytes(hbyte * bytes);
hfloat type_float_from_bytes(hbyte * bytes);
hdouble type_double_from_bytes(hbyte * bytes);
hbool	type_bool_from_bytes(hbyte * bytes);
hbyte	type_byte_from_bytes(hbyte * bytes);

hint32 hatoi(hcchar * str);
    
#ifdef __cplusplus
}
#endif

#endif

