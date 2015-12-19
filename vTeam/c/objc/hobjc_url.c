//
//  hobjc_url.c
//  hobjc
//
//  Created by zhang hailong on 13-3-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#define READONLY

#include "hconfig.h"
#include "hobjc_url.h"
#include "hbuffer.h"
#include "hstr.h"

static void hobj_url_destroy_impl(hobj_url_t * obj,InvokeTickDeclare);

static hobj_t * hobj_url_toString_impl(hobj_url_t * obj,InvokeTickDeclare);

HOBJ_KEY_IMP(URL)
HOBJ_KEY_IMP(url)
HOBJ_KEY_IMP(absoluteUrl)
HOBJ_KEY_IMP(path)
HOBJ_KEY_IMP(protocol)
HOBJ_KEY_IMP(token)
HOBJ_KEY_IMP(domain)
HOBJ_KEY_IMP(query)
HOBJ_KEY_IMP(port)
HOBJ_KEY_IMP(user)
HOBJ_KEY_IMP(password)
HOBJ_KEY_IMP(baseUrl)

HOBJ_CLASS_PROPERTY_IMP_BEGIN(URL)

HOBJ_CLASS_PROPERTY_IMP(url, HOBJ_TYPE_STRING, hobj_url_url, NULL)
HOBJ_CLASS_PROPERTY_IMP(absoluteUrl, HOBJ_TYPE_STRING, hobj_url_absoluteUrl, NULL)
HOBJ_CLASS_PROPERTY_IMP(path, HOBJ_TYPE_STRING, hobj_url_path, NULL)
HOBJ_CLASS_PROPERTY_IMP(protocol, HOBJ_TYPE_STRING, hobj_url_protocol, NULL)
HOBJ_CLASS_PROPERTY_IMP(token, HOBJ_TYPE_STRING, hobj_url_token, NULL)
HOBJ_CLASS_PROPERTY_IMP(domain, HOBJ_TYPE_STRING, hobj_url_domain, NULL)
HOBJ_CLASS_PROPERTY_IMP(query, HOBJ_TYPE_STRING, hobj_url_query, NULL)
HOBJ_CLASS_PROPERTY_IMP(port, HOBJ_TYPE_INTEGER, hobj_url_port, NULL)
HOBJ_CLASS_PROPERTY_IMP(user, HOBJ_TYPE_STRING, hobj_url_user, NULL)
HOBJ_CLASS_PROPERTY_IMP(password, HOBJ_TYPE_STRING, hobj_url_password, NULL)
HOBJ_CLASS_PROPERTY_IMP(baseUrl, HOBJ_TYPE_OBJECT, hobj_url_baseUrl, NULL)

HOBJ_CLASS_PROPERTY_IMP_END(URL)

HOBJ_CLASS_METHOD_IMP_BEGIN(URL)

HOBJ_CLASS_METHOD_IMP(destroy, hobj_url_destroy_impl)

HOBJ_CLASS_METHOD_IMP(toString, hobj_url_toString_impl)

HOBJ_CLASS_METHOD_IMP_END(URL)

HOBJ_CLASS_IMP_P_I(URL, hobj_url_t, &hobj_class)

hobj_t * hobj_url_encode(hcchar * value,InvokeTickDeclare){
    hbuffer_t buf = buffer_alloc(128, 128);
    huchar * p = (huchar *) value;
    char sx[4];
    hobj_t * v = NULL;
    while(p && *p != '\0'){
        
        if(*p == ' '){
            buffer_append(buf, "+", 1);
        }
        else if((*p >='0' && *p <='9') || (*p >= 'a' && *p <='z') || (*p >= 'A' && *p <='Z')){
            buffer_append(buf, p, 1);
        }
        else{
            sprintf(sx, "%%%02X",*p);
            buffer_append(buf, sx, 3);
        }
        
        p++;
    }
    
    v = (hobj_t *) hobj_value_string_new(buffer_to_str(buf), InvokeTickArg);
    
    buffer_dealloc(buf);
    
    return v;
}

hcchar * hobj_url_encode_c(hcchar * value,InvokeTickDeclare){
    hobj_t * v = hobj_url_encode(value,InvokeTickArg);
    if(v){
        return hobj_value_stringValue((hobj_value_t *)v, InvokeTickArg);
    }
    return NULL;
}

hobj_t * hobj_url_decode(hcchar * value,InvokeTickDeclare){
    hbuffer_t buf = buffer_alloc(128, 128);
    huchar * p = (huchar *) value,uc;
    huint32 c;
    char sx[4];
    hobj_t * v = NULL;
    while(p && *p != '\0'){
        
        if(*p == '+'){
            buffer_append(buf, " ", 1);
        }
        else if(*p == '%'){
            c = 0;
            sx[0] = p[1];
            sx[1] = p[2];
            sx[2] = 0;
            
            sscanf(sx, "%02x",&c);
            
            uc = c;
            buffer_append(buf, &uc, 1);
            
            p += 2;
        }
        else{
            buffer_append(buf, p, 1);
        }
        
        p++;
    }
    
    v = (hobj_t *) hobj_value_string_new(buffer_to_str(buf), InvokeTickArg);
    
    buffer_dealloc(buf);
    
    return v;
}

