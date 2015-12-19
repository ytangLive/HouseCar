/*
 *  url.c
 *  C Library
 *
 *  Created by hailong zhang on 10-7-30.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hbuffer.h"
#include "hurl.h"
#include "hstr.h"
#include "hlist.h"
#include "hmap.h"

#undef url_alloc

#undef url_dealloc

#undef url_protocol

#undef url_domain

#undef url_port

#undef url_domain_resolv

#undef url_path

#undef url_query

#undef url_token

#undef url_mask

#undef url_param_names

#undef url_param

#undef url_encode

#undef url_decode

typedef struct {
	char * protocol;
	char * domain;
	int port;
	struct in_addr addr;
	char * path;
	char * query;
	char * token;
	hbuffer_t buffer;
	hlist_t param_names;
	hmap_t params;
}Url;

typedef struct{
	hchar * name;
	hchar * value;
}url_param_t;

static url_param_t * url_param_alloc(hcchar * name,hcchar *value, InvokeTickDeclare){
	url_param_t * param = mem_malloc( sizeof(url_param_t));
	memset(param,0,sizeof(url_param_t));
	str_cpy(&param->name, name);
	str_cpy(&param->value, value);
	
	return param;
}

static void url_param_dealloc( url_param_t * param, InvokeTickDeclare){
	mem_free( param->name);
	mem_free( param->value);
	mem_free( param);
}

hurl_t url_alloc(hcchar *url,hint32 defaultPort, InvokeTickDeclare){
	Url *u = (Url *)mem_malloc(sizeof(Url));
	int size  = (int)strlen(url) +1;
	char *p = (char *)url,s =0,tl=0;
	hbuffer_t protocol,domain,port,path,query,token;
	memset((void *)u,0,sizeof(Url));

	u->buffer = buffer_alloc(size, 128);

	protocol = buffer_alloc(10,  20);
	domain = buffer_alloc(size,  32);
	port = buffer_alloc(10,  10);
	path = buffer_alloc(size,  128);
	query = buffer_alloc(size,  128);
	token = buffer_alloc(128,  128);
	
	
	
	buffer_append_str(path, "/");
	
	while(*p != '\0'){
		
		if(s == 0){
			// protocol
			if( *p == ':'){
				s ++;
			}
			else{
				buffer_append(protocol, p, 1);
			}
		}
		else if(s == 1){
			// domain
			if( *p == '/'){
				if(tl ==2){
					s = 3;
				}
				else{
					tl ++;
				}
			}
			else if( *p == '?' ){
				s = 4;
			}
			else if( *p == '#'){
				s = 5;
			}
			else if( *p == ':'){
				s = 2;
			}
			else if(tl==2){
				buffer_append(domain,p,1);
			}
		}
		else if(s == 2){
			// port
			if( *p == '/'){
				s = 3;
			}
			else if( *p == '?'){
				s = 4;
			}
			else if( *p == '#'){
				s = 5;
			}
			else{
				buffer_append(port,p,1);
			}
		}
		else if(s == 3){
			// path
			if( *p == '?'){
				s = 4;
			}
			else if( *p == '#'){
				s = 5;
			}
			else{
				buffer_append(path, p, 1);
			}
		}
		else if(s == 4){
			// query
			if( *p == '#'){
				s = 5;
			}
			else{
				buffer_append(query,p,1);
			}
		}
		else if(s == 5){
			// token
			buffer_append(token, p, 1);
		}
		
		p++;
	}
	
	if(buffer_length(protocol)>0){
		u->protocol = mem_malloc( buffer_length(protocol) +1);
		strcpy(u->protocol,buffer_to_str(protocol));
	}
	
	if(buffer_length(domain)>0){
		u->domain = mem_malloc( buffer_length(domain)+1);
		strcpy(u->domain,buffer_to_str(domain));
	}
	
	if(defaultPort == URL_PORT_AUTO){
		if(str_cmp_non_case(u->protocol, "https") ==0){
			u->port = 443;
		}
		else{
			u->port = 80;
		}
	}
	else{
		u->port = defaultPort;
	}
	
	if(buffer_length(port)>0){
		int iport = atoi(buffer_to_str(port));
		if(iport>0){
			u->port = iport;
		}
	}
	
	if(buffer_length(path)>0){
		u->path = mem_malloc( buffer_length(path)+1);
		strcpy(u->path,buffer_to_str(path));
	}
			   
	if(buffer_length(query)>0){
		u->query = mem_malloc( buffer_length(query)+1);
		strcpy(u->query,buffer_to_str(query));
	}
	
	if(buffer_length(token)>0){
		u->token = mem_malloc( buffer_length(token) +1);
		strcpy(u->token,buffer_to_str(token));
	}
	
	buffer_dealloc(protocol);
	buffer_dealloc(domain);
	buffer_dealloc(port);
	buffer_dealloc(path);
	buffer_dealloc(query);
	buffer_dealloc(token);
	
	return (hurl_t)u;
}

static void url_param_dealloc_map_each(hmap_t map,hany key ,hany value,hany arg0,hany arg1, InvokeTickDeclare){
	url_param_dealloc((url_param_t *)value,InvokeTickArg);
}

void url_dealloc(hurl_t url, InvokeTickDeclare){
	if(url){
		Url *u = (Url *)url;
		mem_free(u->protocol);
		mem_free(u->domain);
		mem_free(u->path);
		mem_free(u->query);
		mem_free(u->token);
		list_dealloc(u->param_names);
		map_each(u->params, url_param_dealloc_map_each, NULL, NULL);
		map_dealloc(u->params);
		buffer_dealloc(u->buffer);
		mem_free(u);
	}
}

const char * url_protocol(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->protocol;
}

const char * url_domain(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->domain;
}

hint32 url_port(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->port;
}

const char * url_path(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->path;
}

const char * url_query(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->query;
}

const char * url_token(hurl_t url, InvokeTickDeclare){
	return ((Url *)url)->token;
}

const char * url_mask(hurl_t url,int mark, InvokeTickDeclare){
	Url *u = (Url *)url;
	hbuffer_t b = u->buffer;
	buffer_clear(b);
	
	if(u->protocol && (mark & URL_PROTOCOL)){
		buffer_append_str(b, u->protocol);
		buffer_append_str(b,"://");
	}
	
	if(u->domain && (mark & URL_DOMAIN)){
		buffer_append_str(b, u->domain);
	}
	
	if(mark & URL_PORT ){
		char sport[12]={0};
		sprintf(sport,"%d",u->port);
		if(buffer_length(b)>0){
			buffer_append_str(b, ":");
		}
		buffer_append_str(b, sport);
	}
	
	if(u->path && (mark & URL_PATH )){
		buffer_append_str(b, u->path);
	}
	
	if(u->query && (mark & URL_QUERY )){
		if(buffer_length(b)>0){
			buffer_append_str(b,"?");
		}
		buffer_append_str(b, u->query);
	}
	
	if(u->token && (mark & URL_TOKEN  )){
		if(buffer_length(b)>0){
			buffer_append_str(b,"#");
		}
		buffer_append_str(b, u->token);
	}
	
	
	return buffer_to_str(b);
}

static void url_param_parse(Url * url, InvokeTickDeclare){
	
	if(url->query && url->param_names == NULL){
		hchar * p = (hchar *)url->query,s=0;
		hbuffer_t name = buffer_alloc(128, 128);
		hbuffer_t value = buffer_alloc(1024,  1024);
		hbuffer_t value_decode = buffer_alloc(1024,  1024);
		url_param_t * param, *temp;
		url->param_names = list_alloc( 20, 20);
		url->params = map_alloc( hash_code_str, equal_str);
		
		while(*p != '\0'){
			if(s == 0){
				if( *p == '='){
					s = 1;
				}
				else{
					buffer_append(name, p,1);
				}
			}
			else if(s ==1){
				if( *p == '&'){
					url_decode(buffer_to_str(value),value_decode,InvokeTickArg);
					param = url_param_alloc(buffer_to_str(name), buffer_to_str(value_decode),InvokeTickArg);
					
					temp = map_put(url->params, param->name, param);
					if(temp){
						list_remove(url->param_names, temp->name);
						url_param_dealloc(temp,InvokeTickArg);
					}
					
					list_add(url->param_names, param->name);
					
					buffer_clear(name);
					buffer_clear(value);
					buffer_clear(value_decode);
					
					s = 0;
				}
				else{
					buffer_append(value, p ,1);
				}
			}
			p++;
		}
		
		if(buffer_length(name) > 0 || buffer_length(value) >0){
			url_decode(buffer_to_str(value),value_decode,InvokeTickArg);
			param = url_param_alloc( buffer_to_str(name), buffer_to_str(value_decode),InvokeTickArg);
			
			temp = map_put(url->params, param->name, param);
			if(temp){
				list_remove(url->param_names, temp->name);
				url_param_dealloc(temp,InvokeTickArg);
			}
			
			list_add(url->param_names, param->name);
			
		}
		
		buffer_dealloc(name);
		buffer_dealloc(value);
		buffer_dealloc(value_decode);
	}
	
}

hlist_t url_param_names(hurl_t url, InvokeTickDeclare){
	if(url){
		Url *u = (Url *)url;
		
		url_param_parse(u,InvokeTickArg);
		
		return u->param_names;
	}
	return NULL;
}

hcchar * url_param(hurl_t url,hcchar * name, InvokeTickDeclare){
	if(url){
		Url *u = (Url *)url;
		url_param_t * param;
		url_param_parse(u,InvokeTickArg);
		
		param = map_get(u->params, (hany)name);
		
		return param ? param->value : NULL;
	}
	return NULL;
}


struct in_addr  url_domain_resolv(hurl_t url, InvokeTickDeclare){
	Url *u = (Url *)url;
	
	if(u->addr.s_addr ==0){
		u->addr = url_resolv(u->domain);
	}
	
	return u->addr;
}

	void url_encode(const char *url,hbuffer_t buffer, InvokeTickDeclare){
		unsigned char *p ;
		char c[10] = {0};
		if(url && buffer){
			p = (unsigned char *)url;
			while(*p != '\0'){
                               if((*p<'0' || *p>'9') && (*p<'A' || *p >'Z') && (*p<'a' || *p>'z')){
					sprintf(c,"%%%02x",*p);
					buffer_append_str(buffer, c);
				}
				else {
					buffer_append(buffer,p,1);
				}
				p++;
			}
		}
	}
	
	void url_decode(const char *url,hbuffer_t buffer, InvokeTickDeclare){
		unsigned char *p,s=0,c;
		if(url && buffer){
			p = (unsigned char *)url;
			while(*p != '\0'){
				if(s == 0){
					if( *p =='+'){
						c = ' ';
						buffer_append(buffer,&c,1);
					}
					else if(*p == '%'){
						c = 0;
						s =1;
					}
					else{
						buffer_append(buffer,(void *)p,1);
					}
				}
				else if(s == 1 || s ==2 || s==3){
					if(s++ == 3){
						buffer_append(buffer, &c, 1);
						s = 0;
						continue;
					}
					if(*p >='0' && *p <='9'){
						c = c * 0x10 + (*p - '0');
					}
					else if(*p>='A' && *p <='F'){
						c = c * 0x10 + (*p - 'A' + 0x0A);
					}
					else if(*p>='a' && *p <='f'){
						c = c * 0x10 + (*p - 'a' + 0x0A);
					}
					else {
						s = 0;
						continue;
					}
				}
				
				p++;
			}
			
			if(s == 1 && c){
				buffer_append(buffer, &c, 1);
			}
		}
	}

struct in_addr url_resolv(const char *domain){
	struct in_addr addr;
	struct hostent *host;
	
	addr.s_addr = inet_addr(domain);
	
	if(addr.s_addr != INADDR_BROADCAST){
		return addr;
	}
	
	host = gethostbyname(domain);
	if(host && host->h_addr_list && host->h_length >0){
#if defined( __WIN32__) || defined( __ANDROID__)
		addr.s_addr = *(unsigned long *)host->h_addr_list[0];
#else
		addr.s_addr = *(in_addr_t *)host->h_addr_list[0];
#endif
	}
	
	return addr;
}
