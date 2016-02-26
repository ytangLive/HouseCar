//
//  HCTravelNotesDetailDataSource.m
//  HouseCar
//
//  Created by tangyin on 16/2/4.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCTravelNotesDetailDataSource.h"
#import "HCTravelNotesTableDataController.h"
#import "HCHeaderCell.h"

@implementation HCTravelNotesDetailDataSource

- (void)loadResultsData:(id)resultsData
{
    [super loadResultsData:resultsData];
    
    [self handleResultsData:resultsData];
}

- (void)handleResultsData:(NSDictionary *)resultsData
{
    
    for(int i = 0; i < 2; i++){
        UIViewController * viewController = [[UIViewController alloc] initWithNibName:@"HCTrabelNotesDetailHeaderCell" bundle:nil];
        
        UIView * view = [viewController view];
        
        if([view isKindOfClass:[HCHeaderCell class]]){
            HCTravelNotesTableDataControllerSection *section = [[HCTravelNotesTableDataControllerSection alloc] init];
            HCHeaderCell *headerCell = (HCHeaderCell *)view;
            headerCell.titleLabel.text = [NSString stringWithFormat:@"第%d天 %@",i+1,@"2015.12.15"];
            section.headerView = headerCell;
            section.index = i + 1;
            [self.sections addObject:section];
            
            if(i == 0){
                NSDictionary *travelNotesInfo1 = @{@"imageUrl":@"dali1", @"content":@"这个时候我迎来了到达大理之后的第一个大surprise，不是这里的天气多好，云多美丽。更不是所谓的单身艳遇。而是，这这这白天的大中午，整个机场居然没有一个人！", @"location":@"中国云南大理",@"travelDate":@"2015.12.15 21:00"};
                NSDictionary *travelNotesInfo2 = @{@"imageUrl":@"dali2", @"content":@"朋友把客栈定在了才村码头，其实这边不如对岸双廊豪华，这边都是些比较接地气儿的客栈，但胜在这里离大理古镇只要5分钟车程，而且背靠苍山，洱海。",@"travelDate":@"2015.12.15 22:05"};
                
                NSArray *travelNotes = @[travelNotesInfo1, travelNotesInfo2];
                [self.travelNotesInfos setObject:travelNotes forKey:[NSString stringWithFormat:@"day_%d",i]];
            }else{
                
                NSDictionary *travelNotesInfo = @{@"imageUrl":@"dali3", @"content":@"吃完饭打了15块钱出租车到古镇，都是讲价的，没有打表这么一说。但真的可能只有3公里。由此可见当地如此商业化的物价。",@"travelDate":@"2015.12.16 12:05"};
                
                NSArray *travelNotes = @[travelNotesInfo];
                [self.travelNotesInfos setObject:travelNotes forKey:[NSString stringWithFormat:@"day_%d",i]];
                
            }
            
        }
    }
}

- (NSMutableDictionary *)travelNotesInfos
{
    if(!_travelNotesInfos){
        _travelNotesInfos = [[NSMutableDictionary alloc] init];
    }
    return _travelNotesInfos;
}

- (NSMutableArray *)sections
{
    if(!_sections){
        _sections = [[NSMutableArray alloc] init];
    }
    return _sections;
}



@end
