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

typedef NS_ENUM(NSInteger,TableSectionType){
    TableSectionTypeTitle,//标题section
    TableSectionTypeComment,//评论Section
};


@interface HCCampPlayTableDataControllerSection : NSObject

@property(nonatomic,retain) IBOutlet UIView * headerView;
@property(nonatomic,assign) NSInteger index;

@end


@class HCCampPlayTableDataController;

@protocol HCCampPlayTableDataDelegate <NSObject>

-(void)HCCampPlayTableDataClickedWithData:(NSDictionary*)data;

@end

@interface HCCampPlayTableDataController : HCTableDataController<HCTableFocusViewProtocol>

@property(weak,nonatomic)id<HCCampPlayTableDataDelegate> tableDatadelegate;

@property (strong, nonatomic) IBOutlet HCTableFocusViewController *focusController;
@property (strong,nonatomic) NSString *dataUrlKey;

@property(nonatomic,retain) IBOutletCollection(HCCampPlayTableDataControllerSection) NSArray * sections;

@property (nonatomic,strong) IBOutlet UIView *noCommentHeadView;

@end
