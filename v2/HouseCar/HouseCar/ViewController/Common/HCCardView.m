//
//  HCCardView.m
//  HouseCar
//
//  Created by tangyin on 15/12/2.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCardView.h"

@implementation HCCardView

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        self.layer.cornerRadius = 5.0;
        self.layer.masksToBounds = YES;
        self.layer.borderWidth = 1.0;
        self.layer.borderColor = COLOR_RGB(190, 190, 190).CGColor;
    }
    return self;
}

@end
