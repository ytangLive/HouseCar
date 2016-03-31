//
//  HCAlertLoadingView.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import <vTeam/vTeam.h>

@interface HCAlertLoadingView : UIActivityIndicatorView

-(void) showToView:(UIView *) view animated:(BOOL) animated;

-(void) showAnimated:(BOOL) animated;

-(void) closeAnimated:(BOOL) animated;

@end
