//
//  HCBaseURLDataSource.m
//  HouseCar
//
//  Created by tangyin on 16/3/28.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCBaseURLDataSource.h"

#define PrefixKey @"custom_fields"

@implementation HCBaseURLDataSource

- (NSString *)customFieldsDataForKey:(NSDictionary *)customFieldsData fieldsSubKey:(NSString *)subKey
{
    if(customFieldsData && [customFieldsData count] > 0){
        NSString *fullKeyPath = @"";
        if(subKey && [subKey length] > 0){
            fullKeyPath = [NSString stringWithFormat:@"%@.%@",PrefixKey,subKey];
        }else{
            fullKeyPath = PrefixKey;
        }
        NSArray *array = [customFieldsData arrayValueForKeyPath:fullKeyPath];
        
        if(array && [array count] > 0){
            return [array lastObject];
        }else{
            return @"";
        }
    }
    
    return @"";
}

@end
