//
//  NSDate+Helper.h
//  VTTeamProject
//
//  Created by sinanews on 13-8-29.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDate (Helper)

- (BOOL)isTheSameDayCompareWithDate:(NSDate *)date;

- (NSString *) labelDateTimeString;

- (NSString *) labelDateTimeStringWithYear;

+(BOOL)currentInDate:(NSString *)begDate AndEnd:(NSString *)endDate;

@end
