//
//  HCCampPlayViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/8.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCCampPlayTableDataController.h"
#import "HCCampPlayDataSource.h"

@interface HCCampPlayViewController : HCViewController <VTTableDataControllerDelegate, HCCampPlayTableDataDelegate>

@property (strong, nonatomic) IBOutlet HCCampPlayTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet HCCampPlayDataSource *dataSource;

@end
