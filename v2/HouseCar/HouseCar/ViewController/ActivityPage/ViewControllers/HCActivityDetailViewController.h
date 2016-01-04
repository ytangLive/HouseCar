//
//  HCActivityDetailViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/24.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCActivityDetailTableDataController.h"
#import "HCActivityDetailDataSource.h"

@interface HCActivityDetailViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCActivityDetailTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet HCActivityDetailDataSource *dataSource;

@end
