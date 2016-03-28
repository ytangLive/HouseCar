//
//  HCBaseURLDataSource.h
//  HouseCar
//
//  Created by tangyin on 16/3/28.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCBaseURLDataSource : VTURLDataSource

- (NSString *)customFieldsDataForKey:(NSDictionary *)customFieldsData fieldsSubKey:(NSString *)subKey;

@end
