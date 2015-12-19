//
//  hprocess.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-16.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hprocess.h"
#include "hlog.h"

#ifndef __WIN32__

typedef struct{
    pid_t pid;
    int pipe_in;
    int pipe_out;
}process_t;

hprocess_t process_alloc(hcchar * command){
    pid_t pid;
    
    int pipe_in[2],pipe_out[2];
    
    pipe(pipe_in);
    pipe(pipe_out);
    
    pid = fork();
    
    if(pid < 0){
        hlog("fork error");
        
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
    }
    else if(pid ==0 ){
        
        close(pipe_out[1]);
        dup2(pipe_out[0],STDIN_FILENO);
        close(pipe_in[0]);
        dup2(pipe_in[1],STDOUT_FILENO);
        execl(command,NULL);
        exit(EXIT_SUCCESS);
    }
    else{
        {
            process_t * process = (process_t *)mem_malloc(sizeof(process_t));
#ifdef O_NONBLOCK
            hint32 fl;
#endif
            mem_memset(process, 0, sizeof(process_t));
            
            close(pipe_in[1]);
            close(pipe_out[0]);
            
#ifdef O_NONBLOCK
            fl =  fcntl(pipe_in[0], F_GETFL) ;
            fcntl(pipe_in[0], F_SETFL, fl | O_NONBLOCK);
            fl =  fcntl(pipe_out[1], F_GETFL) ;
            fcntl(pipe_out[1], F_SETFL, fl | O_NONBLOCK);
#endif
            
            process->pipe_in = pipe_in[0];
            process->pipe_out = pipe_out[1];
            process->pid = pid;
            
            hlog("%s\n",command);
            
            return (hprocess_t)process;
        }
    }
    
    return NULL;
}

hprocess_t process_alloc_with_io(hcchar * command,int inno,int outno){
    
    pid_t pid;
    
    pid = fork();
    
    if(pid < 0){
        
        hlog("fork error");
        close(inno);
        if(inno != outno){
            close(outno);
        }
    }
    else if(pid ==0 ){
        dup2(outno,STDIN_FILENO);
        dup2(inno,STDOUT_FILENO);
        execl(command,NULL);
        exit(EXIT_SUCCESS);
    }
    else{
        {
            process_t * process = (process_t *)mem_malloc(sizeof(process_t));
#ifdef O_NONBLOCK
            hint32 fl;
#endif
            mem_memset(process, 0, sizeof(process_t));
            
#ifdef O_NONBLOCK
            fl =  fcntl(inno, F_GETFL) ;
            fcntl(inno, F_SETFL, fl | O_NONBLOCK);
            fl =  fcntl(outno, F_GETFL) ;
            fcntl(outno, F_SETFL, fl | O_NONBLOCK);
#endif
            
            process->pipe_in = inno;
            process->pipe_out = outno;
            process->pid = pid;
            
            hlog("%s\n",command);
            
            return (hprocess_t)process;
        }
    }
    
    return NULL;
}

static void sig_process_quit_handler(int signo)
{
    hlog("process exit\n");
}

void process_dealloc(hprocess_t proc){
    process_t * process = (process_t *)proc;
    fd_set ds;
    struct timeval timeo = {0, 0};
    hbyte buffer[1024];
    hint32 len,res;
   
    close(process->pipe_out);
    
    timeo.tv_usec = 100;

    while(hbool_true){
        FD_ZERO(&ds);
        FD_SET(process->pipe_in, &ds);
        res = select(process->pipe_in +1, &ds,NULL, NULL, &timeo);
        if(res == 0){
            close(process->pipe_in);
            break;
        }
        if(res == -1){
            break;
        }
        else{
            while((len = (hint32)read(process->pipe_in, buffer, sizeof(buffer))) >0);
            close(process->pipe_in);
            break;
        }
    }
    
    signal(SIGCHLD, sig_process_quit_handler);
    signal(SIGINT, sig_process_quit_handler);    
    signal(SIGTERM, sig_process_quit_handler);
    
    kill(process->pid, SIGKILL);
    
    waitpid(process->pid,NULL,0);
    
    mem_free(process);
    
}

ProcessState process_has_data(hprocess_t proc,hdouble timeout){
    process_t * process = (process_t *) proc;
    ProcessState state = ProcessStateNone;
    hint32 res;
    fd_set rds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    FD_ZERO(&rds);
    
    FD_SET(process->pipe_in, &rds);
    
    res = select(process->pipe_in + 1, &rds, NULL, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = ProcessStateError;
    }
    else{
        if(FD_ISSET(process->pipe_in, &rds)){
            state = ProcessStateOK;
        }
    }
    
    return state;
}