hcchar * hobj_url_decode_c(hcchar * value,InvokeTickDeclare){
    hobj_t * v = hobj_url_decode(value,InvokeTickArg);
    if(v){
        return hobj_value_stringValue((hobj_value_t *)v, InvokeTickArg);
    }
    return NULL;
}


hobj_url_t * hobj_url_alloc(hobj_url_t * baseUrl,hcchar * url,InvokeTickDeclare){
    
    hbuffer_t absoluteUrl = buffer_alloc(128, 128);
    hchar * p ,* lp , *lp2, sport[128];
    hobj_url_t * u  = (hobj_url_t *) hobj_alloc(&hobj_URL_class, InvokeTickArg);
    
    if(baseUrl){
        u->baseUrl = (hobj_url_t *) hobj_retain((hobj_t *) baseUrl, InvokeTickArg);
    }
    
    u->url = hobj_value_string_alloc(url, InvokeTickArg);
        
    if(baseUrl){
        if(baseUrl->protocol){
            buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->protocol,InvokeTickArg));
            buffer_append_str(absoluteUrl, "://");
        }
        if(baseUrl->user){
            buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->user,InvokeTickArg));
            if(baseUrl->password){
                buffer_append_str(absoluteUrl, ":");
                buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->password,InvokeTickArg));
            }
            buffer_append_str(absoluteUrl, "@");
        }
        if(baseUrl->domain){
            buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->domain,InvokeTickArg));
            if(baseUrl->port){
                buffer_append_format(absoluteUrl, ":%u",InvokeTickArg,baseUrl->port);
            }
        }
        
        if(baseUrl->path){
            buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->path,InvokeTickArg));
        }
        
        p = (char *) url;
        if(p && *p =='/'){
            buffer_append_str(absoluteUrl, url);
        }
        else if(p && *p == '?'){
            buffer_append_str(absoluteUrl, url);
        }
        else if(p && *p == '#'){
            if(baseUrl->query){
                buffer_append_str(absoluteUrl, hobj_value_stringValue(baseUrl->query,InvokeTickArg));
            }
            buffer_append_str(absoluteUrl, url);
        }
        else {
            
            while(p && *p != '\0'){
                if(p[0] == '.' && p[1] == '.'){
                    
                    p += 2;
                    
                    while(*p == '/'){
                        p++;
                    }
                    
                    lp = buffer_data(absoluteUrl) + buffer_length(absoluteUrl);
                    
                    while(lp != buffer_data(absoluteUrl) && * lp != '/'){
                        lp --;
                    }
                    
                    if(*lp == '/'){
                        buffer_length_set(absoluteUrl, lp - buffer_data(absoluteUrl) + 1);
                    }
                    else{
                        break;
                    }
                }
                else{
                    break;
                }
            }
            
            if(p && *p !='\0'){
                if(buffer_length(absoluteUrl) ==0){
                    buffer_append_str(absoluteUrl, p);
                }
                else{
                    lp = buffer_data(absoluteUrl) + buffer_length(absoluteUrl);
                    lp --;
                    if(* lp == '/'){
                        buffer_append_str(absoluteUrl, p);
                    }
                    else{
                        while(lp != buffer_data(absoluteUrl) && * lp != '/'){
                            lp --;
                        }
                        if(* lp == '/'){
                            buffer_length_set(absoluteUrl, lp - buffer_data(absoluteUrl) + 1);
                        }
                        buffer_append_str(absoluteUrl, p);
                    }
                }
            }
            
        }
    }
    else{
        buffer_append_str(absoluteUrl, url);
    }
    
    p = (hchar *)buffer_to_str(absoluteUrl);
    
    lp = str_find_sub(p,"://");
    
    if(lp){
        u->protocol = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
        p = lp + 3;
    }
    
    lp = str_find_sub(p, "@");
    
    if(lp){
        lp2 = str_find_sub(p, ":");
        if(lp2 && lp2 < lp){
            u->user = hobj_value_string_alloc_with_length(p, lp2 - p, InvokeTickArg);
            u->password = hobj_value_string_alloc_with_length(lp2 +1, lp - lp2 -1, InvokeTickArg);
        }
        else{
            u->user = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
        }
        p = lp +1;
    }
    
    lp = str_find_sub(p, "/");
    
    if(lp){
        if(lp - p != 0){
            lp2 = str_find_sub(p, ":");
            if(lp2 && lp2 < lp){
                u->domain = hobj_value_string_alloc_with_length(p, lp2 - p, InvokeTickArg);
                strncpy(sport, lp2 +1, MIN(lp - lp2 -1,sizeof(sport)));
                u->port = atoi(sport);
            }
            else{
                u->domain = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
            }
            
            p = lp;
        }
        
        lp = str_find_sub(p, "?");
        
        if(lp){
            u->path = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
            p = lp +1;
            lp = str_find_sub(p, "#");
            if(lp){
                u->query = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
                u->token = hobj_value_string_alloc(lp +1, InvokeTickArg);
            }
            else{
                u->query = hobj_value_string_alloc(p, InvokeTickArg);
            }
            if(u->query){
                u->queryValues = hobj_dictionary_alloc(InvokeTickArg);
                
                {
                    hbuffer_t key = buffer_alloc(64, 64);
                    hbuffer_t value = buffer_alloc(128, 128);
                    hint32 s = 0;
                    hchar * p = (hchar *) hobj_value_stringValue(u->query, InvokeTickArg);
                    
                    while(p){
                        
                        switch (s) {
                            case 0:
                            {
                                if(*p == '='){
                                    s = 1;
                                }
                                else if(*p == '&' || *p == '\0'){
                                    if(buffer_length(key)){
                                        hobj_setValueForKey((hobj_t *)u->queryValues, buffer_to_str(key), hobj_url_decode(buffer_to_str(value), InvokeTickArg), InvokeTickArg);
                                    }
                                    buffer_clear(key);
                                    buffer_clear(value);
                                }
                                else{
                                    buffer_append(key, p, 1);
                                }
                            }
                                break;
                            case 1:
                            {
                                if(* p == '&' || * p == '\0'){
                                    if(buffer_length(key)){
                                        hobj_setValueForKey((hobj_t *)u->queryValues, buffer_to_str(key), hobj_url_decode(buffer_to_str(value), InvokeTickArg), InvokeTickArg);
                                    }
                                    buffer_clear(key);
                                    buffer_clear(value);
                                    s = 0;
                                }
                                else{
                                    buffer_append(value, p, 1);
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
                    
                    buffer_dealloc(key);
                    buffer_dealloc(value);
                }
            }
        }
        else{
            
            lp = str_find_sub(p, "#");
            
            if(lp){
                u->path = hobj_value_string_alloc_with_length(p, lp - p, InvokeTickArg);
                u->token = hobj_value_string_alloc(lp +1, InvokeTickArg);
            }
            else{
                u->path = hobj_value_string_alloc(p, InvokeTickArg);
            }

        }
    }
    else{
        u->domain = hobj_value_string_alloc(p, InvokeTickArg);
        u->path = hobj_value_string_alloc("/", InvokeTickArg);
    }
    
    u->absoluteUrl = hobj_value_string_alloc(buffer_to_str(absoluteUrl), InvokeTickArg);
    
    buffer_dealloc(absoluteUrl);

    return u;
}

hobj_url_t * hobj_url_new(hobj_url_t * baseUrl,hcchar * url,InvokeTickDeclare){
    hobj_url_t * v = hobj_url_alloc(baseUrl,url,InvokeTickArg);
    return (hobj_url_t *) hobj_autorelease((hobj_t *) v, InvokeTickArg);
}

hcchar * hobj_url_url(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->url, InvokeTickArg);
}

hcchar * hobj_url_absoluteUrl(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->absoluteUrl, InvokeTickArg);
}

hcchar * hobj_url_path(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->path, InvokeTickArg);
}

