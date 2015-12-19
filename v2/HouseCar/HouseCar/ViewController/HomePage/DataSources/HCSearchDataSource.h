//
//  HCSearchDataSource.h
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCSearchDataSource : VTDataSource

@property(nonatomic, strong) NSMutableArray * hotSearchArr;
@property(nonatomic, strong) NSMutableArray * historySearchKeys;

@end
