//
//  hnetbios.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-19.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hnetbios.h"
#include "hext_obj.h"
#include "hlog.h"

typedef struct{
    netbios_t base;
    hubyte buffer[2048];
}netbios_impl_t;

static void netbios_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    netbios_t * netbios = (netbios_t *) obj;
    struct sockaddr_in addr;
    hint32 port = (hint32)(hintptr)arg;
    hint32 fp = 1;
    netbios->sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    
    netbios->broadcast.sin_family = AF_INET;
    netbios->broadcast.sin_port = htons(port);
    netbios->broadcast.sin_addr.s_addr = INADDR_BROADCAST;
    
    mem_memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);
    
    setsockopt(netbios->sock, SOL_SOCKET,SO_BROADCAST, &fp, sizeof(fp));
    
    if(bind(netbios->sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        hlog("socket bind %d error",port);
        closesocket(netbios->sock);
        netbios->sock = -1;
    }
    
}

static void netbios_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    netbios_t * netbios = (netbios_t *) obj;
    
    if(netbios->sock != -1){
        {
            fd_set ds;
            struct timeval timeo = {0, 0};
            hbyte buffer[1024];
            hint32 len,res;
            struct sockaddr_in addr;
            socklen_t addrlen = sizeof(struct sockaddr_in);
            
            if(-1 != getsockname(netbios->sock, (struct sockaddr *)&addr,&addrlen)){
                printf("close %s:%d\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
            }
            
            timeo.tv_usec = 100;
            shutdown(netbios->sock, 1);
            FD_ZERO(&ds);
            FD_SET(netbios->sock, &ds);
            while(0 == select(netbios->sock +1, NULL, &ds, NULL, &timeo)){
                FD_ZERO(&ds);
                FD_SET(netbios->sock, &ds);
            }
            while(hbool_true){
                FD_ZERO(&ds);
                FD_SET(netbios->sock, &ds);
                res = select(netbios->sock +1, &ds,NULL, NULL, &timeo);
                if(res == 0){
                    shutdown(netbios->sock, 0);
                    break;
                }
                if(res == -1){
                    break;
                }
                else{
                    while((len = (hint32)recv(netbios->sock, buffer, sizeof(buffer), 0)) >0);
                    shutdown(netbios->sock, 0);
                    break;
                }
            }
            
            closesocket(netbios->sock);
            netbios->sock = -1;
        }
    }

}

static ext_class_t netbios_ext_class = {netbios_ext_init,netbios_ext_destroy,sizeof(netbios_impl_t)};


netbios_t * netbios_alloc(hint32 port,InvokeTickDeclare){
    return (netbios_t *) ext_obj_alloc((hany)(hintptr)port,&netbios_ext_class);
}

netbios_t * netbios_retain(netbios_t * netbios,InvokeTickDeclare){
    return (netbios_t *) ext_obj_retain((hext_obj_t)netbios);
}

void netbios_release(netbios_t * netbios,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)netbios);
}

NetBiosState netbios_browser_broadcast(netbios_t * netbios,hdouble timeout,InvokeTickDeclare){

    NetBiosState state = NetBiosStateNone;
    hint32 res;
    fd_set wds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    NCB ncb = {0};
    hint32 len;
    FD_ZERO(&wds);
    
    FD_SET(netbios->sock, &wds);
    
    res = select(netbios->sock + 1, NULL, &wds, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = NetBiosStateError;
    }
    else{
        if(FD_ISSET(netbios->sock, &wds)){
            ncb.NCB_COMMAND = RECEIVE_ANY;
            len = (hint32) sendto(netbios->sock, &ncb, sizeof(NCB), 0,(struct sockaddr *)&netbios->broadcast,sizeof(struct sockaddr_in));
            
            if(len == sizeof(ncb)){
                printf("RECEIVE_ANY\n");
                state = NetBiosStateOK;
            }
            else{
                state = NetBiosStateError;
            }
        }
    }
    
    return state;
}

NetBiosState netbios_browser_user(netbios_t * netbios,netbios_user_t * user,hdouble timeout,InvokeTickDeclare){
    netbios_impl_t * bios = (netbios_impl_t *)netbios; 
    NetBiosState state = NetBiosStateNone;
    hint32 res;
    hint32 len;
    fd_set rds;
    struct timeval timeo = {(hint32)timeout, (hint32)((timeout - (hint32)timeout) * 1000)};
    NCB * ncb;
    FD_ZERO(&rds);
    
    FD_SET(netbios->sock, &rds);
    
    res = select(netbios->sock + 1, &rds, NULL, NULL, &timeo);
    
    if(res == 0){
        
    }
    else if(res == -1){
        state = NetBiosStateError;
    }
    else{
        if(FD_ISSET(netbios->sock, &rds)){
            printf("recv\n");
            if((len = (hint32)recv(netbios->sock, bios->buffer, sizeof(bios->buffer), 0)) >0){
                if(len >= sizeof(NCB)){
                    ncb = (NCB *)bios->buffer;
                }
                state = NetBiosStateOK;
            }
            else{
                state = NetBiosStateError;
            }
            
        }
    }
    
    return state;

}