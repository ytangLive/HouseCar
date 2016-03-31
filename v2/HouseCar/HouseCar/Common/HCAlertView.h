//
//  HCAlertView.h
//  HourseCar
//
//  Created by Tang yin on 16-3-31.
//  Copyright (c) 2016年 ytang.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface HCAlertView : UIView

-(id) initWithTitle:(NSString *) title font:(UIFont *)font size:(CGSize) size edgeInsets:(UIEdgeInsets) edgeInsets;

-(id) initWithTitle:(NSString *) title font:(UIFont *)font size:(CGSize) size edgeInsets:(UIEdgeInsets) edgeInsets image:(UIImage *)image imageViewHeight:(float)imageViewHeight;

-(id) initWithTitle:(NSString *) title;

-(id) initWithTitle:(NSString *) title image:(UIImage *)image;

-(id) initWithTitle:(NSString *) title edgeInsets:(UIEdgeInsets)edgeInsets;

-(id) initWithTitle:(NSString *) title edgeInsets:(UIEdgeInsets)edgeInsets image:(UIImage *) image imageViewHeight:(float)imageViewHeight;

-(void) showDuration:(NSTimeInterval) duration;

@end
