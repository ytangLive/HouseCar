//
//  hreport.c
//  vTeam
//
//  Created by zhang hailong on 13-6-8.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hreport.h"

#include <libxml/parser.h>
#include <libxml/xpath.h>

typedef struct {
    ReportDocument base;
    xmlDocPtr xmlDocument;
} ReportDocumentInternal;

static hbool ReportDocumentCreateInit(ReportDocumentInternal * document){
    xmlXPathContextPtr ctx;
    xmlXPathObjectPtr result;
    xmlChar * version = NULL;
    
    ctx = xmlXPathNewContext(document->xmlDocument);
    
    if(!ctx){
        return hbool_false;
    }
    
    result = xmlXPathEvalExpression((xmlChar *) "/report/version",ctx);
    
    if(!result){
        xmlXPathFreeContext(ctx);
        return hbool_false;
    }
    
    if(result->type == XPATH_NODESET && result->nodesetval->nodeNr >0){
        version = xmlNodeGetContent(result->nodesetval->nodeTab[0]);
    }
    else{
        xmlXPathFreeContext(ctx);
        xmlXPathFreeObject(result);
        return hbool_false;
    }
    
    if(!version){
        xmlXPathFreeContext(ctx);
        xmlXPathFreeObject(result);
        return hbool_false;
    }
    
    document->base.version = atoi((char *)version);

    xmlXPathFreeContext(ctx);
    xmlXPathFreeObject(result);
    
    return hbool_true;
}

ReportDocument * ReportDocumentCreateBytes(const void * bytes,huint32 length){
    ReportDocumentInternal * doc = (ReportDocumentInternal *) malloc(sizeof(ReportDocumentInternal));

    memset(doc, 0, sizeof(ReportDocumentInternal));
    
    doc->xmlDocument = xmlParseMemory(bytes, length);
    
    if(!doc->xmlDocument){
        free(doc);
        return NULL;
    }
    
    if(!ReportDocumentCreateInit(doc)){
        free(doc);
        return NULL;
    }
    
    return (ReportDocument *)doc;
}

ReportDocument * ReportDocumentCreateFilePath(hcchar * filePath){
    ReportDocumentInternal * doc = (ReportDocumentInternal *) malloc(sizeof(ReportDocumentInternal));
    
    memset(doc, 0, sizeof(ReportDocumentInternal));
    
    doc->xmlDocument = xmlParseFile(filePath);
    
    if(!doc->xmlDocument){
        free(doc);
        return NULL;
    }
    
    if(!ReportDocumentCreateInit(doc)){
        free(doc);
        return NULL;
    }
    
    return (ReportDocument *)doc;
}

void ReportDocumentDelete(ReportDocument * document){
    ReportDocumentInternal * doc = (ReportDocumentInternal *) document;
    if(doc->xmlDocument){
        xmlFreeDoc(doc->xmlDocument);
    }
    free(doc);
}

