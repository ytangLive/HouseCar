//
//  HCMainWindow.m
//  VTTeamProject
//
//  Created by zhang hailong on 13-8-26.
//  Copyright (c) 2013年 ytang.com. All rights reserved.
//

#import "HCMainWindow.h"

@implementation HCMainWindow

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

-(UIScrollView *) topScrollViewInView:(UIView *)view{
    
    if(![view isHidden]){
        
        CGRect r = [self convertRect:view.frame fromView:view.superview];
        CGRect rr = self.bounds;
        
        rr.origin = CGPointZero;
        
        if(CGRectIntersectsRect(rr, r)){
            
            if([view isKindOfClass:[UIScrollView class]]
               && [(UIScrollView *) view scrollsToTop]
               && [(UIScrollView *) view contentSize].height >0){
                
                return (UIScrollView *) view;
            }
            
            NSEnumerator * objectEnum = [[view subviews] reverseObjectEnumerator];
            
            UIView * r;
            
            while((r = [objectEnum nextObject])){
                
                r = [self topScrollViewInView:r];
                if(r){
                    return (UIScrollView *) r;
                }
            }
            
        }
        
    }
    
    return nil;
}

-(void) scrollToTop{
    [[self topScrollViewInView:self] setContentOffset:CGPointZero animated:YES];
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event{
    
    CGRect statusFrame = [[UIApplication sharedApplication] statusBarFrame];
    
    if(CGRectContainsPoint(statusFrame, point)){
        
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scrollToTop) object:nil];
        [self performSelector:@selector(scrollToTop) withObject:nil afterDelay:0.0];
        
    }
    
    return [super pointInside:point withEvent:event];
}

@end
