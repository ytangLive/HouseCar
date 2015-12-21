//
//  HCRegionPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCRegionTableDataController.h"

@interface HCRegionPageViewController : HCViewController <VTTableDataControllerDelegate, HCRegionTableDataDelegate>

@property (strong, nonatomic) IBOutlet HCRegionTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
