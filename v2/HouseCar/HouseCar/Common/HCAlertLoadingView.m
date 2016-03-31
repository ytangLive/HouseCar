//
//  HCAlertLoadingView.m
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import "HCAlertLoadingView.h"

#import <QuartzCore/QuartzCore.h>

@interface HCAlertLoadingView(){
    
}

@property(nonatomic,retain) UIWindow * alertWindow;

@end

@implementation HCAlertLoadingView

@synthesize alertWindow = _alertWindow;

-(void) dealloc{
    
    NSLog(@"SFAlertLoadingView dealloc");
    
}

-(void) showToView:(UIView *) view animated:(BOOL) animated{
    
    self.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
    
    [self startAnimating];
    
    CGRect r = self.frame;
    
    CGSize size = view.bounds.size;
    if (size.width == 320) {//屏幕适配添加判断
        size.width = [UIScreen mainScreen].bounds.size.width;
    }
    
    r.origin = CGPointMake((size.width - r.size.width) / 2.0, (size.height - r.size.height) /2.0);
    
    [self setFrame:r];
    
    [view addSubview:self];
    
    if(animated){
        
        [self setAlpha:0.0];
        
        [UIView beginAnimations:nil context:nil];
        [UIView setAnimationDuration:0.2];
        
        [self setAlpha:1.0];
        
        [UIView commitAnimations];
    }

}

-(void) showAnimated:(BOOL) animated{
    
    UIWindow * alertWindow = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    
    [alertWindow setWindowLevel:UIWindowLevelAlert];
    [alertWindow setBackgroundColor:[UIColor colorWithWhite:0.0 alpha:0.4]];
    [alertWindow setHidden:NO];
    
    self.alertWindow = alertWindow;
    
    [self showToView:alertWindow animated:animated];
}

-(void) closeAnimated:(BOOL) animated{
    if(self.alertWindow){
        self.alertWindow = nil;
    }
    else{
        [self removeFromSuperview];
    }
}

@end
