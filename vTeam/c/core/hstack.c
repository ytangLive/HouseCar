/*
 *  stack.c
 *  C Library
 *
 *  Created by hailong zhang on 10-8-5.
 *  Copyright 2010 hailong. All rights reserved.
 *
 */

#include "hconfig.h"
#include "hstack.h"

#undef stack_alloc

#undef stack_dealloc

#undef stack_push

#undef stack_pop

#undef stack_peek

#undef stack_number

#undef stack_clear

typedef struct{
	hany data;
	hany next;
}StackItem;

typedef struct{
	StackItem * begin;
	hint32 count;
}Stack;



hstack_t stack_alloc(InvokeTickDeclare){
	Stack * s = (Stack *)mem_malloc( sizeof(Stack)) ;
	memset(s,0,sizeof(Stack));
	return (hstack_t)s;
}

void stack_dealloc(hstack_t hStack,InvokeTickDeclare){
	if(hStack){
		Stack *stack = (Stack *)hStack;
		stack_clear(hStack,InvokeTickArg);
		mem_free( stack);
	}
}

void stack_clear(hstack_t hStack,InvokeTickDeclare){
	if(hStack){
		Stack * s= (Stack *)hStack;
		StackItem *i = s->begin,*n=NULL;
		while(i){
			n = (StackItem *)i->next;
			mem_free( i);
			i = n;
		}
		s->begin = NULL;
		s->count = 0;
	}
}
void stack_push(hstack_t hStack,hany item,InvokeTickDeclare){
	if(hStack ){
		Stack *s = (Stack *)hStack;
		StackItem *i = (StackItem *)mem_malloc(sizeof(StackItem));
		i->data = item;
		i->next = s->begin;
		s->begin = i;
		s->count ++;
	}
}

hany stack_pop(hstack_t hStack,InvokeTickDeclare){
	hany data = NULL;
	if(hStack){
		Stack *s = (Stack *)hStack;
		StackItem *i = s->begin;
		if(i){
			s->begin =i->next;
			data = i->data;
			mem_free( i);
			s->count --;
		}
	}
	return data;
}

hany stack_peek(hstack_t hStack,InvokeTickDeclare){
	if(hStack){
		Stack * s= (Stack *)hStack;
		return s->begin?s->begin->data:NULL;
	}
	return NULL;
}

hint32 stack_number(hstack_t hStack,InvokeTickDeclare){
	return hStack?((Stack *)hStack)->count:0;
}

