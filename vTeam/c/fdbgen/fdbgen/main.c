//
//  main.c
//  fdbgen
//
//  Created by Zhang Hailong on 13-6-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hfdb.h"
#include "hfdb_index.h"
#include "hdata_json.h"
#include "hfdb_cfg.h"


#include <sqlite3.h>

static void print_using(){
    printf("\nusing:\n");
    printf("\tfdbgen db -config cfg.json -insqlite ./system.db -insql \"SELECT * FROM Drug ORDER BY [ID]\" -class Drug -out ./drug \n");
    printf("\tfdbgen index -config cfg.json -indb ./drug -index ShowCatalogID \n");
}

static void command_db(int argc,const char * argv[],FDBConfig * cfg){
    
    const char * insqlite = NULL;
    const char * insql = NULL;
    const char * inclass = NULL;
    const char * out = NULL;
    
    int i;
    
    for (i=2; i <argc; i ++) {
        if(strcmp(argv[i], "-insqlite") ==0 && i + 1 < argc){
            insqlite = argv[++i];
        }
        else if(strcmp(argv[i], "-insql") ==0 && i + 1 < argc){
            insql = argv[++i];
        }
        else if(strcmp(argv[i], "-class") ==0 && i + 1 < argc){
            inclass = argv[++i];
        }
        else if(strcmp(argv[i], "-out") ==0 && i + 1 < argc){
            out = argv[++i];
        }
    }
    
    if(!insqlite){
        printf("\nnot found -insqlite\n");
        return;
    }
    
    if(!insql){
        printf("\nnot found -insql\n");
        return;
    }
    
    if(!inclass){
        printf("\nnot found -class\n");
        return;
    }
    
    if(!out){
        printf("\nnot found -out\n");
        return;
    }
    
    
    FDBClass * dbClass = FDBConfigFindClass(cfg, inclass);
    
    if(!dbClass){
        printf("\nnot found class %s\n",inclass);
        return;
    }
    
    FDB * fdb = FDBCreate(out, dbClass, hbool_false);
    
    if(!fdb){
        printf("\nnot create fdb %s %s\n",inclass,out);
        return;
    }
    
    sqlite3 * db;
    
    if(SQLITE_OK != sqlite3_open(insqlite, &db)){
        printf("\nnot open sqlite3 %s\n",insqlite);
        FDBClose(fdb);
        return;
    }
    
    sqlite3_stmt * stmt;
    
    if(SQLITE_OK != sqlite3_prepare(db, insql, -1, &stmt, NULL)){
        printf("\nsqlite3 : %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        FDBClose(fdb);
        return;
    }
    
    FDBData data;
    
    FDBDataCreate(&data, fdb->dbClass);
    
    FDBDataSetLength(&data, 20000);
    
    FDBDataClear(&data);
    
    FDBDataItem dataItem;
    
    int length = 0;
    int c;
    FDBProperty * prop;
    
    printf("\ndb %s gen ...\n",out);
    
    while(SQLITE_ROW == sqlite3_step(stmt)){
        
        dataItem = FDBDataItemAt(& data, length);
        
        c = sqlite3_column_count(stmt);
        
        for(i = 0;i<c;i++){
            
            prop = FDBClassGetProperty(fdb->dbClass, sqlite3_column_name(stmt, i));
            
            if(prop){
                switch (prop->type) {
                    case FDBPropertyTypeInt32:
                        FDBClassSetPropertyInt32Value(dataItem, prop, sqlite3_column_int(stmt, i));
                        break;
                    case FDBPropertyTypeInt64:
                        FDBClassSetPropertyInt64Value(dataItem, prop, sqlite3_column_int64(stmt, i));
                        break;
                    case FDBPropertyTypeDouble:
                        FDBClassSetPropertyDoubleValue(dataItem, prop, sqlite3_column_double(stmt, i));
                        break;
                    case FDBPropertyTypeString:
                        FDBClassSetPropertyStringValue(dataItem, prop, (hcchar *)sqlite3_column_text(stmt, i));
                        break;
                    case FDBPropertyTypeBlob:
                    {
                        hcchar * cString = (hcchar *)sqlite3_column_text(stmt, i);
                        if(cString){
                            FDBClassSetPropertyBlobValue(dataItem, prop, FDBBlobCreateString(fdb, cString));
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
            
        }
                
        if(++ length == data.length){
            
            FDBInsertData(fdb, &data, 0, length);
            length = 0;
            FDBDataClear(&data);
        }
        
    }
    
    if(length){
        FDBInsertData(fdb, &data, 0, length);
    }
    
    FDBDataDelete(& data);
    
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    
    FDBClose(fdb);
    
    printf("\ndb to %s\n",out);
}

static void command_index(int argc,const char * argv[],FDBConfig * cfg){
    
    const char * indb = NULL;
    const char * inindex = NULL;
    
    int i;
    
    for (i=2; i <argc; i ++) {
        if(strcmp(argv[i], "-indb") ==0 && i + 1 < argc){
            indb = argv[++i];
        }
        else if(strcmp(argv[i], "-index") ==0 && i + 1 < argc){
            inindex = argv[++i];
        }
    }
    
    if(!indb){
        printf("\nnot found -indb\n");
        return;
    }
    
    if(!inindex){
        printf("\nnot found -index\n");
        return;
    }
    
    
    FDB * fdb = FDBOpen(indb);
    
    if(!fdb){
        printf("\nnot open fdb %s\n",indb);
        return;
    }
    
    FDBIndex * index = FDBConfigFindIndex(cfg, inindex);
    
    if(!index){
        printf("\nnot found index %s\n",inindex);
        return;
    }
    
    FDBCursor cursor = {0};
    
    FDBDataCreate(&cursor.data, fdb->dbClass);
    
    FDBDataSetLength(&cursor.data, 20000);
    
    FDBDataItem dataItem;
    
    FDBIndexData data;
    
    FDBIndexDataCreate(& data, index);
    
    FDBIndexDataClear(& data);
    
    int c = index->propertyCount;
    
    FDBProperty * prop, *p;
    
    while((dataItem = FDBCursorNext(fdb, &cursor))){
        
        FDBDataItem indexItem = FDBIndexDataInsert(& data);
        
        c = index->propertyCount;
        prop = & index->rowid;
        
        while(c >0){
            
            p = FDBClassGetProperty(fdb->dbClass, prop->name);
            
            if(p){
                switch (prop->type) {
                    case FDBPropertyTypeInt32:
                        FDBClassSetPropertyInt32Value(indexItem, prop, FDBClassGetPropertyInt32Value(dataItem, p, 0));
                        break;
                    case FDBPropertyTypeInt64:
                        FDBClassSetPropertyInt64Value(indexItem, prop, FDBClassGetPropertyInt64Value(dataItem, p, 0));
                        break;
                    case FDBPropertyTypeDouble:
                        FDBClassSetPropertyDoubleValue(indexItem, prop, FDBClassGetPropertyDoubleValue(dataItem, p, 0));
                        break;
                    case FDBPropertyTypeString:
                        FDBClassSetPropertyStringValue(indexItem, prop, FDBClassGetPropertyStringValue(dataItem, p, NULL));
                        break;
                    default:
                        break;
                }
            }
            
            c --;
            prop ++;
        }
        
    }
    
    huint32 length = 0;
    
    FDBIndexSortProperty * sortPropertys = FDBConfigFindIndexSortPropertys(cfg, inindex, & length);
    
    if(sortPropertys){
        FDBIndexSortPropertys(& data, sortPropertys, length);
    }
    
    FDBIndexWrite(indb, inindex, & data);
    
    FDBIndexDataDelete(& data);
    
    FDBDataDelete(&cursor.data);
    
    FDBClose(fdb);
    
    
    printf("\nindex %s OK\n",inindex);
}

int main(int argc, const char * argv[])
{
    
    const char * command = NULL;
    const char * config = NULL;
    
    int i;
    
    if(argc >1){
        command = argv[1];
    }
    
    for (i=2; i <argc; i ++) {
        if(strcmp(argv[i], "-config") ==0 && i + 1 < argc){
            config = argv[++i];
        }
    }
    
    if(!command || !config){
        print_using();
        exit(0);
    }
    
    InvokeTickBegin
    
    hbuffer_t json = buffer_alloc(10240, 1024);
    char sbuf[10240];
    FILE * f = fopen(config, "rb");
    int len;
    
    if(f){
        while ((len = fread(sbuf, 1, sizeof(sbuf), f))) {
            buffer_append(json, sbuf, len);
        }
        fclose(f);
    }
    
    hext_data_t data = ext_data_json_alloc(buffer_to_str(json));
    
    buffer_dealloc(json);
    
    FDBConfig * cfg = FDBConfigCreate(data);
    
    if(strcmp(command, "db") ==0){
        
        {
            command_db(argc,argv,cfg);
        }
        
    }
    else if(strcmp(command, "index") ==0){
        
        {
            command_index(argc,argv,cfg);
        }
        
    }
    else{
        
        print_using();
    }
    
    FDBConfigDelete(cfg);
    ext_data_release(data);
    
    return 0;
}

