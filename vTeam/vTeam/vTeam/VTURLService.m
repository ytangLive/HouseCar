//
//  VTURLService.m
//  vTeam
//
//  Created by Zhang Hailong on 13-6-22.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//
//  Modified by xuhui3 on 15-7-24 IAP

#import "VTURLService.h"

@interface VTURLServiceItem : NSObject

@property(nonatomic,assign) Protocol * taskType;
@property(nonatomic,retain) id task;
@property(nonatomic,retain) id httpTask;

@end

@implementation VTURLServiceItem

@synthesize taskType = _taskType;
@synthesize task = _task;
@synthesize httpTask = _httpTask;

-(void) dealloc{
    [_task release];
    [_httpTask release];
    [super dealloc];
}

@end

@interface VTURLService(){
    NSMutableArray * _items;
}

@end

@implementation VTURLService

-(void) dealloc{
    [_items release];
    [super dealloc];
}

-(BOOL) handle:(Protocol *)taskType task:(id<IVTTask>)task priority:(NSInteger)priority{
    
    if(taskType == @protocol(IVTURLDownlinkTask)){
        
        id<IVTURLDownlinkTask> urlTask = (id<IVTURLDownlinkTask>) task;
        
        if(![urlTask isSkipCached] && [urlTask vtDownlinkPageTaskPageIndex] == 1){
            [self vtDownlinkTaskDidLoadedFromCache:urlTask forTaskType:taskType];
        }
        
        NSString * url = [urlTask url];
        
        if(url == nil){
            url = [self.config valueForKey:[urlTask urlKey]];
        }
        
        NSInteger offset = ( [urlTask vtDownlinkPageTaskPageIndex] - 1) * [urlTask vtDownlinkPageTaskPageSize];
        
        url = [url stringByReplacingOccurrencesOfString:@"{offset}" withString:[NSString stringWithFormat:@"%d",(int) offset]];
        
        url = [url stringByReplacingOccurrencesOfString:@"{pageIndex}" withString:[NSString stringWithFormat:@"%d",(int)[urlTask vtDownlinkPageTaskPageIndex]]];
        
        url = [url stringByReplacingOccurrencesOfString:@"{pageSize}" withString:[NSString stringWithFormat:@"%d",(int)[urlTask vtDownlinkPageTaskPageSize]]];
        
        url = [url stringByDataOutlet:urlTask stringValue:^NSString *(id data, NSString *keyPath) {
            id v = [data dataForKeyPath:keyPath];
            if([v isKindOfClass:[NSString class]]){
                return [v stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            }
            else if(v){
                return [NSString stringWithFormat:@"%@",v];
            }
            return @"";
        }];
        
        VTHttpTask * httpTask = nil;
        
        Class httpClass = [urlTask httpClass];
        
        if(httpClass == nil){
            httpTask = [[VTHttpTask alloc] init];
            [httpTask setResponseType:VTHttpTaskResponseTypeJSON];
        }
        else{
            httpTask = [[httpClass alloc] init];
        }

        [httpTask setUserInfo:urlTask];
        [httpTask setSource:[task source]];
        [httpTask setDelegate:self];
        
        NSURL *curUrl = nil;
        
        //apache 统计
        
        NSMutableDictionary *mDic = [[NSMutableDictionary alloc]initWithCapacity:10];
        [mDic setValuesForKeysWithDictionary:[urlTask queryValues]];
        
        curUrl = [NSURL URLWithString:url queryValues:mDic];
        
        NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:curUrl cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:120];
        
        id header = [self.config dictionaryValueForKey:@"headers"];
        
        for (NSString * key in header) {
            if ([key isEqualToString:@"Referer"]&&[url hasPrefix:[header stringValueForKey:key]]) {
                [request setValue:[header stringValueForKey:key] forHTTPHeaderField:key];
            }
        }
        
        [httpTask setRequest:request];
        
        NSLog(@"%@",request);
        NSLog(@"%@",[request allHTTPHeaderFields]);
        
        VTURLServiceItem * item = [[[VTURLServiceItem alloc] init] autorelease];
        
        item.taskType = taskType;
        item.task = urlTask;
        item.httpTask = httpTask;
        
        if(_items == nil){
            _items = [[NSMutableArray alloc] init];
        }
        
        [_items addObject:item];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.context handle:@protocol(IVTHttpAPITask) task:httpTask priority:priority];
        });
        
        [httpTask release];
        
        return YES;
    }
    
    return [super handle:taskType task:task priority:0];
}

-(BOOL) cancelHandle:(Protocol *)taskType task:(id<IVTTask>)task{
    
    if(taskType == @protocol(IVTURLDownlinkTask)){
        
        NSInteger c = [_items count];
        NSInteger i = 0;
        
        while(i < c){
            id item = [_items objectAtIndex:i];
            if([item task] == task){
                if([item httpTask]){
                    [[item httpTask] setDelegate:nil];
                    [self.context cancelHandle:@protocol(IVTHttpTask) task:[item httpTask]];
                }
                [_items removeObjectAtIndex:i];
                c --;
            }
            else{
                i ++;
            }
            
        }
        
        return YES;
    }
    
    return NO;
}

