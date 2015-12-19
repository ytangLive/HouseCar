//
//  hserver.h
//  vMessageServer
//
//  Created by zhang hailong on 13-6-20.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vMessageServer_hserver_h
#define vMessageServer_hserver_h


#ifdef __cplusplus
extern "C" {
#endif
    
    struct _SRVProcess;
    struct _SRVServer;
    
    typedef void (* SRVServerLogCallback)(struct _SRVServer * srv,const char * format,va_list va);
    
    typedef struct _SRVServer {
        struct {
            struct {
                int argc;
                char ** args;
            } arg;
            struct {
                struct _SRVProcess * data;
                huint32 length;
            } process;
            struct {
                int port;
            } socket;
            hbool isBackgroundRun;
        } config;
        struct {
            pid_t mainpid;
            int port;
            int listenSocket;
            pthread_mutex_t listenMutex;
        } run;
        SRVServerLogCallback logCallback;
    } SRVServer;
    
    int SRVServerRun(SRVServer * server);
    
    int SRVServerAccept(SRVServer * server,double timeout,struct sockaddr * addr,socklen_t * socklen);
    
    struct _SRVProcess;
    
    typedef int (* SRVProcessCreate) (SRVServer * server,struct _SRVProcess * process);
    
    typedef void (* SRVProcessExit) (SRVServer * server,struct _SRVProcess * process);
    
    typedef void (* SRVProcessOpen) (SRVServer * server,struct _SRVProcess * process);
    
    typedef double (* SRVProcessTick) (SRVServer * server,struct _SRVProcess * process);
    
    typedef void (* SRVProcessClose) (SRVServer * server,struct _SRVProcess * process);
    
    typedef struct _SRVProcessClass{
        SRVProcessCreate create;
        SRVProcessExit exit;
        SRVProcessOpen open;
        SRVProcessTick tick;
        SRVProcessClose close;
    } SRVProcessClass;
    
    typedef struct _SRVProcess {
        const SRVProcessClass * clazz;
        pid_t pid;
        int exit;
    } SRVProcess;
    
    void SRVServerLog(const char * format,...);
        
#ifdef __cplusplus
}
#endif

#endif
