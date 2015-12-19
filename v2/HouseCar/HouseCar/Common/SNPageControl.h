//
//  SNPageControl.h
//  SinaNews
//
//  Created by li na on 13-4-15.
//  Copyright (c) 2013年 sina. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SNPageControl : UIPageControl 
{
    UIImage* activeImage;
    UIImage* inactiveImage;
}
@property(nonatomic,retain)UIImage* activeImage;
@property(nonatomic,retain)UIImage* inactiveImage;
@property(nonatomic,assign)int spacer;
- (id)initWithFrame:(CGRect)frame activeImage:(UIImage *)activeImage inactiveImage:(UIImage *)inactiveImage;

@end
