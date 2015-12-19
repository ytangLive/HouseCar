/*
 *  hmem.h
 *  C Library
 *
 *  Created by hailong zhang on 10-8-28.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#ifndef HMEM_H
#define HMEM_H


#ifdef __cplusplus
extern "C" {
#endif

#define MEM_ARG __FILE__,__LINE__
    
    typedef struct _InvokeTick{
        struct _InvokeTick * parent;
        hcchar * file;
        hint32 line;
    }InvokeTick;
    
    InvokeTick _InvokeTickMake(InvokeTick * parent,hcchar * file,hint32 line);
    
    void _InvokeTickPrint(InvokeTick invokeTick);
    
    void _InvoekTickPrintForMem(hany m);
    
#ifdef ZOMBIES
    
#define InvokeTickBegin     InvokeTick __InvokeTick__ = _InvokeTickMake(NULL,__FILE__,__LINE__);
#define InvokeTickArg       _InvokeTickMake(&__InvokeTick__,__FILE__,__LINE__)
#define InvokeTickRoot      _InvokeTickMake(NULL,__FILE__,__LINE__)
#define InvokeTickDeclare   InvokeTick __InvokeTick__
#define InvokeTickPrint()     _InvokeTickPrint(InvokeTickArg)
#define InvokeTickFile      (__InvokeTick__.file)
#define InvokeTickLine      (__InvokeTick__.line)
    
#else
  
#define InvokeTickBegin     long __InvokeTick__ = 0;
#define InvokeTickArg       __InvokeTick__
#define InvokeTickRoot      0
#define InvokeTickDeclare   long __InvokeTick__
#define InvokeTickPrint()   
#define InvokeTickFile      (NULL)
#define InvokeTickLine      (0)
    
#endif

    void mem_init(void);
    
    hany mem_malloc(hint32 size,InvokeTickDeclare);
    
    void mem_assert(hany m,hint32 minSize,InvokeTickDeclare);
    
    hany mem_realloc(hany m,hint32 size,InvokeTickDeclare);
        
    void mem_free(hany m,InvokeTickDeclare);
    
    void mem_memcpy(hany dest,hany src,hint32 len,InvokeTickDeclare);
    
    void mem_memset(hany dest,hint32 val,hint32 len,InvokeTickDeclare);
    
    void mem_memmove(hany dest,hany src,hint32 len,InvokeTickDeclare);
    
    hint32 mem_memcmp(hany dest,hany src, hint32 len,InvokeTickDeclare);
    
    void mem_strcpy(hchar * dest,hcchar * src,InvokeTickDeclare);
    
    void mem_strcat(hchar * dest,hcchar * src,InvokeTickDeclare);
    
    hint32 mem_strcmp(hcchar * str1,hcchar * str2,InvokeTickDeclare);
    
    void mem_print(InvokeTickDeclare);
    
#ifdef ZOMBIES
    
#define mem_malloc(s) mem_malloc((s),InvokeTickArg)
    
#define mem_realloc(m,s) mem_realloc((m),(s),InvokeTickArg)
    
#define mem_free(m) mem_free((m),InvokeTickArg)
    
#define mem_memcpy(d, s, l) mem_memcpy((d),(s),(l),InvokeTickArg)

#define mem_memset(d, v, l) mem_memset((d),(v),(l),InvokeTickArg)
    
#define mem_memmove(d, s, l) mem_memmove((d),(v),(l),InvokeTickArg)
    
#define mem_memcmp(d,s,l) mem_memcmp((d),(s),(l),InvokeTickArg)
    
#define mem_strcpy(d, s) mem_strcpy((d),(s),InvokeTickArg)
    
#define mem_strcat(d, s) mem_strcat((d),(s),InvokeTickArg)
    
#define mem_strcmp(d, s) mem_strcmp((d),(s),InvokeTickArg)
    
#define mem_print() mem_print(InvokeTickArg)
    
#else
 
#define mem_init
    
#define mem_malloc(s) malloc(s)
    
#define mem_realloc(m,s) realloc((m),(s))
    
#define mem_free(m) free(m)
    
#define mem_memcpy(d, s, l) memcpy((d),(s),(l))
    
#define mem_memset(d, v, l) memset((d),(v),(l))
    
#define mem_memmove(d, s, l) memmove((d),(s),(l))
    
#define mem_memcmp(d, s, l) memcmp((d),(s),(l))
    
#define mem_strcpy(d, s) strcpy((d),(s))
    
#define mem_strcat(d, s) strcat((d),(s))

#define mem_strcmp(d, s) strcmp((d),(s))
    
#define mem_print()
    
#endif
    
	
#ifdef __cplusplus
}
#endif
		
#endif

