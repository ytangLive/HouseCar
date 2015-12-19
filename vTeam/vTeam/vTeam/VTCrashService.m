//
//  VTCrashService.m
//  vTeam
//
//  Created by zhang hailong on 13-11-15.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTCrashService.h"

#import "VTCrashTask.h"

#import "VTCrashUploadTask.h"

#import "UIDevice+VTUUID.h"

#import "VTBECrashObject.h"

#include <execinfo.h>

#include <libkern/OSAtomic.h>

static VTCrashService * gVTCrashService = nil;

// 收集崩溃信息并格式化
NSString* detailedBacktrace(NSException *exception,int signal)
{
    NSMutableString *backTrace = [NSMutableString string];
    NSMutableString *exceptionReason = [NSMutableString string];
    if (exception != nil)
    {
        // iOS >= 4.0
        if ([[[UIDevice currentDevice] systemVersion] integerValue] >= 4)
        {
            NSArray *callStackSyms = [exception callStackSymbols]; // NS_AVAILABLE(4_0);
            for (int index=0; index < [callStackSyms count]; index++)
            {
                [backTrace appendString:[callStackSyms objectAtIndex:index]];
                [backTrace appendString:@"\n"];
            }
        }
        // 异常产生的原因
        [exceptionReason appendString:[exception reason]];
    }
    
    if (backTrace.length == 0)
    {
        // 捕获信号
        if ([[[UIDevice currentDevice] systemVersion] integerValue] >= 4)
        {
            NSMutableArray *callStackSyms = [NSMutableArray arrayWithArray:[NSThread callStackSymbols]]; // NS_AVAILABLE(4_0);
            if ([callStackSyms count] > 3)
            {
                int index = 0; // 屏蔽移动统计SDK内部函数调用序列
                for (; index < [callStackSyms count]; index++)
                {
                    [backTrace appendString:[callStackSyms objectAtIndex:index]];
                    [backTrace appendString:@"\n"];
                }
            }
        }
        else
        {
            // iOS < 4.0 && 未捕获异常
            void* callstack[128];
            int frames = backtrace(callstack, 128);
            char **strs = backtrace_symbols(callstack, frames);
            int index = 0;
            {
                [backTrace appendString:[NSString stringWithUTF8String:strs[index]]];
                [backTrace appendString:@"\n"];
            }
            free(strs);
        }
        [exceptionReason appendString:[NSString stringWithFormat:@"signal:%d, errno:%d",signal,errno]];
    }
    
    NSMutableString *detailInfo = [NSMutableString string];
    [detailInfo appendString:@"Hi \n"];
    [detailInfo appendString:@"使用app时发生了崩溃, 详细错误信息如下:\n"];
    [detailInfo appendString:exceptionReason];
    [detailInfo appendString:@"\n\n"];
    [detailInfo appendString:@"详细错误信息:\n ( \n"];
    [detailInfo appendString:backTrace];
    [detailInfo appendString:@") \n"];
    return detailInfo;
}

static void VTCrashServiceUncaughtExceptionHandler(NSException *exception){
    VTCrashTask * task = [[VTCrashTask alloc] init];
    [task setException:[[NSException alloc] initWithName:@"VTCrashServiceUncaughtException" reason:detailedBacktrace(exception,SIGUSR1) userInfo:nil]];
    [gVTCrashService.context handle:@protocol(IVTCrashTask) task:task priority:0];
    [task release];
}

static void VTCrashServiceSignalHandler(int signal)
{
    VTCrashTask * task = [[VTCrashTask alloc] init];
    [task setException:[[NSException alloc] initWithName:@"VTCrashServiceSignal" reason:detailedBacktrace(nil,signal) userInfo:nil]];
    [gVTCrashService.context handle:@protocol(IVTCrashTask) task:task priority:0];
    [task release];
}

@interface VTCrashService()
@property(retain) VTHttpTask * httpTask;
@property(readonly) VTDBContext * dbContext;
@end

@implementation VTCrashService
@synthesize httpTask = _httpTask;
@synthesize dbContext = _dbContext;

-(VTDBContext *) dbContext{
    
    if(_dbContext == nil){
        
        VTSqlite * db = [[VTSqlite alloc] initWithPath:[NSHomeDirectory() stringByAppendingPathComponent:@"Documents/behavior.db"]];
        _dbContext = [[VTDBContext alloc] init];
        [_dbContext setDb:db];
        
        [db release];
        
    }
    
    return _dbContext;
}

