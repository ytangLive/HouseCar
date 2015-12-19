//
//  hvermin_compile_scanner.c
//  C Library
//
//  Created by Hailong Zhang on 11-10-24.
//  Copyright (c) 2011年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "hvermin_compile_scanner.h"
#include "hvermin_compile_expression.h"
#include "hlog.h"
#include "hlist.h"

#undef vmScannerCompileBufferExpressionAppend

#undef vmScannerCompileBufferInvokeAppend

#undef vmScannerCompileBufferClear

vmScannerFilterState vmScannerCompileBufferExpressionAppend(vmScannerContext * context,vmScannerCompileBuffer * buffer,hchar c,hchar endChar,InvokeTickDeclare){
   
    switch (buffer->state) {
        case 0:
            if(c == '"'){
                buffer_append(buffer->value, &c, 1);
                buffer->state ++;
            }
            else if(c == endChar){
                if(buffer->level ==0){
                    return vmScannerFilterStateMatch;
                }
                else if(endChar != ')'){
                    return vmScannerFilterStateError;
                }
                else{
                    if(c == ')'){
                        buffer->level --;
                    }
                    buffer_append(buffer->value, &c, 1);
                }
            }
            else if(c == '('){
                buffer_append(buffer->value, &c, 1);
                buffer->level ++;
            }
            else if(c == ')'){
                buffer_append(buffer->value, &c, 1);
                if(--buffer->level  <0){
                    return vmScannerFilterStateError;
                }
            }
            else if(c == '{' || c == '}'){
                return vmScannerFilterStateError;
            }
            else{
                buffer_append(buffer->value, &c, 1);
            }
            break;
        case 1:
            if(c == '\\'){
                buffer_append(buffer->value, &c, 1);
                buffer->state ++;
            }
            else if(c == '"'){
                buffer_append(buffer->value, &c, 1);
                buffer->state = 0;
            }
            else{
                buffer_append(buffer->value, &c, 1);
            }
            break;
        case 2:
            buffer_append(buffer->value, &c, 1);
            buffer->state = 1;
            break;
        default:
            break;
    }
    return vmScannerFilterStateNone;
}

vmScannerFilterState vmScannerCompileBufferInvokeAppend(vmScannerContext * context,vmScannerCompileBuffer * buffer,hchar c,InvokeTickDeclare){
    switch (buffer->state) {
        case 0:
            if(c == '"'){
                buffer_append(buffer->value, &c, 1);
                buffer->state ++;
            }
            else if(c == '{'){
                buffer_append(buffer->value, &c, 1);
                buffer->level ++;
            }
            else if(c == '}'){
                if(buffer->level==0){
                    return vmScannerFilterStateMatch;
                }
                else{
                    buffer->level --;
                    buffer_append(buffer->value, &c, 1);
                }
            }
            else{
                buffer_append(buffer->value, &c, 1);
            }
            break;
        case 1:
            if(c == '\\'){
                buffer_append(buffer->value, &c, 1);
                buffer->state ++;
            }
            else if(c == '"'){
                buffer_append(buffer->value, &c, 1);
                buffer->state = 0;
            }
            else{
                buffer_append(buffer->value, &c, 1);
            }
            break;
        case 2:
            buffer_append(buffer->value, &c, 1);
            buffer->state = 1;
            break;
        default:
            break;
    }
    return vmScannerFilterStateNone;
}

void vmScannerCompileBufferClear(vmScannerCompileBuffer * buffer,InvokeTickDeclare){
    buffer->state =0;
    buffer->level = 0;
    buffer->line = 0;
    buffer->index = 0;
    buffer_clear(buffer->value);
}



#undef vmScannerClassContextInit

#undef vmScannerClassContextDestroy

#undef vmScannerInvokeContextInit

#undef vmScannerInvokeContextDestroy


