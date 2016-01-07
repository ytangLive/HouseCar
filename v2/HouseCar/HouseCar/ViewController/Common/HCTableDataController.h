//
//  HCTableDataController.h
//  HouseCar
//
//  Created by shieh fabo on 14-9-28.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCTableDataController : VTTableDataController

- (void)tabButton:(NSArray *)tabButtons clickWithButton:(UIButton *)clickButton tabCurView:(UIView *)tabCurView;

- (void)useDefaultTopLoadingView:(BOOL)isUse stockAnimation:(BOOL)stockAnimation;

- (void)useDefaultBottomLoadingView:(BOOL)isUse stockAnimation:(BOOL)stockAnimation;

@end
