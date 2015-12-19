//
//  SFImageViewAutoFit.m
//  SinaFinance
//
//  Created by liming on 15/4/8.
//  Copyright (c) 2015年 ytang.com. All rights reserved.
//

#import "HCGuideImageViewFit.h"

@implementation HCGuideImageViewFit

- (void)awakeFromNib
{
    if ([UIScreen mainScreen].bounds.size.height < 568.0f){
        if(self.tag == 1){
            [self setImage:[UIImage imageNamed:@"guide_1.png"]];
        }else if(self.tag == 2){
            [self setImage:[UIImage imageNamed:@"guide_2.png"]];
        }if(self.tag == 3){
            [self setImage:[UIImage imageNamed:@"guide_3.png"]];
        }
    }
}

@end
