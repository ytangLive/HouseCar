//
//  hserver.c
//  vMessageServer
//
//  Created by zhang hailong on 13-6-20.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hserver.h"

static SRVServer * gServer = NULL;
static void SVRServerRunProcess(SRVProcess * process);
static void SVRServerSIGProcessQuit(int signo);
static void SVRServerSIGNAN(int signo);
static int processExit;

int SRVServerRun(SRVServer * server){
    
    int i;

    gServer = server;
    
    for(i=1;i<server->config.arg.argc;i++){
        if(strcmp(server->config.arg.args[i], "-p") ==0 && i + 1 <server->config.arg.argc){
            i ++;
            if(server->config.socket.port == 0){
                server->config.socket.port = atoi(server->config.arg.args[i]);
            }
        }
        else if(strcmp(server->config.arg.args[i], "-b") ==0){
            server->config.isBackgroundRun = hbool_true;
        }
    }
    
    if(server->config.isBackgroundRun){
        {
            pid_t pid;
            if(( pid = fork() ) < 0 )
            {
                fprintf(stderr, "fork error\n");
                exit(EXIT_FAILURE);
            }
            else if(pid == 0)
            {
                
            }
            else{
                exit(EXIT_SUCCESS);
            }
        }
    }
    
    server->run.mainpid = getpid();
    
    SRVServerLog("SRVServerRun(%d) begin\n",server->run.mainpid);
    
    
    server->run.listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    pthread_mutex_init(&server->run.listenMutex, NULL);
    
    {
        int res;
        struct sockaddr_in addr ;
        socklen_t socklen = sizeof(struct sockaddr_in);
        hint32 fl;
        int fn = 1;
        
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server->config.socket.port);
        addr.sin_addr.s_addr = INADDR_ANY;
        
        res = bind(server->run.listenSocket, (struct sockaddr *) & addr, sizeof(struct sockaddr_in));
        
        if(res != 0){
            fprintf(stderr,"socket bind error\n");
            close(server->run.listenSocket);
            exit(EXIT_FAILURE);
        }
        
        getsockname(server->run.listenSocket, (struct sockaddr *) & addr, &socklen);

        server->run.port = ntohs(addr.sin_port);
        
        SRVServerLog("bind %d\n",server->run.port);
        
        res = listen(server->run.listenSocket, SOMAXCONN);
        
        if(res != 0){
            fprintf(stderr,"socket listen error\n");
            close(server->run.listenSocket);
            exit(EXIT_FAILURE);
        }
        
        fl =  fcntl(server->run.listenSocket, F_GETFL) ;
        fcntl(server->run.listenSocket, F_SETFL, fl | O_NONBLOCK);
        
        setsockopt(server->run.listenSocket, SOL_SOCKET, SO_RCVLOWAT, (void *)&fn, sizeof(fn));
        setsockopt(server->run.listenSocket, SOL_SOCKET, SO_SNDLOWAT, (void *)&fn, sizeof(fn));
        
    }
    
    signal(SIGCHLD, SVRServerSIGProcessQuit);
    signal(SIGINT, SVRServerSIGProcessQuit);
    signal(SIGTERM, SVRServerSIGProcessQuit);
    signal(SIGKILL, SVRServerSIGProcessQuit);
    signal(SIGABRT, SVRServerSIGProcessQuit);
    signal(SIGPIPE, SVRServerSIGNAN);
    signal(SIGTTOU, SVRServerSIGNAN);
    signal(ETIMEDOUT, SVRServerSIGNAN);
    
    {
        huint32 c = server->config.process.length;
        SRVProcess * p = server->config.process.data;
        
        while(c > 0 && p->clazz){
            
            SVRServerRunProcess(p);
            
            p++;
            c --;
        }
    }
    
    {
        char command[128] = "";
        fd_set rds;
        int res;
        struct timeval timeo = {30,0};
        
        while(strcmp(command, "exit")){
            
            memset(command, 0, sizeof(command));
            
            if(server->config.isBackgroundRun){
                usleep(300000);
            }
            
            FD_ZERO(&rds);
            
            FD_SET(STDIN_FILENO, &rds);
            
            res = select(STDIN_FILENO + 1, &rds, NULL, NULL, &timeo);
            
            if(res == 0){
                
            }
            else if(res == -1){
                if(errno == EINTR){
                    
                }
                else{
                    break;
                }
            }
            else{
                if(FD_ISSET(server->run.listenSocket, &rds)){
                    
                    scanf("%s",command);
                    
                }
            }
            

        }
    }

    signal(SIGCHLD, SIG_IGN);
    
    {
        huint32 c = server->config.process.length;
        SRVProcess * p = server->config.process.data;
        int     stat;
        
        while(c > 0 && p->clazz){
            
            if(p->pid){
                kill(p->pid, SIGKILL);
                waitpid(p->pid, &stat, WNOHANG);
                (* p->clazz->exit)(server,p);
                p->pid = 0;
            }
            
            p++;
            c --;
        }
    }
    
    
    close(server->run.listenSocket);
    pthread_mutex_destroy(&server->run.listenMutex);
    
    SRVServerLog("main end\n");

    gServer = NULL;
    
    return 0;
}


