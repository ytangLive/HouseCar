//
//  HCTravelNotesDetailDataSource.h
//  HouseCar
//
//  Created by tangyin on 16/2/4.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCTravelNotesDetailDataSource : VTURLDataSource

@property(nonatomic,strong) NSMutableArray *sections;
@property(nonatomic, strong) NSMutableDictionary *travelNotesInfos;

@end
