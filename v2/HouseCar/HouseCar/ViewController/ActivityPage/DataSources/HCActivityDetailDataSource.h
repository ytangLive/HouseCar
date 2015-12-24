//
//  HCActivityDetailDataSource.h
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCActivityDetailDataSource : VTURLDataSource

@property (nonatomic,strong)NSString *headerViewUrl;

@property (nonatomic,strong)NSMutableDictionary *locationInfos;

@property (nonatomic,strong)NSMutableDictionary *attendInfos;

@property (nonatomic,strong)NSMutableArray *travelRouteInfos;

@property (nonatomic,strong)NSMutableArray *costActivityInfos;

@end
