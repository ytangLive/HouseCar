/*
 *  hserial_map.c
 *  C_Library
 *
 *  Created by hailong zhang on 11-3-8.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hserial_map.h"


#undef serial_map_alloc

#undef serial_map_dealloc

#undef serial_map_extra

#undef serial_map_extra_bytes

#undef serial_map_count

#undef serial_map_key_at

#undef serial_map_value_at

#undef serial_map_value_set

#undef serial_map_get

#undef serial_map_put

#undef serial_map_remove

#undef serial_map_clear

#undef serial_map_load_bytes

#undef serial_map_save_bytes


#define KEY_NOT_FOUND	0x7fffffff
#define KEY_ITEM_EXTEND_MIN	10

typedef struct{
	hbyte * data;
	hint32 length;
    hint32 item_length;
	hint32 item_bytes;
	hint32 key_bytes;
	hint32 value_bytes;
	hint32 extra_bytes;

}serial_map_t;


static void _serial_map_data_extend(serial_map_t * map,hint32 length,InvokeTickDeclare){
	if(length > map->length){
		map->length = length < KEY_ITEM_EXTEND_MIN * map->item_bytes ?  KEY_ITEM_EXTEND_MIN * map->item_bytes : length;
		if(map->data){
			map->data = mem_realloc(map->data, map->length);
		}
		else{
			map->data = mem_malloc(map->length);
		}
	}
}


hserial_map_t serial_map_alloc(hint32 key_bytes,hint32 value_bytes,hint32 extra_bytes,InvokeTickDeclare){
	if(  key_bytes >0 && value_bytes >0 && extra_bytes >=0){
		serial_map_t * map = (serial_map_t *)mem_malloc(sizeof(serial_map_t) + extra_bytes);
		mem_memset(map, 0,sizeof(serial_map_t) + extra_bytes);
		map->item_bytes = key_bytes + value_bytes;
		map->key_bytes = key_bytes;
		map->value_bytes = value_bytes;
		map->extra_bytes = extra_bytes;
		
		return (hserial_map_t)map;
	}
	return NULL;
}


void serial_map_dealloc(hserial_map_t hMap,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map){
        if(map->data){
            mem_free(map->data);
        }
		mem_free( map);
	}
}

hany serial_map_extra(hserial_map_t hMap,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && map->extra_bytes >0){
		return (hbyte *)map + sizeof(serial_map_t);
	}
	return NULL;
}

hint32 serial_map_extra_bytes(hserial_map_t hMap,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map){
		return map->extra_bytes;
	}
	return 0;
}

hint32 serial_map_count(hserial_map_t hMap,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map){
		return map->item_length;
	}
	return 0;
}

#define SERIAL_MAP_ITEM_AT(map,index)			(hany )((map)->data ? (map)->data + (index) * (map)->item_bytes : NULL)
#define SERIAL_MAP_ITEM_VALUE_AT(map,index)		(hany)((hbyte *)SERIAL_MAP_ITEM_AT(map,index) + (map)->key_bytes)


hany serial_map_key_at(hserial_map_t hMap,hint32 i,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && i >=0 && i< map->item_length){
		return SERIAL_MAP_ITEM_AT(map,i);
	}
	return NULL;
}

hany serial_map_value_at(hserial_map_t hMap,hint32 i,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && i >=0 && i< map->item_length){
		return SERIAL_MAP_ITEM_VALUE_AT(map,i);
	}
	return NULL;
}

hbool serial_map_value_set(hserial_map_t hMap,hint32 i,hany  value,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && i >=0 && i< map->item_length){
		mem_memcpy(SERIAL_MAP_ITEM_VALUE_AT(map,i),value,map->value_bytes);
		return hbool_true;
	}
	return hbool_false;
}

static hint32 _serial_map_key_comp(hany key1,hany key2,hint32 keyBytes){
    huintptr * p1 = (huintptr *) key1;
    huintptr * p2 = (huintptr *) key2;
    hubyte *bp1 , *bp2;
    hint32 len = keyBytes / sizeof(huintptr);
    hint32 m = keyBytes % sizeof(huintptr);
    hint32 rs = 0;
    
    while(len >0){
        rs = (hint32) ((hint64) *p1 - (hint64)*p2);
        if(rs != 0){
            return rs;
        }
        p1 ++;
        p2 ++;
        len --;
    }
    
    bp1 = (hubyte *) p1;
    bp2 = (hubyte *) p2;
    while(m >0){
        rs = *bp1 - *bp2;
        if(rs !=0){
            return rs;
        }
        bp1 ++;
        bp2 ++;
        m --;
    }
    
    return rs;
}

static hint32 _serial_map_find(serial_map_t * map,hany  key,hint32 * pIndex,hint32 * nIndex,InvokeTickDeclare){
	hint32 i = -1 ;
	hint32 rs = KEY_NOT_FOUND;
	*pIndex = 0;
	*nIndex = map->item_length - 1;
	
	while(*pIndex < *nIndex){
		i = (*pIndex + *nIndex) / 2;
		rs = _serial_map_key_comp(key, (hany)SERIAL_MAP_ITEM_AT(map,i), map->key_bytes);
		if(rs == 0){
			*nIndex = *pIndex = i;
			break;
		}
		else if(rs <0){
			*nIndex = i -1;
		}
		else{
			*pIndex = i +1;
		}
	}
	
	if(*pIndex > *nIndex){
		if(rs != KEY_NOT_FOUND){
			if(rs <0){
				*nIndex = (*nIndex) +1;
			}
			else{
				*pIndex = (*pIndex) -1;
			}
		}
	}
	else if(*pIndex == *nIndex){
		rs = _serial_map_key_comp(key, (hany)SERIAL_MAP_ITEM_AT(map,*pIndex), map->key_bytes);
	}
	
	return rs;
}

hany serial_map_get(hserial_map_t hMap,hany key,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map){
		hint32 pIndex,nIndex;
		hint32 rs = _serial_map_find(map, key, &pIndex, &nIndex,InvokeTickArg);
		if(rs ==0 && pIndex == nIndex){
			return SERIAL_MAP_ITEM_VALUE_AT(map,pIndex);
		}
	}
	return NULL;
}


hany serial_map_put(hserial_map_t hMap,hany key, hany value,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && key && value){
		hint32 pIndex,nIndex,i;
		hint32 rs = _serial_map_find(map, key, &pIndex, &nIndex,InvokeTickArg);
		hbyte * item;
		if(rs == KEY_NOT_FOUND){
			assert(map->item_length ==0);
			_serial_map_data_extend(map, map->item_length * map->item_bytes + map->item_bytes,InvokeTickArg);
			item = SERIAL_MAP_ITEM_AT(map,0);
			mem_memcpy(item, key, map->key_bytes);
			item += map->key_bytes;
			mem_memcpy(item, value, map->value_bytes);
			map->item_length ++;
			return item;
		}
		else if(rs == 0){
			item = SERIAL_MAP_ITEM_VALUE_AT(map,pIndex);
			mem_memcpy(item, value, map->value_bytes);
			return item;
		}
		else if(rs <0){
			_serial_map_data_extend(map, map->item_length * map->item_bytes + map->item_bytes,InvokeTickArg);
			for(i=map->item_length ;i>nIndex;i--){
				mem_memcpy(SERIAL_MAP_ITEM_AT(map,i), SERIAL_MAP_ITEM_AT(map,i-1), map->item_bytes);
			}
			item = SERIAL_MAP_ITEM_AT(map,nIndex);
			mem_memcpy(item, key, map->key_bytes);
			item += map->key_bytes;
			mem_memcpy(item, value, map->value_bytes);
			map->item_length ++;
			return item;
		}
		else{
			_serial_map_data_extend(map, map->item_length * map->item_bytes + map->item_bytes,InvokeTickArg);
			nIndex = pIndex +1;
			for(i=map->item_length ;i>nIndex;i--){
				mem_memcpy(SERIAL_MAP_ITEM_AT(map,i), SERIAL_MAP_ITEM_AT(map,i-1), map->item_bytes);
			}
			item = SERIAL_MAP_ITEM_AT(map,nIndex);
			mem_memcpy(item, key, map->key_bytes);
			item += map->key_bytes;
			mem_memcpy(item, value, map->value_bytes);
			map->item_length ++;
			return item;
		}
	}
	return NULL;
}

hbool serial_map_remove(hserial_map_t hMap,hany key,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map && key){
		hint32 pIndex,nIndex,i;
		hint32 rs = _serial_map_find(map, key, &pIndex, &nIndex,InvokeTickArg);
		if(rs == 0){
			for(i = pIndex;i<map->item_length-1;i++){
				mem_memcpy(SERIAL_MAP_ITEM_AT(map,i), SERIAL_MAP_ITEM_AT(map,i+1), map->item_bytes);
			}
			map->item_length --;
			return hbool_true;
		}
	}
	return hbool_false;	
}

hbool serial_map_clear(hserial_map_t hMap,InvokeTickDeclare){
	serial_map_t * map = (serial_map_t *)hMap;
	if(map ){
		map->item_length = 0;
		return hbool_true;
	}
	return hbool_false;
}

typedef struct{
	hint32 item_bytes;
	hint32 item_length;
	hint32 key_bytes;
	hint32 value_bytes;
}serial_map_bytes_t;

hint32 serial_map_load_bytes(hserial_map_t map,hbyte * bytes,hint32 length,InvokeTickDeclare){
    if(length >= sizeof(serial_map_bytes_t)){
        serial_map_t * m = (serial_map_t *) map;
        serial_map_bytes_t * b = (serial_map_bytes_t *) bytes;
        hbyte * item;
        if(b->item_bytes == m->item_bytes && b->value_bytes == m->value_bytes && b->key_bytes == m->key_bytes
           && b->item_length >=0 && length == b->item_length * b->item_bytes + sizeof(serial_map_bytes_t)){
            item = bytes + sizeof(serial_map_bytes_t);
            _serial_map_data_extend(m,b->item_length * b->item_length,InvokeTickArg);
            
            if(b->item_length >0){
                memcpy(m->data, item, b->item_length * b->item_length);
            }
            
            m->item_length = b->item_length;
            
            return b->item_length;
        }
    }
    return 0;
}

hint32 serial_map_save_bytes(hserial_map_t map,hbuffer_t buffer,InvokeTickDeclare){
    serial_map_t * m = (serial_map_t *) map;
    serial_map_bytes_t bytes ;
    
    mem_memset(&bytes, 0, sizeof(serial_map_bytes_t));
    
    bytes.item_bytes = m->item_bytes;
    bytes.item_length = m->item_length;
    bytes.key_bytes = m->key_bytes;
    bytes.value_bytes = m->value_bytes;
    
    buffer_append(buffer, &bytes, sizeof(serial_map_bytes_t));
    
    buffer_append(buffer, m->data, m->item_length * m->item_bytes);
    
    return sizeof(serial_map_bytes_t) + m->item_length * m->item_bytes;
}
