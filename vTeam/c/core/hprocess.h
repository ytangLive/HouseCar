//
//  hprocess.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-3-16.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HPROCESS_H
#define HPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif
    
    
    HANDLER(hprocess_t)
    
    typedef enum{
        ProcessStateNone,ProcessStateOK,ProcessStateError
    }ProcessState;
    
    hprocess_t process_alloc(hcchar * command);
    
    hprocess_t process_alloc_with_io(hcchar * command,int inno,int outno);
    
    void process_dealloc(hprocess_t proc);
    
    ProcessState process_has_data(hprocess_t proc,hdouble timeout);
    
    ProcessState process_has_space(hprocess_t proc,hdouble timeout);
    
    hint32 process_read(hprocess_t proc,hbyte * data,hint32 length);
    
    hint32 process_write(hprocess_t proc,hbyte * data,hint32 length);
    
    
    
#ifdef __cplusplus
}
#endif

#endif
