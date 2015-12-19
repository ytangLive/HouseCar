//
//  hmap2.c
//  hclib
//
//  Created by zhang hailong on 13-2-26.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hmap.h"
#include "hrecursive.h"
#include "hstr.h"
#include "hlist.h"
#include "hserial_list.h"
#include "hserial_map.h"
#include "zlib.h"

#define MAP_HASH_TABLE_SIZE 512
#define MAP_NODE_EXTEND_SIZE 32
#define KEY_NOT_FOUND	0x7fffffff

#undef map_alloc

#undef map_dealloc

#undef map_put

#undef map_get

#undef map_remove

#undef map_get_and_exist

#undef map_get_by_defalut

#undef map_each

#undef map_clear

#undef map_count

huintptr hash_code_str(hany key ,InvokeTickDeclare){
	return crc32(0, key, (hint32)strlen((hchar *)key));
}

huintptr hash_code_any(hany key,InvokeTickDeclare){
	return (huintptr)key;
}

hbool equal_str(hany key1,hany key2,InvokeTickDeclare){
	return str_cmp((hcchar *)key1, (hcchar *)key2) ==0;
}

hbool equal_any(hany key1,hany key2,InvokeTickDeclare){
	return key1 == key2;
}

typedef struct{
    hany key;
    hany value;
} map_value_t;


typedef struct{
    hserial_list_t values;
} map_node_t;

typedef struct{
    hserial_map_t nodes;
} map_table_t;

typedef struct {
    struct _map_t base;
    map_table_t tables[MAP_HASH_TABLE_SIZE];
    hash_code_t hash_code;
    equal_t equal;
} map_t;

static void map_node_dealloc(const map_node_t * node, InvokeTickDeclare){
    serial_list_dealloc(node->values);
}

static void map_table_dealloc(const map_table_t * table,InvokeTickDeclare){
    hint32 i,c = serial_map_count(table->nodes);
    map_node_t * node;
    for(i=0;i<c;i++){
        node = (map_node_t *) serial_map_value_at(table->nodes, i);
        map_node_dealloc(node,InvokeTickArg);
    }
    serial_map_dealloc(table->nodes);
}

hmap_t map_alloc(hint32 capacity,hash_code_t hash_code ,equal_t equal,InvokeTickDeclare){
    map_t * map = (map_t *) mem_malloc(sizeof(map_t));
    mem_memset(map, 0, sizeof(map_t));
    map->hash_code = hash_code;
    map->equal = equal;
    return (hmap_t)map;
}

void map_dealloc(hmap_t map,InvokeTickDeclare){
    if(map){
        map_t * impl = (map_t *) map;
        map_table_t * table = impl->tables;
        hint32 i=0;
        while(i < MAP_HASH_TABLE_SIZE){
            
            map_table_dealloc(table,InvokeTickArg);
            
            table ++;
            i++;
        }
        mem_free(impl);
    }
}


hany map_put(hmap_t map,hany key,hany value,InvokeTickDeclare){
    if(map && key){
        map_t * impl = (map_t *) map;
        huintptr hash = (* impl->hash_code)(key,InvokeTickArg);
        map_table_t * table = impl->tables + (hash % MAP_HASH_TABLE_SIZE);
        map_node_t * pnode = NULL,node;
        map_value_t * v,vv;
        hint32 i,c;
        hany retValue = NULL;
        if(table->nodes == NULL){
            table->nodes = serial_map_alloc(sizeof(huintptr), sizeof(map_node_t), 0);
        }
        
        pnode = (map_node_t *)serial_map_get(table->nodes,&hash);
        
        if(pnode == NULL){
            mem_memset(&node, 0, sizeof(node));
            pnode = serial_map_put(table->nodes, &hash, &node);
        }
        
        assert(pnode);
        if(pnode->values == NULL){
            pnode->values = serial_list_alloc(sizeof(map_value_t), 0);
        }
        c = serial_list_count(pnode->values);
        for (i = 0; i < c; i ++) {
            v = (map_value_t *) serial_list_item_at(pnode->values, i);
            if((* impl->equal)(key,v->key,InvokeTickArg)){
                retValue = v->value;
                v->key = key;
                v->value = value;
                return retValue;
            }
        }
        mem_memset(&vv, 0, sizeof(vv));
        vv.key = key;
        vv.value = value;
        serial_list_item_add(pnode->values, &vv);
        impl->base.count ++;
        return retValue;
    }
    return NULL;
}

