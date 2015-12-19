//
//  hactree.h
//  hclib
//
//  Created by 张海龙 张海龙 on 12-5-11.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#ifndef HACTREE_H
#define HACTREE_H

#ifdef __cplusplus
extern "C" {
#endif
   
#include "hmem.h"
#include "hserial_list.h"

    typedef struct {
        hwchar * unicodeString;
        hint32 length;
    }ac_string_t;
    
    typedef struct{
        hserial_list_t strings;
        hwchar unicode;
        hserial_list_t childs;
    }ac_tree_item_t;
    
    typedef struct{
        ac_tree_item_t head;
    }ac_tree_t;
    
    ac_tree_t * ac_tree_alloc(InvokeTickDeclare);
    
    ac_tree_t * ac_tree_retain(ac_tree_t * tree,InvokeTickDeclare);
    
    void ac_tree_release(ac_tree_t * tree,InvokeTickDeclare);
    
    void ac_tree_add(ac_tree_t * tree,ac_string_t string,InvokeTickDeclare);
    
    hserial_list_t ac_tree_search(ac_tree_t * tree,ac_string_t keyword,InvokeTickDeclare);
    
    hint32 ac_tree_match(ac_tree_t * tree,ac_string_t body,hserial_list_t strings,InvokeTickDeclare);
    
#ifdef __cplusplus
}
#endif

#endif
