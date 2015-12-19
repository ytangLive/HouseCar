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
#import "HCHotActivitiesDataSource.h"
#import "HCTravelNotesDataSource.h"

typedef NS_ENUM(NSInteger,TableSectionType){
    TableSectionHotActivity,//热门活动
    TableSectionTravelNotes,//精彩游记
};


@interface  HCHomePageTableDataControllerSection : NSObject

@property(nonatomic,retain) IBOutlet UITableViewCell *headerView;
@property(nonatomic,assign) NSInteger index;

@end

@class HCHomePageTableDataController;

@protocol HCHomePageTableDataDelegate <NSObject>

-(void)HCHomePageTableDataClickedWithData:(NSDictionary*)data;

@end

@interface HCHomePageTableDataController : HCTableDataController<HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCHomePageTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;

@property(nonatomic,retain) IBOutletCollection(HCHomePageTableDataControllerSection) NSArray * sections;


@property (strong, nonatomic) IBOutlet HCHotActivitiesDataSource *hotActivitiesDataSource;

@end
