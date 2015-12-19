/*
 *  hclass.h
 *  C Library
 *
 *  Created by hailong zhang on 10-10-10.
 *  Copyright 2010 hailongz. All rights reserved.
 *
 */




#ifndef HCLASS_H
#define HCLASS_H

#include "hmem.h"
#include "hext_obj.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	HANDLER(hclass_t)
	HANDLER(hobject_t)
	
	typedef void (*hclass_init_t)(hclass_t hclass,hobject_t obj,hany ptr,InvokeTickDeclare);
	
	typedef void (*hclass_destroy_t)(hclass_t hclass,hobject_t obj,hany ptr,InvokeTickDeclare);
	
	hclass_t hclass_alloc(hclass_t super_class,hint32 size,hclass_init_t init,hclass_destroy_t destroy,InvokeTickDeclare);
	
	hclass_t hclass_super_class(hclass_t hclass,InvokeTickDeclare);
    
    hbool hclass_is_kind_class(hclass_t subclass,hclass_t hclass,InvokeTickDeclare);
	
	hint32 hclass_size(hclass_t hclass,InvokeTickDeclare);
	
	hint32 hclass_total_size(hclass_t hclass,InvokeTickDeclare);
	
	
	extern ext_class_t ext_hobject_class;
	

#define hobject_alloc(class)	(hobject_t)ext_obj_alloc(class, &ext_hobject_class)
#define hobject_retain(obj)			ext_obj_retain((hext_obj_t)(obj))
#define hobject_retain_count(obj)	ext_obj_retain_count((hext_obj_t)(obj))
#define hobject_release(obj)		ext_obj_release((hext_obj_t)(obj))
	
	hclass_t hobject_class(hobject_t hobj,InvokeTickDeclare);
	
	hclass_t hobject_super_class(hobject_t hobj,InvokeTickDeclare);
    
    hbool hobject_is_kind_class(hobject_t hobj,hclass_t hclass,InvokeTickDeclare);
	
	hany hobject_ptr(hobject_t hobj,InvokeTickDeclare);
	
	hany hobject_ptr_by_super(hobject_t hobj,InvokeTickDeclare);
	
	hany hobject_ptr_by_class(hobject_t hobj,hclass_t hclass,InvokeTickDeclare);
	
   
   
    
#define hclass_alloc(a,b,c,d) hclass_alloc((a),(b),(c),(d),InvokeTickArg)
	
#define hclass_super_class(a) hclass_super_class((a),InvokeTickArg)
    
#define hclass_is_kind_class(a,b) hclass_is_kind_class((a),(b),InvokeTickArg)
	
#define hclass_size(a) hclass_size((a),InvokeTickArg)
	
#define hclass_total_size(a) hclass_total_size((a),InvokeTickArg)
    
#define hobject_class(a) hobject_class((a),InvokeTickArg)
	
#define hobject_super_class(a) hobject_super_class((a),InvokeTickArg)
    
#define hobject_is_kind_class(a,b) hobject_is_kind_class((a),(b),InvokeTickArg)
	
#define hobject_ptr(a) hobject_ptr((a),InvokeTickArg)
	
#define hobject_ptr_by_super(a) hobject_ptr_by_super((a),InvokeTickArg)
	
#define hobject_ptr_by_class(a,b) hobject_ptr_by_class((a),(b),InvokeTickArg)
    
    
#define hobject_struct_ptr(obj,struct)					((struct *)(hobject_ptr(obj)))
#define hobject_struct_ptr_by_super(obj,struct)			((struct *)(hobject_ptr_by_super(obj)))
#define hobject_struct_ptr_by_class(obj,struct,class)	((struct *)(hobject_ptr_by_class(obj,class)))
    

	
#ifdef __cplusplus
}
#endif


#endif
