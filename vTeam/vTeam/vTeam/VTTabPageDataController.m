//
//  VTTabPageDataController.m
//  vTeam
//
//  Created by zhang hailong on 13-7-12.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#import "VTTabPageDataController.h"
#define tabBackDefaultWidth 48

@interface VTTabPageDataController(){
    BOOL _animating;
    CGSize _tabBackgroundViewSize;
}

@end

@implementation VTTabPageDataController

@synthesize pageContentView = _pageContentView;
@synthesize tabBackgroundView = _tabBackgroundView;
@synthesize leftSpaceWidth = _leftSpaceWidth;
@synthesize rightSpaceWidth = _rightSpaceWidth;

-(void) dealloc{
    [_pageContentView setDelegate:nil];
    [_pageContentView release];
    [_tabBackgroundView release];
    [super dealloc];
}

-(void) scrollToTabBackgroundVisable:(BOOL) animated{
    if([_tabBackgroundView.superview isKindOfClass:[UIScrollView class]]){
        
        UIButton * selButton = [self tabButtonAtIndex:self.selectedIndex];
        UIScrollView * scrollView = (UIScrollView *) _tabBackgroundView.superview;
        
        CGFloat scrollViewWidth = CGRectGetWidth(scrollView.frame);
        CGFloat scrollContentOffSetX = scrollView.contentOffset.x;
        
        if (self.selectedIndex==0) {
//            if (CGRectGetMinX(selButton.frame)<scrollContentOffSetX){
//                [scrollView setContentOffset:CGPointMake(0, 0) animated:YES];
//            }
            [scrollView setContentOffset:CGPointMake(0, 0) animated:YES];
        }else if(self.selectedIndex==[self.tabButtons count]-1){
            if (CGRectGetMaxX(selButton.frame)>scrollViewWidth+scrollContentOffSetX) {
                [scrollView setContentOffset:CGPointMake(CGRectGetMaxX(selButton.frame)-scrollViewWidth, 0) animated:YES];
            }
        }else{
            UIButton *preBtn = [self tabButtonAtIndex:self.selectedIndex-1];
            if (CGRectGetMinX(preBtn.frame)<scrollContentOffSetX){
                [scrollView setContentOffset:CGPointMake(CGRectGetMinX(preBtn.frame), 0) animated:YES];
            }else{
                UIButton *nextBtn = [self tabButtonAtIndex:self.selectedIndex+1];
                if (CGRectGetMaxX(nextBtn.frame)>scrollContentOffSetX+scrollViewWidth){
                    [scrollView setContentOffset:CGPointMake(CGRectGetMaxX(nextBtn.frame)-scrollViewWidth, 0) animated:YES];
                }
            }
        }
    }
}

-(void) scrollToTabButton:(NSUInteger) index{
    UIButton * tabButton = [self tabButtonAtIndex:index];
    CGRect r = [_tabBackgroundView frame];
    CGRect rr = [tabButton frame];
    if (![self.tabBackgroundView isKindOfClass:[UIImageView class]]) {
        float realWidth = tabButton.titleLabel.text.length*tabBackDefaultWidth/2;
        if (realWidth > CGRectGetWidth(tabButton.frame)) {
            realWidth = CGRectGetWidth(tabButton.frame);
        }
        r.size.width = realWidth;
    }else{
        if ((int)(rr.size.width - r.size.width)%2==1) {
            r.size.width = _tabBackgroundViewSize.width+1;
        }
    }
//    if ((int)(rr.size.width - r.size.width)%2==1) {
//        r.size.width = _tabBackgroundViewSize.width+1;
//    }
    r.origin.x = rr.origin.x + (rr.size.width - r.size.width) / 2.0;
    [_tabBackgroundView setFrame:r];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self performSelector:@selector(scrollToTabBackgroundVisable:) withObject:@true afterDelay:0.0];
}

//旧版
//-(void) scrollToTabBackgroundVisable:(BOOL) animated{
//    if([_tabBackgroundView.superview isKindOfClass:[UIScrollView class]]){
//        UIScrollView * scrollView = (UIScrollView *) _tabBackgroundView.superview;
//        CGPoint contentOffset = [scrollView contentOffset];
//        CGSize size = [scrollView bounds].size;
//        CGRect r = _tabBackgroundView.frame;
//        if(r.origin.x - _leftSpaceWidth < contentOffset.x){
//            [scrollView setContentOffset:CGPointMake(r.origin.x - _leftSpaceWidth, 0) animated:YES];
//        }
//        else if(r.origin.x + r.size.width + _rightSpaceWidth > contentOffset.x + size.width){
//            [scrollView setContentOffset:CGPointMake(r.origin.x + r.size.width + _rightSpaceWidth - size.width, 0) animated:animated];
//        }
//    }
//}

