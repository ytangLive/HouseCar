//
//  HCCommentService.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import "HCCommentService.h"
#import "HCCampCommentTask.h"
#import "HCSendStatusView.h"

@implementation HCCommentService

-(BOOL) handle:(Protocol *)taskType task:(id<IVTTask>)task priority:(NSInteger)priority{

    if(taskType == @protocol(IHCCampCommentTask)){
        
        id<IHCCampCommentTask> commentTask = (id<IHCCampCommentTask>) task;
        
        VTAPIRequestTask * httpTask = [[VTAPIRequestTask alloc] init];
        
        [httpTask setTask:task];
        [httpTask setTaskType:@protocol(IHCCampCommentTask)];
        [httpTask setSource:[task source]];
        
        NSString * url  = [self.config valueForKey:@"url"];
        url = [url stringByAppendingFormat:@"&post_id=%@",[commentTask postID]];
        [httpTask setApiUrl:url];

        
        NSMutableDictionary *queryDic = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                         [commentTask userName],@"name"
                                         ,[commentTask email],@"email"
                                         ,[commentTask content],@"content"
                                         , nil];

        //id _uid = [(id<HCContext>)self.context uid];
        
        [httpTask setQueryValues:queryDic];
        
        [self.context handle:@protocol(IVTAPIRequestTask) task:httpTask priority:0];
        
        return YES;
    }
    else if(taskType == @protocol(IVTAPIResponseTask)){
        
        id<IVTAPIResponseTask> respTask = (id<IVTAPIResponseTask>) task;
        
        if([respTask taskType] == @protocol(IHCCampCommentTask)){
            
            if([respTask error]){
                
                NSLog(@"%@",[respTask error]);
                
                HCSendStatusView * statusView = [[HCSendStatusView alloc] initWithImage:@"sendfail.png" title:@"发送失败"];
                [statusView show:YES duration:1.2];

            }
            else {
                NSLog(@"%@",[respTask resultsData]);
                
                if (![[respTask resultsData] isKindOfClass:[NSDictionary class]]) {
                    HCSendStatusView * statusView = [[HCSendStatusView alloc] initWithImage:@"sendfail.png" title:@"发送失败"];
                    [statusView show:YES duration:1.2];
                    
                    return YES;
                }
                
                if([[respTask resultsData] intValueForKeyPath:@"result.status.code"]== 0){
                    HCSendStatusView * statusView = [[HCSendStatusView alloc] initWithImage:@"sendok.png" title:@"发送成功"];
                    [statusView show:YES duration:1.2];
                }
                else{
                    HCSendStatusView * statusView = [[HCSendStatusView alloc] initWithImage:@"sendfail.png" title:@"发送失败"];
                    [statusView show:YES duration:1.2];
                }
                
            }
            
            return YES;
        }
    }
    
    return NO;
}

-(BOOL) cancelHandle:(Protocol *)taskType task:(id<IVTTask>)task{
    
    if(taskType == @protocol(IHCCampCommentTask)){
        
        VTAPITask * cancelTask = [[VTAPITask alloc] init];
        
        [cancelTask setTask:task];
        [cancelTask setTaskType:@protocol(IHCCampCommentTask)];
        
        [self.context cancelHandle:@protocol(IVTAPICancelTask) task:cancelTask];
        
        return YES;
    }
    
    return NO;
}

@end
