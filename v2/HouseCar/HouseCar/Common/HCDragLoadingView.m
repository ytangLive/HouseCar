//
//  HCDragLoadingView.m
//  SinaFinance
//
//  Created by ytang on
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import "HCDragLoadingView.h"

@implementation HCDragLoadingView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(void) stopAnimation
{
    if( self.animating ){
        if([self.loadingView respondsToSelector:@selector(stopAnimating)]){
            [self.loadingView performSelector:@selector(stopAnimating) withObject:nil];
        }
        
        self.animating = NO;
    }
}

@end
