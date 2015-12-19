/*
 *  hlog.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-12.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */


#ifndef HLOG_H
#define HLOG_H


#ifdef __cplusplus
extern "C" {
#endif
    
    
    
#define LOG_ARG_DECL    LOG * __LOG_ARG__,hcchar * __LOG_FILE__,hint32 __LOG_LINE__
#define LOG_ARG_BEGIN   LOG __LOG__ = {__LOG_ARG__,__LOG_FILE__,__LOG_LINE__};
#define LOG_ARG         &__LOG__,__FILE__,__LINE__
#define LOG_ARG_ROOT    NULL,__FILE__,__LINE__
#define LOG_THROW(format,...)       hlog_throw(&__LOG__,__FILE__,__LINE__,format,...)
#define LOG_ASSERT(as,format,...)   hlog_assert(&__LOG__,__FILE__,__LINE__,as,format,...)
    
    typedef struct _LOG{
        struct _LOG * parent;
        hcchar * file;
        hint32 line;
    }LOG;

	void hlog_throw(LOG_ARG_DECL,hcchar * format,...);
    
    void hlog_assert(LOG_ARG_DECL, hbool as,hcchar * format,...);

	void hlog_none(const char *format,...);
    
    void hlog_file_path_set(hcchar * path);
    
    void hlog_file(hcchar * format,...);
    
    void hlog_file_close(void);
    
	
#ifdef HLOG_CONSOLE
	
#define hlog printf

#endif
    
#ifdef HLOG_FILE

#define hlog hlog_file
    
#endif

#ifdef HLOG_ANDROID

#define hlog hlog_android

extern void hlog_android(const char * format,...);

#endif
	
#ifndef hlog
	
#define hlog hlog_none
	
#endif
	
#ifdef __cplusplus
}
#endif

#endif