-(id) init{
    
    if((self = [super init])){
        
        NSSetUncaughtExceptionHandler(VTCrashServiceUncaughtExceptionHandler);
        
        signal(SIGABRT, VTCrashServiceSignalHandler);
        signal(SIGILL, VTCrashServiceSignalHandler);
        signal(SIGSEGV, VTCrashServiceSignalHandler);
        signal(SIGFPE, VTCrashServiceSignalHandler);
        signal(SIGBUS, VTCrashServiceSignalHandler);
        signal(SIGPIPE, VTCrashServiceSignalHandler);
        
        signal(SIGHUP, VTCrashServiceSignalHandler);
        signal(SIGINT, VTCrashServiceSignalHandler);
        signal(SIGQUIT, VTCrashServiceSignalHandler);
        
        gVTCrashService = self;
    }
    
    return self;
}

-(void) dealloc{
    
    if(gVTCrashService == self){
        gVTCrashService = nil;
    }
    
    [super dealloc];
}

-(void) unregisterUncaughtExceptionHandler{
    NSSetUncaughtExceptionHandler(NULL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
}

-(BOOL) handle:(Protocol *)taskType task:(id<IVTTask>)task priority:(NSInteger)priority{
    
    
    if(@protocol(IVTCrashTask) == taskType){
        
        [self unregisterUncaughtExceptionHandler];
        
        NSException * exception = [(id<IVTCrashTask>)task exception];
        
        if(exception){
            
            NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
            
            [data setValue:exception.name forKey:@"name"];
            [data setValue:exception.reason forKey:@"reason"];
//            [data setValue:exception.callStackSymbols forKey:@"callStackSymbols"];
//            [data setValue:exception.callStackReturnAddresses forKey:@"callStackReturnAddresses"];
            
            VTBECrashObject * dataObject = [[VTBECrashObject alloc] init];
            
            [dataObject setException:[VTJSON encodeObject:data]];
            
            VTBehaviorTask * beTask = [[VTBehaviorTask alloc] init];
            
            [beTask setDataObject:dataObject];
            
            [self.context handle:@protocol(IVTBehaviorTask) task:beTask priority:0];
            
            [beTask release];
            
        }
        
        return YES;
    }else if (taskType == @protocol(IVTCrashUploadTask)){
        @autoreleasepool {
            [self upLoadCrashInfo];
        }
//        [self upLoadCrashInfo];
    }
    
    return NO;
}

-(void)upLoadCrashInfo
{
    if(_httpTask == nil){
        
        __block VTHttpTask * httpTask = [[VTHttpTask alloc] init];
        
        self.httpTask = httpTask;
        
        VTBECrashObject * dataObject = nil;
        
        __weak __block VTCrashService * weakSelf = self;
        
        __block NSMutableDictionary * allData = [[NSMutableDictionary alloc] initWithCapacity:2];
        
        __block NSMutableArray *crashArr = [[NSMutableArray alloc]initWithCapacity:1];
        NSMutableDictionary *body = [[NSMutableDictionary alloc]initWithCapacity:1];
        
        id<IVTSqliteCursor> cursor = [self.dbContext query:[VTBECrashObject tableClass] sql:@" ORDER BY [rowid] ASC" data:nil];
        
        while([cursor next]){
            
            dataObject = [[VTBECrashObject alloc] init];
            
            [cursor toDataObject:dataObject];
            
            NSMutableDictionary* data = [[NSMutableDictionary alloc]initWithCapacity:10];
            
            [data setValue:dataObject.exception forKeyPath:@"report"];
            
            NSDateFormatter * dateFormatter = [[NSDateFormatter alloc] init];
            [dateFormatter setDateFormat:@"yyyy-MM-dd"];
            [data setValue:[dateFormatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:dataObject.timestamp]] forKey:@"date"];
            
            [dateFormatter setDateFormat:@"HH:mm:ss"];
            [data setValue:[dateFormatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:dataObject.timestamp]] forKey:@"time"];
            
            [crashArr addObject:data];
        }
        [cursor close];
        
        [body setValue:crashArr forKey:@"crash"];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
            if([crashArr count]>0){
                
                [allData setValue:body forKey:@"body"];
                
                [crashArr release];
                [body release];
                
                NSString * url = [self.config valueForKey:@"url"];
                
                NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url] cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:300];
                
                [request setHTTPMethod:@"POST"];
                
                NSMutableDictionary * header = [NSMutableDictionary dictionaryWithCapacity:4];
                
                UIDevice * device = [UIDevice currentDevice];
                NSBundle * bundle = [NSBundle mainBundle];
                
                NSString* sysVersion = [device systemVersion];
                
                CGRect screenBounds = [[UIScreen mainScreen] bounds];
                CGFloat scale = 1.0;
                if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
                    scale = [[UIScreen mainScreen] scale];
                }
                screenBounds.size.width = screenBounds.size.width*scale;
                screenBounds.size.height = screenBounds.size.height*scale;
                
                NSString* screenBoundsString = [NSString stringWithFormat:@"%dx%d",(int)screenBounds.size.width,(int)screenBounds.size.height];
                
                BOOL bWifi = [device isActiveWLAN];
                BOOL b3G = [device isActiveWWAN];
                
                NSString * deviceId = [[NSString stringWithFormat:@"%@%@",[device vtUniqueIdentifier],[device MACAddress]] vtMD5String];
                NSString * wm = [self.config valueForKey:@"wm"];
                
                [header setValue:@"iPhone OS" forKey:@"os"];
                [header setValue:deviceId forKey:@"uid"];
                [header setValue:wm forKey:@"wm"];
                [header setValue:@"4135432745" forKey:@"appkey"];
                [header setValue:deviceId forKey:@"device_id"];
                [header setValue:[device MACAddress] forKey:@"mac"];
                [header setValue:[[bundle infoDictionary] valueForKeyPath:@"Channel.from"] forKey:@"from"];
                [header setValue:[[bundle infoDictionary] valueForKeyPath:@"Channel.wm"] forKey:@"chwm"];
                [header setValue:screenBoundsString forKey:@"resolution"];
                [header setValue:b3G ? @"WWAN" : (bWifi ? @"WiFi" : @"NotReachable")  forKey:@"access"];
                [header setValue:sysVersion forKey:@"os_version"];
                [header setValue:@"" forKey:@"imei"];
                [header setValue:[(NSObject *)weakSelf.context dataForKey:@"mobileCarrier"] forKey:@"carrier"];
                [header setValue:@"1.0.0" forKey:@"stat_version"];
                [header setValue:[device standardPlat]?[device standardPlat]:@"" forKey:@"model"];
                [header setValue:@"b122" forKey:@"wm"];
