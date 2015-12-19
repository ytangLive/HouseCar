//
//  HCSearchViewController.h
//  HouseCar
//
//  Created by tangyin on 15/12/11.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCViewController.h"
#import "HCSearchBarView.h"
#import "HCSearchDataSource.h"
#import "HCSearchTableDataController.h"

@interface HCSearchViewController : HCViewController <VTTableDataControllerDelegate>

@property (strong, nonatomic) IBOutlet HCSearchTableDataController *tableDataController;
@property (strong, nonatomic) IBOutlet HCSearchDataSource *dataSource;

@property (weak, nonatomic) IBOutlet HCSearchBarView *searchBarView;

@end
