//
//  HCActivityListDataSource.m
//  HouseCar
//
//  Created by tangyin on 16/1/12.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityListDataSource.h"

@implementation HCActivityListDataSource

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    [self handleResultsData];
    
}

- (void)handleResultsData
{
    if(self.dataObjects && [self.dataObjects count] > 0){
        for (int i = 0; i < [self.dataObjects count]; i++) {
            NSDictionary *dataObject = [self.dataObjects objectValueAtIndex:i];
            NSMutableDictionary *newDataObject = [dataObject mutableCopy];
            NSString *title = [dataObject stringValueForKey:@"title"];
            NSString *activityDate = [self customFieldsDataForKey:dataObject fieldsSubKey:@"activity-date"];
            NSString *activityLocation = [self customFieldsDataForKey:dataObject fieldsSubKey:@"location"];
            NSString *destination = [self customFieldsDataForKey:dataObject fieldsSubKey:@"destination"];
            if(destination && [destination length ] > 0){
                destination = [NSString stringWithFormat:@"活动地点 %@",destination];
            }else{
                destination = @"";
            }
            NSString *imageUrl = [dataObject stringValueForKeyPath:@"thumbnail_images.medium.url"];
           
            [newDataObject setValue:title forKey:@"title"];
            [newDataObject setValue:activityDate forKey:@"activityDate"];
            [newDataObject setValue:activityLocation forKey:@"activityLocation"];
            [newDataObject setValue:destination forKey:@"destination"];
            [newDataObject setValue:imageUrl forKey:@"imageUrl"];
            
            [self.dataObjects replaceObjectAtIndex:i withObject:newDataObject];
        }
    }
}

@end
