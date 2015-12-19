/*
 *  htypes.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-11.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"

#define BYTE_BITS	8

hint32 type_int16_to_bytes(hbyte * bytes,hint16 v){
	huint16 value =(huint16) v;
	hint32 c = HINT16_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c--){
		*p = value & 0x0ff;
		value = value >> BYTE_BITS;
		p++;
	}
	return HINT16_BYTES;
}

hint32 type_int32_to_bytes(hbyte * bytes,hint32 v){
	huint32 value =(huint32) v;
	hint32 c = HINT32_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c--){
		*p = value & 0x0ff;
		value = value >> BYTE_BITS;
		p++;
	}
	return HINT32_BYTES;
}

hint32 type_int64_to_bytes(hbyte * bytes,hint64 v){
	huint64 value =(huint64) v;
	hint32 c = HINT64_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c--){
		*p = value & 0x0ff;
		value = value >> BYTE_BITS;
		p++;
	}
	return HINT64_BYTES;
}

hint32 type_float_to_bytes(hbyte * bytes,hfloat v){
	huint32 value =* (huint32 *)&v;
	hint32 c = HFLOAT_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c--){
		*p = value & 0x0ff;
		value = value >> BYTE_BITS;
		p++;
	}
	return HFLOAT_BYTES;
}

hint32 type_double_to_bytes(hbyte * bytes,hdouble v){
	memcpy(bytes,&v,HDOUBLE_BYTES);
	/*
	huint64 value =*(huint64 *)&v;
	hint32 c = HDOUBLE_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c--){
		*p = value & 0x0ff;
		value = value >> BYTE_BITS;
		p++;
	}*/
	return HDOUBLE_BYTES;
}

hint32 type_bool_to_bytes(hbyte * bytes,hbool v){
	bytes[0] = v;
	return HBOOL_BYTES;
}

hint32 type_byte_to_bytes(hbyte * bytes,hbyte v){
	bytes[0] = v;
	return HBYTE_BYTES;
}

hint16 type_int16_from_bytes(hbyte * bytes){
	huint16 v =0,cv;
	hint32 c = HINT16_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c){
		cv = *p;
		v = v | (cv << 8* (HINT16_BYTES - c));
		p ++;
		c --;
	}
	return (hint16)v;
}

hint32 type_int32_from_bytes(hbyte * bytes){
	huint32 v =0,cv;
	hint32 c = HINT32_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c){
		cv = *p;
		v = v | (cv << 8* (HINT32_BYTES - c));
		p ++;
		c --;
	}
	return (hint32)v;
}

hint64 type_int64_from_bytes(hbyte * bytes){
	huint64 v =0,cv;
	hint32 c = HINT64_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c){
		cv = *p;
		v = v | (cv << 8* (HINT64_BYTES - c));
		p ++;
		c --;
	}
	return (hint64)v;
}

hfloat type_float_from_bytes(hbyte * bytes){
	huint32 v =0,cv;
	hint32 c = HINT32_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c){
		cv = *p;
		v = v | (cv << 8* (HINT32_BYTES - c));
		p ++;
		c --;
	}
	return *(hfloat *)&v;
}

hdouble type_double_from_bytes(hbyte * bytes){
	hdouble v = 0;
	memcpy(&v,bytes,HDOUBLE_BYTES);
	return v;
	/*
	huint64 v =0,cv;
	hint32 c = HINT64_BYTES;
	hubyte *p = (hubyte *)bytes;
	while(c){
		cv = *p;
		v = v | (cv >> (HINT64_BYTES - c));
		p ++;
		c --;
	}
	return *(hdouble *)&v;
	 */
}

hbool type_bool_from_bytes(hbyte * bytes){
	return bytes[0];
}

hbyte type_byte_from_bytes(hbyte * bytes){
	return bytes[0];
}

hint32 hatoi(hcchar * str){
	hchar * p = (hchar *)str;
	while(p && *p !='\0' && (*p ==' ' || *p =='\t')){
		p++;
	}
	return p?atoi(p):0;
}

