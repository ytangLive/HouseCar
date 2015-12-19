//
//  hvermin_scanner.c
//  C Library
//
//  Created by Hailong Zhang on 11-10-24.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_scanner.h"


#undef vmScanner


static vmScannerResult vmScannerBytes(vmScannerContext * context,hchar * bytes,hint32 length,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    
    hchar * p = bytes;
    hint32 len = length;
    vmScannerFilter * filter;
    hint32 size ;
    hbool match ;
    
    while(len >0){
        
        size = context->size - sizeof(vmScannerContext);
        filter = (vmScannerFilter *) ((hbyte *)context + sizeof(vmScannerContext));
        
        match = hbool_false;

        

        while(size >= sizeof(vmScannerFilter)){
            
            context->isContinue = hbool_true;
            context->step = 0;
            
            if(filter->state == vmScannerFilterStateNone && filter->callback){
                
                result = (*filter->callback)(context,filter,*p,InvokeTickArg);
                
                if(result.state == vmScannerResultStateError){
                    return result;
                }
                
                if(filter->state == vmScannerFilterStateMatch){
                    match = hbool_true;
                    break;
                }
            }
            if(!context->isContinue){
                break;
            }
            else{
                context->step = 0;
            }
            size -= filter->size;
            filter = (vmScannerFilter *)((hbyte *) filter + filter->size);
        }

        
        if(match){
            size = context->size - sizeof(vmScannerContext);
            filter = (vmScannerFilter *) ((hbyte *)context + sizeof(vmScannerContext));
            while(size >= sizeof(vmScannerFilter)){

                if(filter->reset){
                    result = (*filter->reset)(context,filter,InvokeTickArg);
      
                    if(result.state == vmScannerResultStateError){
                        return result;
                    }
                }
                
                filter->state = vmScannerFilterStateNone;
                
                size -= filter->size;
                filter = (vmScannerFilter *)((hbyte *) filter + filter->size);
                
                
            }
        }
      
        if(context->step != 0){
            p = p + context->step;
            len = len - context->step;
            context->step = 0;
        }
        
        if( *p == '\n'){
            context->line ++;
            context->index =0;
        }
        else {
            context->index ++;
        }
        
        p++;
        len --;
    }
    
    return result;
}

static vmScannerResult vmScannerFile(vmScannerContext * context,FILE * file,InvokeTickDeclare){
    hchar buffer[2048];
    hint32 length ;
    vmScannerResult result = {vmScannerResultStateOK,0};
    
    while(!feof(file)){
        length = (hint32)fread(buffer, 1, sizeof(buffer), file);
        result = vmScannerBytes(context,buffer,length,InvokeTickArg);
        if(result.state == vmScannerResultStateError){
            return result;
        }
    }
    
    return result;
}

vmScannerResult vmScanner(vmScannerContext * context,vmScannerSource source,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};

    
    if(source.type == vmScannerSourceTypeString){
        result =  vmScannerBytes(context,(hchar *)source.source.string,(hint32)strlen(source.source.string),InvokeTickArg);
    }
    else if(source.type == vmScannerSourceTypeFile){
        result =  vmScannerFile(context,source.source.file,InvokeTickArg);
    }
    else if(source.type == vmScannerSourceTypeBuffer){
        result = vmScannerBytes(context, source.source.buffer.data,source.source.buffer.length,InvokeTickArg);
    }

    return result;
}

