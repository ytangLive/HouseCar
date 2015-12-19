//
//  NSDate+Helper.m
//  VTTeamProject
//
//  Created by sinanews on 13-8-29.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "NSDate+Helper.h"

@implementation NSDate (Helper)

- (BOOL)isTheSameDayCompareWithDate:(NSDate *)date
{
    NSCalendar* calendar = [NSCalendar currentCalendar];
    unsigned unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit |  NSDayCalendarUnit;
    NSDateComponents* comp1 = [calendar components:unitFlags fromDate:self];
    NSDateComponents* comp2 = [calendar components:unitFlags fromDate:date];
    return [comp1 day]   == [comp2 day] && [comp1 month] == [comp2 month] && [comp1 year]  == [comp2 year];
}

- (NSString *) labelDateTimeString{
    
    static NSDateFormatter * dateFormatter = nil;
    
    if(dateFormatter == nil){
        dateFormatter = [[NSDateFormatter alloc] init];
    }
    
    NSCalendar* calendar = [NSCalendar currentCalendar];
    unsigned unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit |  NSDayCalendarUnit;
    
    NSDateComponents* nowComponents= [calendar components:unitFlags fromDate:[NSDate date]];
    
    NSDateComponents* components = [calendar components:unitFlags fromDate:self];

    if(nowComponents.year == components.year && nowComponents.month == components.month && nowComponents.day == components.day){
        [dateFormatter setDateFormat:@"今天 HH:mm"];
    }
    else if(nowComponents.year == components.year){
        [dateFormatter setDateFormat:@"MM-dd HH:mm"];
    }
    else {
        [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm"];
    }
    
    return [dateFormatter stringFromDate:self];
}

- (NSString *) labelDateTimeStringWithYear{
    
    static NSDateFormatter * dateFormatter = nil;
    
    if(dateFormatter == nil){
        dateFormatter = [[NSDateFormatter alloc] init];
    }
    
    [dateFormatter setDateFormat:@"yyyy-MM-dd"];
    
    return [dateFormatter stringFromDate:self];
}

+(BOOL)currentInDate:(NSString *)begDate AndEnd:(NSString *)endDate{
    NSDate * nowDate = [NSDate date];
    NSDateFormatter *formater = [[ NSDateFormatter alloc] init];
    [formater setDateFormat:@"yyyy-MM-dd"]; //yyyy-MM-dd HH:mm:ss
    
    NSString * nowStr = [formater stringFromDate:nowDate];
    NSString * begDateStr = [NSString stringWithFormat:@"%@ %@",nowStr,begDate];
    NSString * endDateStr = [NSString stringWithFormat:@"%@ %@",nowStr,endDate];
    
    [formater setDateFormat:@"yyyy-MM-dd HH:mm"]; //yyyy-MM-dd HH:mm:ss
    
    NSDate * dateBeg = [formater dateFromString:begDateStr];
    NSDate * dateEnd = [formater dateFromString:endDateStr];
    if ([dateBeg compare:dateEnd] == NSOrderedDescending) {
        if (([nowDate compare:dateBeg] == NSOrderedDescending) || ([nowDate compare:dateEnd] == NSOrderedAscending)) {
            return YES;
        }else{
            return NO;
        }
    }else{
        if (([nowDate compare:dateBeg] ==NSOrderedDescending) && ([nowDate compare:dateEnd] == NSOrderedAscending) ) {
            return YES;
        }else{
            return NO;
        }
    }
}


@end
