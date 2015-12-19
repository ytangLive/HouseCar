//
//  HCCarOrderTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/1.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCTableFocusViewController.h"

@class HCCarOrderTableDataController;

@protocol HCCarOrderTableDataDelegate <NSObject>

- (void)HCCarOrderTableDataClickedWithData:(NSDictionary*)data;

- (void)orderDaysViewTap:(id)sender;

- (void)carTakeWayViewTap:(id)sender;

@end

@interface HCCarOrderTableDataController : HCTableDataController <HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCCarOrderTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;

@property (strong,nonatomic) NSString *dataUrlKey;

@property (strong, nonatomic) NSDictionary *detailInfo;

@end
