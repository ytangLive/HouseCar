//
//  HCActivityTabPageDataController.m
//  HouseCar
//
//  Created by tangyin on 16/1/6.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCActivityTabPageDataController.h"

@implementation HCActivityTabPageDataController

- (void)setTabButtons:(NSArray *)tabButtons
{
    [super setTabButtons:tabButtons];
    
    UIButton *subButton = [self.tabButtons objectAtIndex:0];
    id superView = subButton.superview;
    if([superView isKindOfClass:[UIScrollView class]]){
        UIScrollView *scrollView = superView;
        CGRect scrollFrame = scrollView.frame;
        CGRect superFrame = scrollView.superview.frame;
        scrollView.superview.frame = CGRectMake(superFrame.origin.x, superFrame.origin.y, KScreenWidth, superFrame.size.height);
        scrollView.frame = CGRectMake(scrollFrame.origin.x, scrollFrame.origin.y, KScreenWidth, scrollFrame.size.height);
        scrollView.contentSize = CGSizeMake(KScreenWidth, 0);
        
        NSUInteger count = [self.tabButtons count];
        NSUInteger buttonWidth = KScreenWidth / count;
        for (int i = 0; i < count; i++) {
            UIButton *button = self.tabButtons[i];
            CGRect buttonFrame = button.frame;
            button.frame = CGRectMake(i * buttonWidth, buttonFrame.origin.y, buttonWidth, buttonFrame.size.height);
        }
    }
}

- (void)setPageContentView:(UIScrollView *)pageContentView
{
    CGRect scrollFrame = pageContentView.frame;
    pageContentView.frame = CGRectMake(scrollFrame.origin.x, scrollFrame.origin.y, KScreenWidth, scrollFrame.size.height);
    pageContentView.contentSize = CGSizeMake([self.contentViews count] * KScreenWidth, 0);
    
    [super setPageContentView:pageContentView];
}

- (void)setContentViews:(NSArray *)contentViews
{
    [super setContentViews:contentViews];
    
    for (int i = 0; i < [self.contentViews count]; i++) {
        id contentView = self.contentViews[i];
        if([contentView isKindOfClass:[VTTableView class]]){
            VTTableView *tableView = (VTTableView *)contentView;
            CGRect viewFrame = tableView.frame;
            tableView.frame = CGRectMake(i * KScreenWidth, viewFrame.origin.y, viewFrame.size.width, viewFrame.size.height);
        }else{
            UIView *view = (UIView *)contentView;
            CGRect viewFrame = view.frame;
            view.frame = CGRectMake(i * KScreenWidth, viewFrame.origin.y, viewFrame.size.width, viewFrame.size.height);
            for (UIView *subView in view.subviews) {
                subView.frame = CGRectMake(subView.frame.origin.x, subView.frame.origin.y, view.frame.size.width, subView.frame.size.height);
            }
        }
    }
}

@end
