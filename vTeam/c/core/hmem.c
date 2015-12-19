/*
 *  hmem.c
 *  C Library
 *
 *  Created by hailong zhang on 10-8-28.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef ZOMBIES
#define ZOMBIES
#endif

#include "hconfig.h"
#include "hlog.h"
#include "hmem.h"
#include "hmutex.h"

#define MEM_TOKEN_SIZE  8

#undef mem_malloc

#undef mem_realloc

#undef mem_free

#undef mem_memcpy

#undef mem_memset

#undef mem_memmove

#undef mem_memcmp

#undef mem_strcpy

#undef mem_strcat

#undef mem_strcmp

#undef mem_print

#undef mem_init

#define MEM_INVOEK_TICK_SIZE    30


typedef struct{
    hint32 size;
    hbool del;
    InvokeTick ticks[MEM_INVOEK_TICK_SIZE];
}MemTick;

static MemTick ** memTicks = NULL;
static hint32 memTicksLength = 0;
static hint32 memTicksSize = 0;
static hmutex_t memTicksLocker = NULL;

#define MEM_TICKS_EXTEND_SIZE   1024

InvokeTick _InvokeTickMake(InvokeTick * parent,hcchar * file,hint32 line){
    InvokeTick tick = {parent,file,line};
    return tick;
}

static void _InvokeTickPrintLevel(InvokeTick * invokeTick,hint32 level){
    hint32 i=0;
    
    if(invokeTick){
        for(i=0;i<level;i++){
            hlog("\t");
        }
        
        hlog("%s(%d)\n",invokeTick->file,invokeTick->line);
        
        _InvokeTickPrintLevel(invokeTick->parent, level +1);
    }
}

void _InvokeTickPrint(InvokeTick invokeTick){
    _InvokeTickPrintLevel(&invokeTick,0);
}

static void _InvokeTickFillToMemTick(InvokeTick * tick , MemTick * memTick,hint32 index){
    if(index >= 0 && index <MEM_INVOEK_TICK_SIZE){
        if(tick == NULL){
            memset(memTick->ticks+ index,0,sizeof(InvokeTick));
        }
        else{
            memcpy(memTick->ticks+ index,tick,sizeof(InvokeTick));
            _InvokeTickFillToMemTick(tick->parent, memTick, index +1);
        }
    }
}

static void _InvoekTickPrintForMemTick(MemTick * memTick,hint32 index){
    if(index >= 0 && index <MEM_INVOEK_TICK_SIZE){
        InvokeTick * tick = memTick->ticks+index;
        hint32 i=0;
        if(tick->file && tick->line){
            for(i=0;i<index;i++){
                hlog("\t");
            }
            hlog("%s(%d)\n",tick->file,tick->line);
            _InvoekTickPrintForMemTick(memTick,index +1);
        }
    }
}

void _InvoekTickPrintForMem(hany m){
    if(m){
        MemTick * tick = (MemTick *)((hbyte *)m - sizeof(MemTick));
        _InvoekTickPrintForMemTick(tick,0);
    }
}

void mem_init(void){
    if(memTicks == NULL){
        memTicks = malloc(sizeof(MemTick *) * MEM_TICKS_EXTEND_SIZE);
        memTicksLength =0;
        memTicksSize = MEM_TICKS_EXTEND_SIZE;
        memTicksLocker = hmutex_alloc();
    }
}

hany mem_malloc(hint32 size,InvokeTickDeclare){
    hubyte * p = (hubyte *)malloc(size + sizeof(MemTick) + MEM_TOKEN_SIZE + MEM_TOKEN_SIZE);
    hint32 len = MEM_TOKEN_SIZE;
    MemTick * tick;
    while(len >0){
        *p++ = 0xcd;
        len --;
    }
    tick = (MemTick *) p;
    memset(tick,0,sizeof(MemTick));
    tick->size = size;
    _InvokeTickFillToMemTick(&__InvokeTick__,tick,0);
    tick->del = hbool_false;
    p += sizeof(MemTick) + size;
    len = MEM_TOKEN_SIZE;
    while(len >0){
        *p ++ = 0xcd;
        len --;
    }
    
    hmutex_lock(memTicksLocker);
    if(memTicks == NULL){
        memTicks = malloc(sizeof(MemTick *) * MEM_TICKS_EXTEND_SIZE);
        memTicksLength =0;
        memTicksSize = MEM_TICKS_EXTEND_SIZE;
    }
    
    if(memTicksLength +1 > memTicksSize){
        memTicksSize = memTicksSize + MEM_TICKS_EXTEND_SIZE;
        memTicks = realloc(memTicks, sizeof(MemTick *) * memTicksSize );
    }
    
    memTicks[memTicksLength ++] = tick;
    
    hmutex_unlock(memTicksLocker);
    
    return (hbyte *)tick + sizeof(MemTick);
}

hany mem_realloc(hany m,hint32 size,InvokeTickDeclare){
    MemTick * tick = (MemTick *)((hbyte *)m - sizeof(MemTick));
    hany r = NULL;
    mem_assert(m, 0, InvokeTickArg);
    r = mem_malloc(size, InvokeTickArg);
    mem_memcpy(r, m, size < tick->size ? size:tick->size, InvokeTickArg);
    mem_free(m, InvokeTickArg);
    return r;
}

void mem_assert(hany m,hint32 minSize,InvokeTickDeclare){

    MemTick * tick = (MemTick *)((hbyte *)m - sizeof(MemTick));
    hubyte * p = (hubyte *) tick - MEM_TOKEN_SIZE;
    hint32 len = MEM_TOKEN_SIZE;

    
    if(m == 0 || (tick->size <=0 && minSize ==0) || (minSize>0 && tick->size < minSize) || tick->del){
        hlog("Creator size:%d\n",tick->size);
        _InvoekTickPrintForMemTick(tick,0);
        hlog("Self \n%s(%d)\n",InvokeTickFile,InvokeTickLine);
        InvokeTickPrint();
        assert(0);
    }
    
    while(len >0){
        if(*p ++ != 0xcd){
            hlog("Creator size:%d\n",tick->size);
            _InvoekTickPrintForMemTick(tick,0);
            hlog("Self \n%s(%d)\n",InvokeTickFile,InvokeTickLine);
            InvokeTickPrint();
            assert(0);
        }
        len --;
    }
    p += sizeof(MemTick) + tick->size;
    len = MEM_TOKEN_SIZE;
    while(len >0){
        if(*p++ != 0xcd){
            hlog("Creator size:%d\n",tick->size);
            _InvoekTickPrintForMemTick(tick,0);
            hlog("Self \n%s(%d)\n",InvokeTickFile,InvokeTickLine);
            InvokeTickPrint();
            assert(0);
        }
        len --;
    }
}

void mem_free(hany m,InvokeTickDeclare){
    MemTick * tick = (MemTick *)((hbyte *)m - sizeof(MemTick));
    mem_assert(m, 0,InvokeTickArg);
    tick->del = hbool_true;
}


void mem_print(InvokeTickDeclare){
    hint32 totalSize = 0,overflowSize = 0,overflowTicks = 0,printOverflows = 0;
    hint32 i;
    MemTick * tick;
    hmutex_lock(memTicksLocker);
    hlog("MemTick Total Count: %d\n",memTicksLength);
    hlog("Overflow:\n");
    for(i=0;i<memTicksLength;i++){
        tick = memTicks[i];
        if(!tick->del){
            if(printOverflows <50){
                _InvoekTickPrintForMemTick(tick,0);
            }
            else if(printOverflows == 50){
                hlog("...\n");
            }
            printOverflows ++;
            overflowSize += tick->size;
            overflowTicks ++;
        }
        totalSize += tick->size;
    }
    hlog("Overflow Tick Count: %d\n",overflowTicks);
    hlog("Total Bytes: %d\n",totalSize);
    hlog("Overflow Bytes: %d\n",overflowSize);
    hmutex_unlock(memTicksLocker);
}


void mem_memcpy(hany dest,hany src,hint32 len,InvokeTickDeclare){
    //mem_assert(dest,len, InvokeTickArg);
    //mem_assert(src, len,InvokeTickArg);
    memcpy(dest, src, len);
}

void mem_memset(hany dest,hint32 val,hint32 len,InvokeTickDeclare){
    //mem_assert(dest, len,InvokeTickArg);
    memset(dest, val, len);
}

void mem_memmove(hany dest,hany src,hint32 len,InvokeTickDeclare){
    //mem_assert(dest,len, InvokeTickArg);
    //mem_assert(src, len,InvokeTickArg);
    memmove(dest, src, len);
}

hint32 mem_memcmp(hany dest,hany src,hint32 len,InvokeTickDeclare){
    //mem_assert(dest,len, InvokeTickArg);
    //mem_assert(src, len,InvokeTickArg);
    return memcmp(dest, src, len);
}

void mem_strcpy(hchar * dest,hcchar * src,InvokeTickDeclare){
    //hint32 len = strlen(src) + 1;
    //mem_assert(dest, len,InvokeTickArg);
    strcpy(dest, src);
}

void mem_strcat(hchar * dest,hcchar * src,InvokeTickDeclare){
    //hint32 len1 = strlen(dest) ;
    //hint32 len2 = strlen(src) ;
    //mem_assert(dest,len1 + len2 +1 ,InvokeTickArg);
    strcat(dest, src);
}

hint32 mem_strcmp(hcchar * dest,hcchar * src,InvokeTickDeclare){
    //hint32 len1 = strlen(dest) ;
    //hint32 len2 = strlen(src) ;
    //mem_assert(dest,len1 + len2 +1 ,InvokeTickArg);
    return strcmp(dest, src);
}

