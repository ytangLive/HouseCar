//
//  hbytes.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-20.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hbytes.h"


hint16 bytes_swap_int16(hint16 x){
    return ( (x & 0x0ff) << 8 ) | ( (x >> 8) & 0x0ff );
}

huint16 bytes_swap_uint16(huint16 x){
    return ( (x & 0x0ff) << 8 ) | ( (x >> 8) & 0x0ff );
}

hint32 bytes_swap_int32(hint32 x){
    return ( (x & 0x0ff) << 24 ) | ( ((x >> 8) & 0x0ff) << 16 ) | ( ((x >> 16) & 0x0ff) << 8 ) | ( ((x >> 24) & 0x0ff)  );
}

huint32 bytes_swap_uint32(huint32 x){
    return ( (x & 0x0ff) << 24 ) | ( ((x >> 8) & 0x0ff) << 16 ) | ( ((x >> 16) & 0x0ff) << 8 ) | ( ((x >> 24) & 0x0ff)  );
}

hint64 bytes_swap_int64(hint64 x){
    return ( (x & 0x0ff) << 56 ) | ( ((x >> 8) & 0x0ff) << 48 ) | ( ((x >> 16) & 0x0ff) << 40 ) | ( ((x >> 24) & 0x0ff) << 32 ) 
        | ( ((x >> 32) & 0x0ff) << 24 )  | ( ((x >> 40) & 0x0ff) << 16 ) | ( ((x >> 48) & 0x0ff) << 8 ) | ( ((x >> 56) & 0x0ff) ) ;
}

huint64 bytes_swap_uint64(huint64 x){
    return ( (x & 0x0ff) << 56 ) | ( ((x >> 8) & 0x0ff) << 48 ) | ( ((x >> 16) & 0x0ff) << 40 ) | ( ((x >> 24) & 0x0ff) << 32 ) 
        | ( ((x >> 32) & 0x0ff) << 24 )  | ( ((x >> 40) & 0x0ff) << 16 ) | ( ((x >> 48) & 0x0ff) << 8 ) | ( ((x >> 56) & 0x0ff) ) ;
}

