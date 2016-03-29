//
//  NSString+HCHelper.m
//  HouseCar
//
//  Created by ytang on 16/1/8.
//  Copyright © 2016年 ytang.com. All rights reserved.
//

#import "NSString+HCHelper.h"

@implementation NSString(SFHelper)

- (NSString *)stringFilterScanner{

    NSString * html = [[NSString alloc] initWithString:self];
    
    if ([[UIDevice currentDevice].systemVersion floatValue] < 7.0) {
        
        NSScanner * scanner = [NSScanner scannerWithString:html];
        NSString * text = nil;
        while([scanner isAtEnd]==NO)
        {
            //找到标签的起始位置
            [scanner scanUpToString:@"<" intoString:nil];
            //找到标签的结束位置
            [scanner scanUpToString:@">" intoString:&text];
            //替换字符
            html = [html stringByReplacingOccurrencesOfString:[NSString stringWithFormat:@"%@>",text] withString:@""];
        }
        //    NSString * regEx = @"<([^>]*)>";
        //    html = [html stringByReplacingOccurrencesOfString:regEx withString:@""];
    }else{
        @try {
            NSAttributedString * attrStr = [[NSAttributedString alloc] initWithData:[html dataUsingEncoding:NSUnicodeStringEncoding] options:@{ NSDocumentTypeDocumentAttribute: NSHTMLTextDocumentType } documentAttributes:nil error:nil];
            html = attrStr.string;
        }
        @catch (NSException *exception) {}
    }
    
    return html;
}

- (NSString *)thousandNumString
{
    NSString * numStr = @"";
    double like_num = [self doubleValue];
    if (like_num >= 100000) {
        NSString * like_num_str = [NSString stringWithFormat:@"%.2lf", like_num/10000.0];
        if ([[like_num_str componentsSeparatedByString:@"."] count] > 1 &&
            [[[like_num_str componentsSeparatedByString:@"."] objectAtIndex:1] isEqualToString:@"0"]) {
            like_num_str = [[like_num_str componentsSeparatedByString:@"."] objectAtIndex:0];
        }
        numStr = [NSString stringWithFormat:@"%@万", like_num_str];
    }else{
        numStr = [NSString stringWithFormat:@"%@", @(like_num)];
    }
    return numStr;
}

@end
