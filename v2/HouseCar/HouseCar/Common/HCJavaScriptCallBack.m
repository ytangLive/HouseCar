//
//  MFJavaScriptCallBack.m
//  multiframe
//
//  Created by Tianyu on 14-12-17.
//  Copyright (c) 2014年 YTANG. All rights reserved.
//

#import "HCJavaScriptCallBack.h"
//#import "iToast.h"

@implementation HCJavaScriptCallBack
{
    UIViewController *_viewController;
}

- (instancetype)initWithSender:(UIViewController *)viewController
{
    self = [super init];
    if(self){
        _viewController = viewController;
    }
    return self;
}

- (void)showToast:(NSString *)msg
{
    //[[[[iToast makeText:msg] setGravity:iToastGravityTop] setDuration:1000] show];
}

@end
