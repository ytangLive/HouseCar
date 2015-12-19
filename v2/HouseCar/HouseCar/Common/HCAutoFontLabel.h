//
//  SFSpringFontLabel.h
//  SinaFinance
//
//  Created by Sina_Mobile on 15/4/20.
//  Copyright (c) 2015年 sina.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface HCAutoFontLabel : UILabel

@property(nonatomic, assign)BOOL needAutoFont;
@property(nonatomic, strong)NSString * defaultFontSize;
@property(nonatomic, strong)NSString * miniFontSize;

@end
