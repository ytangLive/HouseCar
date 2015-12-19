//
//  hactree.c
//  hclib
//
//  Created by 张海龙 张海龙 on 12-5-11.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hactree.h"
#include "hext_obj.h"
#include "hrecursive.c"


static void ac_tree_item_dealloc(ac_tree_item_t * item,InvokeTickDeclare);

static void ac_tree_ext_init(hext_obj_t obj,hint32 size,hany arg,InvokeTickDeclare){
    ac_tree_t * tree =(ac_tree_t *) obj;
    tree->head.strings = serial_list_alloc(sizeof(ac_string_t), 0);
    tree->head.childs = NULL;
}

static void ac_tree_ext_destroy(hext_obj_t obj,hint32 size,InvokeTickDeclare){
    ac_tree_t * impl = (ac_tree_t *) obj;
    ac_tree_item_dealloc(&impl->head, InvokeTickArg);
}

static ext_class_t ac_tree_ext_class = {ac_tree_ext_init,ac_tree_ext_destroy,sizeof(ac_tree_t)};

ac_tree_t * ac_tree_alloc(InvokeTickDeclare){
    return (ac_tree_t *) ext_obj_alloc(NULL, &ac_tree_ext_class);
}

ac_tree_t * ac_tree_retain(ac_tree_t * tree,InvokeTickDeclare){
    return (ac_tree_t *) ext_obj_retain((hext_obj_t)tree);
}

void ac_tree_release(ac_tree_t * tree,InvokeTickDeclare){
    ext_obj_release((hext_obj_t)tree);
}

static hint32 ac_tree_item_find(hserial_list_t items,hint32 * cIndex,hwchar unicode,InvokeTickDeclare){
    ac_tree_item_t * item;
    hint32 rs = -1;
    hint32 startIndex = 0;
    hint32 endIndex = serial_list_count(items) - 1;
    * cIndex = 0;
    while( startIndex <= endIndex){
        * cIndex = ( startIndex + endIndex ) / 2;
        item = serial_list_item_at(items, * cIndex);
        if(unicode == item->unicode){
            rs = 0;
            break;
        }
        else if( item->unicode < unicode){
            startIndex = * cIndex + 1;
            rs = 1;
        }
        else {
            endIndex = * cIndex -1;
            rs = -1;
        }
    }
    
    return rs;
}

typedef struct{
    hint32 index;
    ac_string_t * string;
}ac_tree_item_add_recursive_callback_param_t;

static void ac_tree_item_add_recursive_callback(hrecursive_t recursive,hany caller,hany arg0,hany arg1,hany arg2,hany arg3,hany * result,InvokeTickDeclare){
    ac_tree_item_add_recursive_callback_param_t * param = (ac_tree_item_add_recursive_callback_param_t *) arg0;
    ac_tree_item_t * item = (ac_tree_item_t *)caller;
    hint32 index = param->index,i,c;
    ac_string_t * string = param->string, * s;
    
    if(index < string->length){
        hwchar unicode = string->unicodeString[index];
        hint32 cIndex=0;
        hint32 rs = ac_tree_item_find(item->childs,  & cIndex, unicode, InvokeTickArg);
        ac_tree_item_t * target = NULL;
        ac_tree_item_t t = {0};
        
        if(rs == 0){
            target = serial_list_item_at(item->childs, cIndex);
        }
        else if(rs <0){
            t.unicode = unicode;
            t.strings = serial_list_alloc(sizeof(ac_string_t), 0);
            t.childs = NULL;
            if(item->childs == NULL){
                item->childs = serial_list_alloc(sizeof(ac_tree_item_t), 0);
            }
            target = serial_list_item_insert(item->childs, & t, cIndex);
        }
        else {
            t.unicode = unicode;
            t.strings = serial_list_alloc(sizeof(ac_string_t), 0);
            t.childs = NULL;
            if(item->childs == NULL){
                item->childs = serial_list_alloc(sizeof(ac_tree_item_t), 0);
            }
            target = serial_list_item_insert(item->childs, & t, cIndex +1);
        }
        
        c = serial_list_count(target->strings);
        for(i=0;i<c;i++){
            s = serial_list_item_at(target->strings, i);
            if(s->length > string->length){
                break;
            }
        }
        
        serial_list_item_insert(target->strings, string, i);
        
        param->index ++;
        
        recursive_add_caller(recursive, target, InvokeTickArg);
    }
}

void ac_tree_add(ac_tree_t * tree,ac_string_t string,InvokeTickDeclare){

    hrecursive_t r = recursive_alloc(InvokeTickArg);
    ac_tree_item_add_recursive_callback_param_t param = {0,&string};
    recursive_call(r, &tree->head, &param, NULL, NULL, NULL, NULL, ac_tree_item_add_recursive_callback, InvokeTickArg);
    recursive_dealloc(r,InvokeTickArg);
}


hserial_list_t ac_tree_search(ac_tree_t * tree,ac_string_t keyword,InvokeTickDeclare){
    ac_tree_item_t * item = & tree->head;
    hint32 index = 0;
    hint32 rs = -1,cIndex = 0;
    hword unicode =0;
    while(index < keyword.length){
        unicode = keyword.unicodeString[index];
        cIndex = 0;
        rs = ac_tree_item_find(item->childs,& cIndex,unicode,InvokeTickArg);
        if(rs != 0){
            break;
        }
        item = serial_list_item_at(item->childs, cIndex);
        index++;
    }
    return rs == 0 && item ? item->strings : NULL;
}


static void ac_tree_item_dealloc(ac_tree_item_t * item,InvokeTickDeclare){
    hint32 i,c = serial_list_count(item->childs);
    ac_tree_item_t * child;
    for(i=0;i<c;i++){
        child = serial_list_item_at(item->childs, i);
        ac_tree_item_dealloc(child,InvokeTickArg);
    }
    serial_list_dealloc(item->strings);
    serial_list_dealloc(item->childs);
}

hint32 ac_tree_match(ac_tree_t * tree,ac_string_t body,hserial_list_t strings, InvokeTickDeclare){
    ac_tree_item_t * item = & tree->head;
    ac_string_t * s;
    hint32 index = 0;
    hint32 rs = -1,cIndex = 0;
    hint32 count = 0;
    hword unicode =0;
    while(index < body.length){
        unicode = body.unicodeString[index];
        cIndex = 0;
        rs = ac_tree_item_find(item->childs,& cIndex,unicode,InvokeTickArg);
        if(rs != 0){
            break;
        }
        item = serial_list_item_at(item->childs, cIndex);
        index++;
        
        if(serial_list_count(item->strings) >0){
            s = serial_list_item_at(item->strings, 0);
            if(s->length == index){
                serial_list_item_add(strings, s);
                count ++;
            }
        }
    }
    return  count;

}


