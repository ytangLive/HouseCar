//
//  hvermin_build.c
//  C Library
//
//  Created by Hailong Zhang on 11-11-1.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_build.h"
#include "hvermin_bin.h"
#include "hvermin_compile_scanner.h"
#include "hlist.h"
#include "hlog.h"
#include "hstr.h"

static void vmBuildClassNameForFile(hcchar * file,hbuffer_t className,InvokeTickDeclare){
    hchar * p = (hchar *)file;
    hchar * s, *e;
    hint32 len = (hint32)strlen(file);
    
    s = p + len;
    e =  NULL;
    while(s >= p){
        
        if(*s == '/' || *s == '\\'){
            break;
        }
        
        if(*s == '.' && !e){
            e = s;
        }
        s--;
    }
    
    buffer_append(className,s+1,e > s ? (hint32)(e -s -1) : (hint32)(p - s -1));
    
}

vmScannerResult vmBuild(hcchar * files,hcchar * forDir,hcchar *outFile,InvokeTickDeclare){
    hlist_t fileList =list_alloc(64, 64);
    hint32 i,c;
    hbuffer_t file = buffer_alloc(1024, 1024);
    hbuffer_t className = buffer_alloc(128, 128);
    FILE * inFile;
    vmBinary binary;
    vmScannerClassContext classContext;
    vmScannerSource source;
    vmScannerResult rs;
    vmBinaryInit(&binary);
    
    list_split_str(fileList, files, ";");
    
    c = list_count(fileList);
    
    for(i=0;i<c;i++){
        if(str_has_suffix((hcchar *)list_get(fileList, i), ".vm")){
            buffer_clear(file);
            if(forDir){
                buffer_append_str(file, forDir);
            }
            
            buffer_append_str(file, (hcchar *)list_get(fileList, i));
            
            hlog("build file %s\n",buffer_to_str(file));
            
            inFile = fopen(buffer_to_str(file), "r");
            
            if(inFile){
                source.type = vmScannerSourceTypeFile;
                source.source.file = inFile;
                
                vmScannerClassContextInit(&classContext);
                
                rs = vmScanner(&classContext.base,source);
                
                if(rs.state == vmScannerResultStateError){
                    
                    printf("[ERROR] %s(%d,%d): %s\n",buffer_to_str(file),rs.error.line,rs.error.index,rs.error.error);
                    vmScannerClassContextDestroy(&classContext);
                    break;
                }
                else{
                    buffer_clear(className);
                    vmBuildClassNameForFile(buffer_to_str(file),className,InvokeTickArg);
                    printf("className: %s\n",buffer_to_str(className));
                    vmBinaryAddClass(&binary, (vmCompileClassMeta * )classContext.base.extra, buffer_to_str(className));
                }
                
                vmScannerClassContextDestroy(&classContext);
                
                if(rs.state == vmScannerResultStateOK){
                    printf("build ok\n\n");
                }

				fclose(inFile);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = -1;
                rs.error.index = 0;
                mem_strcpy(rs.error.error,"not found file");
                printf("[ERROR] not found file %s\n",buffer_to_str(file));
                break;
            }
        }
    }
    
    if(rs.state == vmScannerResultStateOK){
        vmBinaryWriteFile(&binary, outFile);
    }
    
    printf("\n\nbinary length: %d \n",vmBinaryLength(&binary));
    
    vmBinaryDestroy(&binary);
    
    list_split_free(fileList);
    list_dealloc(fileList);

    buffer_dealloc(file);
    buffer_dealloc(className);
    
    return rs;
}

