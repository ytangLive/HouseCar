//
//  UIDeviceHelper.h
//  SinaNews
//
//  Created by li na on 13-10-22.
//  Copyright (c) 2013年 sina. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UIDeviceHelper : NSObject

+ (NSString *) platform;
+ (NSString *) getReturnPlat:(NSString *)platform;
+ (NSString *) getStandardPlat;

@end