static vmScannerResult vmScannerNoteClearFilterCallback(vmScannerContext * context,vmScannerNoteClearFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};

    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '/'){
                filter->base.state = 1;
                context->isContinue = hbool_false;
            }
            else if(c == '"'){
                filter->base.state = 4;
            }
            break;
        case 1:
            if(c == '/'){
                filter->base.state = 2;
                context->isContinue = hbool_false;
            }
            else if(c == '*'){
                filter->base.state = 3;
                context->isContinue = hbool_false;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                context->step = -2;
                context->isContinue = hbool_false;
            }
            break;
            
        case 2:     // single line begin
            context->isContinue = hbool_false;
            if(c == '\n'){
                filter->base.state = 0;
            }
            break;
        case 3:     // multi line begin
            context->isContinue = hbool_false;
            if(c == '*'){
                filter->base.state = 6;
            }
            break;
        case 4:     // string begin
            if(c == '\\'){
                filter->base.state = 5;
            }
            else if(c == '"'){
                filter->base.state = 0;
            }
            break;
        case 5:
            filter->base.state = 4;
            break;
        case 6:
            context->isContinue = hbool_false;
            if(c == '/'){
                filter->base.state = 0;
            }
            else{
                filter->base.state = 3;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerNoteClearFilterReset(vmScannerContext * context,vmScannerNoteClearFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    filter->base.state = 0;
    return rs;
}

static vmScannerResult vmScannerPropertyFilterCallback(vmScannerContext * context,vmScannerPropertyFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else{
                if(c == 'v'){
                    filter->base.state = 1;
                }
                else{
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    strcpy(filter->base.base.error, "Property");
                }
            }
            break;
        case 1:
            if(c == 'a'){
                filter->base.state = 2;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break;  
        case 2:
            if(c == 'r'){
                filter->base.state = 3;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break; 
        case 3:
            if(ISSPACECHAR(c)){
                filter->base.state = 4;
            }
            else if(c == '('){ // var(type)
                filter->base.state = 9;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break; 
        case 4: // var 
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                filter->base.state = 5;
                buffer_append(filter->name, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break; 
        case 5:  // name
            if(ISSPACECHAR(c)){
                filter->base.state = 6;
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(ISNAME(c)){
                buffer_append(filter->name, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break; 
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(c == '='){
                filter->base.state = 7;
            }
            break;
        case 7:
            if(ISSPACECHAR(c)){
                
            }
            else{
                filter->value.line = context->line;
                filter->value.index = context->index;
                filter->base.state ++;
                vmScannerCompileBufferExpressionAppend(context, &filter->value, c, ';',InvokeTickArg);
            }
            break;
        case 8:
            filter->base.base.state = vmScannerCompileBufferExpressionAppend(context, &filter->value, c, ';',InvokeTickArg);
            if(filter->base.base.state == vmScannerFilterStateError){
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break;
        case 9:
            if(c == ')'){ // var(assign)
                filter->base.state = 10;
            }
            else if(ISNAME(c)){
                buffer_append(filter->type, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break;
        case 10:
            if(ISSPACECHAR(c)){
                filter->base.state = 4;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                strcpy(filter->base.base.error, "Property");
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerPropertyFilterReset(vmScannerContext * context,vmScannerPropertyFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileClassMeta * classMeta = context->extra;
        vmRuntimeOperatorType opType = strcmp("assign", buffer_to_str(filter->type)) ==0 ? vmRuntimeOperatorTypeVarAssign : vmRuntimeOperatorTypeVar;
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(opType, buffer_to_str(filter->name));
        vmCompileMeta * meta = NULL;
        
        if(buffer_length(filter->value.value)){
            if(vmCompileExpressionString(buffer_to_str(filter->value.value), &meta)){
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = context->line;
                rs.error.index = context->index;
                mem_strcpy(rs.error.error, "property exp");
            }
        }
        
        if(rs.state == vmScannerResultStateOK){
            vmCompileObjectArrayAdd(classMeta->propertys, op);
        }

        vmCompileObjectRelease(op);
        
        hlog("var %s = %s;\n",buffer_to_str(filter->name),buffer_to_str(filter->value.value));
    }
    filter->base.state = 0;
    buffer_clear(filter->type);
    buffer_clear(filter->name);
    vmScannerCompileBufferClear(&filter->value, InvokeTickArg);
    
    return rs;
}

static vmScannerResult vmScannerFunctionFilterCallback(vmScannerContext * context,vmScannerFunctionFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "function";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                buffer_append(filter->name, &c, 1);
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                filter->base.state ++;
            }
            else if(c == '('){
                filter->base.state = 11;
            }
            else if(ISNAME(c)){
                buffer_append(filter->name, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 10:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 11:
            
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->args, c, ')',InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state ++;
            }
            break;
        case 12:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(c == '{'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            break;
        case 13:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Function");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    return result;
}

static hbool _vmCompileScannerIsName(hcchar * name){
    if(name){
        hchar * p = (hchar *) name;
        if(ISNAMEFIRST(*p)){
            p ++;
            while(*p !=0){
                if(!ISNAME(*p)){
                    return hbool_false;
                }
                p++;
            }
            return hbool_true;
        }
    }
    return hbool_false;
}

static vmScannerResult vmScannerFunctionFilterReset(vmScannerContext * context,vmScannerFunctionFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileClassMeta * classMeta = context->extra;
        hlist_t argsList = list_alloc(10, 10);
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeFunction, buffer_to_str(filter->name));
        vmCompileMeta * meta = NULL;
        hint32 i,c;

        if(buffer_length(filter->args.value)){
            list_split_str(argsList, buffer_to_str(filter->args.value), ",");
            c = list_count(argsList);
            for(i=0;i<c;i++){
                if(_vmCompileScannerIsName(list_get(argsList, i))){
                    meta = vmCompileMetaCreateWithArg(list_get(argsList, i));
                    vmCompileMetaOperatorAddCompileMeta(op, meta);
                    vmCompileObjectRelease(meta);
                }
            }
            list_split_free(argsList);
            
        }
        
        list_dealloc(argsList);
        
        hlog("function %s(%s){\n",buffer_to_str(filter->name),buffer_to_str(filter->args.value));
		{
			vmScannerInvokeContext invokeContext;
        
			vmScannerSource source = {vmScannerSourceTypeBuffer,0};
        
			source.source.buffer.data = buffer_data(filter->invoke.value);
			source.source.buffer.length = buffer_length(filter->invoke.value);
        
			vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
			invokeContext.base.line = filter->invoke.line;
			invokeContext.base.index = filter->invoke.index;
        
        
			rs = vmScanner(&invokeContext.base,source);

			if(rs.state == vmScannerResultStateError){
				filter->base.base.state = vmScannerFilterStateError;
				filter->base.base.line = rs.error.line;
				filter->base.base.index = rs.error.index;
				strcpy(filter->base.base.error,rs.error.error);
			}
        
			hlog("}\n\n");
        
			if(rs.state == vmScannerResultStateOK){
				meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
				vmCompileMetaOperatorAddCompileMeta(op, meta);
				vmCompileObjectRelease(meta);
            
				vmCompileObjectArrayAdd(classMeta->functions, op);
			}
                
			vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
		}
        vmCompileObjectRelease(op);
    }

    filter->base.state = 0;
    buffer_clear(filter->name);
    vmScannerCompileBufferClear(&filter->args, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    
    return rs;
}

static vmScannerResult vmScannerExtendsFilterCallback(vmScannerContext * context,vmScannerExtendsFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    
    static hchar tag[] = "extends";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 7:
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                buffer_append(filter->name, &c, 1);
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 8:
            if(ISNAME(c)){
                buffer_append(filter->name, &c, 1);
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                filter->base.state ++;
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        default:
            break;
    }
    return result;
}

static vmScannerResult vmScannerExtendsFilterReset(vmScannerContext * context,vmScannerExtendsFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileClassMeta * classMeta = context->extra;
        hlog("extends %s;\n",buffer_to_str(filter->name));
        buffer_clear(classMeta->superClass);
        buffer_append_str(classMeta->superClass, buffer_to_str(filter->name));
    }
    
    filter->base.state = 0;
    buffer_clear(filter->name);
    return rs;
}


hbool vmScannerClassContextInit(vmScannerClassContext * context,InvokeTickDeclare){
    memset(context,0, sizeof(vmScannerClassContext));
    
    context->base.extra = vmCompileClassMetaCreate();
    context->base.size = sizeof(vmScannerClassContext);
    
    context->propertyFilter.name = buffer_alloc(20,  20);
    context->propertyFilter.type = buffer_alloc(20, 20);
    context->propertyFilter.value.value = buffer_alloc(20,  20);
    context->propertyFilter.base.base.size = sizeof(vmScannerPropertyFilter);
    context->propertyFilter.base.base.callback = (vmScannerCallback)vmScannerPropertyFilterCallback;
    context->propertyFilter.base.base.reset = (vmScannerReset)vmScannerPropertyFilterReset;
    
    context->functionFilter.base.base.size = sizeof(vmScannerFunctionFilter);
    context->functionFilter.name = buffer_alloc(20,  20);
    context->functionFilter.args.value = buffer_alloc(20,  20);
    context->functionFilter.invoke.value = buffer_alloc(1024,  1024);
    
    context->functionFilter.base.base.callback = (vmScannerCallback)vmScannerFunctionFilterCallback;
    context->functionFilter.base.base.reset = (vmScannerReset)vmScannerFunctionFilterReset;
    
    context->noteClearFilter.base.base.size = sizeof(vmScannerNoteClearFilter);
    context->noteClearFilter.base.base.callback = (vmScannerCallback)vmScannerNoteClearFilterCallback;
    context->noteClearFilter.base.base.reset = (vmScannerReset)vmScannerNoteClearFilterReset;
    
    context->extendsFilter.name = buffer_alloc(20,  20);
    context->extendsFilter.base.base.size =sizeof(vmScannerExtendsFilter);
    context->extendsFilter.base.base.callback = (vmScannerCallback)vmScannerExtendsFilterCallback;
    context->extendsFilter.base.base.reset = (vmScannerReset)vmScannerExtendsFilterReset;
    
    return hbool_true;
}

void vmScannerClassContextDestroy(vmScannerClassContext * context,InvokeTickDeclare){
    
    buffer_dealloc(context->propertyFilter.name);
    context->propertyFilter.name = NULL;
    buffer_dealloc(context->propertyFilter.value.value);
    context->propertyFilter.value.value = NULL;
    buffer_dealloc(context->propertyFilter.type);
    context->propertyFilter.type = NULL;
    
    context->propertyFilter.base.base.callback = NULL;
    context->propertyFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->functionFilter.name);
    context->functionFilter.name = NULL;
    buffer_dealloc(context->functionFilter.args.value);
    context->functionFilter.args.value = NULL;
    buffer_dealloc(context->functionFilter.invoke.value);
    context->functionFilter.invoke.value = NULL;
    
    context->functionFilter.base.base.callback = NULL;
    context->functionFilter.base.base.reset = NULL;
    
    context->noteClearFilter.base.base.callback = NULL;
    context->noteClearFilter.base.base.reset = NULL;
    
    
    buffer_dealloc(context->extendsFilter.name);
    context->extendsFilter.name = NULL;
    
    context->extendsFilter.base.base.callback = NULL;
    context->extendsFilter.base.base.reset = NULL;
    
    vmCompileObjectRelease(context->base.extra);
    
    context->base.extra = NULL;
}

/**
 Return
 */

static vmScannerResult vmScannerReturnFilterCallback(vmScannerContext * context,vmScannerReturnFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "return";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                
                if(filter->base.state == 6){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "return");
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c ==';'){
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else {
                 filter->base.state ++ ;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);

                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "return");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 7:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);
            
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "return");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }

    
    return result;
}

static vmScannerResult vmScannerReturnFilterReset(vmScannerContext * context,vmScannerReturnFilter * filter,InvokeTickDeclare){
    
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMeta * meta;
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
            if(!meta){
                meta = vmCompileMetaCreate();
            }
            meta->type = meta->type | vmRuntimeMetaTypeReturn;
            vmCompileMetaOperatorAddCompileMeta(invoke, meta);
            vmCompileObjectRelease(meta);
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "return exp");
        }
        
        
        hlog("return %s;\n",buffer_to_str(filter->exp.value));
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp, InvokeTickArg);
    return rs;
}

/**
 Break
 */

static vmScannerResult vmScannerBreakFilterCallback(vmScannerContext * context,vmScannerBreakFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;

    static hchar tag[] = "break";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                
                if(filter->base.state == 5){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "break");
            }
            break;
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c ==';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "break");
            }
            break;
        default:
            break;
    }
    
    
    return result;
}

