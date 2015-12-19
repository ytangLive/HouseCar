//
//  hfdb_cfg.h
//  fdbgen
//
//  Created by Zhang Hailong on 13-6-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef fdbgen_hfdb_cfg_h
#define fdbgen_hfdb_cfg_h


#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct _FDBConfig {
        huint32 classCount;
        huint32 indexCount;
    } FDBConfig;
    
    FDBConfig * FDBConfigCreate(hext_data_t data);
    
    FDBClass * FDBConfigFindClass(FDBConfig * cfg,hcchar * name);
    
    FDBClass * FDBConfigClassAt(FDBConfig * cfg,huint32 index);
    
    hcchar * FDBConfigClassNameAt(FDBConfig * cfg,huint32 index);
    
    FDBIndexSortProperty * FDBConfigFindIndexSortPropertys(FDBConfig * cfg,hcchar * name,huint32 * length);
    
    FDBIndex * FDBConfigFindIndex(FDBConfig * cfg,hcchar * name);
    
    FDBIndex * FDBConfigIndexAt(FDBConfig * cfg,huint32 index);
    
    FDBIndexSortProperty * FDBConfigIndexSortPropertysAt(FDBConfig * cfg,huint32 index,huint32 * length);
    
    hcchar * FDBConfigIndexNameAt(FDBConfig * cfg,huint32 index);
    
    void FDBConfigDelete(FDBConfig * cfg);
    
    
#ifdef __cplusplus
}
#endif



#endif
