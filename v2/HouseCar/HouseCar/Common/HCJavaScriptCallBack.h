//
//  MFJavaScriptCallBack.h
//  multiframe
//
//  Created by Tianyu on 14-12-17.
//  Copyright (c) 2014年 YTANG. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HCJavaScriptCallBack : NSObject

- (instancetype)initWithSender:(UIViewController *)viewController;

- (void)showToast:(NSString *)msg;

@end
