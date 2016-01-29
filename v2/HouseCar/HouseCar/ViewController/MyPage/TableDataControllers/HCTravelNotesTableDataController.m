//
//  HCTravelNotesTableDataController.m
//  HouseCar
//
//  Created by tangyin on 16/1/29.
//  Copyright © 2016年 sina.com. All rights reserved.
//

#import "HCTravelNotesTableDataController.h"

#define ColorChangeOffset 360

@implementation HCTravelNotesTableDataController
{
    CGFloat _backgroundViewHeight;
}

- (void)awakeFromNib
{
    self.toolBarView.backgroundColor = [UIColor clearColor];
}

- (void)setContext:(id<IVTUIContext>)context
{
    [super setContext:context];
    [self.tableView setTableHeaderView:_tableViewHeaderView];
    _backgroundViewHeight = _backgroundView.height;
}


- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    [super scrollViewDidScroll:scrollView];
    
    CGFloat yOffset   = self.tableView.contentOffset.y;
    if (yOffset < 0) {
        //向下滑动，图片高度增加相同的高度，宽度按比例增加 yOffset/3是为了减缓宽度变化速度
        CGFloat factor = ((ABS(yOffset/3)+_backgroundViewHeight) * KScreenWidth)/_backgroundViewHeight;
        //-(factor-ImageWidth)/2 使变宽的图片居中显示
        CGRect changeFrame = CGRectZero;
        changeFrame = CGRectMake(-(factor - KScreenWidth)/2, 0, factor, _backgroundViewHeight + ABS(yOffset));
        
        _backgroundView.frame = changeFrame;
        _shadowView.frame = changeFrame;
        
    }else{
        _backgroundView.frame = CGRectMake(0, -yOffset, KScreenWidth, _backgroundViewHeight);
        _shadowView.frame = CGRectMake(0, -yOffset, KScreenWidth, _backgroundViewHeight);
        
        CGFloat alphaValue = yOffset / ColorChangeOffset;
        
        if(alphaValue >= 1){
            alphaValue = 1;
        }
        
        _toolBarView.backgroundColor = [UIColor colorWithRed:51/255.0 green:187/255.0 blue:235/255.0 alpha:alphaValue];
    }
}

@end
