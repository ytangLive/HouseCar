//
//  NSString+VTStringExtension.h
//  vTeam
//
//  Created by jixuhui on 15/7/24.
//  Copyright (c) 2015年 hailong.org. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (VTStringExtension)

- (NSString*)URLEncodedString;
- (BOOL)checkContainString:(NSString *)str;

@end
