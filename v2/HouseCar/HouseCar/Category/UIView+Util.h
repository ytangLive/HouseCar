//
//  UIView+Util.h
//  Wang
//
//  Created by WangFaquan on 14-7-10.
//  Copyright (c) 2014年 wangfaquan. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (Util)

/** frame.origin.x */
@property (nonatomic) CGFloat minX;

/** frame.origin.y */
@property (nonatomic) CGFloat minY;

/** frame.origin.x + frame.size.width */
@property (nonatomic) CGFloat maxX;

/** frame.origin.y + frame.size.height */
@property (nonatomic) CGFloat maxY;

/** frame.size.width */
@property (nonatomic) CGFloat width;

/** frame.size.height */
@property (nonatomic) CGFloat height;

/** center.x */
@property (nonatomic) CGFloat centerX;

/** center.y */
@property (nonatomic) CGFloat centerY;

/** Return the x coordinate on the screen */
@property (nonatomic, readonly) CGFloat ttScreenX;

/** Return the y coordinate on the screen */
@property (nonatomic, readonly) CGFloat ttScreenY;

/** Return the x coordinate on the screen, taking into account scroll views */
@property (nonatomic, readonly) CGFloat screenViewX;

/** Return the y coordinate on the screen, taking into account scroll views */
@property (nonatomic, readonly) CGFloat screenViewY;

/** Return the view frame on the screen, taking into account scroll views */
@property (nonatomic, readonly) CGRect screenFrame;

/** frame.origin */
@property (nonatomic) CGPoint origin;

/** frame.size */
@property (nonatomic) CGSize size;

- (id)initLineWithFrame:(CGRect)frame color:(UIColor *)color;

+ (UIView *)initLineWidthFrame:(CGRect)frame;

+ (UIView *)initTitleLineWidthFrame:(CGRect)frame;

@end
