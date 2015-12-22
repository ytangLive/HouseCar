//
//  HCMyPageTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/22.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCNavigationTitleBar.h"

@interface HCMyPageTableDataController : HCTableDataController

@property (weak, nonatomic) IBOutlet UIView *backgroundView;
@property (weak, nonatomic) IBOutlet UIView *shadowView;
@property (weak, nonatomic) IBOutlet UIView *tableViewHeaderView;
@property (weak, nonatomic) IBOutlet HCNavigationTitleBar *toolBarView;

@end
