//
//  HCMyPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCTableDataController.h"

@interface HCMyPageViewController : HCViewController

@property (strong, nonatomic) IBOutlet HCTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@property (weak, nonatomic) IBOutlet UIView *tableViewHeaderView;

@end
