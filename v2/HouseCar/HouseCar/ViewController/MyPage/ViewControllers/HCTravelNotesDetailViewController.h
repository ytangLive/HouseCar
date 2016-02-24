//
//  HCTravelNotesDetailViewController.h
//  HouseCar
//
//  Created by tangyin on 16/1/29.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCTravelNotesTableDataController.h"
#import "HCTravelNotesDataSource.h"

@interface HCTravelNotesDetailViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCTravelNotesTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet HCTravelNotesDataSource *dataSource;

@end