//-(void) scrollToTabButton:(NSUInteger) index{
//    UIButton * selButton = [self tabButtonAtIndex:index];
//    CGRect r = [_tabBackgroundView frame];
//    r.size.width = selButton.titleLabel.text.length*20>CGRectGetWidth(selButton.frame)?CGRectGetWidth(selButton.frame):selButton.titleLabel.text.length*20;
//    CGRect rr = [selButton frame];
//    if ((int)(rr.size.width - r.size.width)%2==1) {
//        r.size.width = _tabBackgroundViewSize.width+1;
//    }
//    r.origin.x = rr.origin.x + (rr.size.width - r.size.width) / 2.0;
//    [_tabBackgroundView setFrame:r];
//    
//    [NSObject cancelPreviousPerformRequestsWithTarget:self];
//    [self performSelector:@selector(scrollToTabBackgroundVisable:) withObject:@true afterDelay:0.0];
//}

-(void) setSelectedIndex:(NSUInteger)selectedIndex animated:(BOOL) animated withoutOffset:(BOOL)withoutOffset{
    if(_selectedIndex != selectedIndex){
        
        _selectedIndex = selectedIndex;
        
        id delegate = [self delegate];
        
        if([delegate respondsToSelector:@selector(vtTabDataController:willSelectedChanged:)]){
            if(![delegate vtTabDataController:self willSelectedChanged:selectedIndex]){
                return;
            }
        }
        
        
        NSUInteger index = 0;
        
        for (UIButton * button in self.tabButtons) {
            [button setEnabled:index != selectedIndex];
            index ++;
        }
        
        if(animated){
            [UIView beginAnimations:nil context:nil];
            [UIView setAnimationDuration:0.2];
        }
        
        [self scrollToTabButton:selectedIndex];
        
        if(animated){
            [UIView commitAnimations];
        }
        
        if(!withoutOffset){
            [_pageContentView setContentOffset:CGPointMake(_selectedIndex * _pageContentView.bounds.size.width,0) animated:animated];
        }
        
        if([delegate respondsToSelector:@selector(vtTabDataController:didSelectedChanged:)]){
            [delegate vtTabDataController:self didSelectedChanged:_selectedIndex];
        }
    }
    else{
        [self scrollToTabBackgroundVisable:animated];
    }
}

-(void) setSelectedIndex:(NSUInteger)selectedIndex animated:(BOOL)animated{
    [self setSelectedIndex:selectedIndex animated:animated withoutOffset:NO];
}

-(void) setSelectedIndex:(NSUInteger)selectedIndex{
    [self setSelectedIndex:selectedIndex animated:NO];
}

-(UIButton *) tabButtonAtIndex:(NSUInteger) index{
    if(index < [self.tabButtons count]){
        return [self.tabButtons objectAtIndex:index];
    }
    return [self.tabButtons lastObject];
}

-(UIView *) contentViewAtIndex:(NSUInteger) index{
    if(index < [self.contentViews count]){
        return [self.contentViews objectAtIndex:index];
    }
    return [self.contentViews lastObject];
}

-(VTDataController *) controllerAtIndex:(NSUInteger) index{
    if(index < [self.controllers count]){
        return [self.controllers objectAtIndex:index];
    }
    return [self.controllers lastObject];
}

-(void) reloadDataController:(VTDataController *) dataController{
    VTDataSource * dataSource = dataController.dataSource;
    if(!dataSource.loading && !dataSource.loaded){
        [NSObject cancelPreviousPerformRequestsWithTarget:dataController selector:@selector(reloadData) object:nil];
        [dataController performSelector:@selector(reloadData) withObject:nil afterDelay:0.0];
    }
}

-(void) scrollViewDidEndScrollingAnimation:(UIScrollView *)scrollView{
    [self setSelectedIndex:scrollView.contentOffset.x / scrollView.bounds.size.width animated:NO withoutOffset:YES];
}

-(void) scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate{
    if(!decelerate){
        [self setSelectedIndex:scrollView.contentOffset.x / scrollView.bounds.size.width animated:NO withoutOffset:YES];
    }
}

