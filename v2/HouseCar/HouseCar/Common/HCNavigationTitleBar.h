//
//  MFStoreTitleBar.h
//  multiframe
//
//  Created by ytang on 15/02/22.
//  Copyright (c) 2014年 YTANG. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface  HCNavigationTitleBar : UIView

@property (nonatomic, strong) IBOutlet VTButton *leftButton;
@property (nonatomic, strong) IBOutlet VTButton *rightButton;
@property (nonatomic, strong) IBOutlet UILabel *titleLabel;

@end