hany map_get(hmap_t map,hany key,InvokeTickDeclare){
    if(map && key){
        map_t * impl = (map_t *) map;
        huintptr hash = (* impl->hash_code)(key,InvokeTickArg);
        map_table_t * table = impl->tables + (hash % MAP_HASH_TABLE_SIZE);
        map_node_t * pnode = (map_node_t *) serial_map_get(table->nodes, &hash);
        map_value_t * v ;
        hint32 i,c;
        if(pnode){
            c = serial_list_count(pnode->values);
            for(i = 0;i < c;i ++){
                v = (map_value_t *) serial_list_item_at(pnode->values, i);
                if((* impl->equal)(key,v->key,InvokeTickArg)){
                    return v->value;
                }
            }
        }
    }
    return NULL;
}

hany map_remove(hmap_t map,hany key,InvokeTickDeclare){
    if(map && key){
        map_t * impl = (map_t *) map;
        huintptr hash = (* impl->hash_code)(key,InvokeTickArg);
        map_table_t * table = impl->tables + (hash % MAP_HASH_TABLE_SIZE);
        map_node_t * pnode = (map_node_t *) serial_map_get(table->nodes, &hash);
        map_value_t * v ;
        hany retValue = NULL;
        hint32 i,c;
        if(pnode){
            c = serial_list_count(pnode->values);
            for(i = 0;i < c;i ++){
                v = (map_value_t *) serial_list_item_at(pnode->values, i);
                if((* impl->equal)(key,v->key,InvokeTickArg)){
                    retValue = v->value;
                    serial_list_remove_at(pnode->values, i);
                    impl->base.count --;
                    assert(impl->base.count >=0);
                    return retValue;
                }
            }
        }
        return retValue;
    }
    return NULL;
}

hany map_get_and_exist(hmap_t map,hany key,hbool * exist,InvokeTickDeclare){
    if(map && key){
        map_t * impl = (map_t *) map;
        huintptr hash = (* impl->hash_code)(key,InvokeTickArg);
        map_table_t * table = impl->tables + (hash % MAP_HASH_TABLE_SIZE);
        map_node_t * pnode = (map_node_t *) serial_map_get(table->nodes, &hash);
        map_value_t * v ;
        hint32 i,c;
        if(pnode){
            c = serial_list_count(pnode->values);
            for(i = 0;i < c;i ++){
                v = (map_value_t *) serial_list_item_at(pnode->values, i);
                if((* impl->equal)(key,v->key,InvokeTickArg)){
                    if(exist){
                        * exist = hbool_true;
                    }
                    return v->value;
                }
            }
        }
    }
    if(exist){
        * exist = hbool_false;
    }
    return NULL;
}

hany map_get_by_defalut(hmap_t map,hany key,hany defaultValue,InvokeTickDeclare){
    if(map && key){
        map_t * impl = (map_t *) map;
        huintptr hash = (* impl->hash_code)(key,InvokeTickArg);
        map_table_t * table = impl->tables + (hash % MAP_HASH_TABLE_SIZE);
        map_node_t * pnode = (map_node_t *) serial_map_get(table->nodes, &hash);
        map_value_t * v ;
        hint32 i,c;
        if(pnode){
            c = serial_list_count(pnode->values);
            for(i = 0;i < c;i ++){
                v = (map_value_t *) serial_list_item_at(pnode->values, i);
                if((* impl->equal)(key,v->key,InvokeTickArg)){
                    return v->value;
                }
            }
        }
    }
    return defaultValue;
}

void map_each(hmap_t map,map_each_t each,hany arg0,hany arg1,InvokeTickDeclare){
    if(map && each){
        map_t * impl = (map_t *) map;
        hint32 i=0,ii,cc,iii,ccc;
        map_table_t * table = (map_table_t *) impl->tables;
        map_node_t * node;
        map_value_t * v;
        while(i <MAP_HASH_TABLE_SIZE){
            cc = serial_map_count(table->nodes);
            
            for(ii=0;ii<cc;ii++){
                
                node = (map_node_t *)serial_map_value_at(table->nodes, ii);
                
                ccc = serial_list_count(node->values);
                
                for(iii=0;iii<ccc;iii++){
                    v = (map_value_t *) serial_list_item_at(node->values, iii);
                    
                    (* each)(map,v->key,v->value,arg0,arg1,InvokeTickArg);
                }
            }
            table ++;
            i++;
        }
    }
}

void map_clear(hmap_t map,InvokeTickDeclare){
    if(map){
        map_t * impl = (map_t *) map;
        hint32 i=0,ii,cc;
        map_table_t * table = (map_table_t *) impl->tables;
        map_node_t * node;
        while(i <MAP_HASH_TABLE_SIZE){
            cc = serial_map_count(table->nodes);
            for(ii=0;ii<cc;ii++){
                node = (map_node_t *)serial_map_value_at(table->nodes, ii);
                serial_list_clear(node->values);
            }
            serial_map_clear(table->nodes);
            i++;
        }
        impl->base.count = 0;
    }
}

hint32 map_count(hmap_t map,InvokeTickDeclare){
    if(map){
        return map->count;
    }
    return 0;
}