static vmScannerResult vmScannerBreakFilterReset(vmScannerContext * context,vmScannerBreakFilter * filter,InvokeTickDeclare){
    
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMeta * meta = vmCompileMetaCreate();
        
        meta->type = meta->type |  vmRuntimeMetaTypeBreak;
        vmCompileMetaOperatorAddCompileMeta(invoke, meta);
        
        vmCompileObjectRelease(meta);

        hlog("break;\n");
    }
    filter->base.state = 0;
    return rs;
}


/**
 Continue
 */

static vmScannerResult vmScannerContinueFilterCallback(vmScannerContext * context,vmScannerContinueFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    
    static hchar tag[] = "continue";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                if(filter->base.state == 8){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "continue");
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                
            }
            else if(c ==';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "continue");
            }
            break;
        default:
            break;
    }
    
    
    return result;
}

static vmScannerResult vmScannerContinueFilterReset(vmScannerContext * context,vmScannerContinueFilter * filter,InvokeTickDeclare){
    
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMeta * meta = vmCompileMetaCreate();
        
        meta->type = meta->type |  vmRuntimeMetaTypeContinue;
        vmCompileMetaOperatorAddCompileMeta(invoke, meta);
        
        vmCompileObjectRelease(meta);
        
        hlog("continue;\n");
    }
    filter->base.state = 0;
    return rs;
}

/**
 Var
 */

static vmScannerResult vmScannerVarFilterCallback(vmScannerContext * context,vmScannerVarFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    
    static hchar tag[] = "var";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Var");
            }
            break;
        case 1:
        case 2:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                if(filter->base.state == 3){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "Var");
            }
            break;
        case 3:
            if(ISSPACECHAR(c)){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index; 
                mem_strcpy(filter->base.base.error, "Var");
            }
            break;
        case 4:
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                buffer_append(filter->name, &c, 1);
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;  
                mem_strcpy(filter->base.base.error, "Var");
            }            
            break;
        case 5:
            if(ISSPACECHAR(c)){
                filter->base.state ++;
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(c == '='){
                filter->base.state = 7;
            }
            else if(ISNAME(c)){
                buffer_append(filter->name, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index; 
                mem_strcpy(filter->base.base.error, "Var");
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(c == '='){
                filter->base.state = 7;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index; 
                mem_strcpy(filter->base.base.error, "Var");
            }            
            break;
        case 7:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                filter->base.base.state = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);
                if(filter->base.base.state == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;  
                    mem_strcpy(filter->base.base.error, "Var");
                }
            }
            break;
        case 8:
            filter->base.base.state = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);
            if(filter->base.base.state == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;  
                mem_strcpy(filter->base.base.error, "Var");
            }
            break;
        default:
            break;
    }
    
    
    return result;
}

static vmScannerResult vmScannerVarFilterReset(vmScannerContext * context,vmScannerVarFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        if(_vmCompileScannerIsName(buffer_trim(filter->name))){
            if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
                op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeVar, buffer_to_str(filter->name));
                if(meta){
                    vmCompileMetaOperatorAddCompileMeta(op, meta);
                    vmCompileObjectRelease(meta);
                }
                meta = vmCompileMetaCreateWithOperator(op);
                vmCompileObjectRelease(op);
                vmCompileMetaOperatorAddCompileMeta(invoke,meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = filter->exp.line;
                rs.error.index = filter->exp.index;
                mem_strcpy(rs.error.error, "var exp");
            }
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "var name");
        }
        
        hlog("var %s = %s;\n",buffer_to_str(filter->name),buffer_to_str(filter->exp.value));
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp,InvokeTickArg);
    buffer_clear(filter->name);
    

    return rs;
}

/**
 Statement
 */

