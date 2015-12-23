//
//  HCHistorySearchAddTask.h
//  HouseCar
//
//  Created by tangyin on 15/12/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@protocol IHCHistorySearchAddTask <IVTTask>

@property(nonatomic,retain) NSString * searchKey;
@property(nonatomic) double searchDate;

@end

@interface HCHistorySearchAddTask : VTTask <IHCHistorySearchAddTask>

@end
