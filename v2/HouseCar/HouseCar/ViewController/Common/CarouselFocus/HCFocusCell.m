//
//  SFInfoVer2FocusCell.m
//  SinaFinance
//
//  Created by shieh fabo on 14-9-12.
//  Copyright (c) 2014年 sina.com. All rights reserved.
//

#import "HCFocusCell.h"
#import "SNPageControl.h"

#define SpaceBetweenControlAndLabel 5

@interface HCFocusCell ()
{
    NSInteger curPage;
    BOOL bTouching;
    BOOL bLastPageBeginTouch;
}

@property(strong,nonatomic)NSMutableArray* pageViews;

@end

@implementation HCFocusCell

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(void)reloadData
{
    if (self.pageViews==nil) {
        self.pageViews = [[NSMutableArray alloc] initWithCapacity:0];
    }
    else
    {
        NSInteger viewCount = [self.pageViews count];
        for (int i=0; i<viewCount; i++) {
            
            UIView* tView = [self.pageViews objectAtIndex:i];
            [tView removeFromSuperview];
        }
        [self.pageViews removeAllObjects];
    }
    
    NSInteger count = [self.datas count];
    CGRect bounds = self.frame;
    for (int i=0; i<count; i++) {
        id dataItem = [self.datas objectAtIndex:i];
        
        bounds.origin.x = i*CGRectGetWidth(bounds);
        UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
        btn.tag = i;
        btn.frame = bounds;
        VTImageView* imageView = [[VTImageView alloc] initWithFrame:btn.bounds];
        imageView.autoresizingMask = UIViewAutoresizingFlexibleHeight|UIViewAutoresizingFlexibleWidth;
        imageView.tag = 111;
        imageView.defaultImage = [UIImage imageNamed:@"info_focus_loading.png"];
        [btn addSubview:imageView];
        
       
        imageView.src = [dataItem valueForKey:self.srcKey];
        
        [btn addTarget:self action:@selector(imageClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.pageViews addObject:btn];
        [self.scrollview addSubview:btn];
    }
    
    self.scrollview.contentSize = CGSizeMake(CGRectGetWidth(bounds)*count, CGRectGetHeight(bounds));
    
    for(UIView* btnView in self.pageViews){
        VTImageView* imageView = (VTImageView*)[btnView viewWithTag:111];
        if([imageView isLoading]){
            [self.context cancelHandle:@protocol(IVTImageTask) task:imageView];
        }
        if(![imageView isLoaded]&&![imageView isLoading]){
            [self.context handle:@protocol(IVTImageTask) task:imageView priority:0];
        }
    }
    
    UIImage* active = [UIImage imageNamed:@"focusdot_active.png"];
    UIImage* inactive = [UIImage imageNamed:@"focusdot_inactive.png"];
    [((SNPageControl*)self.pageControl) setActiveImage:active];
    [((SNPageControl*)self.pageControl) setInactiveImage:inactive];
    [((SNPageControl*)self.pageControl) setSpacer:3];
    self.pageControl.numberOfPages = count;
    
    [self setCurrentPage:0];
    
    [self startAutoPage];
    
    if(count <= 1){
        _pageControl.hidden = YES;
    }else{
        _pageControl.hidden = NO;
    }
}

-(void)startAutoPage
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(dealAutoPage) object:nil];
    [self performSelector:@selector(dealAutoPage) withObject:nil afterDelay:4.0];
}

-(void)dealAutoPage
{
    NSInteger pageCount = self.pageControl.numberOfPages;
    NSInteger tcurPage = self.pageControl.currentPage;
    if (pageCount>1) {
        if (tcurPage<pageCount-1) {
            [self setCurrentPage:tcurPage+1 animated:YES];
        }
        else
        {
            [self setCurrentPage:0 animated:YES];
        }
    }
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(dealAutoPage) object:nil];
    [self performSelector:@selector(dealAutoPage) withObject:nil afterDelay:4.0];
}

-(void)stopAutoPage
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(dealAutoPage) object:nil];
}

-(void)setScrollViewPage:(NSInteger)page animated:(BOOL)animated
{
    CGRect bounds = self.scrollview.bounds;
    [self.scrollview setContentOffset:CGPointMake(page*CGRectGetWidth(bounds), 0) animated:animated];
}

-(void)setCurrentPage:(NSInteger)page
{
    [self setCurrentPage:page animated:NO];
}

-(void)setCurrentPage:(NSInteger)page animated:(BOOL)animated
{
    [self setScrollViewPage:page animated:animated];
    if (!animated) {
        curPage = page;
        self.pageControl.currentPage = page;
        [self resetCurTitle];
    }
}

//设置文字
-(void)resetCurTitle
{
    
}


- (void)scrollViewDidScroll:(UIScrollView *)sender {
    CGRect bounds = sender.bounds;
    int page = (sender.contentOffset.x+CGRectGetWidth(bounds)/4) / (CGRectGetWidth(bounds));
    if (page<self.pageControl.numberOfPages) {
        if (page!=curPage) {
            curPage = page;
            self.pageControl.currentPage = page;
            [self resetCurTitle];
        }
    }
}
- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView
{
    [self stopAutoPage];
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
    if (!decelerate) {
        [self startAutoPage];
    }
}


- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView {
    [self startAutoPage];
}


- (IBAction)pageControlValueChanged:(id)sender {
    [self startAutoPage];
    NSInteger page = self.pageControl.currentPage;
    
    if (curPage!=page) {
        curPage = page;
        [self.scrollview setContentOffset:CGPointMake(CGRectGetWidth([UIScreen mainScreen].bounds) * page, 0) animated:YES];
        [self resetCurTitle];
    }
    
}

- (IBAction)imageClicked:(id)sender {
    if ([self.delegate conformsToProtocol:@protocol(HCFocusCellCellProtocol)]) {
        UIButton* btn = (UIButton*)sender;
        NSDictionary* data = nil;
        if (btn.tag<[self.datas count]) {
            data = [self.datas objectAtIndex:btn.tag];
        }
        [self.delegate cellDataDidClickedWithData:data];
    }
    
}

-(void)dealloc
{
    self.scrollview.delegate = nil;
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(dealAutoPage) object:nil];
}


@end
