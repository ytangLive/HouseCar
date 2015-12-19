//
//  hreport.h
//  vTeam
//
//  Created by zhang hailong on 13-6-8.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef HREPORT_H
#define HREPORT_H

#ifdef __cplusplus
extern "C" {
#endif
    
    
    typedef struct _ReportDocument{
        huint32 version;
    } ReportDocument;
    
    ReportDocument * ReportDocumentCreateBytes(const void * bytes,huint32 length);
    
    ReportDocument * ReportDocumentCreateFilePath(hcchar * filePath);
    
    void ReportDocumentDelete(ReportDocument * document);
    
    
    
#ifdef __cplusplus
}
#endif


#endif