//                [header setValue:@"" forKey:@"new_uid"];
                
                [allData setValue:header forKey:@"header"];
                
                NSLog(@"%@",allData);
                
                //3.9.5 在子线程里执行zip压缩及网络请求发送
                dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
                    NSData * bodyBytes = [[VTJSON encodeObject:allData] dataUsingEncoding:NSUTF8StringEncoding];
                    
                    [allData release];
                    
                    bodyBytes = [bodyBytes GZIPEncode];
                    
                    [request setHTTPBody:bodyBytes];
                    
                    [httpTask setResponseType:VTHttpTaskResponseTypeJSON];
                    [httpTask setRequest:request];
                    [httpTask setDelegate:weakSelf];
                    
                    [weakSelf.context handle:@protocol(IVTHttpUploadTask) task:httpTask priority:0];
                });
            }
            else{
                self.httpTask = nil;
            }
        });
    }
}

-(void) vtHttpTask:(id)httpTask didFailError:(NSError *)error{
    if(_httpTask == httpTask){
        [_httpTask setDelegate:nil];
        self.httpTask = nil;
    }
}

-(void) vtHttpTaskDidResponse:(id) httpTask{
    if(_httpTask == httpTask){
        
        if([(NSHTTPURLResponse *) [httpTask response] statusCode] == 200){
            [_dbContext.db execture:[NSString stringWithFormat:@"DELETE FROM [%@]",NSStringFromClass([VTBECrashObject tableClass])] withData:nil];
            
            [_httpTask setDelegate:nil];
            self.httpTask = nil;
        }
        else{
            [_httpTask setDelegate:nil];
            self.httpTask = nil;
        }
        
    }
}

@end
