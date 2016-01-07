//
//  HCTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-28.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCTableDataController.h"
#import "HCAnimationView.h"
#import "HCDragLoadingView.h"

@implementation HCTableDataController

-(void)stopLoading
{
    [super stopLoading];
    if ([self.dataSource respondsToSelector:@selector(hasMoreData)]&&self.bottomLoadingView) {
        BOOL hasMore = [(id)self.dataSource hasMoreData];
        if (hasMore) {
            self.bottomLoadingView.upTitleLabel.text = @"释放刷新";
            CGRect rect = self.bottomLoadingView.upTitleLabel.frame;
            rect.size.width = self.tableView.bounds.size.width/2;
            self.bottomLoadingView.upTitleLabel.frame = rect;
        }
        else
        {
            if ([self.dataSource.dataObjects count]>0) {
                self.bottomLoadingView.upTitleLabel.hidden = NO;
                self.bottomLoadingView.downTitleLabel.hidden = YES;
                self.bottomLoadingView.loadingTitleLabel.hidden = YES;
                self.bottomLoadingView.upTitleLabel.text = @"已显示全部内容";
                CGRect rect = self.bottomLoadingView.upTitleLabel.frame;
                rect.size.width = self.tableView.bounds.size.width/2;
                self.bottomLoadingView.upTitleLabel.frame = rect;
                
                
                
                CGSize contentSize = self.tableView.contentSize;
                CGRect r = self.bottomLoadingView.frame;
                
                r.size.width = self.tableView.bounds.size.width;
                r.origin.y = contentSize.height;
                
                [self.bottomLoadingView setFrame:r];
                [self.tableView addSubview:self.bottomLoadingView];
            }
            
        }
    }
}

-(void) tableView:(UITableView *)tableView didContentOffsetChanged:(CGPoint)contentOffset
{
    [super tableView:tableView didContentOffsetChanged:contentOffset];
    [self.bottomLoadingView setOffsetValue:1.0];
    if ([self.dataSource respondsToSelector:@selector(hasMoreData)]&&self.bottomLoadingView) {
        BOOL hasMore = [(id)self.dataSource hasMoreData];
        if (!hasMore)
        {
            [self.bottomLoadingView setOffsetValue:0.0];
            
        }
    }
}

- (void)tabButton:(NSArray *)tabButtons clickWithButton:(UIButton *)clickButton tabCurView:(UIView *)tabCurView
{
    for(UIButton * tabButton in tabButtons){
        
        [tabButton setEnabled:tabButton != clickButton];
        
        if(![tabButton isEnabled]){
            CGRect r = [tabCurView frame];
            CGRect rr = [tabButton frame];
            r.origin.x = rr.origin.x + (rr.size.width - r.size.width) / 2.0;
            [UIView beginAnimations:nil context:nil];
            [UIView setAnimationDuration:0.3];
            [tabCurView setFrame:r];
            [UIView commitAnimations];
        }
    }
}

- (void)useDefaultTopLoadingView:(BOOL)isUse stockAnimation:(BOOL)stockAnimation
{
    if (self.topLoadingView) {
        for (UIView * view in self.topLoadingView.subviews) {
            [view removeFromSuperview];
        }
        self.topLoadingView = nil;
    }
    if (isUse && !self.topLoadingView) {
        VTDragLoadingView * topLoadingView = nil;
        if (stockAnimation) {
            topLoadingView = [[VTDragLoadingView alloc] initWithFrame:CGRectMake(0, 0, self.tableView.width, 44)];
        }else{
            topLoadingView = [[HCDragLoadingView alloc] initWithFrame:CGRectMake(0, 0, self.tableView.width, 44)];
        }
        topLoadingView.autoresizingMask = UIViewAutoresizingFlexibleWidth;;
        [self setTopLoadingView:topLoadingView];
        
        UIView * backgourdView = [[UIView alloc] initWithFrame:topLoadingView.frame];
        backgourdView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        [backgourdView setBackgroundColor:[UIColor clearColor]];
        [topLoadingView addSubview:backgourdView];
        
        if (stockAnimation) {
            
            HCAnimationView * activityView = [[HCAnimationView alloc] initWithFrame:CGRectMake(16,
                                                                                               0,
                                                                                               25,
                                                                                               20)];
            [activityView setBackgroundColor:[UIColor whiteColor]];
            [activityView setBackgroundImg:@"loading_bg.png"];
            [activityView setCoverImg:@"loading_cover.png"];
            [activityView setBorderImg:@"loading_border.png"];
            [activityView setLoopImg:@"loading_loop.png"];
            activityView.center = CGPointMake(activityView.center.x, backgourdView.center.y);
            activityView.hidden = NO;
            [activityView reloadRes];
            [backgourdView addSubview:activityView];
            [topLoadingView setLoadingView:activityView];
            
            UILabel * downTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 60, backgourdView.height)];
            downTitleLabel.center = CGPointMake(downTitleLabel.center.x, backgourdView.center.y);
            downTitleLabel.text = @"下拉刷新";
            downTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            downTitleLabel.font = [UIFont systemFontOfSize:12];
            downTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [backgourdView addSubview:downTitleLabel];
            [topLoadingView setDownTitleLabel:downTitleLabel];
            
            UILabel * upTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 60, backgourdView.height)];
            upTitleLabel.center = CGPointMake(upTitleLabel.center.x, backgourdView.center.y);
            upTitleLabel.text = @"释放刷新";
            upTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            upTitleLabel.font = [UIFont systemFontOfSize:12];
            [upTitleLabel setHidden:YES];
            upTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [backgourdView addSubview:upTitleLabel];
            [topLoadingView setUpTitleLabel:upTitleLabel];
            
            UILabel * loadingTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 60, backgourdView.height)];
            loadingTitleLabel.center = CGPointMake(loadingTitleLabel.center.x, backgourdView.center.y);
            loadingTitleLabel.text = @"正在加载";
            loadingTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            loadingTitleLabel.font = [UIFont systemFontOfSize:12];
            [loadingTitleLabel setHidden:YES];
            loadingTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [backgourdView addSubview:loadingTitleLabel];
            [topLoadingView setLoadingTitleLabel:loadingTitleLabel];
            
            UILabel * timeLabel = [[UILabel alloc] initWithFrame:CGRectMake(102, 0, 127, backgourdView.height)];
            timeLabel.center = CGPointMake(timeLabel.center.x, backgourdView.center.y);
            timeLabel.text = @"最后更新: 今天";
            timeLabel.textColor = UIColorFromRGB(0xb5b5b5);
            timeLabel.font = [UIFont systemFontOfSize:12];
            timeLabel.textAlignment = NSTextAlignmentCenter;
            timeLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [backgourdView addSubview:timeLabel];
            [topLoadingView setTimeLabel:timeLabel];
        }else{
            
            UIActivityIndicatorView * activityView = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0,
                                                                                                               0,
                                                                                                               20,
                                                                                                               20)];
            activityView.hidesWhenStopped = NO;
            activityView.center = CGPointMake(backgourdView.center.x, backgourdView.center.y);
            activityView.hidden = NO;
            activityView.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
            [backgourdView addSubview:activityView];
            [topLoadingView setLoadingView:activityView];
        }
    }
}

