//
//  HCCampHomeTableDataController.h
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>
#import "HCTableFocusViewController.h"
#import "HCTableDataController.h"

@class HCCampHomeTableDataController;

@protocol HCCampHomeTableDataDelegate <NSObject>

-(void)HCCampHomeTableDataClickedWithData:(NSDictionary*)data;

@end

@interface HCCampHomeTableDataController : HCTableDataController <HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCCampHomeTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;

@property (strong, nonatomic)NSDateFormatter *defaultDateFormatter;

@end
