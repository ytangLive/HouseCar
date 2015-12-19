//
//  hobjc_url.h
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#ifndef hobjc_hobjc_url_h
#define hobjc_hobjc_url_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "hobjc_value.h"
#include "hobjc_dictionary.h"
    
    hobj_t * hobj_url_encode(hcchar * value,InvokeTickDeclare);
    
    hcchar * hobj_url_encode_c(hcchar * value,InvokeTickDeclare);
    
    hobj_t * hobj_url_decode(hcchar * value,InvokeTickDeclare);
    
    hcchar * hobj_url_decode_c(hcchar * value,InvokeTickDeclare);
    
    typedef struct _hobj_url_t{
        hobj_t base;
        hobj_value_t * READONLY url;
        hobj_value_t * READONLY absoluteUrl;
        hobj_value_t * READONLY path;
        hobj_value_t * READONLY protocol;
        hobj_value_t * READONLY token;
        hobj_value_t * READONLY domain;
        hobj_value_t * READONLY query;
        huint32 READONLY port;
        hobj_value_t * READONLY user;
        hobj_value_t * READONLY password;
        hobj_dictionary_t * READONLY queryValues;
        struct _hobj_url_t * baseUrl;
    } hobj_url_t;
    
    hobj_url_t * hobj_url_alloc(hobj_url_t * baseUrl,hcchar * url,InvokeTickDeclare);
    
    hobj_url_t * hobj_url_new(hobj_url_t * baseUrl,hcchar * url,InvokeTickDeclare);
    
    hcchar * hobj_url_url(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_absoluteUrl(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_path(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_protocol(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_token(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_domain(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_query(hobj_url_t * url,InvokeTickDeclare);
    
    huint32 hobj_url_port(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_user(hobj_url_t * url,InvokeTickDeclare);
    
    hcchar * hobj_url_password(hobj_url_t * url,InvokeTickDeclare);
    
    hobj_url_t * hobj_url_baseUrl(hobj_url_t * url,InvokeTickDeclare);
    
    HOBJ_CLASS_DEC(URL)
    
    HOBJ_KEY_DEC(URL)
    HOBJ_KEY_DEC(url)
    HOBJ_KEY_DEC(absoluteUrl)
    HOBJ_KEY_DEC(path)
    HOBJ_KEY_DEC(protocol)
    HOBJ_KEY_DEC(token)
    HOBJ_KEY_DEC(domain)
    HOBJ_KEY_DEC(query)
    HOBJ_KEY_DEC(port)
    HOBJ_KEY_DEC(user)
    HOBJ_KEY_DEC(password)
    HOBJ_KEY_DEC(baseUrl)
    
#ifdef __cplusplus
}
#endif



#endif
