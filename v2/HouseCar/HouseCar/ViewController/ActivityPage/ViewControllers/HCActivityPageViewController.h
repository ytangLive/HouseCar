//
//  HCActivityPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCActivityTableDataController.h"

@interface HCActivityPageViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCActivityTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
