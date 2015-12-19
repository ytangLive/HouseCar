//
//  hfdb.h
//  vTeam
//
//  Created by Zhang Hailong on 13-6-10.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vTeam_hfdb_h
#define vTeam_hfdb_h


#ifdef __cplusplus
extern "C" {
#endif
    
#define FDB_NAME_MAX        64
#define FDB_OK              0
#define FDB_ERROR           -1
#define FDB_VERSION         0x100000
#define FDB_FILE_DB         ".db"
#define FDB_FILE_BOLB       ".bob"
#define FDB_FILE_INDEX      ".idx"
#define FDB_TAG             "FDB"
#define FDB_BLOB_BUF_SIZE   102400
    
    enum {
        FDBPropertyTypeNone,
        FDBPropertyTypeInt32,   // hint32
        FDBPropertyTypeInt64,   // hint64
        FDBPropertyTypeDouble,  // hdouble
        FDBPropertyTypeString,  // 定长 UTF8String
        FDBPropertyTypeBlob,     // 变长 bytes , 不可作为索引
        FDBPropertyTypeMaxCount
    };
    
    typedef huint32 FDBPropertyType;

    typedef hint64 FDBBlobValue;
    
    typedef struct _FDBProperty{
        hchar name[FDB_NAME_MAX];
        FDBPropertyType type;
        huint32 length;
        huint32 offset;
    } FDBProperty;
    
    typedef struct _FDBClass {
        huint32 itemSize;
        huint32 propertyCount;
        hint32 lastRowid;
        FDBProperty rowid;
    } FDBClass;
    
#define FDBClassPropertyCount(t)      ((sizeof(t) - sizeof(FDBClass)) / sizeof(FDBProperty) + 1) 
    
    typedef void  * FDBDataItem;
    
    typedef struct _FDBData {
        FDBClass * dbClass;
        huint32 length;
        void * data;
    } FDBData;
    
    void FDBClassInitialize(FDBClass * const dbClass);
    
    void FDBDataCreate(FDBData * const data,FDBClass * dbClass);
    
    void FDBDataDelete(FDBData * const data);
    
    void FDBDataSetLength(FDBData * const data,huint32 length);
    
    void FDBDataClear(FDBData * const data);
    
    FDBDataItem FDBDataItemAt(FDBData * const data,huint32 index);
    
    huint32 FDBClassSize(FDBClass * const dbClass);
    
    FDBProperty * FDBClassGetProperty(FDBClass * const dbClass,hcchar * name);
    
    void FDBClassSetPropertyInt32Value(FDBDataItem dataItem, FDBProperty * prop,hint32 value);
    
    hint32 FDBClassGetPropertyInt32Value(FDBDataItem dataItem, FDBProperty * prop,hint32 defaultValue);
    
    void FDBClassSetPropertyInt64Value(FDBDataItem dataItem, FDBProperty * prop,hint64 value);
    
    hint64 FDBClassGetPropertyInt64Value(FDBDataItem dataItem, FDBProperty * prop,hint64 defaultValue);
    
    void FDBClassSetPropertyDoubleValue(FDBDataItem dataItem, FDBProperty * prop,hdouble value);
    
    hdouble FDBClassGetPropertyDoubleValue(FDBDataItem dataItem, FDBProperty * prop,hdouble defaultValue);
    
    void FDBClassSetPropertyStringValue(FDBDataItem dataItem, FDBProperty * prop,hcchar * value);
    
    hcchar * FDBClassGetPropertyStringValue(FDBDataItem dataItem, FDBProperty * prop,hcchar * defaultValue);
    
    
    void FDBClassSetPropertyBlobValue(FDBDataItem dataItem,FDBProperty * prop,FDBBlobValue value);
    
    FDBBlobValue FDBClassGetPropertyBlobValue(FDBDataItem dataItem,FDBProperty * prop,FDBBlobValue defaultValue);
    
    
    typedef struct _FDB{
        huint32 version;
        FDBClass * dbClass;
        hint32 rowCount;
    } FDB;
    
    FDB * FDBCreate(hcchar * dbPath,FDBClass * dbClass,hbool isCopyDBClass);
    
    FDB * FDBOpen(hcchar * dbPath);
    
    huint32 FDBRowCount(FDB * fdb);
    
    FDBBlobValue FDBBlobCreate(FDB * fdb,void * data,huint32 length);
    
#define FDBBlobCreateString(fdb,v)  FDBBlobCreate((fdb),(hchar *)(v),(huint32)(strlen(v) + 1))
    
    void * FDBBlobRead(FDB * fdb,FDBBlobValue value,huint32 * length);
    
    hint32 FDBInsertData(FDB * fdb,FDBData * data,huint32 offset,huint32 length);
    
    hint32 FDBInsert(FDB * fdb,FDBDataItem dataItem);
    
    void FDBClose(FDB * fdb);
    
    struct _FDBCursor;
    
    typedef hbool (* FDBCursorFilter) (FDB * fdb,struct _FDBCursor * cursor,FDBDataItem dataItem);
    
    typedef struct _FDBCursor {
        FDBData data;
        huint32 location;
        huint32 index;
        huint32 length;
        FDBCursorFilter filter;
    } FDBCursor;
    
    
    FDBDataItem FDBCursorNext(FDB * fdb,FDBCursor * cursor);
    
    FDBDataItem FDBCursorToRowid(FDB * fdb,FDBCursor * cursor,hint32 rowid);
    
    
    hint32 FDBCursorCommit(FDB * fdb,FDBCursor * cursor);
    
#ifdef __cplusplus
}
#endif


#endif
