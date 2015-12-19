//
//  SNPageControl.m
//  SinaNews
//
//  Created by li na on 13-4-15.
//  Copyright (c) 2013年 sina. All rights reserved.
//

#import "SNPageControl.h"

@implementation SNPageControl
{
    CGSize activeSize;
    CGSize inactiveSize;
    CGFloat totalWidth;
}
@synthesize spacer = _spacer;
@synthesize activeImage=_activeImage,inactiveImage=_inactiveImage;

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    self.spacer = 8;
    _activeImage = [UIImage imageNamed:@"feed_focus_pagecontrol_active"];
    _inactiveImage = [UIImage imageNamed:@"feed_focus_pagecontrol_inactive"];
    activeSize = _activeImage.size;
    inactiveSize = _inactiveImage.size;
    //self.clipsToBounds = NO;
    //self.pageIndicatorTintColor = [UIColor clearColor];
    //self.currentPageIndicatorTintColor = [UIColor clearColor];
    return self;
}

-(id) init:(CGRect)frame
{
    self = [super initWithFrame:frame];
    self.spacer = 8;
    _activeImage = [UIImage imageNamed:@"feed_focus_pagecontrol_active"];
    _inactiveImage = [UIImage imageNamed:@"feed_focus_pagecontrol_inactive"];
    activeSize = _activeImage.size;
    inactiveSize = _inactiveImage.size;
    //self.clipsToBounds = NO;
    //self.pageIndicatorTintColor = [UIColor clearColor];
    //self.currentPageIndicatorTintColor = [UIColor clearColor];
    return self;
}

- (id)initWithFrame:(CGRect)frame activeImage:(UIImage *)iActiveImage inactiveImage:(UIImage *)iInactiveImage
{
    self = [super initWithFrame:frame];
    if (self) {
        _activeImage = iActiveImage;
        _inactiveImage = iInactiveImage;
        self.spacer = 8;
        activeSize = _activeImage.size;
        inactiveSize = _inactiveImage.size;
//        self.pageIndicatorTintColor = [UIColor clearColor];
//        self.currentPageIndicatorTintColor = [UIColor clearColor];
//        self.clipsToBounds = NO;
    }
    
    return self;
}

-(void)setActiveImage:(UIImage *)tactiveImage
{
    _activeImage = tactiveImage;
    activeSize = _activeImage.size;
}

-(void)setInactiveImage:(UIImage *)tinactiveImage
{
    _inactiveImage = tinactiveImage;
    inactiveSize = _inactiveImage.size;
}

-(void)setFrame:(CGRect)frame
{
    [super setFrame:frame];
    [self updateDots];
    
}

-(void)setNumberOfPages:(NSInteger)numberOfPages
{
    [super setNumberOfPages:numberOfPages];
    totalWidth = numberOfPages<=1? (inactiveSize.width*numberOfPages):(inactiveSize.width*numberOfPages + (numberOfPages-1)*self.spacer);
    
    for (int i = 0; i < [self.subviews count]; i++) {
        UIView* sub = [self viewWithTag:i];
        if (sub.tag>0) {
            [sub removeFromSuperview];
        }
    }
    if (numberOfPages>0) {
        UIImageView* dot = nil;
        for (int i = 0; i<numberOfPages; i++) {
            CGFloat firstX = self.bounds.size.width/2 - totalWidth/2;
            
            CGFloat inactivecenterY = self.bounds.size.height/2 - inactiveSize.height/2;
            CGFloat activecenterY = inactivecenterY - (activeSize.height/2 - inactiveSize.height/2);
            if (numberOfPages-1!=self.currentPage) {
                
                dot = [[UIImageView alloc] initWithFrame:CGRectMake(firstX+(inactiveSize.width+self.spacer)*i - (activeSize.width/2 - inactiveSize.width/2), activecenterY, activeSize.width, activeSize.height)] ;
            }
            else {
                dot = [[UIImageView alloc] initWithFrame:CGRectMake(firstX+(inactiveSize.width+self.spacer)*i, inactivecenterY, inactiveSize.width, inactiveSize.height)] ;
            }
            dot.tag = i+1;
            [self addSubview:dot];
        }
    }

}

-(void) updateDots
{
    //if (!SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(IOS_7_0))
    {
        totalWidth = self.numberOfPages<=1? (inactiveSize.width*self.numberOfPages):(inactiveSize.width*self.numberOfPages + (self.numberOfPages-1)*self.spacer);
        for (int i = 0; i < [self.subviews count]; i++)
        {
            UIView* subView = [self.subviews objectAtIndex:i];
            if (subView.tag>0) {
                UIImageView *dot = (UIImageView*)subView;
                CGFloat firstX = self.bounds.size.width/2 - totalWidth/2;
                CGFloat inactivecenterY = self.bounds.size.height/2 - inactiveSize.height/2;
                CGFloat activecenterY = inactivecenterY - (activeSize.height/2 - inactiveSize.height/2);
                if (subView.tag - 1 == self.currentPage)
                {
                    dot.image = _activeImage;
                    dot.frame = CGRectMake(firstX+(inactiveSize.width+self.spacer)*(subView.tag - 1) - (activeSize.width/2 - inactiveSize.width/2), activecenterY, activeSize.width, activeSize.height);
                }
                else
                {
                    dot.image = _inactiveImage;
                    dot.frame = CGRectMake(firstX+(inactiveSize.width+self.spacer)*(subView.tag - 1), inactivecenterY, inactiveSize.width, inactiveSize.height);
                }
                
            }
            else
            {
                
            }
        }
    }
}

-(void) setCurrentPage:(NSInteger)page
{
    [super setCurrentPage:page];
    [self updateDots];
}

@end
