//
//  HCImageView.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-26.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCImageView.h"

@implementation HCImageView

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if(self){
        
    }
    return self;
}

- (void)awakeFromNib
{
    if(_iscornerRound){
        self.layer.cornerRadius = 6;
        self.layer.masksToBounds = YES;
    }
}

-(void)setDefaultImageStr:(NSString *)defaultImageStr
{
    UIImage* tempImage = [UIImage imageNamed:defaultImageStr];
    self.defaultImage = tempImage;
}

@end
