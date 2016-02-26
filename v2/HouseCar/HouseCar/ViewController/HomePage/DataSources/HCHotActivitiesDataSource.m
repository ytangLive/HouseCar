//
//  HCHotActivitiesDataSource.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-15.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCHotActivitiesDataSource.h"
#import "NSDate+Helper.h"

@implementation HCHotActivitiesDataSource

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    [self parseHotActivities];
}

- (void)parseHotActivities
{
    //for (int i = 0 ; i < [self.dataObjects count]; i++) {
    
    if(self.dataObjects && [self.dataObjects count] > 0){
        id dataObject = [self.dataObjects lastObject];
        NSArray *newdataObjects = @[dataObject];
        self.dataObjects = [newdataObjects mutableCopy];
    }
    //}
}

@end
