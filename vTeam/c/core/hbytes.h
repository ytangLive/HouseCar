//
//  hbytes.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-20.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HBYTES_H
#define HBYTES_H

#ifdef __cplusplus
extern "C" {
#endif
    
    hint16 bytes_swap_int16(hint16 x);
    
    huint16 bytes_swap_uint16(huint16 x);
    
    hint32 bytes_swap_int32(hint32 x);
    
    huint32 bytes_swap_uint32(huint32 x);
    
    hint64 bytes_swap_int64(hint64 x);
    
    huint64 bytes_swap_uint64(huint64 x);
    
#ifdef BITS_LOW
    
#define htoles(x) (x)
#define letohs(x) (x)
#define htolel(x) (x)
#define letohl(x) (x)
#define htoleq(x) (x)
#define letohq(x) (x)
    
#define htobes(x) bytes_swap_int16(x)
#define betohs(x) bytes_swap_int16(x)
#define htobel(x) bytes_swap_int32(x)
#define betohl(x) bytes_swap_int32(x)
#define htobeq(x) bytes_swap_int64(x)
#define betohq(x) bytes_swap_int64(x)
    
#else
    
#define htoles(x) bytes_swap_int16(x)
#define letohs(x) bytes_swap_int16(x)
#define htolel(x) bytes_swap_int32(x)
#define letohl(x) bytes_swap_int32(x)
#define htoleq(x) bytes_swap_int64(x)
#define letohq(x) bytes_swap_int64(x)
    
#define htobes(x) (x)
#define betohs(x) (x)
#define htobel(x) (x)
#define betohl(x) (x)
#define htobeq(x) (x)
#define betohq(x) (x)   
    
#endif
    
 
#ifdef __cplusplus
}
#endif

#endif