-(void) scrollViewDidEndDecelerating:(UIScrollView *)scrollView{
    [self setSelectedIndex:scrollView.contentOffset.x / scrollView.bounds.size.width animated:NO withoutOffset:YES];
}

-(void) scrollViewDidScroll:(UIScrollView *)scrollView{
    
}

-(void) scrollView:(UIScrollView *) scrollView didContentOffsetChanged:(CGPoint) contentOffset{
    
    if(scrollView == _pageContentView && scrollView.window){
        CGFloat index = contentOffset.x / scrollView.bounds.size.width;
        NSInteger count = scrollView.contentSize.width / scrollView.bounds.size.width;
        
        if(index <0){
            if (index < -0.16) {
                if ([self.delegate respondsToSelector:@selector(vtTabPageDataControllerWillBackToTopView:)]) {
                    [self.delegate vtTabPageDataControllerWillBackToTopView:self];
                    return;
                }
            }

            [self reloadDataController:[self controllerAtIndex:0]];
            [self scrollToTabButton:0];

        }
        else if(index >= count){
            [self reloadDataController:[self controllerAtIndex:count -1]];
            [self scrollToTabButton:count - 1];
        }
        else{
            CGFloat r = (index - (int) index);
            if(r == 0.0f || index > count -1){
                [self reloadDataController:[self controllerAtIndex:(int) index]];
                [self scrollToTabButton:(int) index];
                
            }
            else{
                
                [self reloadDataController:[self controllerAtIndex:(int) index]];
                [self reloadDataController:[self controllerAtIndex:(int) index + 1]];
                CGPoint p1 = [[self tabButtonAtIndex:(int) index] center];
                CGPoint p2 = [[self tabButtonAtIndex:(int) index + 1] center];
                
                CGRect rect = [_tabBackgroundView frame];
                
                rect.origin.x = p1.x + (p2.x - p1.x) * r - rect.size.width / 2.0;
                
                [UIView beginAnimations:nil context:nil];
                [UIView setAnimationDuration:0.3];
                
                if (![self.tabBackgroundView isKindOfClass:[UIImageView class]]) {
                    float left = index - floor(index);
                    
                    int flag = (int) index;
                    if (left>0.5) {
                        flag += 1;
                    }
                    
                    UIButton *tabButton = [self tabButtonAtIndex:flag];
                    rect.size.width = tabButton.titleLabel.text.length*tabBackDefaultWidth/2>CGRectGetWidth(tabButton.frame)?CGRectGetWidth(tabButton.frame):tabButton.titleLabel.text.length*tabBackDefaultWidth/2;
                }
            
                [_tabBackgroundView setFrame:rect];
                
                [UIView commitAnimations];
                
                [self scrollToTabBackgroundVisable:NO];
            }
        }
    }
}

-(void) setTabButtons:(NSArray *)tabButtons
{
    [super setTabButtons:tabButtons];
    if (self.tabBackgroundView) {
        CGRect tabbackRect = self.tabBackgroundView.frame;
        _tabBackgroundViewSize = tabbackRect.size;
        UIButton * tabButton = [self tabButtonAtIndex:_selectedIndex];
        CGRect rr = [tabButton frame];
        if ((int)(rr.size.width - tabbackRect.size.width)%2==1) {
            tabbackRect.size.width = _tabBackgroundViewSize.width+1;
        }
        tabbackRect.origin.x = rr.origin.x + (rr.size.width - tabbackRect.size.width) / 2.0;
        [_tabBackgroundView setFrame:tabbackRect];
    }
}

-(void)setTabBackgroundView:(UIView *)tabBackgroundView
{
    if (_tabBackgroundView!=tabBackgroundView) {
        [tabBackgroundView retain];
        [_tabBackgroundView release];
        _tabBackgroundView = tabBackgroundView;
    }
    if (self.tabButtons) {
        CGRect tabbackRect = self.tabBackgroundView.frame;
        _tabBackgroundViewSize = tabbackRect.size;
        UIButton * tabButton = [self tabButtonAtIndex:_selectedIndex];
        CGRect rr = [tabButton frame];
        if ((int)(rr.size.width - tabbackRect.size.width)%2==1) {
            tabbackRect.size.width = _tabBackgroundViewSize.width+1;
        }
        tabbackRect.origin.x = rr.origin.x + (rr.size.width - tabbackRect.size.width) / 2.0;
        [_tabBackgroundView setFrame:tabbackRect];
    }
    
}

@end
