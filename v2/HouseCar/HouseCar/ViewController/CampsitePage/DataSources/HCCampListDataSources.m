//
//  HCCampListDataSources.m
//  HouseCar
//
//  Created by tangyin on 16/3/28.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCCampListDataSources.h"

@implementation HCCampListDataSources

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
            NSString *campID = [dataObject stringValueForKey:@"id"];
            NSString *campRating = [self customFieldsDataForKey:dataObject fieldsSubKey:@"camp_rate"];
            
            NSString *imageUrl = [dataObject stringValueForKeyPath:@"thumbnail_images.medium.url"];
            
            NSNumber *campRatingNum = @0;
            if(campRating && [campRating length] > 0){
                campRatingNum = [NSNumber numberWithFloat:[campRating floatValue]];
            }
            
            [newDataObject setValue:campID forKey:@"campID"];
            [newDataObject setValue:title forKey:@"title"];
            [newDataObject setValue:campRatingNum forKey:@"campRating"];
            [newDataObject setValue:imageUrl forKey:@"imageUrl"];
            
            [self.dataObjects replaceObjectAtIndex:i withObject:newDataObject];
        }
    }
}

@end
