//
//  hfdb_cfg.c
//  fdbgen
//
//  Created by Zhang Hailong on 13-6-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hfdb.h"
#include "hfdb_index.h"
#include "hdata.h"
#include "hfdb_cfg.h"
#include "hmap.h"
#include "hlist.h"

typedef struct {
    FDBConfig base;
    hmap_t dbClassMap;
    hmap_t indexMap;
    hlist_t dbClasss;
    hlist_t indexs;
} FDBConfigInternal;

typedef struct {
    hchar name[FDB_NAME_MAX];
    FDBClass * dbClass;
} FDBClassConfig;

typedef struct {
    hchar name[FDB_NAME_MAX];
    FDBIndex * index;
    FDBIndexSortProperty * sortPropertys;
    huint32 sortPropertysCount;
} FDBIndexConfig;

FDBConfig * FDBConfigCreate(hext_data_t data){
    
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) malloc(sizeof(FDBConfigInternal));
    
    memset(cfg, 0, sizeof(FDBConfigInternal));
    
    cfg->dbClasss = list_alloc(32, 32);
    cfg->indexs = list_alloc(32, 32);
    cfg->dbClassMap = map_alloc(hash_code_str, equal_str);
    cfg->indexMap = map_alloc(hash_code_str, equal_str);
    
    {
        hdata_t items = ext_data_object(data, "class");
        hint32 c = hdata_array_size(ext_data_class(data), items);
        hint32 i,size,pc,pi;
        hdata_t item,propertys,property;
        FDBClassConfig * config;
        FDBProperty * prop;
        hcchar * type;
    
        for(i=0;i<c;i++){
            
            config = malloc(sizeof(FDBClassConfig));
            
            memset(config, 0, sizeof(FDBClassConfig));
            
            item = hdata_array(ext_data_class(data), items, i);
            propertys = hdata_object(ext_data_class(data), item, "propertys");
          
            strncpy(config->name, hdata_object_string(ext_data_class(data), item, "name", ""), sizeof(config->name));
            
            size = sizeof(FDBClass) + hdata_array_size(ext_data_class(data), propertys) * sizeof(FDBProperty);
            
            config->dbClass = (FDBClass *) malloc(size);
            
            memset(config->dbClass, 0, size);
            
            config->dbClass->propertyCount = hdata_array_size(ext_data_class(data), propertys) + 1;
            
            pc = config->dbClass->propertyCount;
            prop = & config->dbClass->rowid;
            
            strcpy(prop->name, "rowid");
            prop->type = FDBPropertyTypeInt32;
            
            prop ++;
            pc --;
            pi = 0;
            
            while(pc >0){
                
                property = hdata_array(ext_data_class(data), propertys, pi);
                
                strncpy(prop->name, hdata_object_string(ext_data_class(data), property, "name", ""), sizeof(prop->name));
                
                type = hdata_object_string(ext_data_class(data), property, "type", "string");
                
                if(strcmp(type, "int32") ==0){
                    prop->type = FDBPropertyTypeInt32;
                }
                else if(strcmp(type, "int64") ==0){
                    prop->type = FDBPropertyTypeInt64;
                }
                else if(strcmp(type, "double") ==0){
                    prop->type = FDBPropertyTypeDouble;
                }
                else if(strcmp(type, "blob") ==0){
                    prop->type = FDBPropertyTypeBlob;
                }
                else {
                    prop->type = FDBPropertyTypeString;
                    prop->length = hdata_object_int32(ext_data_class(data), property, "length", 64);
                }
                
                pc --;
                prop ++;
                pi ++;
            }
            
            FDBClassInitialize(config->dbClass);
            
            list_add(cfg->dbClasss, config);
            map_put(cfg->dbClassMap, config->name, config);
        }
    }
    
    {
        hdata_t items = ext_data_object(data, "index");
        hint32 c = hdata_array_size(ext_data_class(data), items);
        hint32 i,size,pc,pi;
        hdata_t item,propertys,property;
        FDBIndexConfig * config;
        FDBProperty * prop;
        FDBIndexSortProperty * sortProperty;
        hcchar * type;
        
        for(i=0;i<c;i++){
            
            config = malloc(sizeof(FDBIndexConfig));
            
            memset(config, 0, sizeof(FDBIndexConfig));
            
            item = hdata_array(ext_data_class(data), items, i);
            propertys = hdata_object(ext_data_class(data), item, "propertys");
            
            strncpy(config->name, hdata_object_string(ext_data_class(data), item, "name", ""), sizeof(config->name));
            
            size = sizeof(FDBIndex) + hdata_array_size(ext_data_class(data), propertys) * sizeof(FDBProperty);
            
            config->index = (FDBIndex *) malloc(size);
            
            memset(config->index, 0, size);
            
            config->index->propertyCount = hdata_array_size(ext_data_class(data), propertys) + 1;
            
            pc = config->index->propertyCount;
            prop = & config->index->rowid;
            
            strcpy(prop->name, "rowid");
            prop->type = FDBPropertyTypeInt32;
            
            prop ++;
            pc --;
            pi = 0;
            
            while(pc >0){
                
                property = hdata_array(ext_data_class(data), propertys, pi);
                
                strncpy(prop->name, hdata_object_string(ext_data_class(data), property, "name", ""), sizeof(prop->name));
                
                type = hdata_object_string(ext_data_class(data), property, "type", "string");
                
                if(strcmp(type, "int32") ==0){
                    prop->type = FDBPropertyTypeInt32;
                }
                else if(strcmp(type, "int64") ==0){
                    prop->type = FDBPropertyTypeInt64;
                }
                else if(strcmp(type, "double") ==0){
                    prop->type = FDBPropertyTypeDouble;
                }
                else {
                    prop->type = FDBPropertyTypeString;
                    prop->length = hdata_object_int32(ext_data_class(data), property, "length", 64);
                }
                
                pc --;
                prop ++;
                pi ++;
            }
            
            FDBIndexInitialize(config->index);
            
            
            propertys = hdata_object(ext_data_class(data), item, "sort");
            
            config->sortPropertysCount = hdata_array_size(ext_data_class(data), propertys);
            
            if(config->sortPropertysCount >0){
                config->sortPropertys = malloc(sizeof(FDBIndexSortProperty) * config->sortPropertysCount);
                memset(config->sortPropertys , 0, sizeof(FDBIndexSortProperty) * config->sortPropertysCount);
                sortProperty = config->sortPropertys;
                for(pi = 0;pi <config->sortPropertysCount;pi ++){
                    property = hdata_array(ext_data_class(data), propertys, pi);
                    sortProperty[pi].property = FDBIndexGetProperty(config->index,hdata_object_string(ext_data_class(data), property, "name", ""));
                    type = hdata_object_string(ext_data_class(data), property, "mode", "asc");
                    if(strcmp(type, "desc") ==0){
                        sortProperty[pi].mode = FDBIndexCompareOrderDesc;
                    }
                    else{
                        sortProperty[pi].mode = FDBIndexCompareOrderAsc;
                    }
                }
            }
            
            list_add(cfg->indexs, config);
            map_put(cfg->indexMap, config->name,config);
        }
    }
    
    cfg->base.classCount = list_count(cfg->dbClasss);
    cfg->base.indexCount = list_count(cfg->indexs);
    
    return (FDBConfig *)cfg;
}

