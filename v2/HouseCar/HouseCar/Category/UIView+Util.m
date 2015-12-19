//
//  UIView+Util.m
//  Wang
//
//  Created by WangFaquan on 14-7-10.
//  Copyright (c) 2014年 wangfaquan. All rights reserved.
//

#import "UIView+Util.h"

@implementation UIView (Util)

/** 一个视图的左边x的位置 */
- (CGFloat)minX {
	return self.frame.origin.x;
}

/** 设置视图左边x的位置 */
- (void)setMinX:(CGFloat)x {
	CGRect frame = self.frame;
	frame.origin.x = x;
	self.frame = frame;
}

/** 一个视图的顶部y的位置 */
- (CGFloat)minY {
	return self.frame.origin.y;
}

/** 设置视图上边y的位置 */
- (void)setMinY:(CGFloat)y {
	CGRect frame = self.frame;
	frame.origin.y = y;
	self.frame = frame;
}

/** 一个视图的右边x的位置 */
- (CGFloat)maxX {
	return self.frame.origin.x + self.frame.size.width;
}

/** 设置视图右边x的位置 */
- (void)setMaxX:(CGFloat)x {
	CGRect frame = self.frame;
	frame.origin.x = x - frame.size.width;
	self.frame = frame;
}

/** 一个视图的底部y的位置 */
- (CGFloat)maxY {
	return self.frame.origin.y + self.frame.size.height;
}

/** 设置视图下边y的位置 */
- (void)setMaxY:(CGFloat)y {
	CGRect frame = self.frame;
	frame.origin.y = y - frame.size.height;
	self.frame = frame;
}

/** 中心x位置 */
- (CGFloat)centerX {
	return self.center.x;
}


- (void)setCenterX:(CGFloat)centerX {
	self.center = CGPointMake(centerX, self.center.y);
}

/** 中心y位置 */
- (CGFloat)centerY {
	return self.center.y;
}

- (void)setCenterY:(CGFloat)centerY {
	self.center = CGPointMake(self.center.x, centerY);
}

- (CGFloat)width {
	return self.frame.size.width;
}

- (void)setWidth:(CGFloat)width {
	CGRect frame = self.frame;
	frame.size.width = width;
	self.frame = frame;
}

- (CGFloat)height {
	return self.frame.size.height;
}

- (void)setHeight:(CGFloat)height {
	CGRect frame = self.frame;
	frame.size.height = height;
	self.frame = frame;
}

- (CGFloat)ttScreenX {
	CGFloat x = 0;
	for (UIView* view = self; view; view = view.superview) {
		x += view.minX;
	}
	return x;
}

- (CGFloat)ttScreenY {
	CGFloat y = 0;
	for (UIView* view = self; view; view = view.superview) {
		y += view.minY;
	}
	return y;
}

- (CGFloat)screenViewX {
	CGFloat x = 0;
	for (UIView* view = self; view; view = view.superview) {
		x += view.minX;
		
		if ([view isKindOfClass:[UIScrollView class]]) {
			UIScrollView* scrollView = (UIScrollView*)view;
			x -= scrollView.contentOffset.x;
		}
	}
	
	return x;
}

- (CGFloat)screenViewY {
	CGFloat y = 0;
	for (UIView* view = self; view; view = view.superview) {
		y += view.minY;
		
		if ([view isKindOfClass:[UIScrollView class]]) {
			UIScrollView* scrollView = (UIScrollView*)view;
			y -= scrollView.contentOffset.y;
		}
	}
	return y;
}

- (CGRect)screenFrame {
	return CGRectMake(self.screenViewX, self.screenViewY, self.width, self.height);
}

- (CGPoint)origin {
	return self.frame.origin;
}

- (void)setOrigin:(CGPoint)origin {
	CGRect frame = self.frame;
	frame.origin = origin;
	self.frame = frame;
}

- (CGSize)size {
	return self.frame.size;
}

- (void)setSize:(CGSize)size {
	CGRect frame = self.frame;
	frame.size = size;
	self.frame = frame;
}

- (CGPoint)centerBounds {
	return CGPointMake(self.bounds.size.width / 2, self.bounds.size.height / 2);
}

//初始化一根线
- (id)initLineWithFrame:(CGRect)frame color:(UIColor *)color{
    self = [self initWithFrame:frame];
	self.backgroundColor = color;
	return self;
}

//获取一根分割线
+ (UIView *)initLineWidthFrame:(CGRect)frame
{
    UIImageView *cellline = [[UIImageView alloc] initWithFrame:frame];
    cellline.image=[UIImage imageNamed:@"split_line"];
    return cellline;
}

//获取标题标识线
+ (UIView *)initTitleLineWidthFrame:(CGRect)frame
{
    UIView *verticalLine = [[UIView alloc] initWithFrame:frame];
    verticalLine.backgroundColor = DefaultNavBarColor;
    verticalLine.layer.cornerRadius = 3.0;
    verticalLine.layer.masksToBounds = YES;
    return verticalLine;
}

@end

