//
//  HCHistorySearchDataObject.h
//  HouseCar
//
//  Created by tangyin on 15/12/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCHistorySearchDataObject : VTDBDataObject

@property(nonatomic,retain) NSString * searchKey;
@property(nonatomic) double searchDate;

@end
