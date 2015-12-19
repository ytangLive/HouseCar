//
//  HCCarOrderViewcontroller.h
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCCarOrderDataSource.h"
#import "HCCarOrderTableDataController.h"

@interface HCCarOrderViewcontroller : HCViewController <VTTableDataControllerDelegate, HCCarOrderTableDataDelegate>

@property (strong, nonatomic) IBOutlet HCCarOrderDataSource *dataSource;
@property (strong, nonatomic) IBOutlet HCCarOrderTableDataController *tableDataController;


@end
