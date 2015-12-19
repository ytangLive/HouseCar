//
//  NSString+URLCoding.h
//  VTTeamProject
//
//  Created by sinanews on 13-9-3.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (URLCoding)
- (NSString *)encodeToPercentEscapeString;
- (NSString *)decodeFromPercentEscapeString;
- (NSDictionary *) queryValues;
@end