static vmScannerResult vmScannerStatementFilterCallback(vmScannerContext * context,vmScannerStatementFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    
    switch (filter->base.state) {
        case 0:
            if(c == '"'){
                buffer_append(filter->expression, &c, 1);
                filter->base.state ++;
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                buffer_append(filter->expression, &c, 1);
            }
            break;
        case 1:
            if(c == '\\'){
                filter->base.state ++;
                buffer_append(filter->expression, &c, 1);
            }
            else if(c == '"'){
                filter->base.state = 0;
                buffer_append(filter->expression, &c, 1);
            }
            else{
                buffer_append(filter->expression, &c, 1);
            }
            break;
        case 2:
            filter->base.state = 1;
            buffer_append(filter->expression, &c, 1);
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerStatementFilterReset(vmScannerContext * context,vmScannerStatementFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMeta * meta = NULL;
        
        if(vmCompileExpressionString(buffer_to_str(filter->expression), &meta)){
            if(meta){
                vmCompileMetaOperatorAddCompileMeta(invoke, meta);
                vmCompileObjectRelease(meta);
            }
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            
            mem_strcpy(rs.error.error, "statement exp");
        }
        
        hlog("%s;\n",buffer_to_str(filter->expression));
    }
    filter->base.state = 0;
    buffer_clear(filter->expression);
    return rs;
}

/**
 While
 */

static vmScannerResult vmScannerWhileFilterCallback(vmScannerContext * context,vmScannerWhileFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "while";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                if(filter->base.state == 5){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "while");
            }
            break;
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ')'){
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                filter->base.state = 8;
            }
            else{
                filter->base.state ++;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "while");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state = 8;
                }
            }
            break;
        case 7:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "while");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state = 8;
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->base.state ++;
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state = 11;
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '}'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "while");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 10:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "while");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        case 11:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "while");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 12:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "while");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerWhileFilterReset(vmScannerContext * context,vmScannerWhileFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        hlog("while(%s){\n",buffer_to_str(filter->exp.value));
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
         
            
            op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeWhile, NULL);
            
            if(!meta){
                meta = vmCompileMetaCreateWithBoolean(hbool_false);
            }
            
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
            
            {
                vmScannerInvokeContext invokeContext;
                
                vmScannerSource source = {vmScannerSourceTypeBuffer,0};

                source.source.buffer.data = buffer_data(filter->invoke.value);
                source.source.buffer.length = buffer_length(filter->invoke.value);
                
                
                vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
                invokeContext.base.line = filter->invoke.line;
                invokeContext.base.line = filter->invoke.index;
                
                
                rs = vmScanner(&invokeContext.base,source);
                               
                if(rs.state == vmScannerResultStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = rs.error.line;
                    filter->base.base.index = rs.error.index;
                    strcpy(filter->base.base.error,rs.error.error);
                }
                

                if(rs.state == vmScannerResultStateOK){
                    meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                    vmCompileMetaOperatorAddCompileMeta(op, meta);
                    vmCompileObjectRelease(meta);
                    
                    meta = vmCompileMetaCreateWithOperator(op);
                    vmCompileMetaOperatorAddCompileMeta(invoke, meta);
                    vmCompileObjectRelease(meta);
                }
                
                vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
                
            }
            
            
            vmCompileObjectRelease(op);
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = filter->exp.line;
            rs.error.index = filter->exp.index;
            mem_strcpy(rs.error.error, "while exp");
        }

        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    
    return rs;
        
}


/**
 For
 */

static vmScannerResult vmScannerForFilterCallback(vmScannerContext * context,vmScannerForFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "for";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            break;
        case 1:
        case 2:
            if(c == tag[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 3){
                    filter->base.state = 5;
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            break;

        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->exp1.line = context->line;
                filter->exp1.index = context->index;
                filter->base.state += 2;
            }
            else{
                filter->base.state += 1;
                filter->exp1.line = context->line;
                filter->exp1.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp1, c, ';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state += 1;
                }
            }
            break;
        case 7:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp1, c, ';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state += 1;
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->exp2.line = context->line;
                filter->exp2.index = context->index;
                filter->base.state += 2;
            }
            else{
                filter->base.state += 1;
                filter->exp2.line = context->line;
                filter->exp2.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp2, c, ';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state += 1;
                }
            }
            break;
        case 9:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp2, c, ';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state += 1;
            }
            break;
        case 10:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->exp3.line = context->line;
                filter->exp3.index = context->index;
                filter->base.state += 2;
            }
            else{
                filter->base.state += 1;
                filter->exp3.line = context->line;
                filter->exp3.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp3, c, ')', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state += 1;
                }
            }
            break;
        case 11:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp3, c, ')', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state += 1;
            }
            break;
        case 12:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->base.state += 1; 
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);

            }
            else{
                filter->base.state += 3; 
            }
            break;
        case 13:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '}'){
                filter->base.base.state = vmScannerFilterStateMatch;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
            }
            else{
                filter->base.state += 1;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 14:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        case 15:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            }
            else{
                filter->base.state += 1;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 16:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerForFilterReset(vmScannerContext * context,vmScannerForFilter * filter,InvokeTickDeclare){
    vmScannerResult rs ={vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeFor, NULL);
        vmCompileMeta * meta;
        
        hlog("for(%s;%s;%s){\n",buffer_to_str(filter->exp1.value),buffer_to_str(filter->exp2.value),buffer_to_str(filter->exp3.value));
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp1.value), &meta)){
            if(!meta){
                meta = vmCompileMetaCreate();
            }
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "for exp1");
        }
        
        if(rs.state == vmScannerResultStateOK){
            if(vmCompileExpressionString(buffer_to_str(filter->exp2.value), &meta)){
                if(!meta){
                    meta = vmCompileMetaCreate();
                }
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = context->line;
                rs.error.index = context->index;
                mem_strcpy(rs.error.error, "for exp2");
            }
        }
        
        if(rs.state == vmScannerResultStateOK){
            if(vmCompileExpressionString(buffer_to_str(filter->exp3.value), &meta)){
                if(!meta){
                    meta = vmCompileMetaCreate();
                }
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = context->line;
                rs.error.index = context->index;
                mem_strcpy(rs.error.error, "for exp3");
            }
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};
            
            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);
            
            
            
            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }

            if(rs.state == vmScannerResultStateOK){
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
                
                meta = vmCompileMetaCreateWithOperator(op);
                vmCompileMetaOperatorAddCompileMeta(invoke, meta);
                vmCompileObjectRelease(meta);
            }
            
            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
        }
        
        vmCompileObjectRelease(op);
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp1, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->exp2, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->exp3, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    return rs;
    
}

/**
 For in
 */

