//
//  HCTableDataController.m
//  HouseCar
//
//  Created by shieh fabo on 14-9-28.
//  Copyright (c) 2014年 ytang.com. All rights reserved.
//

#import "HCTableDataController.h"

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

@end
