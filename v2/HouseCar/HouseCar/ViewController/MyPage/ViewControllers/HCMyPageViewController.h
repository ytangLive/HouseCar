//
//  HCMyPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCMyPageTableDataController.h"

@interface HCMyPageViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCMyPageTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
