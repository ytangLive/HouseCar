//
//  hfdb.c
//  vTeam
//
//  Created by Zhang Hailong on 13-6-10.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hfdb.h"

#include "hfile.h"


void FDBClassInitialize(FDBClass * const dbClass){
    FDBProperty * prop = & dbClass->rowid;
    huint32 c = dbClass->propertyCount;
    dbClass->itemSize = 0;
    dbClass->lastRowid = 0;
    
    while(c > 0){
        
        assert(prop->type > FDBPropertyTypeNone && prop->type < FDBPropertyTypeMaxCount);
        
        switch (prop->type) {
            case FDBPropertyTypeInt32:
                prop->length = sizeof(hint32);
                break;
            case FDBPropertyTypeInt64:
                prop->length = sizeof(hint64);
                break;
            case FDBPropertyTypeDouble:
                prop->length = sizeof(hdouble);
                break;
            case FDBPropertyTypeString:
                if(prop->length == 0){
                    prop->length = 512;
                }
                break;
            case FDBPropertyTypeBlob:
                prop->length = sizeof(FDBBlobValue);
                break;
            default:
                assert(0);
                break;
        }
        
        prop->offset = dbClass->itemSize;
        
        dbClass->itemSize += prop->length;
        
        prop ++;
        c --;
    }
}

huint32 FDBClassSize(FDBClass * const dbClass){
    return sizeof(FDBClass) + (dbClass->propertyCount - 1) * sizeof(FDBProperty);
}

void FDBDataSetLength(FDBData * const data,huint32 length){
    if(data->data){
        if(data->length < length){
            data->data = realloc(data->data, length * data->dbClass->itemSize);
        }
        data->length =length;
    }
    else{
        data->data = malloc(length * data->dbClass->itemSize);
        data->length = length;
    }
}

void FDBDataClear(FDBData * const data){
    memset(data->data, 0, data->dbClass->itemSize * data->length);
}

void FDBDataCreate(FDBData * const data,FDBClass * dbClass){
    memset(data, 0, sizeof(FDBData));
    data->dbClass = dbClass;
}

void FDBDataDelete(FDBData * const data){
    if(data->data){
        free(data->data);
    }
    memset(data, 0, sizeof(FDBData));
}

FDBDataItem FDBDataItemAt(FDBData * const data,huint32 index){
    if(data && index < data->length){
        return (hbyte *)data->data + index * data->dbClass->itemSize;
    }
    return NULL;
}


FDBProperty * FDBClassGetProperty(FDBClass * const dbClass,hcchar * name){
    huint32 c = dbClass->propertyCount;
    FDBProperty * prop = & dbClass->rowid;
    
    while(c >0){
        
        if(strcmp(prop->name, name) ==0){
            return prop;
        }
        
        c --;
        prop ++;
    }
    
    return NULL;
}

