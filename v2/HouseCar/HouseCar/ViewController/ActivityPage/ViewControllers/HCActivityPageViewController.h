//
//  HCActivityPageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCActivityTableDataController.h"
#import "HCActivityTabPageDataController.h"

@interface HCActivityPageViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCActivityTabPageDataController *tabPageDataController;
@property (strong, nonatomic) IBOutlet HCActivityTableDataController *recentTableDataController;
@property (strong, nonatomic) IBOutlet HCActivityTableDataController *myTableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *recentDataSource;
@property (strong, nonatomic) IBOutlet VTDataSource *mydataSource;

@end
