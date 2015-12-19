//
//  NSString+VTUrlStringEncode.m
//  vTeam
//
//  Created by jixuhui on 15/7/24.
//  Copyright (c) 2015年 hailong.org. All rights reserved.
//

#import "NSString+VTUrlStringEncode.h"

@implementation NSString (VTUrlStringEncode)

- (NSString*)URLEncodedString
{
    NSString *newString = NSMakeCollectable([(NSString *)CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,(CFStringRef)self, NULL, CFSTR(":/?#[]@!$ &'()*+,;=\"<>%{}|\\^~`"),kCFStringEncodingUTF8) autorelease]);
    if (newString) {
        return newString;
    }
    return @"";
}

@end
