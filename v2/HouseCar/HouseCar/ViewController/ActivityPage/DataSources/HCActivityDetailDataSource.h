//
//  HCActivityDetailDataSource.h
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>
#import "HCBaseURLDataSource.h"

@interface HCActivityDetailDataSource : HCBaseURLDataSource

@property (nonatomic,strong)NSDictionary *headerViewDic;

@property (nonatomic,strong)NSMutableArray *attendUsers;

@property (nonatomic,strong)NSMutableArray *locationInfos;

@property (nonatomic,strong)NSMutableArray *attendInfos;

@property (nonatomic,strong)NSMutableArray *travelRouteInfos;

@property (nonatomic,strong)NSMutableArray *costActivityInfos;

@property(nonatomic, strong) NSMutableArray * activityComments;

@end