static vmScannerResult vmScannerForInFilterCallback(vmScannerContext * context,vmScannerForInFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "for";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
            if(c == tag[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 3){
                    filter->base.state = 5;
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
                
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
            
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
                filter->exp1.line = context->line;
                filter->exp1.index = context->index;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for in");
            }
            break;
        case 6:
            if(c == 'i'){
                filter->base.state ++;
            }
            else{
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp1, c, ')', InvokeTickArg);
                if(retState != vmScannerFilterStateNone){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
            }
            break;
        case 7:
            if(c == 'n'){
                filter->base.state ++;
                invokeContext->forFilter.base.base.state = vmScannerFilterStateError;
            }
            else{
                filter->base.state --;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp1, 'i', ')', InvokeTickArg);
                if(retState != vmScannerFilterStateNone){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp1, c, ')', InvokeTickArg);
                if(retState != vmScannerFilterStateNone){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for in");
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                
            }
            else{
                filter->base.state ++;
                filter->exp2.line = context->line;
                filter->exp2.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp2, c, ')', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
                else if(retState ==vmScannerFilterStateMatch){
                    filter->base.state +=1;
                }
            }
            break;
        case 10:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp2, c, ')', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for in");
            }
            else if(retState ==vmScannerFilterStateMatch){
                filter->base.state +=1;
            }
            break;
        case 11:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->base.state ++;
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state +=3;
            }
            break;
        case 12:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '}'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
                else if(retState ==vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 13:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for in");
            }
            else if(retState ==vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        case 14:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "for in");
                }
                else if(retState ==vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 15:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "for in");
            }
            else if(retState ==vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerForInFilterReset(vmScannerContext * context,vmScannerForInFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeForeach,NULL);
        vmCompileMeta * meta;
        
        hlog("for(%s in %s){\n",buffer_to_str(filter->exp1.value),buffer_to_str(filter->exp2.value));
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp1.value), &meta)){
            if(meta){
                
                if(meta->type == vmRuntimeMetaTypeObject){
                    buffer_append_str(op->uniqueKey,buffer_to_str(meta->value.objectKey));
                }
                else{
                    rs.state = vmScannerResultStateError;
                    rs.error.line = context->line;
                    rs.error.index =context->index;
                    mem_strcpy(rs.error.error, "foreacn exp1 not is object");
                }
                
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = context->line;
                rs.error.index =context->index;
                mem_strcpy(rs.error.error, "foreacn exp1");
            }
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index =context->index;
            mem_strcpy(rs.error.error, "foreach exp1");
        }
        
        if(rs.state == vmScannerResultStateOK){
            if(vmCompileExpressionString(buffer_to_str(filter->exp2.value), &meta)){
                if(!meta){
                    meta = vmCompileMetaCreate();
                }
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = context->line;
                rs.error.index =context->index;
                mem_strcpy(rs.error.error, "foreach exp2");
            }
        }
        
        if(rs.state == vmScannerResultStateOK){
        
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};

            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);
            
            
            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
                
                meta = vmCompileMetaCreateWithOperator(op);
                
                vmCompileMetaOperatorAddCompileMeta(invoke, meta);
                
                vmCompileObjectRelease(meta);
            }
            
            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
        }
        
        vmCompileObjectRelease(op);
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp1, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->exp2, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    
    return rs;
}

/**
 If
 */

static vmScannerResult vmScannerIfFilterCallback(vmScannerContext * context,vmScannerIfFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "if";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            break;
        case 1:
            if(c == tag[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 2){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            break;
            
        case 2:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            break;
        case 3:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ')'){
                filter->base.state = 5;
            }
            else{
                filter->base.state ++;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')',InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "if");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state ++;
                }
            }
            break;
        case 4:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')',InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state ++;
            }
            break;
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index =  context->index;
                filter->base.base.state = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c,InvokeTickArg);
                if(filter->base.base.state == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "if");
                }
            }
            else if(c == '{'){
                filter->invoke.line = context->line;
                filter->invoke.index =  context->index;
                filter->base.state ++;
            }
            else{
                filter->invoke.line = context->line;
                filter->invoke.index =  context->index;
                filter->base.base.state = vmScannerCompileBufferExpressionAppend(context, &filter->invoke,';', c,InvokeTickArg);
                if(filter->base.base.state == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "if");
                }
                filter->base.state = 7;
            }
            break;
        case 6:
            filter->base.base.state = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c,InvokeTickArg);
            if(filter->base.base.state == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            break;
        case 7:
            filter->base.base.state = vmScannerCompileBufferExpressionAppend(context, &filter->invoke,';', c,InvokeTickArg);
            if(filter->base.base.state == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "if");
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerIfFilterReset(vmScannerContext * context,vmScannerIfFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeIfElseIfElse, NULL);
        vmCompileMeta * meta;
        
        hlog("if(%s){\n",buffer_to_str(filter->exp.value));
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
            
            if(!meta){
                meta = vmCompileMetaCreateWithBoolean(hbool_false);
            }
              
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "if exp");
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};
 
            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);

            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                meta  = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
                meta = vmCompileMetaCreateWithOperator(op);
                vmCompileMetaOperatorAddCompileMeta(invoke, meta);
                vmCompileObjectRelease(meta);
            }

            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
        }
       
        vmCompileObjectRelease(op);
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    
    return rs;
}

/**
 Else If
 */

static vmScannerResult vmScannerElseIfFilterCallback(vmScannerContext * context,vmScannerElseIfFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag1[] = "else";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag1[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            break;
        case 1:
        case 2:
        case 3:
            if(c == tag1[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 4){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            break;
        case 4:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == 'i'){
                filter->base.state++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            break;
        case 5:
            if(c == 'f'){
                filter->base.state++;
                invokeContext->elseFilter.base.base.state = vmScannerFilterStateError;
                invokeContext->ifFilter.base.base.state = vmScannerFilterStateError;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 7:
            
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ')'){
                filter->base.state = 9;
            }
            else{
                filter->base.state ++;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error,"Else If");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.state = 9;
                }
            }
           
            break;
        case 8:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ')', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.state = 9;
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->base.base.state = vmScannerFilterStateMatch;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            }
            else if(c == '{'){
                filter->base.state ++;
            }
            else{
                filter->base.state = 12;
            }
            break;
        case 10:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '}'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error,"Else If");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 11:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        case 12:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error,"Else If");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 13:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else If");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerElseIfFilterReset(vmScannerContext * context,vmScannerElseIfFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        hlog("else if(%s){\n",buffer_to_str(filter->exp.value));
        
        meta = (vmCompileMeta *)vmCompileObjectArrayLast(invoke->compileMetas);
        
        if(meta && (meta->type & vmRuntimeMetaTypeOperator) && meta->value.operatorMeta->type == vmRuntimeOperatorTypeIfElseIfElse){
            op = meta->value.operatorMeta;
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = filter->exp.line;
            rs.error.index = filter->exp.index;
            mem_strcpy(rs.error.error, "else if");
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
                if(!meta){
                    meta = vmCompileMetaCreateWithBoolean(hbool_false);
                }
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
            }
            else{
                rs.state = vmScannerResultStateError;
                rs.error.line = filter->exp.line;
                rs.error.index = filter->exp.index;
                mem_strcpy(rs.error.error, "else if exp");
            }
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};

            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);
            
                        
            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
                
            }

            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);

        }
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp, InvokeTickArg);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    return rs;
}

/**
 Else
 */

static vmScannerResult vmScannerElseFilterCallback(vmScannerContext * context,vmScannerElseFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag1[] = "else";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag1[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else");
            }
            break;
        case 1:
        case 2:
        case 3:
            if(c == tag1[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 4){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else");
            }
            break;
        case 4:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else if(c == '{'){
                filter->base.state ++;
            }
            else{
                filter->base.state = 7;
            }
            break;
        case 5:
            
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '}'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error,"Else");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 6:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        case 7:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ';'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else{
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error,"Else");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 8:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->invoke, c,';', InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Else");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerElseFilterReset(vmScannerContext * context,vmScannerElseFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        hlog("else {\n");
        
        meta = (vmCompileMeta *)vmCompileObjectArrayLast(invoke->compileMetas);
        
        if(meta && ( meta->type & vmRuntimeMetaTypeOperator)  && meta->value.operatorMeta->type == vmRuntimeOperatorTypeIfElseIfElse){
            op = meta->value.operatorMeta;
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "else");
        }
        
        if(rs.state == vmScannerResultStateOK){
            meta = vmCompileMetaCreateWithBoolean(hbool_true);
        
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
        }
        
        if(rs.state == vmScannerResultStateOK){
        
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};
 
            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);

            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                
                vmCompileObjectRelease(meta);
                
            }
            
            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
            
        }
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);    
    
    return rs;
}


