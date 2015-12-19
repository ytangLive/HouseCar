//
//  hnetbios.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-4-19.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HNETBIOS_H
#define HNETBIOS_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hmem.h"
    
#define NETBIOS_PORT    137
    
#define PF_NETBIOS	AF_NETBIOS
    
#define	NBPROTO_TCPSSN	1		/* NETBIOS session over TCP */
    
#define NB_NAMELEN	16
#define	NB_ENCNAMELEN	NB_NAMELEN * 2
#define	NB_MAXLABLEN	63
    
#define	NB_MINSALEN	(sizeof(struct sockaddr_nb))
    
    /*
     * name types
     */
#define	NBT_WKSTA	0x00
#define	NBT_CLIENT	0x03
#define	NBT_RASSRVR	0x06
#define	NBT_DMB		0x1B
#define	NBT_IP		0x1C
#define	NBT_MB		0x1D
#define	NBT_BS		0x1E
#define	NBT_NETDDE	0x1F
#define	NBT_SERVER	0x20
#define	NBT_RASCLNT	0x21
#define	NBT_NMAGENT	0xBE
#define	NBT_NMUTIL	0xBF
    
    /*
     * Session packet types
     */
#define	NB_SSN_MESSAGE		0x0
#define	NB_SSN_REQUEST		0x81
#define	NB_SSN_POSRESP		0x82
#define	NB_SSN_NEGRESP		0x83
#define	NB_SSN_RTGRESP		0x84
#define	NB_SSN_KEEPALIVE	0x85
    
    /*
     * resolver: Opcodes
     */
#define	NBNS_OPCODE_QUERY	0x00
#define	NBNS_OPCODE_REGISTER	0x05
#define	NBNS_OPCODE_RELEASE	0x06
#define	NBNS_OPCODE_WACK	0x07
#define	NBNS_OPCODE_REFRESH	0x08
#define	NBNS_OPCODE_RESPONSE	0x10	/* or'ed with other opcodes */
    
    /*
     * resolver: NM_FLAGS
     */
#define	NBNS_NMFLAG_BCAST	0x01
#define	NBNS_NMFLAG_RA		0x08	/* recursion available */
#define	NBNS_NMFLAG_RD		0x10	/* recursion desired */
#define	NBNS_NMFLAG_TC		0x20	/* truncation occured */
#define	NBNS_NMFLAG_AA		0x40	/* authoritative answer */
    
    /* 
     * resolver: Question types
     */
#define	NBNS_QUESTION_TYPE_NB		0x0020
#define NBNS_QUESTION_TYPE_NBSTAT	0x0021
    
    /* 
     * resolver: Question class 
     */
#define NBNS_QUESTION_CLASS_IN	0x0001
    
    /*
     * resolver: Limits
     */
#define	NBNS_MAXREDIRECTS	3	/* maximum number of accepted redirects */
#define	NBDG_MAXSIZE		576	/* maximum nbns datagram size */
    
    /*
     * NETBIOS addressing
     */
    struct nb_name {
        unsigned	nn_type;
        u_char		nn_name[NB_NAMELEN + 1];
    };
    
    /*
     * Socket address
     */
    struct sockaddr_nb {
        u_char				snb_len;
        u_char				snb_family;
        struct sockaddr_in	snb_addrin;
        u_char				snb_name[1 + NB_ENCNAMELEN + 1];	/* encoded */
    };
    
#define GET_IPV4_ADDRESS(a, b) \
if (a->sa_family == AF_NETBIOS) { \
b = (struct sockaddr_in *)((void *)&((struct sockaddr_nb *)((void *)a))->snb_addrin); \
} else if (a->sa_family == AF_INET) {\
b = (struct sockaddr_in *)((void *)a); \
} else { \
b = (struct sockaddr_in *)NULL; \
}

    
    
    typedef unsigned char byte;
    typedef unsigned int word;
    
    
    /* Structure of Network Control Block (NCB)                           */
    typedef struct
    {
        byte NCB_COMMAND;		/* command                            */
        byte NCB_RETCODE;		/* function return code               */
        byte NCB_LSN;			/* local session number               */
        byte NCB_NUM;			/* number of network name             */
        word NCB_BUFFER_OFFSET;	/* offset of message buffer           */
        word NCB_BUFFER_SEGMENT;	/* segment of message buffer          */
        word NCB_LENGTH;		/* length of message buffer   	      */
        byte NCB_CALLNAME[16];	/* name of session user is talking to */
        byte NCB_NAME[16];		/* user's network name                */
        byte NCB_RTO;			/* receive time-out in 500 ms. incrs. */
        byte NCB_STO;			/* send time-out - 500 ms. increments */
        word NCB_POST_OFFSET;		/* offset of "no-wait" interrupt call */
        word NCB_POST_SEGMENT;	/* segment of "no-wait" interrupt     */
        byte NCB_LANA_NUM;		/* adapter number (must be 0 or 1)    */
        byte NCB_CMD_CPLT;		/* command completion status          */
        byte NCB_RESERVE[14];		/* Reserved area for Token-Ring       */
    } NCB;
    
    
    /* NetBIOS error return codes - returned in NCB_RETCODE               */
