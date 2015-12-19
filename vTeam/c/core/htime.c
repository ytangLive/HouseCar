/*
 *  time_ext.c
 *  C Library
 *
 *  Created by hailong zhang on 10-8-3.
 *  Copyright 2010 hailong.org. All rights reserved.
 *
 */

#include "hconfig.h"
#include "htime.h"

#define RFC1123_FORMAT "%3s, %02d %3s %4d %02d:%02d:%02d GMT"

//static const char *const rfc1123_weekdays[7] = {
//    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
//};

static const char *const short_months[12] = { 
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

time_t gmt_parse(const char *date)
{
    struct tm gmt ;
    char wkday[4], mon[4];
    int n;
	
	if(date == NULL){
		return time(NULL);
	}
	
	/*  it goes: Sun, 06 Nov 1994 08:49:37 GMT */
    n = sscanf(date, RFC1123_FORMAT,
			   wkday, &gmt.tm_mday, mon, &gmt.tm_year, &gmt.tm_hour,
			   &gmt.tm_min, &gmt.tm_sec);
    /* Is it portable to check n==7 here? */
    gmt.tm_year -= 1900;
    for (n=0; n<12; n++)
		if (strcmp(mon, short_months[n]) == 0)
			break;
    /* tm_mon comes out as 12 if the month is corrupt, which is desired,
     * since the mktime will then fail */
    gmt.tm_mon = n;
    gmt.tm_isdst = -1;
    return mktime(&gmt);
}

void time_to_gmt_str(time_t * time,hany buf,int length){
	strftime(buf,length,"%a, %d %b %Y %H:%M:%S GMT",localtime(time));
}

void time_to_format_str(time_t *time,hcchar *format,hchar *buf,hint32 length){
	strftime(buf,length,format,localtime(time));
}

