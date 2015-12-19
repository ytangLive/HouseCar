//
//  VTBECrashObject.m
//  vTeam
//
//  Created by jixuhui on 14/10/29.
//  Copyright (c) 2014年 hailong.org. All rights reserved.
//

#import "VTBECrashObject.h"

@implementation VTBECrashObject
@synthesize exception = _exception;


-(void)dealloc
{
    [_exception release];
    [super dealloc];
}
@end