FDBClass * FDBConfigClassAt(FDBConfig * config,huint32 index){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBClassConfig * c = (FDBClassConfig *) list_get(cfg->dbClasss, index);
    return c ? c->dbClass : NULL;
}

hcchar * FDBConfigClassNameAt(FDBConfig * config,huint32 index){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBClassConfig * c = (FDBClassConfig *) list_get(cfg->dbClasss, index);
    return c ? c->name : NULL;
}


FDBClass * FDBConfigFindClass(FDBConfig * config,hcchar * name){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBClassConfig * c = (FDBClassConfig *)  map_get(cfg->dbClassMap, (hany)name);
    return  c ? c->dbClass : NULL;
}

FDBIndex * FDBConfigIndexAt(FDBConfig * config,huint32 index){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBIndexConfig * c = (FDBIndexConfig *) list_get(cfg->indexs, index);
    return c ? c->index : NULL;
}

hcchar * FDBConfigIndexNameAt(FDBConfig * config,huint32 index){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBIndexConfig * c = (FDBIndexConfig *) list_get(cfg->indexs, index);
    return c ? c->name : NULL;
}

FDBIndexSortProperty * FDBConfigIndexSortPropertysAt(FDBConfig * config,huint32 index,huint32 * length){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBIndexConfig * c = (FDBIndexConfig *) list_get(cfg->indexs, index);
    if(c){
        if(length){
            * length = c->sortPropertysCount;
        }
        return c->sortPropertys;
    }
    return NULL;
}

FDBIndex * FDBConfigFindIndex(FDBConfig * config,hcchar * name){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBIndexConfig * icfg = (FDBIndexConfig *) map_get(cfg->indexMap, (hany)name);
    return icfg ? icfg->index : NULL;
}

FDBIndexSortProperty * FDBConfigFindIndexSortPropertys(FDBConfig * config,hcchar * name,huint32 * length){
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBIndexConfig * icfg = (FDBIndexConfig *) map_get(cfg->indexMap, (hany)name);
    if(icfg){
        if(length){
            *length = icfg->sortPropertysCount;
        }
        return icfg->sortPropertys;
    }
    return NULL;
}

void FDBConfigDelete(FDBConfig * config){
    hint32 i;
    InvokeTickBegin
    FDBConfigInternal * cfg = (FDBConfigInternal *) config;
    FDBClassConfig * cls;
    FDBIndexConfig * index;
    
    for(i=0;i< cfg->base.classCount;i++){
        cls = (FDBClassConfig *) list_get(cfg->dbClasss, i);
        free(cls->dbClass);
        free(cls);
    }
    
    list_dealloc(cfg->dbClasss);
    
    
    for(i=0;i< cfg->base.indexCount;i++){
        index = (FDBIndexConfig *) list_get(cfg->indexs, i);
        if(index->sortPropertys){
            free(index->sortPropertys);
        }
        free(index->index);
        free(index);
    }
    
    list_dealloc(cfg->indexs);
    
    map_dealloc(cfg->dbClassMap);
    map_dealloc(cfg->indexMap);
    
    free(cfg);
}