-(BOOL) cancelHandleForSource:(id)source{
    
    NSInteger c = [_items count];
    NSInteger i = 0;
    
    while(i < c){
        id item = [_items objectAtIndex:i];
        if([[item task] source] == source){
            if([item httpTask]){
                [[item httpTask] setDelegate:nil];
                [self.context cancelHandle:@protocol(IVTHttpTask) task:[item httpTask]];
            }
            [_items removeObjectAtIndex:i];
            c --;
        }
        else{
            i ++;
        }
        
    }
    
    return NO;
}

-(NSString *) dataKey:(id<IVTDownlinkTask>)task forTaskType:(Protocol *)taskType{
    if(taskType == @protocol(IVTURLDownlinkTask)){
        
        NSString * url = [(id<IVTURLDownlinkTask>)task url];
        
        if(url == nil){
            url = [self.config valueForKey:[(id<IVTURLDownlinkTask>)task urlKey]];
        }
        
        id<IVTURLDownlinkTask> urlTask = (id<IVTURLDownlinkTask>) task;
        
        NSInteger offset = ( [urlTask vtDownlinkPageTaskPageIndex] - 1) * [urlTask vtDownlinkPageTaskPageSize];
        
        url = [url stringByReplacingOccurrencesOfString:@"{offset}" withString:[NSString stringWithFormat:@"%d",(int)offset]];
        
        url = [url stringByReplacingOccurrencesOfString:@"{pageIndex}" withString:[NSString stringWithFormat:@"%d",(int)[urlTask vtDownlinkPageTaskPageIndex]]];
        
        url = [url stringByReplacingOccurrencesOfString:@"{pageSize}" withString:[NSString stringWithFormat:@"%d",(int)[urlTask vtDownlinkPageTaskPageSize]]];
        
        url = [url stringByDataOutlet:urlTask stringValue:^NSString *(id data, NSString *keyPath) {
            id v = [data dataForKeyPath:keyPath];
            if([v isKindOfClass:[NSString class]]){
                return [v stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            }
            else if(v){
                return [NSString stringWithFormat:@"%@",v];
            }
            return @"";
        }];
        
        url = [[NSURL URLWithString:url queryValues:[(id<IVTURLDownlinkTask>)task queryValues]] absoluteString];
        
        return [url vtMD5String];
    }
    return [super dataKey:task forTaskType:taskType];
}

-(void) vtHttpTask:(id) httpTask didFailError:(NSError *) error{
    
    id task = [httpTask userInfo];
    
    Protocol * taskType = nil;
    
    NSInteger c = [_items count];
    NSInteger i = 0;
    
    while(i < c){
        id item = [_items objectAtIndex:i];
        if([item httpTask] == httpTask){
            taskType = [item taskType];
            [_items removeObjectAtIndex:i];
            break;
        }
        i ++;
    }
    if(taskType && task){
        [self vtDownlinkTask:task didFitalError:error forTaskType:taskType];
    }
}

-(void) didLoaded:(NSDictionary *) userInfo{
    Protocol * taskType = NSProtocolFromString([userInfo valueForKey:@"taskType"]);
    id task = [userInfo valueForKey:@"task"];
    id body = [userInfo valueForKey:@"body"];
    NSString * responseUUID = [userInfo valueForKey:@"responseUUID"];
    
    if(task && taskType){
        NSError * error = [self errorByResponseBody:body task:task];
        if(error == nil){
            [self vtDownlinkTask:task didResponse:body isCache:[task vtDownlinkPageTaskPageIndex] == 1 responseUUID:responseUUID forTaskType:taskType];
        }
        else{
            [self vtDownlinkTask:task didFitalError:error forTaskType:taskType];
        }
    }
}

-(void) vtHttpTaskDidLoaded:(id) httpTask{
    
    id task = [httpTask userInfo];
    
    Protocol * taskType = nil;
    
    NSInteger c = [_items count];
    NSInteger i = 0;
    
    while(i < c){
        id item = [_items objectAtIndex:i];
        if([item httpTask] == httpTask){
            taskType = [item taskType];
            [_items removeObjectAtIndex:i];
            break;
        }
        i ++;
    }
    
    NSLog(@"%@",[httpTask responseBody]);
    
    if(task && taskType){
        [self performSelector:@selector(didLoaded:) withObject:
            [NSDictionary dictionaryWithObjectsAndKeys:
             NSStringFromProtocol(taskType),@"taskType"
             ,task,@"task"
             ,[httpTask responseBody],@"body",[httpTask responseUUID],@"responseUUID", nil] afterDelay:0];
    }
}

-(NSError *) errorByResponseBody:(id) body task:(id) task{
    NSString * errorCodeKeyPath = [task errorCodeKeyPath];
    NSString * errorKeyPath = [task errorKeyPath];
    if(errorCodeKeyPath){
        int errorCode = 0;
        
        if ([body dataForKeyPath:errorCodeKeyPath]) {
            errorCode = [[body dataForKeyPath:errorCodeKeyPath] intValue];
        }else{
            //对于response的status code为200，但是responseBody内容错误的默认errorcode
            errorCode = 20150511;
        }
        
        if(errorCode ==0){
            return nil;
        }
        NSString * error = [body dataForKeyPath:errorKeyPath];
        
        if(error == nil){
            error = @"";
        }
        
        return [NSError errorWithDomain:@"VTURLService" code:errorCode
                               userInfo:[NSDictionary dictionaryWithObject:error forKey:NSLocalizedDescriptionKey]];
    }
    return nil;
}

@end