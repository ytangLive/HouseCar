//
//  hvermin_scanner.h
//  C Library
//
//  Created by Hailong Zhang on 11-10-24.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#ifndef _HVERMIN_SCANNER_H
#define _HVERMIN_SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif
    
    #include "hvermin_runtime.h"
    
    struct _vmScannerContext;
    struct _vmScannerFilter;
    struct _vmScannerResult;
    
    typedef struct _vmScannerResult (* vmScannerCallback)(struct _vmScannerContext * context, struct _vmScannerFilter * filter,hchar c,InvokeTickDeclare);
    
    typedef struct _vmScannerResult (* vmScannerReset)(struct _vmScannerContext * context,struct _vmScannerFilter * filter,InvokeTickDeclare);
    
    typedef enum{
        vmScannerFilterStateNone,vmScannerFilterStateMatch,vmScannerFilterStateError
    }vmScannerFilterState;
    
    typedef struct _vmScannerFilter{
        hint32 size;
        vmScannerFilterState state;
        hint32 line;
        hint32 index;
        hchar error[32];
        vmScannerCallback callback;
        vmScannerReset reset;
    }vmScannerFilter;
    
    typedef struct _vmScannerContext{
        hint32 size;
        hany extra;
        hint32 line;
        hint32 index;
        hbool isContinue;
        hint32 step;
    }vmScannerContext;
    
    typedef enum{
        vmScannerResultStateOK,vmScannerResultStateError
    }vmScannerResultState;
    
    typedef struct _vmScannerResult{
        vmScannerResultState state;
        struct{
            hint32 line;
            hint32 index;
            hchar error[32];
        }error;
    }vmScannerResult;
    
    typedef enum{
         vmScannerSourceTypeString,vmScannerSourceTypeBuffer,vmScannerSourceTypeFile   
    }vmScannerSourceType;
    
    typedef struct{
        vmScannerSourceType type;
        union{
            hcchar * string;
            FILE * file;
            struct{
                hchar *data;
                hint32 length;
            }buffer;
        }source;
    }vmScannerSource;
    

    vmScannerResult vmScanner(vmScannerContext * context,vmScannerSource source,InvokeTickDeclare);
    
#define vmScanner(a,b) vmScanner((a),(b),InvokeTickArg)
    
#ifdef __cplusplus
}
#endif


#endif
