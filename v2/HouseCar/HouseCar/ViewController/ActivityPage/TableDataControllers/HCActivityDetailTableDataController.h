//
//  HCActivityDetailTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCTableHeaderView.h"

typedef NS_ENUM(NSInteger,TableSectionType){
    TableSectionTypeAttendUser,//报名用户section
    TableSectionTypeLocation,//地图定位section
    TableSectionTypeAttendInfo,//活动信息section
    TableSectionTypetravelRoute,//活动路线section
    TableSectionTypeComment//用户评论section
};

@interface HCActivityDetailTableDataController : HCTableDataController

@property (strong, nonatomic) IBOutlet HCTableHeaderView *tableHeaderView;
@property (strong, nonatomic) IBOutlet UITableViewCell *commentHeaderView;

@end
