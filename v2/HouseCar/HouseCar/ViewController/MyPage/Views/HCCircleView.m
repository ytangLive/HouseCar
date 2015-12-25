//
//  HCCircleView.m
//  HouseCar
//
//  Created by tangyin on 15/12/22.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCCircleView.h"

@interface HCCircleView()

@end

@implementation HCCircleView

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        self.layer.cornerRadius     = CGRectGetWidth(self.bounds)/2.f;
        self.layer.masksToBounds    = NO;
        self.clipsToBounds          = YES;
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self){
        self.layer.cornerRadius     = CGRectGetWidth(self.bounds)/2.f;
        self.layer.masksToBounds    = NO;
        self.clipsToBounds          = YES;
    }
    return self;
}

-(void)setDefaultImageStr:(NSString *)defaultImageStr
{
    UIImage* tempImage = [UIImage imageNamed:defaultImageStr];
    self.defaultImage = tempImage;
}

@end