/**
 throw
 */

static vmScannerResult vmScannerThrowFilterCallback(vmScannerContext * context,vmScannerThrowFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "throw";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            if(c == tag[filter->base.state]){
                filter->base.state ++;
                if(filter->base.state == 5){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "throw");
            }
            break;
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c ==';'){
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            else {
                filter->base.state ++ ;
                filter->exp.line = context->line;
                filter->exp.index = context->index;
                retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);
                
                if(retState == vmScannerFilterStateError){
                    filter->base.base.state = vmScannerFilterStateError;
                    filter->base.base.line = context->line;
                    filter->base.base.index = context->index;
                    mem_strcpy(filter->base.base.error, "throw");
                }
                else if(retState == vmScannerFilterStateMatch){
                    filter->base.base.state = vmScannerFilterStateMatch;
                }
            }
            break;
        case 6:
            retState = vmScannerCompileBufferExpressionAppend(context, &filter->exp, c, ';', InvokeTickArg);
            
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "throw");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    
    return result;
}

static vmScannerResult vmScannerThrowFilterReset(vmScannerContext * context,vmScannerThrowFilter * filter,InvokeTickDeclare){
    
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMeta * meta;
        
        if(vmCompileExpressionString(buffer_to_str(filter->exp.value), &meta)){
            if(!meta){
                meta = vmCompileMetaCreate();
            }
            meta->type = meta->type | vmRuntimeMetaTypeThrowable;
            vmCompileMetaOperatorAddCompileMeta(invoke, meta);
            vmCompileObjectRelease(meta);
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "throw exp");
        }
        
        
        hlog("throw %s;\n",buffer_to_str(filter->exp.value));
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->exp, InvokeTickArg);
    return rs;
}

/**
 Try
 */

static vmScannerResult vmScannerTryFilterCallback(vmScannerContext * context,vmScannerTryFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag[] = "try";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "try");
            }
            break;
        case 1:
        case 2:
            if(c == tag[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 3){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "try");
            }
            break;
            
        case 3:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->base.state ++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "try");
            }
            break;
        case 4:
            
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error, "try");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerTryFilterReset(vmScannerContext * context,vmScannerTryFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeTryCatchFinally, NULL);
        vmCompileMeta * meta;
        

        vmScannerInvokeContext invokeContext;
        
        vmScannerSource source = {vmScannerSourceTypeBuffer,0};
        
		hlog("try{\n");
  
        source.source.buffer.data = buffer_data(filter->invoke.value);
        source.source.buffer.length = buffer_length(filter->invoke.value);
        
        
        vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
        invokeContext.base.line = filter->invoke.line;
        invokeContext.base.line = filter->invoke.index;
        
        
        rs = vmScanner(&invokeContext.base,source);
        
        if(rs.state == vmScannerResultStateError){
            filter->base.base.state = vmScannerFilterStateError;
            filter->base.base.line = rs.error.line;
            filter->base.base.index = rs.error.index;
            strcpy(filter->base.base.error,rs.error.error);
        }
        
        if(rs.state == vmScannerResultStateOK){
            meta  = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
            meta = vmCompileMetaCreateWithOperator(op);
            vmCompileMetaOperatorAddCompileMeta(invoke, meta);
            vmCompileObjectRelease(meta);
        }
        
        vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);

        
        vmCompileObjectRelease(op);
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    
    return rs;
}

/**
 Catch
 */

static vmScannerResult vmScannerCatchFilterCallback(vmScannerContext * context,vmScannerCatchFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag1[] = "catch";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag1[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            if(c == tag1[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 5){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 5:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '('){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 6:
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                filter->base.state++;
                buffer_append(filter->className, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            
            break;
        case 7:
            if(ISSPACECHAR(c)){
                filter->base.state++;
            }
            else if(ISNAME(c)){
                buffer_append(filter->className, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 8:
            if(ISSPACECHAR(c)){
                
            }
            else if(ISNAMEFIRST(c)){
                filter->base.state++;
                buffer_append(filter->name, &c, 1);
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 9:
            if(ISSPACECHAR(c)){
                filter->base.state++;
            }
            else if(ISNAME(c)){
                buffer_append(filter->name, &c, 1);
            }
            else if(c == ')'){
                filter->base.state = 11;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 10:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == ')'){
                filter->base.state++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 11:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->base.state++;
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            break;
        case 12:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"Catch");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerCatchFilterReset(vmScannerContext * context,vmScannerCatchFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        hlog("catch(%s %s){\n",buffer_to_str(filter->className),buffer_to_str(filter->name));
        
        meta = (vmCompileMeta *)vmCompileObjectArrayLast(invoke->compileMetas);
        
        if(meta && (meta->type & vmRuntimeMetaTypeOperator) && meta->value.operatorMeta->type == vmRuntimeOperatorTypeTryCatchFinally){
            op = meta->value.operatorMeta;
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = filter->invoke.line;
            rs.error.index = filter->invoke.index;
            mem_strcpy(rs.error.error, "catch");
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            meta = vmCompileMetaCreateWithString(buffer_to_str(filter->className));
            
            meta->type = meta->type | vmRuntimeMetaTypeCatch;
            
            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);

            meta = vmCompileMetaCreateWithString(buffer_to_str(filter->name));

            vmCompileMetaOperatorAddCompileMeta(op, meta);
            vmCompileObjectRelease(meta);
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};
            
            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);
            
            
            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                vmCompileObjectRelease(meta);
                
            }
            
            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
            
        }
        
        hlog("}\n\n");
    }
    filter->base.state = 0;
    buffer_clear(filter->className);
    buffer_clear(filter->name);
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);
    return rs;
}

/**
 Finally
 */

static vmScannerResult vmScannerFinallyFilterCallback(vmScannerContext * context,vmScannerFinallyFilter * filter,hchar c,InvokeTickDeclare){
    vmScannerResult result = {vmScannerResultStateOK,0};
    vmScannerInvokeContext * invokeContext = (vmScannerInvokeContext *) context;
    vmScannerFilterState retState = vmScannerFilterStateNone;
    
    static hchar tag1[] = "finally";
    
    switch (filter->base.state) {
        case 0:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == tag1[0]){
                filter->base.state ++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"finally");
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if(c == tag1[filter->base.state]){
                filter->base.state++;
                if(filter->base.state == 7){
                    invokeContext->statementFilter.base.base.state = vmScannerFilterStateError;
                    invokeContext->statementFilter.base.base.line = context->line;
                    invokeContext->statementFilter.base.base.index = context->index;
                }
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"finally");
            }
            break;
        case 7:
            if(ISSPACECHAR(c)){
                
            }
            else if(c == '{'){
                filter->invoke.line = context->line;
                filter->invoke.index = context->index;
                filter->base.state++;
            }
            else{
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"finally");
            }
            break;
        case 8:
            retState = vmScannerCompileBufferInvokeAppend(context, &filter->invoke, c, InvokeTickArg);
            if(retState == vmScannerFilterStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = context->line;
                filter->base.base.index = context->index;
                mem_strcpy(filter->base.base.error,"finally");
            }
            else if(retState == vmScannerFilterStateMatch){
                filter->base.base.state = vmScannerFilterStateMatch;
            }
            break;
        default:
            break;
    }
    
    return result;
}

