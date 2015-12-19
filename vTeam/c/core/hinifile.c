/*
 *  hinifile.c
 *  C Library
 *
 *  Created by hailong zhang on 10-10-13.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hinifile.h"
#include "hfile.h"
#include "hbuffer.h"

#undef inifile_alloc

#undef inifile_dealloc

#undef inifile_read

#undef inifile_section

#undef inifile_key

#undef inifile_value

typedef struct{
	hbuffer_t key_buffer;
	hbuffer_t value_buffer;
	hbuffer_t section_buffer;
	hbuffer_t buffer;
	hchar * p;
	hint32 s;
}inifile_t;


hinifile_t inifile_alloc(hcchar * file_path,InvokeTickDeclare){
	inifile_t * inifile =NULL;
	hint32 file_length ;
	FILE * f;
	if((file_length = file_size(file_path))>0 &&(f=fopen(file_path, "r"))){
		inifile = (inifile_t *)mem_malloc( sizeof(inifile_t));
		memset(inifile,0,sizeof(inifile_t));

		inifile->buffer = buffer_alloc(file_length+1,  1024);
		
		if(1 == fread(buffer_data(inifile->buffer), file_length, 1, f)){
			buffer_length_set(inifile->buffer, file_length);
			inifile->p = (hchar *)buffer_to_str(inifile->buffer);
			inifile->key_buffer = buffer_alloc(128,  128);
			inifile->value_buffer = buffer_alloc(1024,1024);
			inifile->section_buffer = buffer_alloc(128,  256);
		}
		
		fclose(f);
	}
	
	return (hinifile_t)inifile;
}

void inifile_dealloc(hinifile_t hinifile,InvokeTickDeclare){
	if(hinifile){
		inifile_t * inifile = (inifile_t *)hinifile;
		buffer_dealloc(inifile->buffer);
		buffer_dealloc(inifile->key_buffer);
		buffer_dealloc(inifile->value_buffer);
		buffer_dealloc(inifile->section_buffer);
		mem_free(inifile);
	}
}

hbool inifile_read(hinifile_t hinifile,InvokeTickDeclare){
	if(hinifile){
		inifile_t * inifile = (inifile_t *)hinifile;
		hchar *p = inifile->p;
		buffer_clear(inifile->key_buffer);
		buffer_clear(inifile->value_buffer);
		while(p && *p !='\0'){
			if(inifile->s ==0){
				if(*p == '['){
					inifile->s =1;
					buffer_clear(inifile->section_buffer);
				}
			}
			else if(inifile->s==1){
				//section
				if(*p == ']'){
					inifile->s = 2;
				}
				else{
					buffer_append(inifile->section_buffer, p, 1);
				}
			}
			else if(inifile->s ==2){
				// key 
                if(*p == ' ' || *p == '\t' || *p == '\n' || *p =='\r'){
                    
                }
				else if(*p == '['){
					inifile->s =1;
					buffer_clear(inifile->section_buffer);
				}
				else if(*p == '='){
					inifile->s =3;
				}
				else {
					buffer_append(inifile->key_buffer, p, 1);
				}
			}
			else if(inifile->s ==3){
				// value
				if(*p == '\r'){
				}
				else if(*p == '\n'){
					inifile->s = 2 ;
					break;
				}
				else{
					buffer_append(inifile->value_buffer,p,1);
				}
			}
			p++;
		}
		inifile->p = p;
		return p != NULL && *p !='\0';
	}
	return hbool_false;
}

hcchar * inifile_section(hinifile_t inifile,InvokeTickDeclare){
	return inifile?buffer_to_str(((inifile_t *)inifile)->section_buffer):NULL;
}

hcchar * inifile_key(hinifile_t inifile,InvokeTickDeclare){
	return inifile?buffer_to_str(((inifile_t *)inifile)->key_buffer):NULL;
}

hcchar * inifile_value(hinifile_t inifile,InvokeTickDeclare){
	return inifile?buffer_to_str(((inifile_t *)inifile)->value_buffer):NULL;
}
