//
//  SFInfoVer2FinanTableDataController.h
//  HouseCar
//
//  Created by shieh fabo on 14-9-10.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>
#import "HCTableFocusViewController.h"
#import "HCTableDataController.h"

@class HCCampHomeTableDataController;

@protocol HCCarBaseTableDataDelegate <NSObject>

-(void)HCCarBaseTableDataClickedWithData:(NSDictionary*)data;

@end

@interface HCCarBaseTableDataController : HCTableDataController <HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCCarBaseTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;

@property (nonatomic, strong)NSDate *defaultDate;
@property (nonatomic, strong)NSDate *takeDate;
@property (nonatomic, strong)NSDate *returnDate;
@property (strong, nonatomic)NSDateFormatter *defaultDateFormatter;

@property (strong, nonatomic) NSString *pageType;

@property (strong, nonatomic) VTTableViewCell *menuTableCell;

@end
