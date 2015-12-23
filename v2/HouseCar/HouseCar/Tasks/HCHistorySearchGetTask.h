//
//  HCHistorySearchGetTask.h
//  HouseCar
//
//  Created by tangyin on 15/12/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@protocol IHCHistorySearchGetTask <IVTTask>

@property(nonatomic,retain) NSArray * searchKeys;

@end

@interface HCHistorySearchGetTask : VTTask <IHCHistorySearchGetTask>

@end
