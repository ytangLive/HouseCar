//
//  hfdb_index.h
//  vTeam
//
//  Created by zhang hailong on 13-6-14.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef vTeam_hfdb_index_h
#define vTeam_hfdb_index_h

#ifdef __cplusplus
extern "C" {
#endif
    
#define FDB_INDEX_VERSION         0x100000
#define FDB_INDEX_EXPAND_SIZE       10240
#define FDB_INDEX_TAG                "IDX"
    
    typedef struct _FDBIndex {
        huint32 itemSize;
        huint32 propertyCount;
        FDBProperty rowid;
    } FDBIndex;
    
#define FDBIndexPropertyCount(t)      ((sizeof(t) - sizeof(FDBIndex)) / sizeof(FDBProperty) + 1)
    
    void FDBIndexInitialize(FDBIndex * const index);
    
    huint32 FDBIndexSize(FDBIndex * const index);
    
    FDBProperty * FDBIndexGetProperty(FDBIndex * const index,hcchar * name);
    
    typedef struct _FDBIndexData {
        FDBIndex * index;
        huint32 length;
        huint32 size;
        void * data;
    } FDBIndexData;
    
    typedef enum FDBIndexCompareOrder {
        FDBIndexCompareOrderSame = 0,FDBIndexCompareOrderDesc = 1,FDBIndexCompareOrderAsc = -1
    } FDBIndexCompareOrder;
    
    typedef FDBIndexCompareOrder (* FDBIndexDataItemCompare) (FDBIndex * index, FDBDataItem dataItem1,FDBDataItem dataItem2,hany context);
    
    void FDBIndexDataCreate(FDBIndexData * const data,FDBIndex * index);
    
    void FDBIndexDataDelete(FDBIndexData * const data);
    
    huint32 FDBIndexDataExpandSize(FDBIndexData * const data,huint32 size);
    
    void FDBIndexDataClear(FDBIndexData * const data);

    FDBDataItem FDBIndexDataAt(FDBIndexData * const data,huint32 index);
    
    FDBDataItem FDBIndexDataInsert(FDBIndexData * const data);
    
    huint32 FDBIndexSort(FDBIndexData * const data,FDBIndexDataItemCompare compare,hany context);
    
    typedef struct _FDBIndexSortProperty {
        FDBProperty * property;
        FDBIndexCompareOrder mode;
    } FDBIndexSortProperty;
    
    huint32 FDBIndexSortPropertys(FDBIndexData * const data, FDBIndexSortProperty * propertys,huint32 length);
    
    typedef struct _FDBIndexDB {
        huint32 version;
        FDBIndex * index;
        hint32 rowCount;
    } FDBIndexDB;
    
    FDBIndexDB * FDBIndexOpen(hcchar * dbPath,hcchar * name);
    
    huint32 FDBIndexRowCount(FDBIndexDB * dbIndex);
    
    void FDBIndexClose(FDBIndexDB * dbIndex);
    
    hint32 FDBIndexWrite(hcchar * dbPath,hcchar * name,FDBIndexData * const indexData);
    
    typedef struct _FDBIndexCursor {
        FDBIndexData data;
        huint32 location;
        huint32 index;
    } FDBIndexCursor;
    
    typedef FDBIndexCompareOrder (* FDBIndexCursorCompare) (FDBIndexDB * indexDB,struct _FDBIndexCursor * cursor,FDBDataItem dataItem,hany context);
    
    FDBDataItem FDBIndexCursorToBegin(FDBIndexDB * indexDB,FDBIndexCursor * cursor,FDBIndexCursorCompare compare,hany context);
    
    typedef enum _FDBIndexCursorPropertyStringMatch {
        FDBIndexCursorPropertyStringMatchEqual,FDBIndexCursorPropertyStringMatchPrefix
    } FDBIndexCursorPropertyStringMatch;
    
    typedef struct _FDBIndexCursorProperty {
        FDBProperty * property;
        FDBIndexCompareOrder mode;
        union {
            hint32 int32Value;
            hint64 int64Value;
            hdouble doubleValue;
            hcchar * stringValue;
        };
        FDBIndexCursorPropertyStringMatch stringMatch;
    } FDBIndexCursorProperty;
    
    FDBDataItem FDBIndexCursorToBeginPropertys(FDBIndexDB * indexDB,FDBIndexCursor * cursor,FDBIndexCursorProperty * propertys,huint32 length);
    
    FDBDataItem FDBIndexCursorToNext(FDBIndexDB * indexDB,FDBIndexCursor * cursor,FDBIndexCursorCompare compare,hany context);
    
    FDBDataItem FDBIndexCursorToNextPropertys(FDBIndexDB * indexDB,FDBIndexCursor * cursor,FDBIndexCursorProperty * propertys,huint32 length);
    
    
    
#ifdef __cplusplus
}
#endif


#endif
