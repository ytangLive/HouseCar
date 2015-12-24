//
//  HCActivityDetailTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"

typedef NS_ENUM(NSInteger,TableSectionType){
    TableSectionTypeLocation,//地图定位section
    TableSectionTypeAttendInfo,//活动信息section
    TableSectionTypetravelRoute,//活动路线section
    TableSectionTypeCostActivity//活动花费和活动须知section
};

@interface HCActivityDetailTableDataController : HCTableDataController

@end
