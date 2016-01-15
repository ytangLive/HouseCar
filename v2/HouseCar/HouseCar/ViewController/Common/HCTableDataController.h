//
//  HCTableDataController.h
//  HouseCar
//
//  Created by shieh fabo on 14-9-28.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>

typedef enum _SFLoadingViewStyle : NSInteger
{
    SFLoadingViewStyle_ActivityIndicatorNone = 0,
    SFLoadingViewStyle_ActivityIndicatorSystem,
    SFLoadingViewStyle_ActivityIndicatorStockAnimaiton
}SFLoadingViewStyle;

@interface HCTableDataController : VTTableDataController

- (void)tabButton:(NSArray *)tabButtons clickWithButton:(UIButton *)clickButton tabCurView:(UIView *)tabCurView;

- (void)topLoadingViewWithStyle:(SFLoadingViewStyle)loadingViewStyle;

- (void)bottomLoadingViewWithStyle:(SFLoadingViewStyle)loadingViewStyle;

- (void)notFoundDataWithAlertText:(NSString *)alertText imageNamed:(NSString *)imageName;

@end

@protocol HCTableDataControllerDelegate <VTTableDataControllerDelegate>
@optional
-(void) HCTableDataControllerDidFinishLoaded:(HCTableDataController *) dataController;

@end