static void SVRServerSIGNAN(int signo){
    SRVServerLog("sig_nan %d\n",signo);
}

static void SVRServerSIGProcessQuit(int signo)
{
    int c = 0;
    int     stat;
    SRVProcess *p;
    if(signo == SIGCHLD){
        if(getpid() == gServer->run.mainpid){
            c = gServer->config.process.length;
            p = gServer->config.process.data;
            while(c > 0 && p->clazz){
                if(p->pid){
                    if(p->pid == waitpid(p->pid, &stat, WNOHANG)){
                        (* p->clazz->exit)(gServer,p);
                        p->pid = 0;
                        SVRServerRunProcess(p);
                    }
                }
                else{
                    SVRServerRunProcess(p);
                }
                
                p++;
                c --;
            }
        }
    }
    else{
        if(getpid() == gServer->run.mainpid){
            c = gServer->config.process.length;
            p = gServer->config.process.data;
            while(c > 0 && p->clazz){
                
                if(p->pid){
                    kill(p->pid, SIGKILL);
                    waitpid(p->pid, &stat, WNOHANG);
                    (* p->clazz->exit)(gServer,p);
                    p->pid = 0;
                }
                
                p++;
                c --;
            }
            exit(EXIT_FAILURE);
        }
        else {
            SRVServerLog("process(%d) exit\n",getpid());
            processExit = 1;
        }
    }
}

static void SVRServerSIGChildProcessQuit(int signo)
{
    int     stat;
    waitpid(-1, &stat, WNOHANG);
}


static void SVRServerRunProcess(SRVProcess * process){
    
    pid_t pid;
    double t;
    
    if((* process->clazz->create)(gServer,process)){
        
        if(( pid = fork() ) < 0 )
        {
            fprintf(stderr, "fork error\n");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            signal(SIGCHLD, SVRServerSIGChildProcessQuit);
            signal(SIGKILL, SIG_DFL);
            signal(SIGABRT, SIG_DFL);
            signal(SIGPIPE, SVRServerSIGNAN);
            signal(SIGTTOU, SVRServerSIGNAN);
            signal(ETIMEDOUT, SVRServerSIGNAN);
            
            process->pid  = getpid();
            
            (* process->clazz->open)(gServer,process);
            
            while(!processExit && !process->exit){
                
                t = (* process->clazz->tick)(gServer,process);
                
                if(t){
                    usleep(t * 1000000);
                }
            }
            
            (* process->clazz->close)(gServer,process);
            
            exit(EXIT_SUCCESS);
        }
        
        process->pid = pid;
        
    }
    
}

int SRVServerAccept(SRVServer * server,double timeout,struct sockaddr * addr,socklen_t * socklen){
    
    int client = 0;
    fd_set rds;
    int res;
    int fl;
    int fn = 1;
    
    struct timeval timeo = {(int)timeout, (timeout - (int) timeout) * 1000000};
    
    pthread_mutex_lock(&server->run.listenMutex);
    
    FD_ZERO(&rds);
    
    FD_SET(server->run.listenSocket, &rds);
    
    res = select(server->run.listenSocket + 1, &rds, NULL, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        if(errno == EINTR){
            
        }
        else{
            client = -1;
        }
    }
    else{
        if(FD_ISSET(server->run.listenSocket, &rds)){
            
            client = accept(server->run.listenSocket, addr,socklen);
            
            if(client != -1){
                fl =  fcntl(client, F_GETFL) ;
                fcntl(client, F_SETFL, fl | O_NONBLOCK);
                setsockopt(server->run.listenSocket, SOL_SOCKET, SO_RCVLOWAT, (void *)&fn, sizeof(fn));
                setsockopt(server->run.listenSocket, SOL_SOCKET, SO_SNDLOWAT, (void *)&fn, sizeof(fn));
            }
        }
    }
    
    pthread_mutex_unlock(&server->run.listenMutex);
    
    return client;
}

void SRVServerLog(const char * format,...){
    
    va_list va;
    
    va_start(va, format);
    
    if(gServer && gServer->logCallback){
        
        (*gServer->logCallback)(gServer,format,va);
       
        va_end(va);
        
        return;
    }
    
#ifdef DEBUG
  
    vprintf(format, va);
    
#else
    
    {
        fd_set rds;
        int res;
        
        struct timeval timeo = {0, 0.1 * 1000000};
        
        FD_ZERO(&rds);
        
        FD_SET(STDOUT_FILENO, &rds);
        
        res = select(STDOUT_FILENO + 1, &rds, NULL, NULL, &timeo);
        
        if(res == 0){
            
        }
        else if(res == -1){
            if(errno == EINTR){
                
            }
            else{
                
            }
        }
        else{
            if(FD_ISSET(STDOUT_FILENO, &rds)){
                vprintf(format, va);
            }
        }
        
    }

    
#endif
    
    
    va_end(va);
    
}