ProcessState process_has_space(hprocess_t proc,hdouble timeout){
    process_t * process = (process_t *) proc;
    ProcessState state = ProcessStateNone;
    hint32 res;
    fd_set wds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    FD_ZERO(&wds);
    
    FD_SET(process->pipe_out, &wds);
    
    res = select(process->pipe_out + 1, NULL,&wds, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = ProcessStateError;
    }
    else{
        if(FD_ISSET(process->pipe_out, &wds)){
            state = ProcessStateOK;
        }
    }
    
    return state;
}

hint32 process_read(hprocess_t proc,hbyte * data,hint32 length){
    process_t * process = (process_t *) proc;
    return (hint32)read(process->pipe_in, data, length);
}

hint32 process_write(hprocess_t proc,hbyte * data,hint32 length){
    process_t * process = (process_t *) proc;
    return (hint32)write(process->pipe_out, data, length);
}

#else

typedef struct{
	HANDLE hProcess;
	HANDLE hThread;
	HANDLE pipe_in;
    HANDLE pipe_out;
	HANDLE std_in;
	HANDLE std_out;
}process_t;

hprocess_t process_alloc(hcchar * command){
    
	HANDLE  hStdInRead;         
	HANDLE  hStdInWrite;         

	HANDLE  hStdOutRead;  
	HANDLE  hStdOutWrite; 
		
	STARTUPINFO siStartInfo; 
	PROCESS_INFORMATION piProcInfo;  

	BOOL bSuccess;
	WCHAR commandLine[1024];
	process_t * proc = NULL;

	wsprintf(commandLine,L"%s",command);

	bSuccess = CreatePipe(&hStdInRead, &hStdInWrite,NULL, 0);

	if(!bSuccess){
		return NULL;
	}

	bSuccess = CreatePipe(&hStdOutRead, &hStdOutWrite,NULL, 0);

	if(!bSuccess){
		CloseHandle(hStdInRead);
		CloseHandle(hStdInWrite);
		return NULL;
	}

	mem_memset( &siStartInfo,0, sizeof(STARTUPINFO) );

	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.dwFlags  |= STARTF_USESTDHANDLES;  
	siStartInfo.hStdOutput = hStdInWrite; 
	siStartInfo.hStdError  =  NULL;
	siStartInfo.hStdInput  = hStdOutRead;  

	bSuccess = CreateProcess(NULL,  
      commandLine,
      NULL,
      NULL,
      TRUE,
      0,
      NULL,
      NULL,
      &siStartInfo,
      &piProcInfo);

	if(!bSuccess){
		CloseHandle(hStdOutRead);
		CloseHandle(hStdOutWrite);
		CloseHandle(hStdInRead);
		CloseHandle(hStdInWrite);
		return NULL;
	}

	proc = (process_t *)mem_malloc(sizeof(process_t));

	mem_memset(proc,0,sizeof(process_t));

	proc->hProcess = piProcInfo.hProcess;
	proc->hThread = piProcInfo.hThread;
	proc->pipe_in = hStdInRead;
	proc->pipe_out = hStdOutWrite;
	proc->std_in = hStdOutRead;
	proc->std_out = hStdInWrite;

    return (hprocess_t)proc;
}

void process_dealloc(hprocess_t proc){
    process_t * process = (process_t *)proc;
    
	
	CloseHandle(process->pipe_out);
	CloseHandle(process->pipe_in);
	CloseHandle(process->std_in);
	CloseHandle(process->std_out);

	TerminateProcess(process->hProcess,0);

	WaitForSingleObject(process->hProcess,100);

	CloseHandle(process->hThread);
	CloseHandle(process->hProcess);

	mem_free(process);

}

ProcessState process_has_data(hprocess_t proc,hdouble timeout){
    process_t * process = (process_t *) proc;
    ProcessState state = ProcessStateNone;
	DWORD s = WaitForSingleObject(process->pipe_in,(DWORD)(timeout * 1000));
    
	switch(s){
		case WAIT_OBJECT_0:
			state = ProcessStateOK;
			break;
		case WAIT_FAILED:
			state = ProcessStateError;
			break;
	}

    return state;
}

ProcessState process_has_space(hprocess_t proc,hdouble timeout){
    process_t * process = (process_t *) proc;
    ProcessState state = ProcessStateNone;
	DWORD s = WaitForSingleObject(process->pipe_out,(DWORD)(timeout * 1000));
    
	switch(s){
		case WAIT_OBJECT_0:
			state = ProcessStateOK;
			break;
		case WAIT_FAILED:
			state = ProcessStateError;
			break;
	}

    return state;
}

hint32 process_read(hprocess_t proc,hbyte * data,hint32 length){
    process_t * process = (process_t *) proc;
	DWORD ret = 0;
	if(!ReadFile(process->pipe_in,data,length,&ret,NULL)){
		ret = 0;
	}
	return ret;
}

hint32 process_write(hprocess_t proc,hbyte * data,hint32 length){
    process_t * process = (process_t *) proc;
	DWORD ret = 0;
	if(!WriteFile(process->pipe_in,data,length,&ret,NULL)){
		ret = 0;
	}
	return ret;
}

#endif
