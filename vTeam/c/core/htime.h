/*
 *  time_ext.h
 *  C Library
 *
 *  Created by hailong zhang on 10-8-3.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */

#ifndef HTIME_H
#define HTIME_H

#ifdef __cplusplus
extern "C" {
#endif
	
	time_t gmt_parse(hcchar *date);

	void time_to_gmt_str(time_t *time,hany buf,int length);

	time_t file_last_modified_get(hcchar *filePath);
	
	void file_last_modified_set(hcchar *filePath,time_t t);
	
	void time_to_format_str(time_t *time,hcchar *format,hchar *buf,hint32 length);
	
#ifdef __cplusplus
}
#endif

#endif
