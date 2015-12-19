/*
 *  hcgi.h
 *  C Library
 *
 *  Created by hailong zhang on 11-3-9.
 *  Copyright 2011 hailong.org. All rights reserved.
 *
 */

#ifndef HCGI_H
#define HCGI_H

#ifdef __cplusplus
extern "C" {
#endif


#define CGI_CONTENT_TYPE	"CONTENT_TYPE"
#define CGI_CONTENT_LENGTH	"CONTENT_LENGTH"
#define CGI_QUERY_STRING	"QUERY_STRING"
#define CGI_HTTP_REFERER	"HTTP_REFERER"
#define CGI_REMOTE_HOST		"REMOTE_HOST"
#define CGI_REQUEST_METHOD	"REQUEST_METHOD"
#define CGI_SCRIPT_NAME		"SCRIPT_NAME"
#define CGI_SCRIPT_FILENAME	"SCRIPT_FILENAME"
#define CGI_SERVER_NAME		"SERVER_NAME"
#define CGI_SERVER_PORT		"SERVER_PORT"
#define CGI_SERVER_PROTOCOL	"SERVER_PROTOCOL"
    
#define CGI_REQUEST_URI     "REQUEST_URI"
    
#define CGI_HTTP_ACCEPT     "HTTP_ACCEPT"
#define CGI_HTTP_ACCEPT_CHARSET     "HTTP_ACCEPT_CHARSET"
	
#ifdef __cplusplus
}
#endif
	
#endif