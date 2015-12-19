//
//  HCHomePageViewController.h
//  HouseCar
//
//  Created by tangyin on 15/11/11.
//  Copyright © 2015年 ytang.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCSearchBarView.h"
#import "HCHomePageTableDataController.h"
#import "HCTravelNotesDataSource.h"
#import "HCCityViewController.h"

@interface HCHomePageViewController : HCViewController <VTTableDataControllerDelegate, HCHomePageTableDataDelegate ,HCSearchBarViewViewDelegate>

@property (strong, nonatomic) IBOutlet HCHomePageTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet HCTravelNotesDataSource *dataSource;

@end
