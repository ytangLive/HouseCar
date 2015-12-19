//
//  NSString+URLCoding.m
//  VTTeamProject
//
//  Created by sinanews on 13-9-3.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "NSString+URLCoding.h"

@implementation NSString (URLCoding)

- (NSString *)encodeToPercentEscapeString
{
    // Encode all the reserved characters, per RFC 3986
    // (<http://www.ietf.org/rfc/rfc3986.txt>)
    NSString *outputStr = CFBridgingRelease(
                                            CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,
                                                                                    (__bridge CFStringRef)self,
                                                                                    NULL,
                                                                                    (CFStringRef)@"!*'();:@&=+$,/?%#[]",
                                                                                    kCFStringEncodingUTF8));
    return outputStr;
}

- (NSString *)decodeFromPercentEscapeString
{
    NSMutableString *outputStr = [NSMutableString stringWithString:self];
    [outputStr replaceOccurrencesOfString:@"+"
                               withString:@" "
                                  options:NSLiteralSearch
                                    range:NSMakeRange(0, [outputStr length])];
    
    return [outputStr stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
}

-(NSDictionary *) queryValues{
    NSMutableDictionary * d = [NSMutableDictionary dictionaryWithCapacity:4];
    NSArray * items = [self componentsSeparatedByString:@"&"];
    for(NSString * item in items){
        NSArray * kv = [item componentsSeparatedByString:@"="];
        if([kv count] >1){
            [d setValue:[kv objectAtIndex:1] forKey:[kv objectAtIndex:0]];
        }
    }
    return d;
}

@end
