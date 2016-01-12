//
//  HCActivityListViewController.h
//  HouseCar
//
//  Created by tangyin on 16/1/12.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCActivityTableDataController.h"

@interface HCActivityListViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCActivityTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
