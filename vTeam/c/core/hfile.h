/*
 * hfile.h
 *
 *  Created on: 2010-8-27
 *      Author: Administrator
 */

#ifndef HFILE_H_
#define HFILE_H_

#ifdef __cplusplus
extern "C" {
#endif
	
	hint32 file_exist(hcchar *filePath);

	hint32 file_size(hcchar *filePath);

	time_t file_last_modified_get(hcchar *filePath);

	void file_last_modified_set(hcchar *filePath,time_t t);

	hcchar * file_name_ext(hcchar * file_path);
	
	hcchar * file_name(hcchar *file_path);

	hbool file_directory_create(hcchar * directory);
	
	void file_delete(hcchar * filePath);
	
	void file_rename(hcchar * filePath,hcchar * fileName);
	
#ifdef __cplusplus
}
#endif

#endif /* HFILE_H_ */