- (void)useDefaultBottomLoadingView:(BOOL)isUse stockAnimation:(BOOL)stockAnimation
{
    if (self.bottomLoadingView) {
        for (UIView * view in self.bottomLoadingView.subviews) {
            [view removeFromSuperview];
        }
        self.bottomLoadingView = nil;
    }
    if (isUse && !self.bottomLoadingView) {
        VTDragLoadingView * bottomLoadingView = [[VTDragLoadingView alloc] initWithFrame:CGRectMake(0, 0, self.tableView.width, 44)];
        bottomLoadingView.autoresizingMask = UIViewAutoresizingFlexibleWidth;
        [self setBottomLoadingView:bottomLoadingView];
        
        UIView * backgourdView = [[UIView alloc] initWithFrame:bottomLoadingView.frame];
        backgourdView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        [backgourdView setBackgroundColor:[UIColor clearColor]];
        [bottomLoadingView addSubview:backgourdView];
        
        if (stockAnimation) {
            HCAnimationView * activityView = [[HCAnimationView alloc] initWithFrame:CGRectMake(16,
                                                                                               0,
                                                                                               25,
                                                                                               20)];
            [activityView setBackgroundColor:[UIColor whiteColor]];
            [activityView setBackgroundImg:@"loading_bg.png"];
            [activityView setCoverImg:@"loading_cover.png"];
            [activityView setBorderImg:@"loading_border.png"];
            [activityView setLoopImg:@"loading_loop.png"];
            activityView.center = CGPointMake(activityView.center.x, backgourdView.center.y);
            activityView.hidden = NO;
            [activityView reloadRes];
            [backgourdView addSubview:activityView];
            [bottomLoadingView setLoadingView:activityView];
            
            UILabel * downTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 100, backgourdView.height)];
            downTitleLabel.center = CGPointMake(downTitleLabel.center.x, backgourdView.center.y);
            downTitleLabel.text = @"上拉加载";
            downTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            downTitleLabel.font = [UIFont systemFontOfSize:12];
            downTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [backgourdView addSubview:downTitleLabel];
            [bottomLoadingView setDownTitleLabel:downTitleLabel];
            
            UILabel * upTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 100, backgourdView.height)];
            upTitleLabel.center = CGPointMake(upTitleLabel.center.x, backgourdView.center.y);
            upTitleLabel.text = @"释放加载";
            upTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            upTitleLabel.font = [UIFont systemFontOfSize:12];
            upTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [upTitleLabel setHidden:YES];
            [backgourdView addSubview:upTitleLabel];
            [bottomLoadingView setUpTitleLabel:upTitleLabel];
            
            UILabel * loadingTitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(50, 0, 100, backgourdView.height)];
            loadingTitleLabel.center = CGPointMake(loadingTitleLabel.center.x, backgourdView.center.y);
            loadingTitleLabel.text = @"正在为您加载中";
            loadingTitleLabel.textColor = UIColorFromRGB(0xb5b5b5);
            loadingTitleLabel.font = [UIFont systemFontOfSize:12];
            loadingTitleLabel.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
            [loadingTitleLabel setHidden:YES];
            [backgourdView addSubview:loadingTitleLabel];
            [bottomLoadingView setLoadingTitleLabel:loadingTitleLabel];
            
        }else{
            UIActivityIndicatorView * activityView = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0,
                                                                                                               0,
                                                                                                               20,
                                                                                                               20)];
            activityView.hidesWhenStopped = NO;
            activityView.center = CGPointMake(backgourdView.center.x, backgourdView.center.y);
            activityView.hidden = NO;
            activityView.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
            [backgourdView addSubview:activityView];
            [bottomLoadingView setLoadingView:activityView];
        }
        
    }
}

@end