static vmScannerResult vmScannerFinallyFilterReset(vmScannerContext * context,vmScannerFinallyFilter * filter,InvokeTickDeclare){
    vmScannerResult rs = {vmScannerResultStateOK,0};
    
    if(filter->base.base.state == vmScannerFilterStateMatch){
        
        vmCompileMetaOperator * invoke = context->extra;
        vmCompileMetaOperator * op;
        vmCompileMeta * meta;
        
        hlog("finally {\n");
        
        meta = (vmCompileMeta *)vmCompileObjectArrayLast(invoke->compileMetas);
        
        if(meta && ( meta->type & vmRuntimeMetaTypeOperator)  && meta->value.operatorMeta->type == vmRuntimeOperatorTypeTryCatchFinally){
            op = meta->value.operatorMeta;
        }
        else{
            rs.state = vmScannerResultStateError;
            rs.error.line = context->line;
            rs.error.index = context->index;
            mem_strcpy(rs.error.error, "finally");
        }
        
        if(rs.state == vmScannerResultStateOK){
            
            vmScannerInvokeContext invokeContext;
            
            vmScannerSource source = {vmScannerSourceTypeBuffer,0};
            
            source.source.buffer.data = buffer_data(filter->invoke.value);
            source.source.buffer.length = buffer_length(filter->invoke.value);
            
            
            vmScannerInvokeContextInit(&invokeContext,InvokeTickArg);
            invokeContext.base.line = filter->invoke.line;
            invokeContext.base.line = filter->invoke.index;
            
            
            rs = vmScanner(&invokeContext.base,source);
            
            if(rs.state == vmScannerResultStateError){
                filter->base.base.state = vmScannerFilterStateError;
                filter->base.base.line = rs.error.line;
                filter->base.base.index = rs.error.index;
                strcpy(filter->base.base.error,rs.error.error);
            }
            
            if(rs.state == vmScannerResultStateOK){
                
                meta = vmCompileMetaCreateWithOperator(invokeContext.base.extra);
                
                meta->type = meta->type | vmRuntimeMetaTypeFinally;
                
                vmCompileMetaOperatorAddCompileMeta(op, meta);
                
                vmCompileObjectRelease(meta);
                
            }
            
            vmScannerInvokeContextDestroy(&invokeContext,InvokeTickArg);
            
        }
        hlog("}\n\n");
    }
    filter->base.state = 0;
    vmScannerCompileBufferClear(&filter->invoke, InvokeTickArg);    
    
    return rs;
}


hbool vmScannerInvokeContextInit(vmScannerInvokeContext * context,InvokeTickDeclare){
    memset(context, 0,sizeof(vmScannerInvokeContext));
    context->base.size = sizeof(vmScannerInvokeContext);
    context->base.extra = vmCompileMetaOperatorCreate(vmRuntimeOperatorTypeInvoke, NULL);
    
    context->varFilter.base.base.size = sizeof(vmScannerVarFilter);
    context->ifFilter.base.base.size = sizeof(vmScannerIfFilter);
    context->elseIfFilter.base.base.size = sizeof(vmScannerElseIfFilter);
    context->elseFilter.base.base.size = sizeof(vmScannerElseFilter);
    context->whileFilter.base.base.size = sizeof(vmScannerWhileFilter);
    context->forFilter.base.base.size = sizeof(vmScannerForFilter);
    context->forInFilter.base.base.size = sizeof(vmScannerForInFilter);
    context->tryFilter.base.base.size = sizeof(vmScannerTryFilter);
    
    context->catchFilter.base.base.size = sizeof(vmScannerCatchFilter);
    context->throwFilter.base.base.size = sizeof(vmScannerThrowFilter);
    context->finallyFilter.base.base.size = sizeof(vmScannerFinallyFilter);
    context->returnFilter.base.base.size = sizeof(vmScannerReturnFilter);
    context->breakFilter.base.base.size = sizeof(vmScannerBreakFilter);
    context->continueFilter.base.base.size = sizeof(vmScannerContinueFilter);
    
    context->statementFilter.base.base.size = sizeof(vmScannerStatementFilter);
    
    context->returnFilter.exp.value = buffer_alloc(20,  128);
    context->returnFilter.base.base.callback = (vmScannerCallback) vmScannerReturnFilterCallback;
    context->returnFilter.base.base.reset = (vmScannerReset) vmScannerReturnFilterReset;
    
    context->breakFilter.base.base.callback = (vmScannerCallback) vmScannerBreakFilterCallback;
    context->breakFilter.base.base.reset = (vmScannerReset) vmScannerBreakFilterReset;
    
    context->continueFilter.base.base.callback = (vmScannerCallback) vmScannerContinueFilterCallback;
    context->continueFilter.base.base.reset = (vmScannerReset) vmScannerContinueFilterReset;
    
    context->tryFilter.invoke.value = buffer_alloc(20, 128);
    context->tryFilter.base.base.callback = (vmScannerCallback) vmScannerTryFilterCallback;
    context->tryFilter.base.base.reset = (vmScannerReset) vmScannerTryFilterReset;
    
    context->catchFilter.invoke.value = buffer_alloc(20, 128);
    context->catchFilter.className = buffer_alloc(20, 128);
    context->catchFilter.name = buffer_alloc(20, 128);
    context->catchFilter.base.base.callback = (vmScannerCallback) vmScannerCatchFilterCallback;
    context->catchFilter.base.base.reset = (vmScannerReset) vmScannerCatchFilterReset;
    
    context->finallyFilter.invoke.value = buffer_alloc(20, 128);
    context->finallyFilter.base.base.callback = (vmScannerCallback) vmScannerFinallyFilterCallback;
    context->finallyFilter.base.base.reset = (vmScannerReset) vmScannerFinallyFilterReset;
    
    context->throwFilter.exp.value = buffer_alloc(20, 128);
    context->throwFilter.base.base.callback = (vmScannerCallback) vmScannerThrowFilterCallback;
    context->throwFilter.base.base.reset = (vmScannerReset) vmScannerThrowFilterReset;
    
    context->varFilter.name = buffer_alloc(20,  128);
    context->varFilter.exp.value = buffer_alloc(20,  128);
    context->varFilter.base.base.callback = (vmScannerCallback) vmScannerVarFilterCallback;
    context->varFilter.base.base.reset = (vmScannerReset) vmScannerVarFilterReset;
    
    context->statementFilter.expression = buffer_alloc(20,  128);
    context->statementFilter.base.base.callback = (vmScannerCallback) vmScannerStatementFilterCallback;
    context->statementFilter.base.base.reset = (vmScannerReset) vmScannerStatementFilterReset;

    context->whileFilter.exp.value = buffer_alloc(20,  128);
    context->whileFilter.invoke.value = buffer_alloc(20,  128);
    context->whileFilter.base.base.callback = (vmScannerCallback) vmScannerWhileFilterCallback;
    context->whileFilter.base.base.reset = (vmScannerReset) vmScannerWhileFilterReset;
    
    context->noteClearFilter.base.base.size = sizeof(vmScannerNoteClearFilter);
    context->noteClearFilter.base.base.callback = (vmScannerCallback)vmScannerNoteClearFilterCallback;
    context->noteClearFilter.base.base.reset = (vmScannerReset)vmScannerNoteClearFilterReset;
    
    context->forFilter.exp1.value = buffer_alloc(20,  128);
    context->forFilter.exp2.value = buffer_alloc(20,  128);
    context->forFilter.exp3.value = buffer_alloc(20,  128);
    context->forFilter.invoke.value = buffer_alloc(20,  128);
    context->forFilter.base.base.callback = (vmScannerCallback) vmScannerForFilterCallback;
    context->forFilter.base.base.reset = (vmScannerReset) vmScannerForFilterReset;
    
    context->forInFilter.exp1.value = buffer_alloc(20,  128);
    context->forInFilter.exp2.value = buffer_alloc(20,  128);
    context->forInFilter.invoke.value = buffer_alloc(20,  128);
    context->forInFilter.base.base.callback = (vmScannerCallback) vmScannerForInFilterCallback;
    context->forInFilter.base.base.reset = (vmScannerReset) vmScannerForInFilterReset;
    
    context->ifFilter.exp.value = buffer_alloc(20, 128);
    context->ifFilter.invoke.value = buffer_alloc(20,  128);
    context->ifFilter.base.base.callback = (vmScannerCallback) vmScannerIfFilterCallback;
    context->ifFilter.base.base.reset = (vmScannerReset) vmScannerIfFilterReset;
    
    context->elseIfFilter.exp.value = buffer_alloc(20, 128);
    context->elseIfFilter.invoke.value = buffer_alloc(20,  128);
    context->elseIfFilter.base.base.callback = (vmScannerCallback) vmScannerElseIfFilterCallback;
    context->elseIfFilter.base.base.reset = (vmScannerReset) vmScannerElseIfFilterReset;
    
    context->elseFilter.invoke.value = buffer_alloc(20,  128);
    context->elseFilter.base.base.callback = (vmScannerCallback) vmScannerElseFilterCallback;
    context->elseFilter.base.base.reset = (vmScannerReset) vmScannerElseFilterReset;
    
    return hbool_true;
}

