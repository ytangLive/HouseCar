//
//  HCHandleNativeDataService.m
//  HouseCar
//
//  Created by tangyin on 15/12/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCHandleNativeDataService.h"
#import "HCHistorySearchAddTask.h"
#import "HCHistorySearchGetTask.h"
#import "HCHistorySearchDataObject.h"
#import "IHCHistorySearchClearTask.h"

@implementation HCHandleNativeDataService

- (BOOL)handle:(Protocol *)taskType task:(id<IVTTask>)task priority:(NSInteger)priority
{
    //历史搜索存储数据
    if(taskType == @protocol(IHCHistorySearchAddTask)){
        id<IHCHistorySearchAddTask> addTask = (id<IHCHistorySearchAddTask>) task;
        
        VTDBContext * dbContext = [(id<HCContext>) self.context appDBContext];
        
        NSString *searchKey = [addTask searchKey];
        double searchDate = [[NSDate date] timeIntervalSince1970];
        
        id<IVTSqliteCursor> cursor = [dbContext query:[HCHistorySearchDataObject class] sql:@" WHERE [searchKey]=:searchKey" data:[NSDictionary dictionaryWithObjectsAndKeys:searchKey,@"searchKey", nil]];
        
        HCHistorySearchDataObject * dataObject = [[HCHistorySearchDataObject alloc] init];
        
        if([cursor next]){
            
            [cursor toDataObject:dataObject];
            dataObject.searchDate = searchDate;
            
            [dbContext updateObject:dataObject];
        }
        else{
            dataObject.searchKey = searchKey;
            dataObject.searchDate = searchDate;
            
            [dbContext insertObject:dataObject];
        }
        
        [cursor close];
        
        return YES;
    }
    //历史搜索获取数据
    if(taskType == @protocol(IHCHistorySearchGetTask)){
        id<IHCHistorySearchGetTask> searchTask = (id<IHCHistorySearchGetTask>) task;
        
        VTDBContext * dbContext = [(id<HCContext>)self.context appDBContext];
        
        NSMutableString * sql = [NSMutableString stringWithCapacity:128];
        
        [sql appendString:@" ORDER BY [searchDate] DESC"];
        
        id<IVTSqliteCursor> cursor = [dbContext query:[HCHistorySearchDataObject tableClass] sql:sql data:nil];
        
        NSMutableArray *searchItems = [NSMutableArray array];
        
        while([cursor next]){
            
            HCHistorySearchDataObject * dataObject = [[HCHistorySearchDataObject alloc] init];
            
            [cursor toDataObject:dataObject];
            
            [searchItems addObject:dataObject];
        }
        
        [cursor close];
        
        [searchTask setSearchKeys:searchItems];
        
        return YES;
    }//清除历史搜索
    if(taskType == @protocol(IHCHistorySearchClearTask)){
        
        VTDBContext * dbContext = [(id<HCContext>)self.context appDBContext];
        
        NSString * sql = @"";
        
        id<IVTSqliteCursor> cursor = [dbContext query:[HCHistorySearchDataObject tableClass] sql:sql data:nil];
        
        while([cursor next]){
            
            HCHistorySearchDataObject * dataObject = [[HCHistorySearchDataObject alloc] init];
            
            [cursor toDataObject:dataObject];
            
            [dbContext deleteObject:dataObject];
        }
        
        [cursor close];
        
        return YES;
    }
    
    return NO;
}

@end
