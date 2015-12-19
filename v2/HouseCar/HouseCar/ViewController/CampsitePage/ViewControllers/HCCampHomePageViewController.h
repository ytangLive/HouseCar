//
//  HCActivityPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCCampHomeTableDataController.h"

@interface HCCampHomePageViewController : HCViewController <VTTableDataControllerDelegate, HCCampHomeTableDataDelegate>

@property (strong, nonatomic) IBOutlet HCCampHomeTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
