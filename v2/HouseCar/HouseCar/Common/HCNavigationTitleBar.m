//
//  HCNavigationTitleBar.m
//  HouseCar
//
//  Created by ytang on 15/02/22.
//  Copyright (c) 2014年 YTANG. All rights reserved.
//

#import "HCNavigationTitleBar.h"

@implementation HCNavigationTitleBar

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        self.backgroundColor = DefaultNavBarColor;
    }
    return self;
}


@end
