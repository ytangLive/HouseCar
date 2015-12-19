//
//  HCCarBookViewController.h
//  HouseCar
//
//  Created by tangyin on 15/11/23.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCCarBaseTableDataController.h"

@interface HCCarBaseViewController : HCViewController <VTTableDataControllerDelegate, HCCarBaseTableDataDelegate>

@property (strong, nonatomic) IBOutlet HCCarBaseTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet VTDataSource *dataSource;

@end
