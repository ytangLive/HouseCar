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

@interface  HCCampHomeTableDataControllerSection : NSObject

@property(nonatomic,retain) IBOutlet UIView *headerView;
@property(nonatomic,assign) NSInteger index;

@end

@class HCCampHomeTableDataController;

@protocol HCCampHomeTableDataDelegate <NSObject>

-(void)HCCampHomeTableDataClickedWithData:(NSDictionary*)data;

@end

@interface HCCampHomeTableDataController : HCTableDataController <HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCCampHomeTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;

@property (strong, nonatomic)NSDateFormatter *defaultDateFormatter;

@property(nonatomic,strong) IBOutletCollection(HCCampHomeTableDataControllerSection) NSArray * sections;

@end