void vmScannerInvokeContextDestroy(vmScannerInvokeContext * context,InvokeTickDeclare){
    
    buffer_dealloc(context->returnFilter.exp.value);
    context->returnFilter.exp.value = NULL;

    context->returnFilter.base.base.callback = NULL;
    context->returnFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->varFilter.exp.value);
    context->varFilter.exp.value = NULL;
    
    buffer_dealloc(context->varFilter.name);
    context->varFilter.name = NULL;
    
    context->varFilter.base.base.callback = NULL;
    context->varFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->statementFilter.expression);
    context->statementFilter.expression = NULL;
    
    context->statementFilter.base.base.callback = NULL;
    context->statementFilter.base.base.reset = NULL;
    
    
    buffer_dealloc(context->whileFilter.exp.value);
    context->whileFilter.exp.value = NULL;
    buffer_dealloc(context->whileFilter.invoke.value);
    context->whileFilter.invoke.value = NULL;
    
    context->whileFilter.base.base.callback = NULL;
    context->whileFilter.base.base.reset = NULL;
    
    
    context->noteClearFilter.base.base.callback = NULL;
    context->noteClearFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->forFilter.exp1.value);
    context->forFilter.exp1.value = NULL;
    
    buffer_dealloc(context->forFilter.exp2.value);
    context->forFilter.exp2.value = NULL;
    
    buffer_dealloc(context->forFilter.exp3.value);
    context->forFilter.exp3.value = NULL;
    
    buffer_dealloc(context->forFilter.invoke.value);
    context->forFilter.invoke.value = NULL;
    
    context->forFilter.base.base.callback = NULL;
    context->forFilter.base.base.reset = NULL;
    
    
    buffer_dealloc(context->forInFilter.exp1.value);
    context->forInFilter.exp1.value = NULL;
    
    buffer_dealloc(context->forInFilter.exp2.value);
    context->forInFilter.exp2.value = NULL;
    
    buffer_dealloc(context->forInFilter.invoke.value);
    context->forInFilter.invoke.value = NULL;
    
    context->forInFilter.base.base.callback = NULL;
    context->forInFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->ifFilter.exp.value);
    context->ifFilter.exp.value = NULL;

    buffer_dealloc(context->ifFilter.invoke.value);
    context->ifFilter.invoke.value = NULL;
    
    context->ifFilter.base.base.callback = NULL;
    context->ifFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->elseIfFilter.exp.value);
    context->elseIfFilter.exp.value = NULL;
    
    buffer_dealloc(context->elseIfFilter.invoke.value);
    context->elseIfFilter.invoke.value = NULL;
    
    context->elseIfFilter.base.base.callback = NULL;
    context->elseIfFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->elseFilter.invoke.value);
    context->elseFilter.invoke.value = NULL;
    
    context->elseFilter.base.base.callback = NULL;
    context->elseFilter.base.base.reset = NULL;

    
    vmCompileObjectRelease(context->base.extra);
    context->base.extra = NULL;
    
    
    buffer_dealloc(context->tryFilter.invoke.value);
    context->tryFilter.invoke.value = NULL;
    context->tryFilter.base.base.callback = NULL;
    context->tryFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->catchFilter.invoke.value);
    context->catchFilter.invoke.value = NULL;
    buffer_dealloc(context->catchFilter.className);
    context->catchFilter.className = NULL;
    buffer_dealloc(context->catchFilter.name);
    context->catchFilter.name = NULL;
    context->catchFilter.base.base.callback = NULL;
    context->catchFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->finallyFilter.invoke.value);
    context->finallyFilter.invoke.value = NULL;
    context->finallyFilter.base.base.callback = NULL;
    context->finallyFilter.base.base.reset = NULL;
    
    buffer_dealloc(context->throwFilter.exp.value);
    context->throwFilter.exp.value = NULL;
    context->throwFilter.base.base.callback = NULL;
    context->throwFilter.base.base.reset = NULL;
}


