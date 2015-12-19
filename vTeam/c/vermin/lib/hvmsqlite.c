//
//  hvmsqlite.c
//  C Library
//
//  Created by Hailong Zhang on 11-11-8.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include <sqlite3.h>
#include "hconfig.h"
#include "hvmsqlite.h"
#include "hbuffer.h"
#include "hvermin_object.h"
#include "hvermin_runtime_variant.h"
#include "hlog.h"

typedef struct{
    vmObject base;
    sqlite3 * _sqlite;
    struct{
        vmUniqueKey exec;
        vmUniqueKey query;
        vmUniqueKey lastRowId;
        vmUniqueKey encode;
        vmUniqueKey sql;
    }uniqueKeys;
}vmSqlite;

/**
 
 Return : Void or Throwable
 */

static vmVariant vmSqliteClassInitCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmVariantList args,InvokeTickDeclare){
    vmVariant rs=  {vmVariantTypeVoid,0};
    vmSqlite * sqlite = (vmSqlite *) object;
    hbuffer_t file = buffer_alloc(128, 128);
    
    vmVariantToString(context, vmVariantListGet(args, 0), file);
    
    if(sqlite3_open(buffer_to_str(file), & sqlite->_sqlite)){
        
        rs = vmRuntimeContextException(context, sqlite3_errcode(sqlite->_sqlite), sqlite3_errmsg(sqlite->_sqlite));
        sqlite3_close(sqlite->_sqlite);
        
        sqlite->_sqlite = NULL;
    }
    
    buffer_dealloc(file);
    
    sqlite->uniqueKeys.exec = vmRuntimeContextGetUniqueKey(context, "exec");
    sqlite->uniqueKeys.query = vmRuntimeContextGetUniqueKey(context, "query");
    sqlite->uniqueKeys.lastRowId = vmRuntimeContextGetUniqueKey(context, "lastRowId");
    sqlite->uniqueKeys.encode = vmRuntimeContextGetUniqueKey(context, "encode");
    sqlite->uniqueKeys.sql = vmRuntimeContextGetUniqueKey(context, "sql");

    return rs;
}

/**
 
 Return : Void or Throwable
 */

static vmVariant vmSqliteClassDestroyCallback(vmRuntimeContext context, vmObject * object,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmSqlite * sqlite = (vmSqlite *) object;
    
    if(sqlite->_sqlite){
        sqlite3_close(sqlite->_sqlite);
        sqlite->_sqlite = NULL;
    }
    
    return rs;
}

/**
 
 Return : Void,Int16,Int32,Int64,Boolean,Double,Object or Throwable
 */
static vmVariant vmSqliteClassGetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    return rs;
}


/**
 
 Return : Void or Throwable
 */
static vmVariant vmSqliteClassSetPropertyCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariant value,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
   
    return rs;
}

/**
 
 Return : any
 */
static vmVariant vmSqliteClassInvokeCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKey name,vmVariantList args,InvokeTickDeclare){
    vmVariant rs = {vmVariantTypeVoid,0};
    vmSqlite * sqlite = (vmSqlite *) object;
    
    if(!sqlite->_sqlite){
        return vmRuntimeContextException(context, 0, "not open sqlite file");
    }
    
    if(sqlite->uniqueKeys.exec == name){
        {
            hbuffer_t sql = buffer_alloc(128, 128);
            hchar * error = NULL;
            vmVariantToString(context, vmVariantListGet(args, 0), sql);
            
            sqlite3_exec(sqlite->_sqlite, buffer_to_str(sql), NULL, NULL, &error);
            
            if(error){
                rs = vmRuntimeContextException(context, sqlite3_errcode(sqlite->_sqlite), error);
            }
            
            buffer_dealloc(sql);
        }
    }
    else if(sqlite->uniqueKeys.query == name){
        {
            hbuffer_t sql = buffer_alloc(128, 128);
            hchar * error = NULL;
            hchar ** dbResult = NULL;
            hint32 nRow = 0;
            hint32 nColumn = 0;
            hint32 i,j;
            vmVariant row;
            vmVariant value;
            vmVariantToString(context, vmVariantListGet(args, 0), sql);
            
            sqlite3_get_table(sqlite->_sqlite, buffer_to_str(sql), &dbResult, &nRow, &nColumn, &error);

            
            if(error){
                sqlite3_free_table(dbResult);
                rs = vmRuntimeContextException(context, sqlite3_errcode(sqlite->_sqlite), error);
            }
            else{
                rs = vmArrayAlloc(context);
                for(i=0;i<nRow;i++){
                    row = vmDictionaryAlloc(context);
                    for(j=0;j<nColumn;j++){
                        value = vmStringAlloc(context, dbResult[nColumn + i * nColumn + j]);
                        vmDictionaryPut(context, row.value.objectValue, dbResult[j], value);
                    }
                    vmArrayAdd(context, rs.value.objectValue, row);
                }
                sqlite3_free_table(dbResult);
            }
            
            buffer_dealloc(sql);
        }
    }
    else if(sqlite->uniqueKeys.lastRowId == name){
        rs.type = vmVariantTypeInt64;
        rs.value.int64Value = sqlite3_last_insert_rowid(sqlite->_sqlite);
    }
    else if(sqlite->uniqueKeys.encode == name){
        {
            hbuffer_t str = buffer_alloc(128, 128);
            hbuffer_t enc = buffer_alloc(128, 128);
            vmVariant  var = vmVariantListGet(args, 0);
            hchar * p ;
            
            if(var.type != vmVariantTypeVoid){
                vmVariantToString(context, var, str);
                
                buffer_append_str(enc, "'");
                
                p = (hchar *)buffer_to_str(str);
                
                while(*p != 0){
                    
                    if( *p == '\''){
                        buffer_append_str(enc, "''");
                    }
                    else if( *p == '\t'){
                        buffer_append_str(enc, "\\t");
                    }
                    else if( *p == '\r'){
                        buffer_append_str(enc, "\\r");
                    }
                    else if( *p == '\n'){
                        buffer_append_str(enc, "\\n");
                    }
                    else{
                        buffer_append(enc,p,1);
                    }
                    
                    p ++;
                }
                
                buffer_append_str(enc, "'");
            }
            else{
                buffer_append_str(enc, "NULL");
            }
            
            rs =vmStringAlloc(context, buffer_to_str(enc));
            
            buffer_dealloc(str);
            buffer_dealloc(enc);
        }
    }
    else if(sqlite->uniqueKeys.sql == name){
        {
            hbuffer_t str = buffer_alloc(128, 128);
            hbuffer_t enc = buffer_alloc(128, 128);
            hbuffer_t key = buffer_alloc(64, 64);
            hbuffer_t value = buffer_alloc(128, 128);
            vmVariant var = vmVariantListGet(args, 0);
            vmVariant data = vmVariantListGet(args, 1);
            vmVariant v;
            hchar * p , *pp;
            hint32 s = 0;
            
            vmVariantToString(context, var, str);
            
            p = (hchar *)buffer_to_str(str);
            
            while(p){
                
                switch (s) {
                    case 0:
                    {
                        if(*p=='@'){
                            buffer_clear(key);
                            s = 1;
                        }
                        else{
                            buffer_append(enc,p,1);
                        }
                    }
                        break;
                    case 1:
                    {
                        if( ( *p >= 'A' && *p <= 'Z' ) || ( *p >= 'a' && *p <= 'z' ) || ( *p >= '0' && *p <= '9' ) || ( *p == '_' )){
                            buffer_append(key,p,1);
                        }
                        else{
                            if((data.type & vmVariantTypeObject) && data.value.objectValue){
                                v = vmObjectGetProperty(context, data.value.objectValue->vmClass, data.value.objectValue, vmRuntimeContextGetUniqueKey(context, buffer_to_str(key)));
                                if(v.type != vmVariantTypeVoid){
                                    buffer_clear(value);
                                    vmVariantToString(context, v, value);
                                    
                                    pp = (hchar *)buffer_to_str(value);
                                  
                                    buffer_append_str(enc, "'");
                                  
                                    while(*pp != 0){
                                        
                                        if( *pp == '\''){
                                            buffer_append_str(enc, "''");
                                        }
                                        else if( *pp == '\t'){
                                            buffer_append_str(enc, "\\t");
                                        }
                                        else if( *pp == '\r'){
                                            buffer_append_str(enc, "\\r");
                                        }
                                        else if( *pp == '\n'){
                                            buffer_append_str(enc, "\\n");
                                        }
                                        else{
                                            buffer_append(enc,pp,1);
                                        }
                                    
                                        pp ++;
                                    }
                                    
                                    buffer_append_str(enc, "'");
                                   
                                }
                                else{
                                    buffer_append_str(enc, "NULL");
                                }

                            }
                            else{
                                buffer_append_str(enc, "NULL");
                            }
                            buffer_append(enc,p,1);
                            s = 0;
                        }
                    }
                        break;
                    default:
                        break;
                }
                
                if(*p == '\0'){
                    break;
                }
                p ++;
            }
            
            
            rs = vmStringAlloc(context, buffer_to_str(enc));
            
            buffer_dealloc(value);
            buffer_dealloc(str);
            buffer_dealloc(enc);
            buffer_dealloc(key);
        }
    }
    
    return rs;
}

/**
 
 Return : Void
 */
static void vmSqliteClassPropertyNamesCallback(vmRuntimeContext context,vmClass * clazz,vmObject * object,vmUniqueKeyList names,InvokeTickDeclare){

}


vmClassBase vmSqliteClass = {vmClassTypeBase,sizeof(vmSqlite),vmSqliteClassInitCallback,vmSqliteClassDestroyCallback,vmSqliteClassGetPropertyCallback,vmSqliteClassSetPropertyCallback,vmSqliteClassPropertyNamesCallback,vmSqliteClassInvokeCallback};

