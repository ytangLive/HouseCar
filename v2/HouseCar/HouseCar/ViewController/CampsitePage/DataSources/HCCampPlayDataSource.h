//
//  HCCampPlayDataSource.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>
#import "HCBaseURLDataSource.h"

@interface HCCampPlayDataSource : HCBaseURLDataSource

@property(nonatomic)NSString *campID;
@property(nonatomic, strong) NSMutableArray * contentArr;
@property(nonatomic, strong) NSMutableArray * commentArr;

@end