hcchar * hobj_url_protocol(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->protocol, InvokeTickArg);
}

hcchar * hobj_url_token(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->token, InvokeTickArg);
}

hcchar * hobj_url_domain(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->domain, InvokeTickArg);
}

hcchar * hobj_url_query(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->query, InvokeTickArg);
}

huint32 hobj_url_port(hobj_url_t * url,InvokeTickDeclare){
    return url->port;
}

hcchar * hobj_url_user(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->user, InvokeTickArg);
}

hcchar * hobj_url_password(hobj_url_t * url,InvokeTickDeclare){
    return hobj_value_stringValue(url->password, InvokeTickArg);
}

hobj_url_t * hobj_url_baseUrl(hobj_url_t * url,InvokeTickDeclare){
    return url->baseUrl;
}

static void hobj_url_destroy_impl(hobj_url_t * obj,InvokeTickDeclare){
    hobj_release((hobj_t *)obj->url, InvokeTickArg);
    hobj_release((hobj_t *)obj->absoluteUrl, InvokeTickArg);
    hobj_release((hobj_t *)obj->path, InvokeTickArg);
    hobj_release((hobj_t *)obj->protocol, InvokeTickArg);
    hobj_release((hobj_t *)obj->token, InvokeTickArg);
    hobj_release((hobj_t *)obj->domain, InvokeTickArg);
    hobj_release((hobj_t *)obj->query, InvokeTickArg);
    hobj_release((hobj_t *)obj->user, InvokeTickArg);
    hobj_release((hobj_t *)obj->password, InvokeTickArg);
    hobj_release((hobj_t *)obj->baseUrl, InvokeTickArg);
}

static hobj_t * hobj_url_toString_impl(hobj_url_t * obj,InvokeTickDeclare){
    return (hobj_t *)obj->absoluteUrl;
}