#define ERR_success 0		/* NetBIOS command completed normally */
#define ERR_bad_buffer_length 1 /* Bad send or status buffer size     */
#define ERR_invalid 3		/* invalid NetBIOS command            */
#define ERR_timeout 5           /* Command time-out has expired       */
#define ERR_buffer_too_small 6  /* Receive buffer not big enough      */
#define ERR_bad_session_num 8   /* Bad value in NCB_LSN               */
#define ERR_no_RAM 9            /* LAN card doesn't have enough memory*/
#define ERR_session_closed 0xa  /* This session is closed             */
#define ERR_cancel 0xb          /* Command has been closed            */
#define ERR_dup_local_name 0xd  /* Name already exists for this PC    */
#define ERR_name_table_full 0xe /* Local name table is full           */
#define ERR_active_session 0xf  /* Can't delete name - used in session*/
#define ERR_sess_table_full 0x11 /* Local session table is full       */
#define ERR_no_listen 0x12      /* Remote PC not listening for call   */
#define ERR_bad_name_num 0x13   /* Bad value in NCB_NUM field         */
#define ERR_no_answer 0x14      /* No answer to CALL or no such remote*/
#define ERR_no_local_name 0x15  /* No such name in local name table   */
#define ERR_duplicate_name 0x16 /* Name is in use elsewhere on net    */
#define ERR_bad_delete 0x17     /* Name incorrectly deleted           */
#define ERR_abnormal_end 0x18   /* Session aborted abnormally         */
#define ERR_name_error 0x19     /* 2 or more identical names in use!  */
#define ERR_bad_packet 0x1a     /* Bad NetBIOS packet on network      */
#define ERR_card_busy 0x21      /* network card is busy               */
#define ERR_too_many_cmds 0x22	/* Too many NetBIOS commands queued   */
#define ERR_bad_card_num 0x23   /* bad NCB_LANA_NUM - must be 0 or 1  */
#define ERR_cancel_done 0x24    /* command finished while cancelling  */
#define ERR_no_cancel 0x26	/* Command can't be cancelled         */
#define ERR_busy 0xff		/* Still processing command           */
    
    
    /* NetBIOS functions list - "WAIT" calls wait until command completes */
    /* while the others jump to the routine in NCB_POST when the NetBIOS  */
    /* command completes and does an interrupt.                           */
#define RESET 0x32		/* Reset adapter card and tables      */
#define CANCEL 0x35		/* Cancel command. NCB_BUFFER = cmd.  */
    
#define STATUS 0xb3		/* status information for adapter     */
#define STATUS_WAIT 0x33
    
#define TRACE 0xf9		/* Token-Ring protocol trace          */
#define TRACE_WAIT 0x79
    
#define UNLINK 0x70		/* unlink from IBM Remote Program     */
    
#define ADD_NAME 0xb0		/* Add name to name table             */
#define ADD_NAME_WAIT 0x30
    
#define ADD_GROUP_NAME 0xb6	/* Add group name to name table       */
#define ADD_GROUP_NAME_WAIT 0x36
    
#define DELETE_NAME 0xb1	/* Delete name from name table        */
#define DELETE_NAME_WAIT 0x31
    
#define CALL 0x90		/* Start session with NCB_NAME name   */
#define CALL_WAIT 0x10
    
#define LISTEN 0x91             /* Listen for call                    */
#define LISTEN_WAIT 0x11
    
#define HANG_UP 0x92		/* End session with NCB_NAME name     */
#define HANG_UP_WAIT 0x12
    
#define SEND 0x94		/* Send data via NCB_LSN              */
#define SEND_WAIT 0x14
    
#define SEND_NO_ACK 0xf1	/* Send data without waiting for ACK  */
#define SEND_NO_ACK_WAIT 0x71
    
#define CHAIN_SEND 0x97		/* Send multiple data buffers         */
#define CHAIN_SEND_WAIT 0x17
    
#define CHAIN_SEND_NO_ACK 0xf2  /* Send multiple buffers without ACK  */
#define CHAIN_SEND_NO_ACK_WAIT 0x72
    
#define RECEIVE 0x95		/* Receive data from a session	      */
#define RECEIVE_WAIT 0x15
    
#define RECEIVE_ANY 0x96	/* Receive data from any session      */
#define RECEIVE_ANY_WAIT 0x16
    
#define SESSION_STATUS 0xb4	/* status of all sessions for name    */
#define SESSION_STATUS_WAIT 0x34
    
#define SEND_DATAGRAM 0xa0	/* send un-ACKed message              */
#define SEND_DATAGRAM_WAIT 0x20
    
#define SEND_BCST_DATAGRAM 0xa2 /* send broadcast message             */
#define SEND_BCST_DATAGRAM_WAIT 0x22
    
#define RECEIVE_DATAGRAM 0xa1	/* receive un-ACKed message           */
#define RECEIVE_DATAGRAM_WAIT 0x21
    
    
#define RECEIVE_BCST_DATAGRAM 0xa3 /* receive broadcast message       */
#define RECEIVE_BCST_DATAGRAM_WAIT 0x23
    
    


    typedef struct{
        hsocket_t READONLY sock;
        struct sockaddr_in READONLY broadcast;
    }netbios_t;

    typedef struct{
        struct sockaddr_in addr;
    }netbios_user_t;
    
    typedef enum{
        NetBiosStateNone,NetBiosStateOK,NetBiosStateError
    }NetBiosState;
    
    netbios_t * netbios_alloc(hint32 port,InvokeTickDeclare);
    
    netbios_t * netbios_retain(netbios_t * netbios,InvokeTickDeclare);
    
    void netbios_release(netbios_t * netbios,InvokeTickDeclare);
    
    NetBiosState netbios_browser_broadcast(netbios_t * netbios,hdouble timeout,InvokeTickDeclare);
    
    NetBiosState netbios_browser_user(netbios_t * netbios,netbios_user_t * user,hdouble timeout,InvokeTickDeclare);

#ifdef __cplusplus
}
#endif

#endif
