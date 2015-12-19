//
//  hvmcompiler_build.c
//  vmcompiler
//
//  Created by Zhang Hailong on 13-3-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvmcompiler_build.h"
#include "hlog.h"
#include "hbuffer.h"
#include "hlist.h"
#include "hvmcompiler_exec.h"
#include "hvmcompiler_bin.h"
#include "hstr.h"
#include "hvmcompiler_debug.h"
#include "md5.h"


hbool vmBuild(hcchar * project ,hobj_array_t * errors, InvokeTickDeclare){
    hbuffer_t file = buffer_alloc(1024, 1024);
    hbuffer_t source = buffer_alloc(1024, 1024);
    FILE * inFile;
    vmBinary * binary = (vmBinary *)hobj_new(HOBJ_CLASS(vmBinary), InvokeTickArg);
    vmCompilerClassMeta * classMeta;
    hbool rs = hbool_true;
    hint32 len;
    hchar buf[1024];
    md5_state_t md5;
    
    hinifile_t inifile = inifile_alloc(project);
    
    while(inifile_read(inifile)){
        
        if(strcmp(inifile_section(inifile), "SOURCES") == 0){
            buffer_clear(file);
            buffer_append_str(file, project);
            buffer_path_last_remove(file);
            buffer_append_str(file, inifile_value(inifile));
            
            inFile = fopen(buffer_to_str(file), "r");
            
            if(inFile){
                
                md5_init(&md5);
                
                buffer_clear(source);
                
                while(!feof(inFile) && (len = fread(buf, 1, sizeof(buf), inFile)) >0){
                    buffer_append(source, buf, len);
                    md5_append(&md5, (hubyte *)buf, len);
                }

                printf("className: %s\n",inifile_key(inifile));
                
                hobj_autorelease_pool_begin();
                
                classMeta = vmCompilerClassExec(buffer_to_str(source),errors,InvokeTickArg);
                
                if(classMeta){
                    
                    md5_finish(&md5, classMeta->binary.UDID);
                    
                    vmBinaryAddClass(binary, classMeta, inifile_key(inifile), InvokeTickArg);
                    
//                    
//                    
//                    {
//                        hint32 i,c,l;
//                        vmCompilerMetaOperator * op;
//                        hchar * p;
//                        
//                        printf("\tpropertys:\n");
//                        
//                        c = hobj_array_count(classMeta->propertys, InvokeTickArg);
//                        for(i=0;i<c;i++){
//                            op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->propertys, i, InvokeTickArg);
//                            printf("\t\t");
//                            p = op->uniqueKey.location.p;
//                            l = op->uniqueKey.length;
//                            while(l >0){
//                                printf("%c",*p);
//                                p ++;
//                                l --;
//                            }
//                            printf("(0x%x)\n",op->binary.uniqueKey);
//                        }
//                        
//                        printf("\tfunctions:\n");
//                        
//                        c = hobj_array_count(classMeta->functions, InvokeTickArg);
//                        for(i=0;i<c;i++){
//                            op = (vmCompilerMetaOperator *) hobj_array_objectAt(classMeta->functions, i, InvokeTickArg);
//                            printf("\t\t");
//                            p = op->uniqueKey.location.p;
//                            l = op->uniqueKey.length;
//                            while(l >0){
//                                printf("%c",*p);
//                                p ++;
//                                l --;
//                            }
//                            printf("(0x%x)\n",op->binary.uniqueKey);
//                        }
//                    }
                }
                else{
                    rs = hbool_false;
                    break;
                }
                
                
                hobj_autorelease_pool_end();
                
                fclose(inFile);
            }
            else{
                printf("[ERROR] not found file %s\n",buffer_to_str(file));
                rs = hbool_false;
                break;
            }
        }
        else if(strcmp(inifile_section(inifile), "RESOURCES") == 0){
            
            buffer_clear(file);
            buffer_append_str(file, project);
            buffer_path_last_remove(file);
            buffer_append_str(file, inifile_value(inifile));
            
            inFile = fopen(buffer_to_str(file), "r");
            
            if(inFile){
                
                buffer_clear(source);
                
                while(!feof(inFile) && (len = fread(buf, 1, sizeof(buf), inFile)) >0){
                    buffer_append(source, buf, len);
                }
                
                printf("resource: %s\n",inifile_key(inifile));
                
                hobj_autorelease_pool_begin();
                
                vmBinaryAddStringResource(binary, inifile_key(inifile), buffer_to_str(source), InvokeTickArg);
                
                hobj_autorelease_pool_end();
                
                fclose(inFile);
            }
            else{
                printf("[ERROR] not found file %s\n",buffer_to_str(file));
                rs = hbool_false;
                break;
            }
        }
        else if(strcmp(inifile_section(inifile), "OUTPUT") == 0){
            
            buffer_clear(file);
            buffer_append_str(file, project);
            buffer_path_last_remove(file);
            buffer_append_str(file, inifile_value(inifile));
            
            if(strcmp(inifile_key(inifile), "BIN") == 0){
                
                vmBinaryWriteFile(binary,buffer_to_str(file),InvokeTickArg);
                
                printf("\n\nbinary length: %d \n",vmBinaryLength(binary,InvokeTickArg));
                
                buffer_clear(source);
                buffer_append_str(source, buffer_to_str(file));
                buffer_append_str(source, ".vmdb");
                
                vmBinaryDebugWriteFile(binary, buffer_to_str(file), buffer_to_str(source), InvokeTickArg);
                
            }
        }
        
    }

    
    buffer_dealloc(file);
    buffer_dealloc(source);
    
    return rs;
}
