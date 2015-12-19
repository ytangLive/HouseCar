//
//  NSString+VTStringExtension.m
//  vTeam
//
//  Created by jixuhui on 15/7/24.
//  Copyright (c) 2015年 hailong.org. All rights reserved.
//

#import "NSString+VTStringExtension.h"

@implementation NSString (VTUrlStringEncode)

- (NSString*)URLEncodedString
{
    NSString *newString = NSMakeCollectable([(NSString *)CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,(CFStringRef)self, NULL, CFSTR(":/?#[]@!$ &'()*+,;=\"<>%{}|\\^~`"),kCFStringEncodingUTF8) autorelease]);
    if (newString) {
        return newString;
    }
    return @"";
}

- (BOOL)checkContainString:(NSString *)str
{
    if ([self getCurDeviceVersion]<8.0) {
        NSRange r = [self rangeOfString:str];
        if(r.location == NSNotFound){
            return NO;
        }else{
            return YES;
        }
    }else{
        return [self containsString:str];
    }
}

-(float)getCurDeviceVersion
{
    return [[[UIDevice currentDevice] systemVersion] floatValue];
}
@end
