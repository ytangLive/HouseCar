//
//  HCSearchDataSource.m
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCSearchDataSource.h"

@implementation HCSearchDataSource

- (NSMutableArray *)hotSearchArr
{
    if(!_hotSearchArr){
        _hotSearchArr = [NSMutableArray arrayWithObjects:@"房车",@"出租车",@"北京",@"齐齐哈尔",@"大海", nil];
    }
    return _hotSearchArr;
}

- (NSArray *)historySearchKeys
{
    if(!_historySearchKeys){
        _historySearchKeys = [NSArray array];
    }
    return _historySearchKeys;
}

@end
