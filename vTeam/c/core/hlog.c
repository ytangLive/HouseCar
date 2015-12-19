/*
 *  hlog.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-12.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hlog.h"
#include "hbuffer.h"
#include "hstr.h"

void hlog_none(const char *format,...){

}

static void _hlog_output(LOG * log){
    if(log){
        hlog("%s(%d)\n",log->file,log->line);
        _hlog_output(log->parent);
    }
}

void hlog_throw(LOG_ARG_DECL,hcchar * format,...){
    LOG_ARG_BEGIN
    InvokeTickBegin
    hbuffer_t buffer = buffer_alloc(1024, 1024);
    va_list va_arg;
    va_start(va_arg, format);
    buffer_append_format_va_list(buffer, format, va_arg);
    va_end(va_arg);
    
    hlog("%s\n",buffer_to_str(buffer));
    
    buffer_dealloc(buffer);
    
    _hlog_output(&__LOG__);
    
    hlog("\n");
}

void hlog_assert(LOG_ARG_DECL, hbool as,hcchar * format,...){
    LOG_ARG_BEGIN
    InvokeTickBegin
    if(!as){
       
        hbuffer_t buffer = buffer_alloc(1024, 1024);
        va_list va_arg;
        va_start(va_arg, format);
        buffer_append_format_va_list(buffer, format, va_arg);
        va_end(va_arg);
        
        hlog("%s\n",buffer_to_str(buffer));
        
        buffer_dealloc(buffer);
        
        _hlog_output(&__LOG__);
        
        hlog("\n");
        
        assert(0);
    }
}

hchar * _log_file_path = NULL;
FILE * _log_file = NULL;

void hlog_file_path_set(hcchar * path){
    InvokeTickBegin
    str_cpy(&_log_file_path, path);
    if(_log_file){
        fclose(_log_file);
        _log_file = NULL;
    }
}

void hlog_file(hcchar * format,...){
    InvokeTickBegin
    hbuffer_t buffer = buffer_alloc(1024, 1024);
    va_list va_arg;
    va_start(va_arg, format);
    buffer_append_format_va_list(buffer, format, va_arg);
    va_end(va_arg);
    
    if(!_log_file){
        if(_log_file_path){
            _log_file = fopen(_log_file_path, "a");
        }
    }
    
    if(_log_file){
        fwrite(buffer_to_str(buffer), 1, buffer_size(buffer), _log_file);
    }
    
    buffer_dealloc(buffer);

}

void hlog_file_close(void){
    if(_log_file){
        fclose(_log_file);
        _log_file = NULL;
    }
}

