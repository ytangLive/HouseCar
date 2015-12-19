//
//  NSError+SFError.m
//  VTTeamProject
//
//  Created by Zhang Hailong on 13-7-14.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "NSError+SFError.h"


@implementation NSError (SFError)

-(NSString *) message{
    
    switch ([self code]) {
        case 21325:
            return @"用户名或密码错误";
            break;
        case 21303:
            return @"连续登录错误受限制,请稍后重试";
            break;
        case 20008:
            return @"内容不能为空";
            break;
        case 21323:
            return @"用户名密码不能为空";
            break;
        case 10016:
            return @"内容不能为空";
            break;
        case 20003:
            return @"没有开通微博";
            break;
        case 20019:
            return @"发布微博失败, 内容重复";
            break;
        case 24113:
            return @"超过最大设置条数";
            break;
        case 20206:
            return @"评论失败，只有作者关注的用户才能评论";
        case 3:
            return @"服务繁忙，请稍后重试.";
        case -3:
            return @"网络超时";
            break;
        case -100:
            return @"您的账号授权可能已经失效,请重新登录试试";
        default:
            break;
    }
    
    NSString * msg = [self localizedDescription];
    
    
    if([msg isKindOfClass:[NSString class]] && ![msg canBeConvertedToEncoding:NSASCIIStringEncoding]){
        return msg;
    }
    
    if([self code] <0){
        return @"网络超时";
    }
    
    return @"服务器繁忙";
}

@end
