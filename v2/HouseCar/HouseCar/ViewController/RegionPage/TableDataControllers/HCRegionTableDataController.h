//
//  HCRegionTableDataController.h
//  HouseCar
//
//  Created by tangyin on 15/12/21.
//  Copyright © 2015年 sina.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCTableFocusViewController.h"

@class HCCampHomeTableDataController;

@protocol HCRegionTableDataDelegate <NSObject>

-(void)HCRegionTableDataClickedWithData:(NSDictionary*)data;

@end


@interface HCRegionTableDataController : HCTableDataController <HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCRegionTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;


@end
