//
//  hdevice.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-16.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hdevice.h"
#include "md5.h"

#define BUFFERSIZE 2048
#define	max(a,b)	((a) > (b) ? (a) : (b))

device_t device(void){
    device_t d = {{0}};
    md5_state_t md5;
    md5_init(&md5);
    
    md5_append(&md5, (md5_byte_t *)"__DEVICE__", 10);
    
    // MAC
    while(hbool_true){
        {
            struct ifconf ifc;
            struct ifreq *ifr;
            int sockfd;
            char buffer[BUFFERSIZE], *cp, *cplim;
            char * t;
            struct sockaddr_dl *sdl;
            
            sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0)
            {
                perror("socket failed");
                break;
            }
            
            ifc.ifc_len = BUFFERSIZE;
            ifc.ifc_buf = buffer;
            
            if (ioctl(sockfd, SIOCGIFCONF, (char *)&ifc) < 0)
            {
                perror("ioctl error");
                close(sockfd);
                break;
            }
            
            ifr = ifc.ifc_req;
            
            cplim = buffer + ifc.ifc_len;
            
            for (cp=buffer; cp < cplim; )
            {
                ifr = (struct ifreq *)cp;
                if (ifr->ifr_addr.sa_family == AF_LINK)
                {
                    sdl = (struct sockaddr_dl *)&ifr->ifr_addr;
                    
                    t = (char *)ether_ntoa((struct ether_addr *)LLADDR(sdl));
                    
                    if(t){
                        md5_append(&md5, (md5_byte_t *)t, (hint32)strlen(t));
                    }
                    
                }
                cp += sizeof(ifr->ifr_name) + max(sizeof(ifr->ifr_addr), ifr->ifr_addr.sa_len);
            }
            
            close(sockfd);
            
            break;
        }
    }
    
    md5_finish(&md5, (md5_byte_t *)&d.uniqueId);
    
    
    return d;
}