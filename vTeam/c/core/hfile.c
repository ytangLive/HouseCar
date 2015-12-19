/*
 *  hfile.c
 *  C Library
 *
 *  Created by hailong zhang on 10-8-28.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hfile.h"

time_t file_last_modified_get(const char *filePath){
	struct stat cacheStat ;
	if(stat(filePath,&cacheStat) == 0){
		return cacheStat.st_mtime;
	}
	return 0;
}

void file_last_modified_set(const char *filePath,time_t t){
	struct utimbuf ut ;
	ut.actime = time(NULL);
	ut.modtime = t;
	utime(filePath,&ut);
}


hint32 file_size(const char *filePath){
	struct stat cacheStat = {0};
	stat(filePath,&cacheStat);
	return (hint32)cacheStat.st_size;
}

hint32 file_exist(const char *filePath){
	return access(filePath,   0)   ==   0;
}

const char * file_name_ext(const char * fileName){
	int len = (int)strlen(fileName);
	char * p = (char *)fileName + len;
	while(--p !=(char *) fileName){
		if(*p == '.'){
			break;
		}
	}
	return p;
}

const char * file_name(const char *file_path){
	int len = (int)strlen(file_path);
	char * p = (char *)file_path + len;
	while(--p !=(char *) file_path){
		if(*p == '/' || *p == '\\'){
			p ++;
			break;
		}
	}
	return p;
}

hbool file_directory_create(hcchar * directory){
	hint32 length = (hint32)strlen(directory);
	char * buffer = malloc(length +1);
	char *p = (char *)directory,*bp = buffer;
	memset(buffer,0,length+1);
	while(*p != '\0'){
		*bp = *p;
		if(*p == '/' || *p =='\\'){
			if(!file_exist(buffer)){
				mkdir(buffer, 0777);
			}
		}
		p ++;
		bp ++;
	}
	free(buffer);
	return file_exist(directory);
}

void file_delete(hcchar * filePath){
	unlink(filePath);
}

void file_rename(hcchar * filePath,hcchar * fileName){
	rename(filePath, fileName);
}
