//
//  HCTravelNotesTableDataController.h
//  HouseCar
//
//  Created by tangyin on 16/1/29.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCNavigationTitleBar.h"

@interface  HCTravelNotesTableDataControllerSection : NSObject

@property(nonatomic,retain) IBOutlet UITableViewCell *headerView;
@property(nonatomic,assign) NSInteger index;

@end

@interface HCTravelNotesTableDataController : HCTableDataController

@property (weak, nonatomic) IBOutlet UIView *backgroundView;
@property (weak, nonatomic) IBOutlet UIView *shadowView;
@property (weak, nonatomic) IBOutlet UIView *tableViewHeaderView;
@property (weak, nonatomic) IBOutlet HCNavigationTitleBar *toolBarView;

@property(nonatomic,strong) NSArray * sections;

@end
