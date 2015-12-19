//
//  hvermin.c
//  C Library
//
//  Created by hailong zhang on 11-6-3.
//  Copyright 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin.h"
#include "hserial_list.h"
#include "hstr.h"

#undef vmUniqueKeyListAlloc

#undef vmUniqueKeyListDealloc

#undef vmUniqueKeyListCount

#undef vmUniqueKeyListGet

#undef vmUniqueKeyListAdd

#undef vmUniqueKeyListRemoveAt

#undef vmUniqueKeyListRemove

#undef vmUniqueKeyListClear





#undef vmVariantListAlloc

#undef vmVariantListDealloc

#undef vmVariantListCount

#undef vmVariantListGet

#undef vmVariantListSet

#undef vmVariantListAdd

#undef vmVariantListRemoveAt

#undef vmVariantListClear

#undef vmVariantListInsert

#undef vmVariantListLast

#undef vmVariantListRemoveLast

vmUniqueKeyList vmUniqueKeyListAlloc(InvokeTickDeclare){
    return (vmUniqueKeyList)serial_list_alloc( sizeof(vmUniqueKey),0);
}

void vmUniqueKeyListDealloc(vmUniqueKeyList list,InvokeTickDeclare){
    serial_list_dealloc((hserial_list_t)list);
}

hint32 vmUniqueKeyListCount(vmUniqueKeyList list,InvokeTickDeclare){
    return serial_list_count((hserial_list_t)list);
}

vmUniqueKey vmUniqueKeyListGet(vmUniqueKeyList list,hint32 index,InvokeTickDeclare){
    return * (vmUniqueKey *)serial_list_item_at((hserial_list_t)list, index);
}

void vmUniqueKeyListAdd(vmUniqueKeyList list,vmUniqueKey key,InvokeTickDeclare){
    hint32 i ,c = serial_list_count((hserial_list_t)list);
    for(i=0;i<c;i++){
        if(vmUniqueKeyListGet(list,i,InvokeTickArg) == key){
            break;
        }
    }
    if(i >= c){
        serial_list_item_add((hserial_list_t)list, &key);
    }
}

void vmUniqueKeyListRemoveAt(vmUniqueKeyList list,hint32 index,InvokeTickDeclare){
    serial_list_remove_at((hserial_list_t)list, index);
}

void vmUniqueKeyListRemove(vmUniqueKeyList list,vmUniqueKey key,InvokeTickDeclare){
    hint32 i ,c = serial_list_count((hserial_list_t)list);
    for(i=0;i<c;i++){
        if(vmUniqueKeyListGet(list,i,InvokeTickArg) == key){
            vmUniqueKeyListRemoveAt(list,i,InvokeTickArg);
            break;
        }
    }
}

void vmUniqueKeyListClear(vmUniqueKeyList list,InvokeTickDeclare){
    serial_list_clear((hserial_list_t)list);
}

vmVariantList vmVariantListAlloc(InvokeTickDeclare){
    return (vmVariantList)serial_list_alloc( sizeof(vmVariant), 0);
}

void vmVariantListDealloc(vmVariantList list,InvokeTickDeclare){
    serial_list_dealloc((hserial_list_t)list);
}

hint32 vmVariantListCount(vmVariantList list,InvokeTickDeclare){
    return serial_list_count((hserial_list_t) list);
}

vmVariant vmVariantListGet(vmVariantList list,hint32 index,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmVariant * value = serial_list_item_at((hserial_list_t)list, index);
    if(value){
        rs = * value;
    }
    return rs;
}

void vmVariantListSet(vmVariantList list,hint32 index,vmVariant value,InvokeTickDeclare){
    vmVariant * v = serial_list_item_at((hserial_list_t)list, index);
    if(v){
        * v = value;
    }
}

void vmVariantListAdd(vmVariantList list,vmVariant value,InvokeTickDeclare){
    serial_list_item_add((hserial_list_t)list, &value);
}

void vmVariantListRemoveAt(vmVariantList list,hint32 index,InvokeTickDeclare){
    serial_list_remove_at((hserial_list_t)list, index);
}

void vmVariantListClear(vmVariantList list,InvokeTickDeclare){
    serial_list_clear((hserial_list_t)list);
}

void vmVariantListInsert(vmVariantList list,hint32 index,vmVariant value,InvokeTickDeclare){
    serial_list_item_insert((hserial_list_t)list, &value, index);
}

vmVariant vmVariantListLast(vmVariantList list ,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmVariant * value = serial_list_item_last((hserial_list_t)list);
    if(value){
        rs = * value;
    }
    return rs;
}

void vmVariantListRemoveLast(vmVariantList list,InvokeTickDeclare){
    serial_list_remove_last((hserial_list_t)list);
}