void FDBClassSetPropertyInt32Value(FDBDataItem dataItem, FDBProperty * prop,hint32 value){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            * (hint32 *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeInt64:
            * (hint64 *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeDouble:
            * (hdouble *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeString:
            snprintf((hchar *)((hbyte *) dataItem + prop->offset), prop->length,"%d",value);
            break;
        default:
            break;
    }
}

hint32 FDBClassGetPropertyInt32Value(FDBDataItem dataItem, FDBProperty * prop,hint32 defaultValue){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            return * (hint32 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeInt64:
            return (hint32) * (hint64 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeDouble:
            return (hint32) * (hdouble *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeString:
            return atoi((hchar *)((hbyte *) dataItem + prop->offset));
            break;
        default:
            break;
    }
    return defaultValue;
}

void FDBClassSetPropertyInt64Value(FDBDataItem dataItem, FDBProperty * prop,hint64 value){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            * (hint32 *)((hbyte *) dataItem + prop->offset) = (hint32)value;
            break;
        case FDBPropertyTypeInt64:
            * (hint64 *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeDouble:
            * (hdouble *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeString:
            snprintf((hchar *)((hbyte *) dataItem + prop->offset), prop->length,"%lld",value);
            break;
        default:
            break;
    }
}

hint64 FDBClassGetPropertyInt64Value(FDBDataItem dataItem, FDBProperty * prop,hint64 defaultValue){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            return * (hint32 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeInt64:
            return  * (hint64 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeDouble:
            return (hint64) * (hdouble *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeString:
            return atoll((hchar *)((hbyte *) dataItem + prop->offset));
            break;
        default:
            break;
    }
    return defaultValue;
}

void FDBClassSetPropertyDoubleValue(FDBDataItem dataItem, FDBProperty * prop,hdouble value){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            * (hint32 *)((hbyte *) dataItem + prop->offset) = (hint32)value;
            break;
        case FDBPropertyTypeInt64:
            * (hint64 *)((hbyte *) dataItem + prop->offset) = (hint64)value;
            break;
        case FDBPropertyTypeDouble:
            * (hdouble *)((hbyte *) dataItem + prop->offset) = value;
            break;
        case FDBPropertyTypeString:
            snprintf((hchar *)((hbyte *) dataItem + prop->offset), prop->length,"%f",value);
            break;
        default:
            break;
    }
}

hdouble FDBClassGetPropertyDoubleValue(FDBDataItem dataItem, FDBProperty * prop,hdouble defaultValue){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            return (hdouble) * (hint32 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeInt64:
            return (hdouble) * (hint64 *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeDouble:
            return * (hdouble *)((hbyte *) dataItem + prop->offset) ;
            break;
        case FDBPropertyTypeString:
            return atof((hchar *)((hbyte *) dataItem + prop->offset));
            break;
        default:
            break;
    }
    return defaultValue;
}

void FDBClassSetPropertyStringValue(FDBDataItem dataItem, FDBProperty * prop,hcchar * value){
    switch (prop->type) {
        case FDBPropertyTypeInt32:
            * (hint32 *)((hbyte *) dataItem + prop->offset) = value ? atoi(value) : 0;
            break;
        case FDBPropertyTypeInt64:
            * (hint64 *)((hbyte *) dataItem + prop->offset) = value ? atoll(value) : 0;
            break;
        case FDBPropertyTypeDouble:
            * (hdouble *)((hbyte *) dataItem + prop->offset) = value ? atof(value) : 0;
            break;
        case FDBPropertyTypeString:
            if(value){
                strncpy((hchar *)((hbyte *) dataItem + prop->offset), value, prop->length);
            }
            else{
                memset(((hbyte *) dataItem + prop->offset), 0 , prop->length);
            }
            break;
        default:
            break;
    }
}

hcchar * FDBClassGetPropertyStringValue(FDBDataItem dataItem, FDBProperty * prop,hcchar * defaultValue){
    switch (prop->type) {
        case FDBPropertyTypeString:
            return (hchar *)((hbyte *) dataItem + prop->offset);
            break;
        default:
            break;
    }
    return defaultValue;
}

void FDBClassSetPropertyBlobValue(FDBDataItem dataItem,FDBProperty * prop,FDBBlobValue value){
    switch (prop->type) {
        case FDBPropertyTypeBlob:
            * (FDBBlobValue *)((hbyte *) dataItem + prop->offset) = value;
            break;
        default:
            break;
    }
}

FDBBlobValue FDBClassGetPropertyBlobValue(FDBDataItem dataItem,FDBProperty * prop,FDBBlobValue defaultValue){
    switch (prop->type) {
        case FDBPropertyTypeBlob:
            return  * (FDBBlobValue *)((hbyte *) dataItem + prop->offset) ;
            break;
        default:
            break;
    }
    return defaultValue;
}

typedef struct {
    FDB base;
    hbool isCopyDBClass;
    hbool isCopyDBIndexs;
    hchar dbPath[PATH_MAX];
    hchar bobPath[PATH_MAX];
    struct {
        hchar * sbuf;
        huint32 size;
        huint32 length;
        hint64 location;
    } blob;
    int dbfno;
    int bobfno;
} FDBInternal;

typedef struct {
    hchar tag[4];
    huint32 version;
    huint32 classSize;
} FDBHead;


FDB * FDBCreate(hcchar * dbPath,FDBClass * dbClass,hbool isCopyDBClass){
    
    if(dbPath && dbClass){
        
        FDBInternal * db = malloc(sizeof(FDBInternal));
        int fno;
        FDBHead head = {FDB_TAG,FDB_VERSION,FDBClassSize(dbClass)};

        memset(db, 0, sizeof(FDBInternal));
        
        snprintf(db->dbPath, sizeof(db->dbPath),"%s%s",dbPath,FDB_FILE_DB);
        snprintf(db->bobPath, sizeof(db->bobPath),"%s%s",dbPath,FDB_FILE_BOLB);
        
        db->base.version = FDB_VERSION;
        db->base.dbClass = dbClass;
        db->base.rowCount = -1;
        db->isCopyDBClass = isCopyDBClass;
        db->dbfno = -1;
        db->bobfno = -1;
        
        fno = open(db->dbPath, O_WRONLY | O_CREAT | O_TRUNC);
        
        if(fno == -1){
            free(db);
            return NULL;
        }
        
        fchmod(fno, S_IRUSR | S_IWUSR);
        
        if(write(fno, &head, sizeof(head)) != sizeof(head)){
            free(db);
            return NULL;
        }
        
        if(write(fno, dbClass, head.classSize) != head.classSize){
            free(db);
            return NULL;
        }
        
        close(fno);
        
        fno = open(db->bobPath,O_WRONLY | O_CREAT | O_TRUNC);
        
        if(fno == -1){
            free(db);
            return NULL;
        }
        
        fchmod(fno, S_IRUSR | S_IWUSR);
        
        close(fno);
        
        if(isCopyDBClass){
            db->base.dbClass = malloc(head.classSize);
            memcpy(db->base.dbClass, dbClass, head.classSize);
        }
        
        return (FDB *)db;
    }
    
    return NULL;
}

FDB * FDBOpen(hcchar * dbPath){
    if(dbPath){
       
        FDBInternal * db = malloc(sizeof(FDBInternal));
        int fno;
        FDBHead head;
        
        memset(db, 0, sizeof(FDBInternal));

        snprintf(db->dbPath, sizeof(db->dbPath),"%s%s",dbPath,FDB_FILE_DB);
        snprintf(db->bobPath, sizeof(db->bobPath),"%s%s",dbPath,FDB_FILE_BOLB);
        
        
        fno = open(db->dbPath, O_RDONLY);
        
        if(fno == -1){
            free(db);
            return NULL;
        }
        
        if(sizeof(head) != read(fno, &head, sizeof(head))){
            free(db);
            close(fno);
            return NULL;
        }
        
        if(strncmp(head.tag, FDB_TAG, sizeof(head.tag))){
            free(db);
            close(fno);
            return NULL;
        }
        
        db->base.version = head.version;
        db->isCopyDBClass = hbool_true;

        db->base.dbClass = malloc(head.classSize);
        db->base.rowCount = -1;
        
        if(head.classSize != read(fno, db->base.dbClass, head.classSize)){
            free(db->base.dbClass);
            free(db);
            close(fno);
            return NULL;
        }
        
        db->dbfno = fno;
        db->bobfno = open(db->bobPath, O_RDONLY);
        
        return (FDB *) db;
    }
    return NULL;
}

void FDBClose(FDB * fdb){
    FDBInternal * db = (FDBInternal *) fdb;
    
    if(db->dbfno != -1){
        close(db->dbfno);
    }
    
    if(db->bobfno != -1){
        close(db->bobfno);
    }
    
    if(db->isCopyDBClass){
        free(db->base.dbClass);
    }
    if(db->blob.sbuf){
        free(db->blob.sbuf);
    }
    free(fdb);
}

huint32 FDBRowCount(FDB * fdb){
    FDBInternal * db = (FDBInternal *) fdb;
    struct stat s;
    
    if(fdb->rowCount == -1){

        if(stat(db->dbPath, &s) == -1){
            fdb->rowCount = 0;
        }
        else{
            fdb->rowCount = ( ((huint32)s.st_size - (huint32)sizeof(FDBHead) - (huint32)FDBClassSize(db->base.dbClass)) / db->base.dbClass->itemSize);
        }
    }
    
    return fdb->rowCount;
}

hint32 FDBInsertData(FDB * fdb,FDBData * data,huint32 offset,huint32 length){
    
    FDBInternal * db = (FDBInternal *) fdb;
    
    hint32 lastRowid = db->base.dbClass->lastRowid;
    hint32 rowid = lastRowid ;
    hint32 i;
    FDBDataItem dataItem;
    
    assert(fdb->dbClass == data->dbClass);
    
    for (i = 0; i < length; i++) {
        dataItem = FDBDataItemAt(data, offset + i);
        assert(dataItem);
        FDBClassSetPropertyInt32Value(dataItem,& data->dbClass->rowid, ++ rowid);
    }
    
    int fno = open(db->dbPath, O_WRONLY);

    if(fno == -1){
        return FDB_ERROR;
    }
    
    flock(fno, LOCK_EX);
    
    if( sizeof(FDBHead) != lseek(fno, sizeof(FDBHead), SEEK_SET)){
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return FDB_ERROR;
    }
    
    db->base.dbClass->lastRowid = rowid;
    
    if( sizeof(FDBClass) != write(fno, db->base.dbClass, sizeof(FDBClass))){
        
        db->base.dbClass->lastRowid = lastRowid;
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return FDB_ERROR;
    }
    

    lseek(fno, 0, SEEK_END);
    
    i = db->base.dbClass->itemSize * length;
    
    dataItem = FDBDataItemAt(data, offset);
    
    if(i != write(fno, dataItem, i)){
        flock(fno, LOCK_UN);
        close(fno);
        return FDB_ERROR;
    }

    flock(fno, LOCK_UN);
    close(fno);
    
    return FDB_OK;
}

static char FDBBlobTag[4] = {'B','L','O','B'};

FDBBlobValue FDBBlobCreate(FDB * fdb,void * data,huint32 length){
    FDBInternal * db = (FDBInternal *) fdb;
    FDBBlobValue v = 0;

    if(db->dbfno != -1){
        close(db->dbfno);
        db->dbfno = -1;
    }
    
    if(db->bobfno != -1){
        close(db->bobfno);
        db->bobfno = -1;
    }
    
    db->base.rowCount = -1;
    
    int fno = open(db->bobPath, O_WRONLY | O_CREAT);
    
    if(fno == -1){
        return 0;
    }

    flock(fno, LOCK_EX);
    
    v = lseek(fno, 0, SEEK_END) ;
    
    if(v == 0){
        if(sizeof(FDBBlobTag) != write(fno, FDBBlobTag, sizeof(FDBBlobTag))){
            flock(fno, LOCK_UN);
            close(fno);
            
            return 0;
        }
        v = sizeof(FDBBlobTag);
    }

    if(sizeof(length) != write(fno, &length, sizeof(length))){
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return 0;
    }
    
    if(length != write(fno, data, length)){
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return 0;
    }
     
    flock(fno, LOCK_UN);
    close(fno);
    
    return v;
}

void * FDBBlobRead(FDB * fdb,FDBBlobValue value,huint32 * length){
    FDBInternal * db = (FDBInternal *) fdb;
    int fno;
    huint32 len;
    
    if(db->blob.sbuf && value >= db->blob.location && value < db->blob.location + db->blob.length){
        if(length){
            * length = * (huint32 *)(db->blob.sbuf + (value - db->blob.location));
        }
        return (db->blob.sbuf + (value - db->blob.location) + sizeof(huint32));
    }
    
    db->blob.length = 0;
    db->blob.location = value;
    
    if(db->bobfno == -1){
        fno = open(db->bobPath, O_RDONLY);
    }
    else{
        fno = db->bobfno;
    }
    
    if(fno == -1){
        return NULL;
    }
    
    flock(fno, LOCK_SH);
    
    if( value != lseek(fno, value, SEEK_SET) ){
        flock(fno, LOCK_UN);
        if(db->bobfno != fno){
            close(fno);
        }
        return NULL;
    }
    
    len = 0;
    
    while(1){
        
        if(sizeof(huint32) != read(fno, &len, sizeof(huint32))){
            break;
        }
        
        if(db->blob.length ==0){
            if(db->blob.sbuf == NULL){
                db->blob.size = MAX(FDB_BLOB_BUF_SIZE,  len + sizeof(huint32));
                db->blob.sbuf = malloc(db->blob.size);
            }
            else if(db->blob.size < len + sizeof(huint32)){
                db->blob.size = len + sizeof(huint32);
                db->blob.sbuf = realloc(db->blob.sbuf, db->blob.size);
            }
        }

        if(db->blob.length + sizeof(huint32) + len <= db->blob.size){
            * (huint32 *)(db->blob.sbuf + db->blob.length) = len;
            if(len != read(fno, db->blob.sbuf + db->blob.length + sizeof(huint32), len)){
                break;
            }
            db->blob.length += sizeof(huint32) + len;
        }
        else{
            break;
        }
    }
    
    flock(fno, LOCK_UN);
    
    if(db->bobfno != fno){
        close(fno);
    }
    
    if(db->blob.sbuf && value >= db->blob.location && value < db->blob.location + db->blob.length){
        if(length){
            * length = * (huint32 *)(db->blob.sbuf + (value - db->blob.location));
        }
        return (db->blob.sbuf + (value - db->blob.location) + sizeof(huint32));
    }
    
    return NULL;
}

hint32 FDBInsert(FDB * fdb,FDBDataItem dataItem){
    
    FDBInternal * db = (FDBInternal *) fdb;

    hint32 lastRowid = db->base.dbClass->lastRowid;
    hint32 rowid = lastRowid + 1;
    
    if(db->dbfno != -1){
        close(db->dbfno);
        db->dbfno = -1;
    }
    
    if(db->bobfno != -1){
        close(db->bobfno);
        db->bobfno = -1;
    }
    
    db->base.rowCount = -1;
    
    int fno = open(db->dbPath, O_WRONLY);
    
    if(fno == -1){
        return FDB_ERROR;
    }
    
    flock(fno, LOCK_EX);
    
    if( sizeof(FDBHead) != lseek(fno, sizeof(FDBHead), SEEK_SET)){
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return FDB_ERROR;
    }
    
    db->base.dbClass->lastRowid = rowid;
    
    if( sizeof(FDBClass) != write(fno, db->base.dbClass, sizeof(FDBClass))){
       
        db->base.dbClass->lastRowid = lastRowid;
        
        flock(fno, LOCK_UN);
        close(fno);
        
        return FDB_ERROR;
    }
    
    lseek(fno, 0, SEEK_END);
    
    FDBClassSetPropertyInt32Value(db->base.dbClass, dataItem, rowid);
    
    if(db->base.dbClass->itemSize != write(fno, dataItem, db->base.dbClass->itemSize)){
        flock(fno, LOCK_UN);
        close(fno);
        return FDB_ERROR;
    }
    
    flock(fno, LOCK_UN);
    close(fno);
    
    return FDB_OK;
}

FDBDataItem FDBCursorNext(FDB * fdb,FDBCursor * cursor){
    
    FDBInternal * db = (FDBInternal *) fdb;
    FDBDataItem dataItem = NULL;
    int fno;
    off_t off;
    huint32 classSize = FDBClassSize(fdb->dbClass);
    ssize_t len;

    while(1){
        
        if(cursor->length ==0){
                
            assert(cursor->data.dbClass == fdb->dbClass && cursor->data.length);
    
            if(db->dbfno == -1){
                fno = open(db->dbPath, O_RDONLY);
            }
            else{
                fno = db->dbfno;
            }
            
            if(fno == -1){
                
                return NULL;
            }
            
            flock(fno, LOCK_SH);
            
            cursor->location = cursor->location + cursor->index;
            cursor->index = 0;
            
            off = sizeof(FDBHead) + classSize + fdb->dbClass->itemSize * cursor->location;
            
            if(off != lseek(fno, off, SEEK_SET)){
                
                flock(fno, LOCK_UN);
                
                if(db->dbfno != fno){
                    close(fno);
                }
                return NULL;
            }
            
            len = cursor->data.length * fdb->dbClass->itemSize;
            
            len = read(fno, cursor->data.data, len);
            
            if(len < 0){
                len = 0;
            }
            
            cursor->length = (huint32) len / fdb->dbClass->itemSize;
            
            flock(fno, LOCK_UN);
            
            if(db->dbfno != fno){
                close(fno);
            }
            
            if(cursor->length ==0){
                break;
            }

        }
        
        dataItem = FDBDataItemAt(& cursor->data, cursor->index);
        
        cursor->index ++;
        cursor->length --;
        
        if(!cursor->filter || ( * cursor->filter)(fdb,cursor,dataItem)){
            return dataItem;
        }
        
    }
    
    return NULL;
}

FDBDataItem FDBCursorToRowid(FDB * fdb,FDBCursor * cursor,hint32 rowid){
    
    FDBInternal * db = (FDBInternal *) fdb;
    huint32 rowCount = FDBRowCount(fdb);
    int fno;
    huint32 classSize = FDBClassSize(fdb->dbClass);
    off_t off;
    ssize_t len;
    FDBDataItem dataItem = NULL;
    
    if(rowid <=0 && rowid > rowCount){
        return NULL;
    }
    
    if(rowid - 1 >= cursor->location && rowid -1 < cursor->location + cursor->index + cursor->length){
        cursor->length = cursor->index + cursor->length;
        cursor->index = rowid -1 - cursor->location;
        cursor->length = cursor->length - cursor->index;
        
        dataItem = FDBDataItemAt(& cursor->data, cursor->index);
        
        cursor->index ++;
        cursor->length --;
        
        return dataItem;
    }
    
    assert(cursor->data.dbClass == fdb->dbClass && cursor->data.length);
    
    if(db->dbfno == -1){
        fno = open(db->dbPath, O_RDONLY);
    }
    else{
        fno = db->dbfno;
    }
    
    if(fno == -1){
        
        return NULL;
    }
    
    flock(fno, LOCK_SH);
    
    cursor->location = rowid -1;
    cursor->index = 0;
    
    off = sizeof(FDBHead) + classSize + fdb->dbClass->itemSize * cursor->location;
    
    if(off != lseek(fno, off, SEEK_SET)){
        
        flock(fno, LOCK_UN);
        
        if(db->dbfno != fno){
            close(fno);
        }
        return NULL;
    }
    
    len = cursor->data.length * fdb->dbClass->itemSize;
    
    len = read(fno, cursor->data.data, len);
    
    if(len < 0){
        len = 0;
    }
    
    cursor->length = (huint32) len / fdb->dbClass->itemSize;
    
    flock(fno, LOCK_UN);
    
    if(db->dbfno != fno){
        close(fno);
    }
    
    dataItem = FDBDataItemAt(& cursor->data, cursor->index);
    
    cursor->index ++;
    cursor->length --;
    
    return dataItem;
}


hint32 FDBCursorCommit(FDB * fdb,FDBCursor * cursor){
    
    FDBInternal * db = (FDBInternal *) fdb;
    int fno;
    off_t off;
    huint32 classSize;
    FDBDataItem dataItem;
    
    if(cursor->index ==0){
        return FDB_ERROR;
    }
    
    classSize = FDBClassSize(fdb->dbClass);
    
    if(db->dbfno != -1){
        close(db->dbfno);
        db->dbfno = -1;
    }
    
    if(db->bobfno != -1){
        close(db->bobfno);
        db->bobfno = -1;
    }
    
    fno = open(db->dbPath, O_WRONLY);
    
    if(fno == -1){
        return FDB_ERROR;
    }
    
    flock(fno, LOCK_EX);
    
    off = sizeof(FDBHead) + classSize + fdb->dbClass->itemSize * (cursor->location + cursor->index -1);
    
    dataItem = FDBDataItemAt(&cursor->data, cursor->index -1);
    
    assert(dataItem);
    
    if(off != lseek(fno, off, SEEK_SET)){
        flock(fno, LOCK_UN);
        close(fno);
        return FDB_ERROR;
    }
    
    if(fdb->dbClass->itemSize != write(fno, dataItem, fdb->dbClass->itemSize)){
        flock(fno, LOCK_UN);
        close(fno);
        return FDB_ERROR;
    }
    
    flock(fno, LOCK_UN);
    close(fno);
    
    return FDB_OK;
}
